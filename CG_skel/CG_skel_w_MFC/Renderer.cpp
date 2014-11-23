#include "StdAfx.h"
#include "Renderer.h"
#include "CG_skel_w_MFC.h"
#include "InitShader.h"
#include "GL\freeglut.h"
#include <assert.h>


#define INDEX(width,x,y,c) (x+y*width)*3+c

Renderer::Renderer() :m_width(512), m_height(512)
{
	InitOpenGLRendering();
	initial_width = 512;
	initial_height = 512;
	CreateBuffers(512, 512);
	drawNormals = false;
	drawFaceNorms = false;
}
Renderer::Renderer(int width, int height) : m_width(width), m_height(height)
{
	InitOpenGLRendering();
	initial_width = width;
	initial_height = height;
	CreateBuffers(width, height);
	drawNormals = false;
	drawFaceNorms = false;
}

Renderer::~Renderer(void)
{
}



void Renderer::CreateBuffers(int width, int height)
{
	m_width = width;
	m_height = height;
	CreateOpenGLBuffer(); //Do not remove this line.
	m_outBuffer = new float[3 * m_width*m_height];
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

void Renderer::reshape(int width, int height){
	resizingMatrix = mat4((float) initial_width / width, 0, 0, 0,
						  0, (float) initial_height / height, 0, 0,
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

// The algorith for drawing line that is used is Bresenham's line algorithm
void Renderer::drawLine(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, Color c) {
	//InitializeBuffer();
	const bool highOctant = (fabs(y2 - y1) > fabs(x2 - x1));
	if (highOctant == true) {
		// switch between x and y
		std::swap(x1, y1);
		std::swap(x2, y2);
	}
	if (x1 > x2) {
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	const GLint dx = x2 - x1;
	const GLint dx2 = 2 * dx;
	const GLint dy2 = 2 * fabs(y2 - y1);

	GLint d = dy2 - dx;
	const GLint ystep = (y1 < y2) ? 1 : -1;
	GLint y = (GLint)y1;

	const GLint Xmax = (GLint)x2;

	for (GLint x = (GLint)x1; x <= Xmax; x++) {

		if (highOctant == true) {
			drawSinglePixel(y, x, c);
		}
		else {
			drawSinglePixel(x, y, c);
		}

		d += dy2;
		if (d >= 0) {
			y += ystep;
			d -= dx2;
		}

	}
}

void Renderer::drawSinglePixel(GLint x, GLint y, Color c) {
	if (!(x >= 0 && x < m_width && y >= 0 && y < m_height)){
		return;
	}
	m_outBuffer[INDEX(m_width, x, y, 0)] = c.getRed();
	m_outBuffer[INDEX(m_width, x, y, 1)] = c.getGreen();
	m_outBuffer[INDEX(m_width, x, y, 2)] = c.getBlue();
}

void Renderer::setBuffer(const vector<shared_ptr<Model>>& models, const Camera& cam) {
	//@TODO: Delete it later
	cout << "The size of models is : " << models.size() << endl;
	const mat4& viewMtx = cam.getViewMatrix(); 
	const mat4& projMtx = cam.getProjectionMatrix() * resizingMatrix;
	
	const mat4& normViewMtx = cam.getViewNormalMatrix();
	//const mat4& vpMtx = pMtx * vMtx;
	for each (const shared_ptr<Model>& pModel in models)
	{
		bool active = pModel->getActive();
		const vector<Face>& modelFaces = pModel->getFaces();

		const mat4& modelViewMtx = viewMtx * pModel->getModelMatrix();

		Color c = active ? Color(1, 1, 0) : Color(1, 1, 1);
		for each (const Face& face in modelFaces)
		{
			if (face.hasNormal() && face.hasMidPoint()) {
				drawFaceNormal(face.getNorm(), face.getMidPoint(), normViewMtx * pModel->getModelNormalMatrix(), modelViewMtx, projMtx);
			}
			drawFace(face,normViewMtx * pModel->getModelNormalMatrix(), modelViewMtx, projMtx, projMtx * modelViewMtx,  c);
		}
	}
}

void Renderer::drawFaceNormal(const vec4& norm, const vec4& midPoint, const mat4& normModelViewMtx, const mat4& modelViewMtx, const mat4& projMtx) {
		vec4 normCords = normModelViewMtx * norm;
		normCords.w = 0;
		normCords = 0.1 * normalize(normCords);

		const vec4& midPointCords = modelViewMtx * midPoint;
		const vec4& endNormViewCoords = vec4(midPointCords.x + normCords.x, midPointCords.y + normCords.y, midPointCords.z + normCords.z, 1);
		const vec3& endNormWindowCoords = windowCoordinates(divideByW(projMtx * endNormViewCoords));
		const vec3& midPointWindowCords = windowCoordinates(divideByW(projMtx * midPointCords));

		drawLine(endNormWindowCoords.x, endNormWindowCoords.y, midPointWindowCords.x, midPointWindowCords.y, Color(0.5, 0.5, 0.5));
}

const vec3 Renderer::normalNDC2Window(const vec4& n) const{
	vec4& ndc = mat4((GLfloat)2 / m_width, 0, 0, 0,
		0, (GLfloat)2 / m_height, 0, 0,
		0, 0, 2, 0,
		-1, -1, -1, 1) * n;
	return vec3(ndc.x, ndc.y, ndc.z);

}


void Renderer::drawFace(const Face& face, const mat4& normModelViewMtx, const mat4& modelViewMtx, const mat4& projMtx, const mat4& mvpMtx, Color c){
	vec3* windowCords = new vec3[face.getVertices().size()];
	const vector<Vertex>& vertices = face.getVertices();
	for (int i = 0; i < vertices.size(); i++) {
		windowCords[i] = windowCoordinates(divideByW(mvpMtx * vertices[i].getCoords()));

		if (drawNormals &&vertices[i].hasNormal()) {
			vec4 normCords = normModelViewMtx * vertices[i].getNorm();
			normCords.w = 0;
			normCords = 0.1 * normalize(normCords);

			const vec4& viewPointCords = modelViewMtx * vertices[i].getCoords();
			const vec4& endNormViewCoords = vec4(viewPointCords.x + normCords.x, viewPointCords.y + normCords.y, viewPointCords.z + normCords.z, 1);
			const vec3& endNormWindowCoords = windowCoordinates(divideByW(projMtx * endNormViewCoords));

			drawLine(windowCords[i].x,
				windowCords[i].y, endNormWindowCoords.x, endNormWindowCoords.y, Color(1, 0, 0));

		}
	}
	for (int i = 0; i < face.getVertices().size(); i++) {
		vec3& windowCordsFirstPoint = windowCords[i];
		vec3& windowCordsSecondPoint = windowCords[(i + 1) % face.getVertices().size()];
		drawLine(windowCordsFirstPoint.x, windowCordsFirstPoint.y, windowCordsSecondPoint.x, windowCordsSecondPoint.y,c);
	}
	delete windowCords;
}



const vec3 Renderer::windowCoordinates(const vec3& vector) const {
	GLfloat halfWidth = m_width / 2;
	GLfloat halfHeight = m_height / 2;
	return vec3((halfWidth*vector.x) + halfWidth, (halfHeight*vector.y) + halfHeight, (vector.z / 2) + 0.5);
}


bool Renderer::getDrawNormals() {
	return this->drawNormals;
}
void Renderer::setDrawNormals(const bool drawNormals) {
	this->drawNormals = drawNormals;
}

bool Renderer::getDrawFaceNormals() {
	return this->drawFaceNorms;
}

bool Renderer::setDrawFaceNormals(const bool drawFaceNorms) {
	this->drawFaceNorms = drawFaceNorms;
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

	int a = glGetError();
	glActiveTexture(GL_TEXTURE0);
	a = glGetError();
	glBindTexture(GL_TEXTURE_2D, gScreenTex);
	a = glGetError();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_FLOAT, m_outBuffer);
	glGenerateMipmap(GL_TEXTURE_2D);
	a = glGetError();

	glBindVertexArray(gScreenVtc);
	a = glGetError();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	a = glGetError();
	glutSwapBuffers();
	a = glGetError();
}