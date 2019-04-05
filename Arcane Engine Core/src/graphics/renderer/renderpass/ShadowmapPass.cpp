#include "pch.h"
#include "ShadowmapPass.h"

#include <utils/loaders/ShaderLoader.h>

namespace arcane {

	ShadowmapPass::ShadowmapPass(Scene3D *scene) : RenderPass(scene, RenderPassType::ShadowmapPassType) 
	{
		m_ShadowmapShader = ShaderLoader::loadShader("src/shaders/shadowmap.vert", "src/shaders/shadowmap.frag");
		m_ShadowmapFramebuffer = new Framebuffer(SHADOWMAP_RESOLUTION_X, SHADOWMAP_RESOLUTION_Y);
		m_ShadowmapFramebuffer->addDepthAttachment(false).createFramebuffer();
	}

	ShadowmapPass::ShadowmapPass(Scene3D *scene, Framebuffer *customFramebuffer) : RenderPass(scene, RenderPassType::ShadowmapPassType), m_ShadowmapFramebuffer(customFramebuffer)
	{
		m_ShadowmapShader = ShaderLoader::loadShader("src/shaders/shadowmap.vert", "src/shaders/shadowmap.frag");
	}

	ShadowmapPass::~ShadowmapPass() {}

	ShadowmapPassOutput ShadowmapPass::generateShadowmaps(ICamera *camera) {
		// TODO: Add rendering state changes (to ensure proper state)
		glViewport(0, 0, m_ShadowmapFramebuffer->getWidth(), m_ShadowmapFramebuffer->getHeight());
		m_ShadowmapFramebuffer->bind();
		m_ShadowmapFramebuffer->clear();

		// Setup
		ModelRenderer *modelRenderer = m_ActiveScene->getModelRenderer();
		Terrain *terrain = m_ActiveScene->getTerrain();
		DynamicLightManager *lightManager = m_ActiveScene->getDynamicLightManager();

		// View setup
		m_GLCache->switchShader(m_ShadowmapShader);
		glm::vec3 dirLightShadowmapLookAtPos = camera->getPosition() + (glm::normalize(glm::vec3(camera->getFront().x, 0.0f, camera->getFront().z)) * 50.0f);
		glm::vec3 dirLightShadowmapEyePos = dirLightShadowmapLookAtPos + (-lightManager->getDirectionalLightDirection() * 100.0f);
		glm::mat4 directionalLightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, SHADOWMAP_NEAR_PLANE, SHADOWMAP_FAR_PLANE);
		glm::mat4 directionalLightView = glm::lookAt(dirLightShadowmapEyePos, dirLightShadowmapLookAtPos, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 directionalLightViewProjMatrix = directionalLightProjection * directionalLightView;
		m_ShadowmapShader->setUniformMat4("lightSpaceViewProjectionMatrix", directionalLightViewProjMatrix);

		// Render models
		m_ActiveScene->addModelsToRenderer();
		modelRenderer->flushOpaque(m_ShadowmapShader, m_RenderPassType);
		modelRenderer->flushTransparent(m_ShadowmapShader, m_RenderPassType);

		// Render terrain
		terrain->Draw(m_ShadowmapShader, m_RenderPassType);

		// Render pass output
		ShadowmapPassOutput passOutput;
		passOutput.directionalLightViewProjMatrix = directionalLightViewProjMatrix;
		passOutput.shadowmapFramebuffer = m_ShadowmapFramebuffer;
		return passOutput;
	}

}