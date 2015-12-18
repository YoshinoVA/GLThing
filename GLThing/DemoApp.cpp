#include "Application.h"

#include <aie\Gizmos.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <iostream>
//#define STB_IMAGE_IMPLEMENTATION
#include "stb-master\stb_image.h"
#include "fbx\FBXFile.h"

using namespace glm;

void APIENTRY oglErrorDefaultCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar *message,
	const void *userParam)
{
	// if 'GL_DEBUG_OUTPUT_SYNCHRONOUS' is enabled, you can place a
	// breakpoint here and the callstack should reflect the problem location!
	// e.g. glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	std::cerr << message << std::endl;
}

struct Vertex
{
	float x, y, z, w;
	float nx, ny, nz, nw;
	float tx, ty, tz, tw;
	float s, t;
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
			sizeof(FBXVertex), 0);

		glEnableVertexAttribArray(1); // texcoord
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE,
			sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);

		glEnableVertexAttribArray(2); // normal
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE,
			sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);

		glEnableVertexAttribArray(3); // tangent
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_TRUE,
			sizeof(FBXVertex), ((char*)0) + FBXVertex::TangentOffset);

		glBindVertexArray(0);	// unbind VAO (this has to come first)
		glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind IBO

		mesh->m_userData = glData;
	}
}

void DemoApp::cleanupOpenGLBuffers(FBXFile* fbx)
{
	if (fbx == nullptr)
		return;

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

//void DemoApp::generateGrid(unsigned int rows, unsigned int cols)
//{
//	Vertex* aoVerts = new Vertex[rows * cols];
//	for (unsigned int r = 0; r < rows; ++r)
//	{
//		for (unsigned int c = 0; c < cols; ++c)
//		{
//			aoVerts[r * cols + c].position = vec4(
//				(float)c, 0, (float)r, 1);
//
//			vec3 color = vec3(sinf((c / (float)(cols - 1))*(r / (float)(rows - 1))));
//			aoVerts[r*cols + c].color = vec4(color, 1);
//		}
//	}
//
//	unsigned int* auiIndices = new unsigned int[(rows - 1) * (cols - 1) * 6];
//
//	unsigned int index = 0;
//	for (unsigned int r = 0; r < (rows - 1); ++r)
//	{
//		for (unsigned int c = 0; c < (cols - 1); ++c)
//		{
//			//Tri 1
//			auiIndices[index++] = r * cols + c;
//			auiIndices[index++] = (r + 1) * cols + c;
//			auiIndices[index++] = (r + 1) * cols + (c + 1);
//
//			//Tri 2
//			auiIndices[index++] = r * cols + c;
//			auiIndices[index++] = (r + 1) * cols + (c + 1);
//			auiIndices[index++] = r * cols + (c + 1);
//		}
//	}
//
//	glGenBuffers(1, &VBO);
//	glGenBuffers(1, &IBO);
//	glGenVertexArrays(1, &VAO);
//
//	glBindVertexArray(VAO);
//
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, (rows * cols) * sizeof(Vertex),
//		aoVerts, GL_STATIC_DRAW);
//
//	glEnableVertexAttribArray(0);
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
//	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
//		(void*)(sizeof(vec4)));
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (cols - 1) * 6 * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);
//
//	glBindVertexArray(0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//	indexCount = (rows - 1) * (cols - 1) * 6;
//
//	delete[] aoVerts;
//	delete[] auiIndices;
//}

void DemoApp::generateQuad()
{
	float vertexData[] = {
	// Position		  // ST (or UVs)
	//   X, Y, Z, W   S, T
		-5, 0, -5, 1, 0, 0, // Vertex 1
		 5, 0, -5, 1, 1, 0, // Vertex 2
		 5, 10,-5, 1, 1, 1, // Vertex 3
		-5, 10,-5, 1, 0, 1, // Vertex 4
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	// Position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);

	glEnableVertexAttribArray(1);	// TexCoord
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (GLvoid *)(sizeof(float) * 4));

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
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	window = glfwCreateWindow(800, 600, "Title", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) {
		glfwDestroyWindow(window);
		glfwTerminate();
		return -3;
	}

	if (glDebugMessageCallback)
	{
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(oglErrorDefaultCallback, nullptr);

		GLuint unusedIDs = 0;
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIDs, true);
	}

	// START RENDERING CODE ------------------

	// vertex shader
	const char* vsSource = "#version 410\n \
							layout(location=0) in vec4 Position; \
							layout(location=1) in vec2 TexCoord; \
							out vec2 vTexCoord; \
							uniform mat4 ProjectionView; \
							void main() { \
							vTexCoord = TexCoord; \
							gl_Position= ProjectionView * Position;}";


	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);

	// fragment shader
	const char* fsSource =  "#version 410\n \
							in vec2 vTexCoord; \
							out vec4 FragColor; \
							uniform sampler2D diffuse; \
							void main() { \
							FragColor = texture(diffuse, vTexCoord);}";

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

	// loading textures -------------------------
	using glm::vec3;
	using glm::vec4;
	using glm::mat4;

	int imageWidth = 0;
	int imageHeight = 0;
	int imageFormat = 0;

	//Load Diffuse mapping
	unsigned char* data = stbi_load("../resources/FBX/soulspear/soulspear_diffuse.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	//Load Normal Mapping
	data = stbi_load("../resources/FBX/soulspear/soulspear_normal.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &normalmap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalmap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	// generate FBO
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// generate and bind a texture for the FBO
	glGenTextures(1, &fboTexture);
	glBindTexture(GL_TEXTURE_2D, fboTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 512, 512);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fboTexture, 0);

	// generate and bind a depth texture for the FBO
	glGenRenderbuffers(1, &fboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, fboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fboDepth);

	// assign attachments to the FBO
	//  - attachments tell the FBO what textures to render the buffer onto
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	// validate/verify that the FBO is in working order
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer Error! U dun fucked up.\n");

	// unbind the FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Load meshes ---------------------
	//fbx = new FBXFile();
	//fbx->load("../resources/FBX/soulspear/soulspear.fbx");
	//createOpenGLBuffers(fbx);

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

	// delta time
	currentTime = glfwGetTime();
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	return true;
}

// - drawing meshes to the backbuffer
void DemoApp::draw()
{
	mat4 view = glm::lookAt(vec3(-10, -10, 10), vec3(0), vec3(0, 1, 0));
	mat4 projection = glm::perspective(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 1000.f);

	// DRAW TO FRAMEBUFFER-----------

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glViewport(0, 0, 512, 512);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::clear();
	Gizmos::addSphere(glm::vec3(0, 0, 0), 5.f, 15, 15, glm::vec4(0, 1, 1, 1));
	Gizmos::addTransform(glm::mat4(1));
	Gizmos::draw(projection * view);

	glUseProgram(projectID);
	int loc = glGetUniformLocation(projectID, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection * view));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalmap);

	loc = glGetUniformLocation(projectID, "diffuse");
	glUniform1i(loc, 0);

	loc = glGetUniformLocation(projectID, "normal");
	glUniform1i(loc, 1);

	glm::vec3 lightColor = {1.0f, 1.0f, 1.0f};

	loc = glGetUniformLocation(projectID, "LightColor");
	glUniform3fv(loc, 1, glm::value_ptr(lightColor));

	glm::vec3 lightDir = {0.0f, 1.0f, 0.0f};

	loc = glGetUniformLocation(projectID, "LightDir");
	glUniform3fv(loc, 1, glm::value_ptr(lightDir));

	glm::vec3 cameraPos = {0.3f, 0.3f, 0.3f};

	loc = glGetUniformLocation(projectID, "CameraPos");
	glUniform3fv(loc, 1, glm::value_ptr(cameraPos));

	glm::float1 specPow = (128.0f);

	loc = glGetUniformLocation(projectID, "SpecPow");
	glUniform1f(loc, 128.0f);

	//vec3 light(sin(glfwGetTime()), 1, cos(glfwGetTime()));
	//loc = glGetUniformLocation(program, "LightDir");
	//glUniform3f(loc, light.x, light.y, light.z);

	//glBindVertexArray(VAO);
	//glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

	if (fbx != nullptr)
	{
		for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
		{
			FBXMeshNode* mesh = fbx->getMeshByIndex(i);

			unsigned int* glData = (unsigned int*)mesh->m_userData;

			glBindVertexArray(glData[0]);
			glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
		}
	}

	// DRAW TO SCREEEEEEEEEEN-----------

	// bind the framebuffer of the screen (0)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);

	glClearColor(0.75f, 0.75f, 0.75f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::clear();
	Gizmos::addTransform(glm::mat4(1));
	Gizmos::draw(projection * view);

	glUseProgram(projectID);
	loc = glGetUniformLocation(projectID, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection * view));

	// bind the framebuffer texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fboTexture);
	loc = glGetUniformLocation(projectID, "diffuse");
	glUniform1i(loc, 0);

	// draw a quad with that texture
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

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
	if (fbx != nullptr)
		fbx->unload();
	Gizmos::destroy();
	glfwTerminate();
}