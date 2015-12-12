#include "Application.h"


void RenderingApp::createOpenGLBuffers(FBXFile* fbx)
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
	createOpenGLBuffers(fbx);

	cleanupOpenGLBuffers(fbx);

	glDeleteProgram(program);
}

void RenderingApp::cleanupOpenGLBuffers(FBXFile* fbx)
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

// load up the FBX file
// load up the shaders
bool RenderingApp::init()
{
	glfwInit();

	window = glfwCreateWindow(800, 600, "Title", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) {
		glfwDestroyWindow(window);
		glfwTerminate();
		return -3;
	}

	const char* vsSource = "#version 410\n \
						layout(location=0) in vec4 Position; \
						layout(location=1) in vec4 Normal; \
						out vec4 vNormal; \
						uniform mat4 ProjectionView; \
						void main() { vNormal = Normal; \
						gl_Position = ProjectionView * Position; }";

	const char* fsSource = "#version 410\n \
						in vec4 vNormal; \
						out vec4 FragColor; \
						void main() {vec4(1,1,1,1);}";

	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	projectID = glCreateProgram();
	glAttachShader(projectID, vertexShader);
	glAttachShader(projectID, fragmentShader);
	glLinkProgram(projectID);

	fbx = new FBXFile();
	fbx->load("../resources/FBX/soulspear/soulspear.fbx");
	createOpenGLBuffers(fbx);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return true;
}

// any calculations
bool RenderingApp::update()
{
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

	return true;
}

// draw the FBXFile
void RenderingApp::draw()
{
	glUseProgram(projectID);
	int loc = glGetUniformLocation(projectID, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(camera.getProjectionView()[0][0]));

	//Bind vertex array object and draw the stuff
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
}

// unload the FBX file
// unload the shaders
void RenderingApp::exit()
{
	fbx->unload();
}