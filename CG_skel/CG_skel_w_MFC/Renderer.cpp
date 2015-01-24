#include "StdAfx.h"
#include "Renderer.h"
#include "CG_skel_w_MFC.h"
#include "InitShader.h"
#include "GL\freeglut.h"
#include <assert.h>
#include "PointLight.h"
#include "ParallelLight.h"
#include <algorithm>
#include <math.h>
#include "Vertex.h"

#include <time.h>


#define INDEX(width,x,y,c) (x+y*width)*3+c
#define INDEXZ(width,x,y) (x+y*width)

#define SPEC_SHININESS 3

void checkError()
{
	int a = glGetError();
	if (a != GL_NO_ERROR)
	{
		throw exception("An openGL error occured: " + a);
	}
}


Renderer::Renderer() :m_width(512), m_height(512), colorMethod(ColorMethod::FLAT)
{
	InitOpenGLRendering();
	initial_width = 512;
	initial_height = 512;
	CreateBuffers(512, 512);
	drawVertexNormals = false;
	drawFaceNorms = false;
}
Renderer::Renderer(int width, int height) : m_width(width), m_height(height), colorMethod(ColorMethod::FLAT)
{
	//InitOpenGLRendering();
	glViewport(0, 0, m_width, m_height);

	program = InitShader("vshader.glsl", "fshader.glsl");
	
	fillShaderParams();
}

Renderer::~Renderer(void)
{
}


void Renderer::fillShaderParams() {
	glUseProgram(program);
	checkError();
	shaderParams[ShaderParamName::V_POSITION] = ShaderParam(glGetAttribLocation(program, "vPosition"), 4);
	checkError();
	shaderParams[ShaderParamName::V_NORMAL] = ShaderParam(glGetAttribLocation(program, "vNormal"), 4);
	checkError();
	shaderParams[ShaderParamName::V_FACE_NORMAL] = ShaderParam(glGetAttribLocation(program, "vFaceNormal"), 4);
	checkError();
	shaderParams[ShaderParamName::U_MODELVIEW_MTX] = ShaderParam(glGetUniformLocation(program, "uModelViewMtx"), 16);
	checkError();
	shaderParams[ShaderParamName::U_NORM_MODELVIEW_MTX] = ShaderParam(glGetUniformLocation(program, "uNormModelViewMtx"), 16);
	checkError();
	shaderParams[ShaderParamName::U_PROJ_MTX] = ShaderParam(glGetUniformLocation(program, "uProjMtx"), 16);
	checkError();
}

void Renderer::CreateBuffers(int width, int height)
{
	m_width = width;
	m_height = height;
	CreateOpenGLBuffer(); //Do not remove this line.
	m_outBuffer = new float[3 * m_width*m_height];
	m_zbuffer = new float[m_width*m_height];
}

void Renderer::SetDemoBuffer()
{
	//vertical line
	for (int i = 0; i < m_width; i++)
	{
		m_outBuffer[INDEX(m_width, 256, i, 0)] = 1;	m_outBuffer[INDEX(m_width, 256, i, 1)] = 0;	m_outBuffer[INDEX(m_width, 256, i, 2)] = 0;

	}
	//horizontal line
	for (int i = 0; i < m_width; i++)
	{
		m_outBuffer[INDEX(m_width, i, 256, 0)] = 1;	m_outBuffer[INDEX(m_width, i, 256, 1)] = 0;	m_outBuffer[INDEX(m_width, i, 256, 2)] = 1;

	}
}

/////////////////////////////////////////////////////
// Shmulik & Eyal stuff

void Renderer::setColorMethod(const ColorMethod& cm){
	colorMethod = cm;
}

void Renderer::reshape(int width, int height){
	resizingMatrix = mat4((float)initial_width / width, 0, 0, 0,
		0, (float)initial_height / height, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
	CreateBuffers(width, height);
}

void Renderer::InitializeBuffer() {
	for (int x = 0; x < m_width; x++) {
		for (int y = 0; y < m_height; y++) {
			m_outBuffer[INDEX(m_width, x, y, 0)] = 0;
			m_outBuffer[INDEX(m_width, x, y, 1)] = 0;
			m_outBuffer[INDEX(m_width, x, y, 2)] = 0;
		}
	}
}

bool Renderer::getDrawNormals() {
	return this->drawVertexNormals;
}
void Renderer::setDrawNormals(const bool drawNormals) {
	this->drawVertexNormals = drawNormals;
}

bool Renderer::getDrawFaceNormals() {
	return this->drawFaceNorms;
}

void Renderer::setDrawFaceNormals(const bool drawFaceNorms) {
	this->drawFaceNorms = drawFaceNorms;
}


GLuint Renderer::addModel(const vector<Face>& faces) {
	GLuint vao;
	map<ShaderParamName, GLuint> vbos;

	// First we arrange everything in vectors
	vector<vec4> vertices;
	vector<vec4> normals;
	vector<vec4> faceNormals;
	for (auto& f : faces) {
		for (auto& v : f.getVertices()) {
			vertices.push_back(v.getCoords());
			normals.push_back(v.getNorm());
			faceNormals.push_back(f.getNorm());
		}
	}

	// Now we will create the vbos
	GLuint buffers[NUMBER_VBOS];
	glGenBuffers(NUMBER_VBOS, buffers);

	// Vertices vbo
	vbos[ShaderParamName::V_POSITION] = buffers[VERTICES];
	glBindBuffer(GL_ARRAY_BUFFER, vbos[ShaderParamName::V_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	// Normals vbo
	vbos[ShaderParamName::V_NORMAL] = buffers[NORMALS];
	glBindBuffer(GL_ARRAY_BUFFER, vbos[ShaderParamName::V_NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * normals.size(), normals.data(), GL_STATIC_DRAW);

	// Face's normals vbo
	vbos[ShaderParamName::V_FACE_NORMAL] = buffers[FACE_NORMALS];
	glBindBuffer(GL_ARRAY_BUFFER, vbos[ShaderParamName::V_FACE_NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * faceNormals.size(), faceNormals.data(), GL_STATIC_DRAW);

	// Now we create the vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	for (auto it = vbos.begin(); it != vbos.end(); ++it) {
		ShaderParam& param = shaderParams.at(it->first);

		assert(param.id != -1 && it->second != -1);

		glEnableVertexAttribArray(param.id);
		checkError();
		cout << "Enabled attribute with id: " << param.id << endl;
		glBindBuffer(GL_ARRAY_BUFFER, it->second);
		checkError();
		cout << "Binded vbo with id: " << it->second << endl;
		glVertexAttribPointer(param.id, param.size, GL_FLOAT, GL_FALSE, 0, 0);
		checkError();
	}

	return vao;
}

void Renderer::setCamera(const mat4& viewMtx, const mat4& normViewMtx, const mat4& projMtx) {
	this->viewMtx = viewMtx;
	this->normViewMtx = normViewMtx;
	this->projMtx = projMtx;
}

void Renderer::drawModel(GLuint vao, int size, const mat4& modelMtx, const mat4& normModelMtx) const {
	glUseProgram(program);
	checkError();

	glUniformMatrix4fv(shaderParams.at(ShaderParamName::U_MODELVIEW_MTX).id, 1, GL_TRUE, viewMtx * modelMtx);
	checkError();
	glUniformMatrix4fv(shaderParams.at(ShaderParamName::U_NORM_MODELVIEW_MTX).id, 1, GL_TRUE, normViewMtx * normModelMtx);
	checkError();
	glUniformMatrix4fv(shaderParams.at(ShaderParamName::U_PROJ_MTX).id, 1, GL_TRUE, projMtx);
	checkError();

	glBindVertexArray(vao);
	checkError();

	glDrawArrays(GL_TRIANGLES, 0, size);
	checkError();
}

/////////////////////////////////////////////////////


/////////////////////////////////////////////////////
//OpenGL stuff. Don't touch.

void Renderer::InitOpenGLRendering()
{
	int a = glGetError();
	a = glGetError();
	glGenTextures(1, &gScreenTex);
	a = glGetError();
	glGenVertexArrays(1, &gScreenVtc);
	GLuint buffer;
	glBindVertexArray(gScreenVtc);
	glGenBuffers(1, &buffer);
	const GLfloat vtc[] = {
		-1, -1,
		1, -1,
		-1, 1,
		-1, 1,
		1, -1,
		1, 1
	};
	const GLfloat tex[] = {
		0, 0,
		1, 0,
		0, 1,
		0, 1,
		1, 0,
		1, 1 };
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc) + sizeof(tex), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);
	GLint  vPosition = glGetAttribLocation(program, "vPosition");

	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0,
		0);

	GLint  vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		(GLvoid *) sizeof(vtc));
	glUniform1i(glGetUniformLocation(program, "texture"), 0);
	a = glGetError();
}

void Renderer::CreateOpenGLBuffer()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gScreenTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_width, m_height, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, m_width, m_height);
}

void Renderer::SwapBuffers()
{
	glutSwapBuffers();
	//int a = glGetError();
	//glActiveTexture(GL_TEXTURE0);
	//a = glGetError();
	//glBindTexture(GL_TEXTURE_2D, gScreenTex);
	//a = glGetError();
	//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_FLOAT, m_outBuffer);
	//glGenerateMipmap(GL_TEXTURE_2D);
	//a = glGetError();

	//glBindVertexArray(gScreenVtc);
	//a = glGetError();
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	//a = glGetError();
	//glutSwapBuffers();
	//a = glGetError();
}
