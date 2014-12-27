#pragma once
#include <vector>
#include "CG_skel_w_MFC.h"
#include "vec.h"
#include "mat.h"
#include "GL/glew.h"
#include "Model.h"
#include "Camera.h"
#include "Light.h"
#include <memory>
#include "LitVertex.h"


using namespace std;

enum class ColorMethod{
	FLAT, GOURAUD, PHONG
};

class Triangle{
	LitVertex vertices[3];
	bool drawWireframe;

public:
	LitVertex& operator[](const int i){
		return vertices[i];
	}

	const LitVertex& operator[](const int i) const{
		return vertices[i];
	}

	bool getDrawWireframe() const { return drawWireframe; }
	void setDrawWireframe(bool draw){ drawWireframe = draw; }
};

class Color {
	float red;
	float green;
	float blue;
public:
	Color() : red(0), green(0), blue(0) {}
	Color(float _red, float _green, float _blue) : red(_red), green(_green), blue(_blue) {}
	Color(const vec3& v) : red(v[0]), green(v[1]), blue(v[2]){}
	float getRed() const {
		return red;
	}
	float getGreen() const {
		return green;
	}
	float getBlue() const {
		return blue;
	}
};

class Renderer
{
	float *m_outBuffer; // 3*width*height
	float *m_zbuffer; // width*height
	int m_width, m_height;
	ColorMethod colorMethod;

	void CreateBuffers(int width, int height);
	void CreateLocalBuffer();

	//////////////////////////////
	// Shmulik & Eyal stuff

	void clipper(vector<Triangle>& triangles, const vector<shared_ptr<Light>>& lights);
	void zBuffer(const vector<Triangle>& polygons, const vector<shared_ptr<Light>>& lights);
	void setColor(const int x, const int y, const Triangle& t, const vector<shared_ptr<Light>>& lights, const float& u, const float& v, const float& w);
	vec4 reflect(const vec4& V1, const vec4& V2);
	vec3 calculateIlluminationIntensity(const Material& pixelMaterial, const Material& lightMaterial, 
		const vec4& lightDirection, const vec4& norm, const vec4& viewDirection);

	bool isClipped(const vector<vec4>& clipCords) const;
	//void drawFace(const Face& face, const mat4& normModelViewMtx, const mat4& modelViewMtx, const mat4& projMtx, const mat4& mvpMtx, Color c);
	//void drawFaceNormal(const vec4& norm, const vec4& midPoint, const mat4& normModelViewMtx, const mat4& modelViewMtx, const mat4& projMtx);

	//const vec3 normalNDC2Window(const vec4& n) const;
	const vec3 windowCoordinates(const vec3& vector) const;

	mat4 resizingMatrix;

	int initial_width, initial_height;

	bool drawVertexNormals;
	bool drawFaceNorms;

	//////////////////////////////

	//////////////////////////////
	// openGL stuff. Don't touch.

	GLuint gScreenTex;
	GLuint gScreenVtc;
	void CreateOpenGLBuffer();
	void InitOpenGLRendering();
	//////////////////////////////
public:
	Renderer();
	Renderer(int width, int height);
	~Renderer(void);
	void Init();
	//void DrawTriangles(const vector<vec3>* vertices, const vector<vec3>* normals=NULL);
	//void SetCameraTransform(const mat4& cTransform);
	//void SetProjection(const mat4& projection);
	//void SetObjectMatrices(const mat4& oTransform, const mat3& nTransform);
	void SwapBuffers();
	void ClearColorBuffer();
	void ClearDepthBuffer();
	void SetDemoBuffer();

	//////////////////////////////
	// Shmulik & Eyal stuff.

	void setColorMethod(const ColorMethod& cm);

	void reshape(int width, int height);
	void InitializeBuffer();
	void drawLine(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, Color c);
	void drawSinglePixel(GLint x, GLint y, Color c);
	void setBuffer(const vector<shared_ptr<Model>>& models, const Camera& cam, const vector<shared_ptr<Light>>& lights);

	bool getDrawNormals();
	void setDrawNormals(const bool drawNormals);
	bool getDrawFaceNormals();
	void setDrawFaceNormals(const bool drawFaceNorms);

	const bool getBarycentricCoordinates(const int x, const int y, const vec2& a, const vec2& b, const vec2&c, float& u, float& v, float& w) const;

	//////////////////////////////
};


