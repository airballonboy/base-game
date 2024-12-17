#include "gameObject.hpp"
#include <cstring>
#include <gl2d/gl2d.h>
#include <algorithm>
#include <ostream>
#include <vector>
#include <iostream>



int objectCount = 0;
loadOnceClass loadOnce;
// Definition of static member
std::vector<gameObject> gameObject::gameObjects;


//Checks if the current texture is loaded or not and loads it if not
int loadOnceClass::checkTextures(const char* Texture, bool atlas, bool minPixelated, bool magPixelated, bool MipMap, 
                                 int atlasdim, glm::vec2 atlasPoint) {
	
    for (const char* T : loadedTexturesNames) {
		if (Texture == T) { return std::distance(loadedTexturesNames.begin() 
			,std::find(loadedTexturesNames.begin(), loadedTexturesNames.end(), Texture)); }
	}
	loadOnceClass::loadedTexturesNames.push_back(Texture);
	gl2d::Texture t;
	if (!atlas) {
		t.loadFromFile(Texture, minPixelated, magPixelated, MipMap);
	} else {
		gl2d::TextureAtlasPadding tPadding;
		t.loadFromFileWithPixelPadding(Texture, atlasdim, minPixelated, magPixelated, MipMap);
		tPadding = gl2d::TextureAtlasPadding(atlasPoint.x, atlasPoint.y, t.GetSize().x, t.GetSize().y);
		loadOnceClass::loadedTextureAtlases.push_back(tPadding);
	}
	loadOnceClass::loadedTextures.push_back(t);
	return std::distance(loadedTexturesNames.begin(), std::find(loadedTexturesNames.begin(), loadedTexturesNames.end(), Texture));
}
int gameObject::getObjectCount() {
	return objectCount;
}

gameObject::~gameObject(){

}

//Creating the object and loading it's texture if it isn't loaded
gameObject::gameObject(objectType _type, const char* _textureFile, textureType _currentTextureType
                            , glm::vec2 _atlasPoint, glm::vec2 _texturePoint, int _atlasdim) {

	objectCount++;
	id = objectCount;
	currentTextureCoords = _texturePoint;
	currentTextureType = _currentTextureType;
	currentType = _type;
	if (currentTextureType == normal) {
		objectTexture = loadOnce.loadedTextures[loadOnce.checkTextures(_textureFile, false, false ,true, true)];
	} else if (currentTextureType == atlas) {
		objectTexture = loadOnce.loadedTextures[loadOnce.checkTextures(_textureFile, true, false, true, true, _atlasdim, _atlasPoint)];
		objectAtlas = loadOnce.loadedTextureAtlases[loadOnce.checkTextures(_textureFile, true, false, true, true, _atlasdim, _atlasPoint)];
	}
    gameObjects.emplace_back(*this); 
}
gameObject::gameObject(){
    
}

//Checks if two objects are the same
bool gameObject::isTheSameObject(gameObject otherObject) {
	if (this->id == otherObject.id) {
		return true;
	} else { return false; }
}

bool gameObject::checkColission(gameObject currentObject){
    //TODO make collision checks
    if (enableCollision) {
        //Code here
        
    }
    return false;
}
void gameObject::move(float deltaTime) {
    glm::vec2 normalizedAcc = glm::normalize(acc);
	this->setAcc(normalizedAcc.x + (normalizedAcc.x * deltaTime * vel.x), normalizedAcc.y + (normalizedAcc.y * deltaTime * vel.y));
}

void gameObject::gravity() {
	//TODO add collision detection and gravity system
    this->setAcc(acc.x, (acc.y + baseGravity));
    //Call collision detection here too?
}

void gameObject::updateAll(float deltaTime, gl2d::Renderer2D& renderer) {
    for (auto go : gameObjects){
        update2(deltaTime, renderer, &go);
    }
}
void gameObject::update2(float deltaTime, gl2d::Renderer2D& renderer, gameObject* that){
	if (that->enableGravity) { that->gravity(); }
	if (that->acc != glm::vec2{0, 0}) { that->move(deltaTime); that->setPos(that->pos.x + that->acc.x, that->pos.y + that->acc.y); }
	if (that->currentTextureType == gameObject::normal) {
		renderer.renderRectangle({ (that->pos.x - that->pivot.x), (that->pos.y - that->pivot.y), that->dim }, that->objectTexture,
			Colors_White, {}, glm::degrees(that->rotation) + 90.f);
	} else if (that->currentTextureType == gameObject::atlas) {
		renderer.renderRectangle({ (that->pos.x - that->pivot.x), (that->pos.y - that->pivot.y), that->dim }, that->objectTexture, Colors_White,
            {},glm::degrees(that->rotation) + 90.f, that->objectAtlas.get(that->currentTextureCoords.x, that->currentTextureCoords.y));
	}
}
void gameObject::printObjectState(gameObject* objectPtr){
    gameObject object = *objectPtr;
    std::cout << "id: " << object.id<< std::endl;
    std::cout << "currentTextureType: " << object.currentTextureType<< std::endl;
    std::cout << "currentType: " << object.currentType<< std::endl;
    std::cout << "baseGravity: " << object.baseGravity<< std::endl;
    std::cout << "rotation: " << object.rotation<< std::endl;
    std::cout << "turningSpeed: " << object.turningSpeed<< std::endl;
    std::cout << "acc.x: " << object.acc.x<< std::endl;
    std::cout << "acc.y: " << object.acc.y<< std::endl;
    std::cout << "vel.x: " << object.vel.x<< std::endl;
    std::cout << "vel.y: " << object.vel.y<< std::endl;
    std::cout << "pos.x: " << object.pos.x<< std::endl;
    std::cout << "pos.y: " << object.pos.y<< std::endl;
    std::cout << "dim.x: " << object.dim.x<< std::endl;
    std::cout << "dim.y: " << object.dim.y<< std::endl;
    std::cout << "pivot.x: " << object.pivot.x<< std::endl;
    std::cout << "pivot.y: " << object.pivot.y<< std::endl;
    std::cout << "currentTextureCoords.x: " << object.currentTextureCoords.x<< std::endl;
    std::cout << "currentTextureCoords.y: " << object.currentTextureCoords.y<< std::endl;
    std::cout << "enableGravity: " << object.enableGravity<< std::endl;
    std::cout << "enableCollision: " << object.enableCollision<< std::endl;


    std::cout << "---------------------------------------------------" << std::endl;
}
void gameObject::update(float deltaTime, gl2d::Renderer2D& renderer) {
	if (enableGravity) { this->gravity(); }
	if (acc != glm::vec2{0, 0}) { this->move(deltaTime); this->setPos(this->getPos().x+this->getAcc().x, this->getPos().y+this->getAcc().y); }
	if (currentTextureType == normal) {
		renderer.renderRectangle({ (pos.x - pivot.x), (pos.y - pivot.y), dim }, objectTexture,
			Colors_White, {}, glm::degrees(this->rotation) + 90.f);
	} else if (currentTextureType == atlas) {
		renderer.renderRectangle({ (pos.x - pivot.x), (pos.y - pivot.y), dim }, objectTexture,
			Colors_White, {}, glm::degrees(this->rotation) + 90.f, objectAtlas.get(currentTextureCoords.x, currentTextureCoords.y));
	}
}







//Setters
void gameObject::setDim(float newDimX, float newDimY) {
	this->dim = { newDimX, newDimY };
	this->pivot = { this->dim.x / 2, this->dim.y / 2 };
    gameObjects[(this->id - 1)].dim = this->dim;
    gameObjects[(this->id - 1)].pivot = this->pivot;
}
void gameObject::setAcc(float newAccX, float newAccY){
    this->acc = { newAccX, newAccY };
    gameObjects[(this->id - 1)].acc = this->acc;
}
void gameObject::setVel(float newVelX, float newVelY){
    this->vel = { newVelX, newVelY };
    gameObjects[(this->id - 1)].vel = this->vel;
}
void gameObject::setPos(float newPosX, float newPosY){
    this->pos = { newPosX, newPosY };
    gameObjects[(this->id - 1)].pos = this->pos;
}
void gameObject::setPivot(float newPivotX, float newPivotY){
    this->pivot = { newPivotX, newPivotY };
    gameObjects[(this->id - 1)].pivot = this->pivot;
}
void gameObject::setEnemyViewDirection(glm::vec2 newDir){
    this->enemyViewDirection = newDir;
    gameObjects[(this->id - 1)].enemyViewDirection = this->enemyViewDirection;
}
void gameObject::setGravityBool(bool newGravityBool){
    this->enableGravity = newGravityBool;
    gameObjects[(this->id - 1)].enableGravity = this->enableGravity;
}
void gameObject::setCollisionBool(bool newCollisionBool){
    this->enableCollision = newCollisionBool;
    gameObjects[(this->id - 1)].enableCollision = this->enableCollision;
}
void gameObject::setBaseGravity(float newBaseGravity){
    this->baseGravity = newBaseGravity;
    gameObjects[(this->id - 1)].baseGravity = this->baseGravity;
}
void gameObject::setRotation(float newRotation){
    this->rotation = newRotation;
    gameObjects[(this->id - 1)].rotation = this->rotation;
}
void gameObject::setTurningSpeed(float newTurningSpeed){
    this->turningSpeed = newTurningSpeed;
    gameObjects[(this->id - 1)].turningSpeed = this->turningSpeed;
}




//Getters
glm::vec2 gameObject::getDim() {
    return gameObjects[(this->id - 1)].dim;
}
glm::vec2 gameObject::getAcc(){
    return gameObjects[(this->id - 1)].acc;
}
glm::vec2 gameObject::getVel(){
    return gameObjects[(this->id - 1)].vel;
}
glm::vec2 gameObject::getPos(){
    return gameObjects[(this->id - 1)].pos;
}
glm::vec2 gameObject::getPivot(){
    return gameObjects[(this->id - 1)].pivot;
}
glm::vec2 gameObject::getEnemyViewDirection(){
    return gameObjects[(this->id - 1)].enemyViewDirection;
}
bool gameObject::getGravityBool(){
    return gameObjects[(this->id - 1)].enableGravity;
}
bool gameObject::getCollisionBool(){
    return gameObjects[(this->id - 1)].enableCollision;
}
float gameObject::getBaseGravity(){
    return gameObjects[(this->id - 1)].baseGravity;
}
float gameObject::getRotation(){
    return gameObjects[(this->id - 1)].rotation;
}
float gameObject::getTurningSpeed(){
    return gameObjects[(this->id - 1)].turningSpeed;
}
int gameObject::getId() {
	return this->id;
}




