#pragma once
#include "textureLoader.hpp"
#include <gl2d/gl2d.h>
#include <vector>

//NOTE TO FUTURE SELF
//
//Please see the names of acc and if it should be movement again because I'm confused and want to sleep
//date:13-12-2024 - time: 3:18am
//


class gameObject {
private:

	size_t id;

	gl2d::Texture objectTexture;
	gl2d::TextureAtlasPadding objectAtlas;
    bool rendered;
	textureLoader::textureCTX currentTextureCTX;
    
    struct renderLayer{
        std::string name = "default";
        int order = 10;
    };
    static std::vector<renderLayer> layer;
    renderLayer currentLayer;

	struct renderOrderStruct{
		int order = 0;
		size_t goId;
	};
	static std::vector<renderOrderStruct> renderOrder;


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

public:
    //Variables
    static std::vector<gameObject> gameObjects; // Declaration
	enum objectType { bullet, background, entity, staticEntity };
	enum textureType { normal, atlas };
	textureType currentTextureType;
	objectType currentType;
	glm::vec2 currentTextureCoords;
    bool erased = false;

    //functions
	gameObject(objectType, const char*, textureType = normal, glm::vec2 = { 1, 1 }, glm::vec2 = { 0, 0 }, int = 128);
    gameObject();
	void update(float, gl2d::Renderer2D&);
	static void updateByRef(float, gl2d::Renderer2D&, gameObject*);
	static void updateAll(float, gl2d::Renderer2D&);
    static void printObjectState(int);
	static int getObjectCount();
	void gravity();
	void move(float);
	bool isTheSameObject(gameObject);
	static void tempReload();

    struct colliderStruct{
        bool collided = false;
        gameObject* collidedWith;
        bool enableCollision = false;
        static void checkColission();
    }collider2d;


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
	int getId();
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



    static void newLayer(std::string, int);
    static void addToLayer(gameObject*, std::string);
  	static bool check(std::string, std::vector<renderLayer>*);
 


};

