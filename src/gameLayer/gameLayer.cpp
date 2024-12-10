#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <gameObject.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
#include "imgui.h"
#include <iostream>
#include <cstdio>
#include "imfilebrowser.h"
#include <gl2d/gl2d.h>
#include <platformTools.h>
#include <tiledRenderer.h>




gl2d::Renderer2D renderer;


bool initGame() {
	//initializing stuff for the renderer
	gl2d::init();
	renderer.create();




	return true;
}



bool gameLogic(float deltaTime) {
	//init after the flush
	int w = 0; int h = 0;
	w = platform::getFrameBufferSizeX(); //window w
	h = platform::getFrameBufferSizeY(); //window h
	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT); //clear screen
	renderer.updateWindowMetrics(w, h);







	renderer.flush();
	return true;
}

//This function might not be be called if the program is forced closed
void closeGame() {



}

