#include "textureLoader.hpp"
#include "gl2d/gl2d.h"
#include <iostream>

//static members
std::vector<textureLoader::textureStruct> textureLoader::textures;
std::vector<textureLoader::textureCTX> textureLoader::textureCTXs;
//Checks if the current texture is loaded or not and loads it if not
int textureLoader::checkTextures(textureLoader::textureCTX ctx) {
	struct iteratorOrError{
		int i = 0;
		std::string err = "";
	};
	auto getIterator = [ctx]() -> iteratorOrError{
		int i = 0;	
		for (auto& T : textures) {
			if (ctx.Texture == T.loadedTextureName) { 
				return {i, ""}; 
			}
			i++;
		}
		return {0, "not found"};
	};

	if(getIterator().err != "not found") return getIterator().i; 
	forceLoadTexture(ctx, false);
	return getIterator().i;
}
int textureLoader::getAtlasIterator(textureLoader::textureCTX ctx){
	int i = 0;
	for (auto& CTX : textureCTXs){
		if(!CTX.atlas) continue;
		if(CTX.Texture == ctx.Texture) return i;
		i++;
	}
	return 0;
}
void textureLoader::reloadTextures(){
	//loadedTextures.clear();
	//loadedAtlases.clear();
	gl2d::Texture eT;
	gl2d::TextureAtlasPadding eTAP;
	for(auto& T : textures){
		T.loadedTexture = eT;
		T.loadedAtlas = eTAP;
	}
	for (auto& T : textures){
		for (auto& ctx : textureCTXs){
			if(T.loadedTextureName == ctx.Texture) forceLoadTexture(ctx, true); 
		}
	}
	std::cout << "reloaded Textures" << std::endl;
}
void textureLoader::forceLoadTexture(textureLoader::textureCTX ctx, bool reload) {

	//if(!reload){ textures.push_back(ctx.Texture); }
	gl2d::Texture t;
	gl2d::TextureAtlasPadding tPadding;
	if (!ctx.atlas) {
		t.loadFromFile(ctx.Texture, ctx.minPixelated, ctx.magPixelated, ctx.MipMap);
	} else {
		t.loadFromFileWithPixelPadding(ctx.Texture, ctx.atlasdim, ctx.minPixelated, ctx.magPixelated, ctx.MipMap);
		tPadding = gl2d::TextureAtlasPadding(ctx.atlasPoint.x, ctx.atlasPoint.y, t.GetSize().x, t.GetSize().y);
	}
	if(!reload) {
		textures.push_back({ctx.Texture, t, tPadding});
		textureLoader::textureCTXs.push_back(ctx);
	}else {
		for(auto& T : textures) {
			if(T.loadedTextureName == ctx.Texture) {
				T.loadedAtlas = tPadding;
				T.loadedTexture = t;
			}
		}
	}
}
