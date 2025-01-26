#include "gameObject.hpp"
#include "glm/fwd.hpp"
#include <cstddef>
#include <string>
#include <gl2d/gl2d.h>
#include <algorithm>
#include <ostream>
#include <vector>
#include <iostream>
#include <textureLoader.hpp>


//textureLoader textureLoad;

int objectCount = 0;
// Definition of static member
std::vector<gameObject> gameObject::gameObjects;
std::vector<gameObject::renderLayer> gameObject::layer;
std::vector<gameObject::renderOrderStruct> gameObject::renderOrder;


void gameObject::newLayer(std::string name, int order){
    renderLayer L = {
        .name = name,
        .order = order
    };
	for(auto& l : layer) if(l.name == L.name){ std::cerr << "layer already exists" << std::endl;return; };
    layer.push_back(L);
    std::sort(layer.begin(), layer.end(), [](const renderLayer& a, const renderLayer& b){
        return a.order < b.order;
    });
}
bool gameObject::check(std::string thisName, std::vector<renderLayer>* L){
	bool C;
	for (renderLayer RL : *L) {
		if (RL.name == thisName) { C = true;break; }
		C = false;
	}
	return C;
}

void gameObject::addToLayer(gameObject* GO, std::string name){
	auto find = [](std::string thisName, std::vector<renderLayer>* L){ 
        for(renderLayer RL : *L){
            if (RL.name == thisName) return RL.order;
        }
        return 0;
    };
    
    if (!check (name, &layer)){
        std::cerr << "name doesn't match any layer";
    }
    GO->currentLayer.name = name;
    GO->currentLayer.order = find(name, &layer);
    gameObjects[GO->id - 1].currentLayer.name = name;
    gameObjects[GO->id - 1].currentLayer.order = GO->currentLayer.order;
}
int gameObject::getObjectCount() {
	return objectCount;
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
		currentTextureCTX = {_textureFile, false, false ,true, true};
		objectTexture = textureLoader::textures[textureLoader::checkTextures(currentTextureCTX)].loadedTexture;
	} else if (currentTextureType == atlas) {
		currentTextureCTX = {_textureFile, true, false, true, true, _atlasdim, _atlasPoint};
		objectTexture = textureLoader::textures[textureLoader::checkTextures(currentTextureCTX)].loadedTexture;
		objectAtlas   = textureLoader::textures[textureLoader::getAtlasIterator(currentTextureCTX)].loadedAtlas;
	}
    gameObjects.emplace_back(*this);
	if(!check("default", &layer)) newLayer("default", 10);
    addToLayer(&gameObjects[this->id - 1], "default");
}
gameObject::gameObject(){}

//Checks if two objects are the same
bool gameObject::isTheSameObject(gameObject otherObject) {
	if (this->id == otherObject.id) {
		return true;
	} else { return false; }
}

void gameObject::colliderStruct::checkColission(){
	struct collCtx{
		collCtx(gameObject* a, gameObject* b){
			aId = a->id; bId = b->id; aPos = a->pos; bPos = b->pos; aDim = a->dim; bDim = b->dim;
		}
		size_t aId, bId;
		glm::vec2 aPos, bPos;
		glm::vec2 aDim, bDim;
	};
	auto collCheck = [](collCtx ctx)-> bool{
			return ((ctx.aId != ctx.bId)                     &&
					(ctx.aPos.x < (ctx.bPos.x + ctx.bDim.x)) &&
					((ctx.aPos.x + ctx.aDim.x) > ctx.bPos.x) &&
					(ctx.aPos.y < (ctx.bPos.y + ctx.bDim.y)) &&
					((ctx.aPos.y + ctx.aDim.y) > ctx.bPos.y)); 
	}; 
    gameObject nu;
    for (auto& objA : gameObjects) {
        if (!objA.collider2d.enableCollision || objA.erased) continue;
		if (!objA.collider2d.collided) {
            for (auto& objB : gameObjects) {
                if ((objA.id == objB.id)||(!objB.collider2d.enableCollision)||(objB.erased)) continue;
				if (collCheck(collCtx(&objA, &objB))) {
                    objA.collider2d = { .collided = true, .collidedWith = &objB, .enableCollision = true };
                    objB.collider2d = { .collided = true, .collidedWith = &objA, .enableCollision = true };
                    //std::cout << "collision connected, obj:" << objA.id << " with obj:" << objB.id << std::endl;
                    break; // Exit inner loop on first collision
                }
            }
		}else {
			if (objA.collider2d.collidedWith->erased || !collCheck(collCtx(&objA, objA.collider2d.collidedWith))) {
				objA.collider2d.collided = false;
				objA.collider2d.collidedWith = &nu;
				//std::cout << objA.collider2d.collided << std::endl;
			}
		}
    }
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
    //the outerLoop goes over every layer and for every layer the inner loop updates every object in the Layer 
	// NOTE: needs a refactor
	int currentRenderOrder = 0;
    colliderStruct::checkColission();
    for (size_t i = 0; i < layer.size(); i++){
        for (auto& go : gameObjects){
			if (go.erased) continue;
            if (!go.rendered && (go.currentLayer.name == layer[i].name)){
				renderOrder.push_back({currentRenderOrder, go.id});
				currentRenderOrder++;
                go.rendered = true;
            }
        }
    }
	std::sort(renderOrder.begin(), renderOrder.end(), [](const renderOrderStruct& a, const renderOrderStruct& b){
        return a.order < b.order;
    });
	for(auto& ro : renderOrder) updateByRef(deltaTime, renderer, &gameObjects[ro.goId - 1]);
    for(auto& go : gameObjects) go.rendered = false;
	renderOrder.clear();
}
void gameObject::updateByRef(float deltaTime, gl2d::Renderer2D& renderer, gameObject* obj){
	if (obj->enableGravity) obj->gravity();
	if (obj->acc != glm::vec2{0, 0}) { obj->move(deltaTime); obj->setPos(obj->pos.x + obj->acc.x, obj->pos.y + obj->acc.y); }
	if (obj->currentTextureType == gameObject::normal) {
		renderer.renderRectangle({ (obj->pos.x - obj->pivot.x), (obj->pos.y - obj->pivot.y), obj->dim }, obj->objectTexture,
			Colors_White, {}, glm::degrees(obj->rotation) + 90.f);
	} else if (obj->currentTextureType == gameObject::atlas) {
		renderer.renderRectangle({ (obj->pos.x - obj->pivot.x), (obj->pos.y - obj->pivot.y), obj->dim }, obj->objectTexture, Colors_White,
            {},glm::degrees(obj->rotation) + 90.f, obj->objectAtlas.get(obj->currentTextureCoords.x, obj->currentTextureCoords.y));
	}
}
void gameObject::tempReload(){
	textureLoader::reloadTextures();
	for(auto& obj : gameObjects){
		if (obj.currentTextureType == normal) {
			obj.objectTexture = textureLoader::textures[textureLoader::checkTextures(obj.currentTextureCTX)].loadedTexture;
		} else if (obj.currentTextureType == atlas) {
			obj.objectTexture = textureLoader::textures[textureLoader::checkTextures(obj.currentTextureCTX)].loadedTexture;
			obj.objectAtlas   = textureLoader::textures[textureLoader::getAtlasIterator(obj.currentTextureCTX)].loadedAtlas;
		}
	}
}

void gameObject::printObjectState(int id){
    gameObject& o = gameObjects[id - 1];
    std::cout << "id: "                  	<< o.id							<< std::endl;
    std::cout << "currentTextureType: "  	<< o.currentTextureType			<< std::endl;
    std::cout << "currentType: "         	<< o.currentType				<< std::endl;
    std::cout << "currentSortingLayer: " 	<< o.currentLayer.name 			<< std::endl;
    std::cout << "currentSortingLayer: " 	<< o.currentLayer.order 		<< std::endl;
    std::cout << "baseGravity: "  		 	<< o.baseGravity				<< std::endl;
    std::cout << "rotation: " 			 	<< o.rotation					<< std::endl;
    std::cout << "turningSpeed: " 		 	<< o.turningSpeed				<< std::endl;
    std::cout << "acc.x: " 				 	<< o.acc.x						<< std::endl;
    std::cout << "acc.y: " 				 	<< o.acc.y						<< std::endl;
    std::cout << "vel.x: " 				 	<< o.vel.x						<< std::endl;
    std::cout << "vel.y: " 				 	<< o.vel.y						<< std::endl;
    std::cout << "pos.x: " 	  			 	<< o.pos.x						<< std::endl;
    std::cout << "pos.y: " 	       		 	<< o.pos.y						<< std::endl;
    std::cout << "dim.x: " 			     	<< o.dim.x						<< std::endl;
    std::cout << "dim.y: " 				 	<< o.dim.y						<< std::endl;
    std::cout << "pivot.x: " 			 	<< o.pivot.x					<< std::endl;
    std::cout << "pivot.y: " 			 	<< o.pivot.y					<< std::endl;
    std::cout << "currentTextureCoords.x: " << o.currentTextureCoords.x		<< std::endl;
    std::cout << "currentTextureCoords.y: " << o.currentTextureCoords.y		<< std::endl;
    std::cout << "enableGravity: " 			<< o.enableGravity				<< std::endl;
    std::cout << "enableCollision: " 		<< o.collider2d.enableCollision << std::endl;
    std::cout << "collided: "          		<< o.collider2d.collided		<< std::endl;
    std::cout << "collidedWith: " 			<< o.collider2d.collidedWith	<< std::endl;


    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
}
void gameObject::update(float deltaTime, gl2d::Renderer2D& renderer) {
	// NOTE: 
	// No longer works / Not needed
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
    this->collider2d.enableCollision = newCollisionBool;
    gameObjects[(this->id - 1)].collider2d.enableCollision = this->collider2d.enableCollision;
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
    return gameObjects[(this->id - 1)].collider2d.enableCollision;
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

