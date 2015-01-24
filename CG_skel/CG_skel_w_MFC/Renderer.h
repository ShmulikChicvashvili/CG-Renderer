#pragma once
#include <vector>
#include <map>
#include "CG_skel_w_MFC.h"
#include "vec.h"
#include "mat.h"
#include "GL/glew.h"
//#include "Model.h"
//#include "Camera.h"
//#include "Light.h"
#include <memory>
#include "Face.h"

#define NUMBER_VBOS 6
#define VERTICES 0
#define NORMALS 1
#define FACE_NORMALS 2

using namespace std;

enum class ColorMethod{
	FLAT, GOURAUD, PHONG
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

enum class ShaderParam{
	V_POSITION,
	V_NORMAL,
	V_FACE_NORMAL,
	U_MODELVIEW_MTX,
	U_NORM_MODELVIEW_MTX,
	U_PROJ_MTX
};

class Renderer
{
	GLuint program;
	map<ShaderParam, pair<GLuint, GLuint> > shaderParams;
	float *m_outBuffer; // 3*width*height
	float *m_zbuffer; // width*height
	int m_width, m_height;
	ColorMethod colorMethod;

	void CreateBuffers(int width, int height);
	void CreateLocalBuffer();

	//////////////////////////////
	// Shmulik & Eyal stuff

	mat4 resizingMatrix;

	int initial_width, initial_height;

	bool drawVertexNormals;
	bool drawFaceNorms;

	mat4 viewMtx;
	mat4 normViewMtx;
	mat4 projMtx;

	//////////////////////////////

	void fillShaderParams();

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

	void reshape(int width, int height);
	void InitializeBuffer();

	void setColorMethod(const ColorMethod& cm);

	bool getDrawNormals();
	void setDrawNormals(const bool drawNormals);
	bool getDrawFaceNormals();
	void setDrawFaceNormals(const bool drawFaceNorms);
	
	GLuint addModel(const vector<Face>& faces) const;
	void setCamera(const mat4& viewMtx, const mat4& normViewMtx, const mat4& projMtx);
	void drawModel(GLuint vao, const mat4& modelMtx, const mat4& normModelMtx) const;
	//////////////////////////////
};


