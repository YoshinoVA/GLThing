#pragma once

#include <gl_core_4_4.h>
#include "GLFW\glfw3.h"
#include "Camera.h"
#include "fbx\FBXFile.h"

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

	// VBO - Vertex Buffer Object
	//  - storing data/information pertaining to the vertices for a mesh
	//  - position, texcoord
	// IBO - Index Buffer Object
	//  - storing the order in which these vertices are drawn
	// VAO - Vertex Array Object
	//  - a container for the VBO and IBO

	unsigned int texture;
	unsigned int normalmap;

	float currentTime = 0;
	float lastTime = 0;
	float deltaTime = 0;

	//void generateGrid(unsigned int rows, unsigned int cols);
	void generateQuad();

	FBXFile* fbx;

	unsigned int program;
	void createOpenGLBuffers(FBXFile* fbx);
	void cleanupOpenGLBuffers(FBXFile* fbx);
};