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

using namespace std;

#define INDEX(width,x,y,c) (x+y*width)*3+c
#define INDEXZ(width,x,y) (x+y*width)

#define NUMBER_VBOS 6
enum VBOIndex{
	VERTICES,
	NORMALS,
	FACE_NORMALS,
	MATERIALS,
	FACE_MID,
	TEXTURES
};

enum class ColorMethod{
	FLAT, GOURAUD, PHONG
};

enum class TextureType{
	NONE, COLOR, NORMAL
};

enum class ShaderParamName{
	V_POSITION,
	V_NORMAL,
	V_FACE_NORMAL,
	V_AMBIENT,
	V_DIFFUSE,
	V_SPECULAR,
	V_FACE_MID,
	V_TEX_COORDS,
	U_MODELVIEW_MTX,
	U_NORM_MODELVIEW_MTX,
	U_PROJ_MTX,
	U_NUM_LIGHTS,
	U_COLOR_METHOD,
	U_TEX_TYPE,
	U_TEX_MAP,
	U_CONST_COLOR,
	U_TOON,
	U_SILHOUETTE,
	U_TICKS,
	U_ANIMATION_COLOR
};

struct ShaderParam{
	GLint id;
	GLuint size;

	ShaderParam() : ShaderParam(0, 0){}
	ShaderParam(GLuint _id, GLuint _size) : id(_id), size(_size){}
};

struct RendererLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	mat4 modelMtx;
	boolean isPointLight;

	RendererLight(vec3 _ambient, vec3 _diffuse, vec3 _specular, mat4 _modelMtx, boolean _isPointLight) :
		ambient(_ambient), diffuse(_diffuse), specular(_specular), modelMtx(_modelMtx), isPointLight(_isPointLight) {}
};

class Renderer
{
	GLuint program;
	map<ShaderParamName, ShaderParam > shaderParams;
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

	boolean silhouette;
	boolean toon;

	//////////////////////////////

	void fillShaderParams();
	
	void setModelTransformations(GLuint vao, const mat4& modelMtx, const mat4& normModelMtx) const;
	void setTexture(GLuint tex, TextureType texType) const;
	void unsetTexture() const;
	void drawArrays(int size) const;
	void setConstColor(const boolean constColor) const;

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
	
	void addModel(const vector<Face>& faces, GLuint& vao, GLuint& colorVbo);
	void setCamera(const mat4& viewMtx, const mat4& normViewMtx, const mat4& projMtx);
	void drawModel(GLuint vao, int size, const mat4& modelMtx, const mat4& normModelMtx, GLuint tex, TextureType texType) const;
	void drawActiveModel(GLuint vao, int size, const mat4& modelMtx, const mat4& normModelMtx) const;
	void setLights(const vector<RendererLight>& lights);
	GLuint add2DTexture(GLubyte* texImg, int width, int height);
	void setSilhouette(const boolean silhouette);
	void setToon(const boolean toon);
	boolean getSilhouette();
	boolean getToon();

	void fillColorVBO(GLuint vbo, const vector<Face>& faces, bool test = false);
	//////////////////////////////
};


