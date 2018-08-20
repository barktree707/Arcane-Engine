#include "Model.h"

#include <iostream>
#include <stb_image_aug.h>
#include <iostream>

#include "Mesh.h"
#include "../../utils/Logger.h"

namespace arcane { namespace graphics {

	Model::Model(const char *path) {
		loadModel(path);
	}

	Model::Model(const Mesh &mesh) {
		m_Meshes.push_back(mesh);
	}

	Model::Model(const std::vector<Mesh> &meshes) {
		m_Meshes = meshes;
	}

	void Model::Draw(Shader &shader, RenderPass pass) const {
		for (unsigned int i = 0; i < m_Meshes.size(); ++i) {
			// Avoid binding material inforomation when it isn't needed
			if (pass != RenderPass::ShadowmapPass)
				m_Meshes[i].m_Material.BindMaterialInformation(shader);
			m_Meshes[i].Draw();
		}
	}

	void Model::loadModel(const std::string &path) {
		Assimp::Importer import;
		const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			utils::Logger::getInstance().error("logged_files/model_loading.txt", "model initialization", import.GetErrorString());
			return;
		}

		m_Directory = path.substr(0, path.find_last_of('/'));

		processNode(scene->mRootNode, scene);
	}

	void Model::processNode(aiNode *node, const aiScene *scene) {
		// Process all of the node's meshes (if any)
		for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
			// Each node has an array of mesh indices, use these indices to get the meshes from the scene
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			m_Meshes.push_back(processMesh(mesh, scene));
		}
		// Process all of the node's children
		for (unsigned int i = 0; i < node->mNumChildren; ++i) {
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> bitangents;
		std::vector<unsigned int> indices;
		indices.reserve(mesh->mNumFaces * 3);

		// Process vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			glm::vec2 uvCoord;
			// Texture Coordinates (check if there is texture coordinates)
			if (mesh->mTextureCoords[0]) {
				// A vertex can contain up to 8 different texture coordinates. We are just going to use one set of TexCoords per vertex so grab the first one
				uvCoord.x = mesh->mTextureCoords[0][i].x;
				uvCoord.y = mesh->mTextureCoords[0][i].y;
			}
			else {
				uvCoord.x = 0.0f;
				uvCoord.y = 0.0f;
			}

			positions.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
			uvs.push_back(glm::vec2(uvCoord.x, uvCoord.y));
			normals.push_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
			tangents.push_back(glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z));
			bitangents.push_back(glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z));
		}

		// Process Indices
		// Loop through every face (triangle thanks to aiProcess_Triangulate) and stores its indices in our meshes indices. This will ensure they are in the right order.
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; ++j) {
				indices.push_back(face.mIndices[j]);
			}
		}

		Mesh newMesh(positions, uvs, normals, tangents, bitangents, indices);
		newMesh.LoadData();

		// Process Materials (textures in this case)
		if (mesh->mMaterialIndex >= 0) {
			aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

			newMesh.m_Material.setDiffuseMap(loadMaterialTexture(material, aiTextureType_DIFFUSE));
			newMesh.m_Material.setSpecularMap(loadMaterialTexture(material, aiTextureType_SPECULAR));
			newMesh.m_Material.setNormalMap(loadMaterialTexture(material, aiTextureType_NORMALS));
			newMesh.m_Material.setEmissionMap(loadMaterialTexture(material, aiTextureType_EMISSIVE));
			float shininess = 0.0f;
			material->Get(AI_MATKEY_SHININESS, shininess); // Assimp scales specular exponent by 4 times since most renderers handle it that way. Value defaults to 0 if not specified
			newMesh.m_Material.setShininess(shininess);
		}

		return newMesh;
	}

	Texture* Model::loadMaterialTexture(aiMaterial *mat, aiTextureType type) {
		// Log material constraints are being violated (1 texture per type for the standard shader)
		if (mat->GetTextureCount(type) > 1)
			utils::Logger::getInstance().error("logged_files/material_creation.txt", "Mesh Loading", "Mesh's default material contains more than 1 texture for the same type, which currently isn't supported by the standard shader");

		// Load the texture of a certain type, assuming there is one
		if (mat->GetTextureCount(type) > 0) {
			aiString str;
			mat->GetTexture(type, 0, &str); // Grab only the first texture (standard shader only supports one texture of each type, it doesn't know how you want to do special blending)

			// Assumption made: material stuff is located in the same directory as the model object
			std::string fileToSearch = (m_Directory + "/" + std::string(str.C_Str())).c_str();
			return utils::TextureLoader::load2DTexture(fileToSearch);
		}

		return nullptr;
	}
} }