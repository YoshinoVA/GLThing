#include "Application.h"

#include <aie\Gizmos.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <iostream>
//#define STB_IMAGE_IMPLEMENTATION
#include "stb-master\stb_image.h"
#include "fbx\FBXFile.h"

using namespace glm;

struct Vertex
{
	vec4 position;
	vec4 color;
};

void DemoApp::createOpenGLBuffers(FBXFile* fbx)
{
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		unsigned int* glData = new unsigned int[3];
		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);

		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);

		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);

		glBufferData(GL_ARRAY_BUFFER,
			mesh->m_vertices.size() * sizeof(FBXVertex),
			mesh->m_vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			mesh->m_indices.size() * sizeof(unsigned int),
			mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_TRUE,
			sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;
	}

	fbx = new FBXFile();
	fbx->load("./resources/FBX/soulspear/soulspear.fbx");
}

void DemoApp::cleanupOpenGLBuffers(FBXFile* fbx)
{
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glDeleteVertexArrays(1, &glData[0]);
		glDeleteBuffers(1, &glData[1]);
		glDeleteBuffers(1, &glData[2]);

		delete[] glData;
	}
}

void DemoApp::generateGrid(unsigned int rows, unsigned int cols)
{
	Vertex* aoVerts = new Vertex[rows * cols];
	for (unsigned int r = 0; r < rows; ++r)
	{
		for (unsigned int c = 0; c < cols; ++c)
		{
			aoVerts[r * cols + c].position = vec4(
				(float)c, 0, (float)r, 1);

			vec3 color = vec3(sinf((c / (float)(cols - 1))*(r / (float)(rows - 1))));
			aoVerts[r*cols + c].color = vec4(color, 1);
		}
	}

	unsigned int* auiIndices = new unsigned int[(rows - 1) * (cols - 1) * 6];

	unsigned int index = 0;
	for (unsigned int r = 0; r < (rows - 1); ++r)
	{
		for (unsigned int c = 0; c < (cols - 1); ++c)
		{
			//Tri 1
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);

			//Tri 2
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			auiIndices[index++] = r * cols + (c + 1);
		}
	}

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, (rows * cols) * sizeof(Vertex),
		aoVerts, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)(sizeof(vec4)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (cols - 1) * 6 * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	indexCount = (rows - 1) * (cols - 1) * 6;

	delete[] aoVerts;
	delete[] auiIndices;
}

void DemoApp::generateQuad()
{
	float vertexData[] = {
		// POSITION			UV
		// x,  y,  z,  w	s, t
		 -5,  0,  5,  1,   0, 1,
		  5,  0,  5,  1,   1, 1,
		  5,  0, -5,  1,   1, 0,
		 -5,  0, -5,  1,   0, 0,
	};

	unsigned int indexData[] = {
		0, 1, 2,
		0, 2, 3,
	};

	// VAO - Vertex Array Object
	//   - stores a handle to the VBO and IBO
	// VBO - Vertex Buffer Object
	//   - stores our vertex information (e.x. position, UV)
	// IBO - Index Buffer Object
	//   - stores the order in which vertices are drawn

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);		// Generation
	glBindBuffer(GL_ARRAY_BUFFER, VBO);		// Bind it, setting it as the current whatever
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	// Position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);

	glEnableVertexAttribArray(1);	// UVs
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, ((char*)0) + 16);

	// once we're done, unbind the buffers
	glBindVertexArray(0);						// VAO comes off first
	glBindBuffer(GL_ARRAY_BUFFER, 0);			// VBO or IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	// whatever's left

	indexCount = 6;
}

// - creates the window
// - loads the shaders
// - loads the external assets
//   - textures
//   - models/meshes
bool DemoApp::init()
{
	glfwInit();

	window = glfwCreateWindow(800, 600, "Title", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) {
		glfwDestroyWindow(window);
		glfwTerminate();
		return -3;
	}

	// START RENDERING CODE 

	// vertex shader
	const char* vsSource = "#version 410\n \
						layout(location=0) in vec4 Position; \
						layout(location=1) in vec4 Normal; \
						out vec4 vNormal; \
						uniform mat4 ProjectionView; \
						void main() { vNormal = Normal; \
						gl_Position = ProjectionView * Position; }";

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);

	// fragment shader
	const char* fsSource = "#version 410\n \
						in vec4 vNormal; \
						out vec4 FragColor; \
						void main() {vec4(1,1,1,1);}";

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);


	// assembling the shader program
	projectID = glCreateProgram();
	glAttachShader(projectID, vertexShader);
	glAttachShader(projectID, fragmentShader);
	glLinkProgram(projectID);


	int success = GL_FALSE;

	glGetProgramiv(projectID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(projectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(projectID, GL_INFO_LOG_LENGTH, 0, infoLog);
		printf("Error: Failed to link shader program.\n");
		printf("%s\n", infoLog);
		delete infoLog;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// loading textures
	using glm::vec3;
	using glm::vec4;
	using glm::mat4;

	int imageWidth = 0;
	int imageHeight = 0;
	int imageFormat = 0;

	unsigned char* data = stbi_load("crate.png", &imageWidth, &imageHeight, &imageFormat, STBI_default);

	fbx = new FBXFile();
	fbx->load("../resources/FBX/soulspear/soulspear.fbx");
	createOpenGLBuffers(fbx);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	//generateGrid(10, 10);
	generateQuad();

	return true;
}

// - performing calculations every frame
// - user input
bool DemoApp::update()
{
	glClearColor(0.25f, 0.25f, 0.25f, 1);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	// if the user presses escape, exit from the program
	if (glfwWindowShouldClose(window) == true ||
		glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		return false;
	}

	bool load(const char* filename,
		FBXFile::UNIT_SCALE scale = FBXFile::UNITS_METER,
		bool loadTextures = true,
		bool loadAnimations = true,
		bool flipTextureY = true);

	unsigned int getMeshCount();

	FBXMeshNode* getMeshByIndex(unsigned int index);

	unsigned int vertexAttributes;
	FBXMaterial* material;
	std::vector<FBXVertex> vertices;
	std::vector<unsigned int> indices;
	glm::mat4 localTransform;
	glm::mat4 globalTransform;
	void* userData;

	// delta time
	currentTime = glfwGetTime();
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	return true;
}

// - drawing meshes to the backbuffer
void DemoApp::draw()
{
	mat4 view = glm::lookAt(vec3(5, 5, 5), vec3(0), vec3(0, 1, 0));
	mat4 projection = glm::perspective(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 1000.f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::clear();
	Gizmos::addTransform(glm::mat4(1));
	Gizmos::draw(projection * view);

	glUseProgram(projectID);
	int loc = glGetUniformLocation(projectID, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection * view));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	loc = glGetUniformLocation(projectID, "diffuse");
	glUniform1i(loc, 0);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

	//for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	//{
	//	FBXMeshNode* mesh = fbx->getMeshByIndex(i);

	//	unsigned int* glData = (unsigned int*)mesh->m_userData;

	//	glBindVertexArray(glData[0]);
	//	glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	//}

	glfwSwapBuffers(window);
	glfwPollEvents();
}

// - clean up
// - deleting opengl buffers that we've created
// - unloading any file data that we have open
// - destroy the window
void DemoApp::exit()
{
	cleanupOpenGLBuffers(fbx);
	glDeleteProgram(program);
	fbx->unload();
	Gizmos::destroy();
	glfwTerminate();
}