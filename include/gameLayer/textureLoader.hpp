#pragma once
#include <vector>
#include "gl2d/gl2d.h"

class textureLoader{
public:
	struct textureCTX{
		const char* Texture;
		bool atlas = false;
		bool minPixelated = false;
		bool magPixelated = true;
		bool MipMap = true;
		int atlasdim= 128; 
		glm::vec2 atlasPoint = { 0, 0 };
	};
	struct textureStruct{
		const char* loadedTextureName;
		gl2d::Texture loadedTexture;
		gl2d::TextureAtlasPadding loadedAtlas;
	};
	//static std::vector<const char*> loadedTexturesNames;
	//std::vector<gl2d::Texture> loadedTextures;
	//std::vector<gl2d::TextureAtlasPadding> loadedAtlases;
	static std::vector<textureStruct> textures;
	static std::vector<textureCTX> textureCTXs;

	static int checkTextures(textureCTX);
	static void reloadTextures();
	static int getAtlasIterator(textureCTX);

private:
	static void forceLoadTexture(textureCTX, bool);

};
