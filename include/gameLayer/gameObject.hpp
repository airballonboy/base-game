#pragma once
#include <gl2d/gl2d.h>
#include <vector>

//NOTE TO FUTURE SELF
//
//Please see the names of acc and if it should be movement again because I'm confused and want to sleep
//date:13-12-2024 - time: 3:18am
//


class gameObject {
private:

	int32_t id;

	gl2d::Texture objectTexture;
	gl2d::TextureAtlasPadding objectAtlas;

    float baseGravity = 9.8;
	float rotation = 0;
	float turningSpeed = 1;
	glm::vec2 acc = { 0, 0 };//Acceleration
    glm::vec2 vel = { 0, 0 };//Velocity
	glm::vec2 pos = { 100, 100 };//Position
	glm::vec2 dim = { 100, 100 };//Dimensions
	glm::vec2 pivot = { dim.x / 2, dim.y / 2 };
	glm::vec2 enemyViewDirection = { 1,0 };
	bool enableGravity = false;
    bool enableCollision = false;

public:
    static std::vector<gameObject> gameObjects; // Declaration
	enum objectType { bullet, background, entity, staticEntity };
	enum textureType { normal, atlas };
	textureType currentTextureType;
	objectType currentType;
	glm::vec2 currentTextureCoords;
	gameObject(objectType, const char*, textureType = normal, glm::vec2 = { 1, 1 }, glm::vec2 = { 0, 0 }, int = 128);
    gameObject();
    virtual ~gameObject();
	void update(float, gl2d::Renderer2D&);
	static void update2(float, gl2d::Renderer2D&, gameObject*);
	static void updateAll(float, gl2d::Renderer2D&);
	void gravity();
    bool checkColission(gameObject);
    static void printObjectState(gameObject*);
	static int getObjectCount();
	int getId();
	bool isTheSameObject(gameObject);
	void move(float);



	//Setters 
    void setAcc(float, float);
    void setVel(float, float);
    void setPos(float, float);
    void setDim(float, float);
    void setPivot(float, float);
    void setEnemyViewDirection(glm::vec2);
    void setGravityBool(bool);
    void setCollisionBool(bool);
    void setBaseGravity(float);
    void setRotation(float);
    void setTurningSpeed(float);


    //Getters
    glm::vec2 getAcc();
    glm::vec2 getVel();
    glm::vec2 getPos();
    glm::vec2 getDim();
    glm::vec2 getPivot();
    glm::vec2 getEnemyViewDirection();
    bool getGravityBool();
    bool getCollisionBool();
    float getBaseGravity();
    float getRotation();
    float getTurningSpeed();

   


};
class loadOnceClass {
public:
	std::vector<const char*> loadedTexturesNames;
	std::vector<gl2d::Texture> loadedTextures;
	std::vector<gl2d::TextureAtlasPadding> loadedTextureAtlases;

	int checkTextures(const char*, bool, bool = false, bool = true, bool = true, int = 128, glm::vec2 = { 0, 0 });

};
