#include "Application.h"

#include <aie\Gizmos.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master\stb_image.h"

using namespace glm;

struct Vertex
{
	vec4 position;
	vec4 color;
};

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

	//glBindVertexArray(VAO);
	//unsigned int indexCount = (rows - 1) * (cols - 1) * 6;
	//glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	indexCount = (rows - 1) * (cols - 1) * 6;

	delete[] aoVerts;
	delete[] auiIndices;
}

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

	const char* vsSource = "#version 410\n \
						layout(location=0) in vec4 Position; \
						layout(location=1) in vec2 TexCoord; \
						out vec4 vTexCoord; \
						uniform mat4 ProjectionView; \
						void main() { vTexCoord = TexCoord \
						gl_Position = ProjectionView * Position; \}";

	const char* fsSource = "#version 410\n \
						in vec2 vTexCoord; \
						out vec4 FragColor; \
						uniform sampler2D diffuse; \
						void main() {FragColor = texture(diffuse vTexcoord);}";

	using glm::vec3;
	using glm::vec4;
	using glm::mat4;

	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	int imageWidth = 0;
	int imageHeight = 0;
	int imageFormat = 0;

	unsigned char* data = stbi_load("./data/textures/crate.png", &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	projectID = glCreateProgram();
	glAttachShader(projectID, vertexShader);
	glAttachShader(projectID, fragmentShader);
	glLinkProgram(projectID);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

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

	generateGrid(10, 10);

	return true;
}

bool DemoApp::update()
{


	glClearColor(0.25f, 0.25f, 0.25f, 1);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	if (glfwWindowShouldClose(window) == true ||
		glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		return false;
	}
	currentTime = glfwGetTime();
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	return true;
}

void DemoApp::draw()
{
	mat4 view = glm::lookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	mat4 projection = glm::perspective(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 1000.f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::clear();

	Gizmos::addTransform(glm::mat4(1));

	vec4 white(1);
	vec4 black(0, 0, 0, 1);

	//for (int i = 0; i < 21; ++i)
	//{
	//	Gizmos::addLine(vec3(-10 + i, 0, 10),
	//		vec3(-10 + i, 0, -10),
	//		i == 10 ? white : black);

	//	Gizmos::addLine(vec3(10, 0, -10 + i),
	//		vec3(-10, 0, -10 + i),
	//		i == 10 ? white : black);
	//}

	Gizmos::draw(projection * view);

	glUseProgram(projectID);
	unsigned int projectionViewUniform = glGetUniformLocation(projectID, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(projection * view));

	/*unsigned int timeUniform = glGetUniformLocation(projectID, "time");
	glUniform1f(timeUniform, currentTime);

	unsigned int heightUniform = glGetUniformLocation(projectID, "heightScale");
	glUniform1f(heightUniform, 3);*/

	glBindVertexArray(VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);


	glfwSwapBuffers(window);

	glfwPollEvents();
}

void DemoApp::exit()
{
	Gizmos::destroy();
	glfwTerminate();
}