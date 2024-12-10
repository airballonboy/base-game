#include "gameObject.hpp"
#include <gl2d/gl2d.h>
#include <algorithm>



int objectCount = 0;
loadOnceClass loadOnce;



//Checks if the current texture is loaded or not and loads it if not
int loadOnceClass::checkTextures(const char* Texture, bool atlas, int atlasSize, glm::vec2 atlasPoint) {
	for (const char* T : loadedTexturesNames) {
		if (Texture == T) { return std::distance(loadedTexturesNames.begin() 
			,std::find(loadedTexturesNames.begin(), loadedTexturesNames.end(), Texture)); }
	}
	loadOnceClass::loadedTexturesNames.push_back(Texture);
	gl2d::Texture t;
	if (!atlas) {
		t.loadFromFile(Texture, true);
	} else {
		gl2d::TextureAtlasPadding tPadding;
		t.loadFromFileWithPixelPadding(Texture, atlasSize, true);
		tPadding = gl2d::TextureAtlasPadding(atlasPoint.x, atlasPoint.y, t.GetSize().x, t.GetSize().y);
		loadOnceClass::loadedTextureAtlases.push_back(tPadding);
	}
	loadOnceClass::loadedTextures.push_back(t);
	return std::distance(loadedTexturesNames.begin(), std::find(loadedTexturesNames.begin(), loadedTexturesNames.end(), Texture));
}
int gameObject::getObjectCount() {
	return objectCount;
}


//Creating the object and loading it's texture if it isn't loaded
void gameObject::createObject(objectType _type, const char* _textureFile, textureType _currentTextureType
			      , glm::vec2 _atlasPoint, glm::vec2 _texturePoint, int _atlasSize) {

	objectCount++;
	id = objectCount;
	currentAtlasPoint = _texturePoint;
	currentTextureType = _currentTextureType;
	currentType = _type;
	if (currentTextureType == normal) {
		objectTexture = loadOnce.loadedTextures[loadOnce.checkTextures(_textureFile, false)];
	} else if (currentTextureType == atlas) {
		objectTexture = loadOnce.loadedTextures[loadOnce.checkTextures(_textureFile, true, _atlasSize, _atlasPoint)];
		objectAtlas = loadOnce.loadedTextureAtlases[loadOnce.checkTextures(_textureFile, true, _atlasSize, _atlasPoint)];
	}
}

//Checks if two objects are the same
bool gameObject::isTheSameObject(gameObject otherObject) {
	if (this->id == otherObject.id) {
		return true;
	} else { return false; }
}

int gameObject::getId() {
	return this->id;
}

void gameObject::setSize(float newSizeX, float newSizeY) {
	size = { newSizeX, newSizeY };
	center = { size.x / 2, size.y / 2 };
}

void gameObject::gravity() {
	//TODO add collision detection and gravity system
}
void gameObject::move(float deltaTime) {
	movement = glm::normalize(movement);
	movement += movement * deltaTime * speed;
}

void gameObject::update(float deltaTime, gl2d::Renderer2D& renderer) {
	if (enableGravity) { this->gravity(); }
	if (movement != glm::vec2{0, 0}) { this->move(deltaTime); this->pos += movement; }

	if (currentTextureType == normal) {
		renderer.renderRectangle({ (pos.x - center.x), (pos.y - center.y), size }, objectTexture,
			Colors_White, {}, glm::degrees(this->rotation) + 90.f);
	} else if (currentTextureType == atlas) {
		renderer.renderRectangle({ (pos.x - center.x), (pos.y - center.y), size }, objectTexture,
			Colors_White, {}, glm::degrees(this->rotation) + 90.f, objectAtlas.get(currentAtlasPoint.x, currentAtlasPoint.y));
	}
}

