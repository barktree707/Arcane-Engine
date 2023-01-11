#pragma once

#include <Arcane/Graphics/Texture/Texture.h>

namespace Arcane
{
	class Texture;
	struct TextureSettings;
	class Cubemap;
	struct CubemapSettings;

	struct TextureGenerationData
	{
		int width, height;
		GLenum dataFormat;
		unsigned char *data;
		Texture *texture;
	};

	struct CubemapGenerationData
	{
		int width, height;
		GLenum dataFormat;
		unsigned char *data;
		Cubemap *cubemap;
		GLenum face;
	};

	class TextureLoader
	{
		friend class AssetManager;
		friend class Application;
	private:
		static void InitializeDefaultTextures();

		static void Load2DTextureData(std::string &path, TextureGenerationData &inOutData);
		static void Generate2DTexture(std::string &path, TextureGenerationData &inOutData);

		static void LoadCubemapTextureData(std::string &path, CubemapGenerationData &inOutData);
		static void GenerateCubemapTexture(std::string &path, CubemapGenerationData &inOutData);
	private:
		// Default Textures
		static Texture *s_DefaultNormal;
		static Texture *s_WhiteTexture, *s_BlackTexture;
	};
}