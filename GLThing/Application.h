#pragma once

#include <gl_core_4_4.h>
#include "GLFW\glfw3.h"
#include "Camera.h"

class Application
{
public:
	GLFWwindow * window;

	virtual bool init() { return true; };	// start the app
	virtual bool update() { return true; };	// update and perform calculations
	virtual void draw() {};	// renders the application
	virtual void exit() {};	// exits and performs cleanup
};

class DemoApp : public Application
{
	virtual bool init();
	virtual bool update();
	virtual void draw();
	virtual void exit();
	FlyCamera camera;

	unsigned int projectID;

	unsigned int indexCount;
	unsigned int VBO, IBO, VAO;
	unsigned int texture;

	float currentTime = 0;
	float lastTime = 0;
	float deltaTime = 0;

	void generateGrid(unsigned int rows, unsigned int cols);
};

class RenderingApp : public Application
{

};