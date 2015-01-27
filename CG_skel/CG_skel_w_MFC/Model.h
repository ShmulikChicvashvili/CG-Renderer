#pragma once
#include <vector>
#include "mat.h"
#include "Face.h"
#include "Renderer.h"

using std::vector;

enum Axes {X, Y, Z, ALL};

class Model {
private:
	GLuint vao;
	GLuint colorVbo;

	void initializeFaces();
protected:
	Renderer* renderer;
	GLubyte* texImg;
	int texWidth, texHeight;
	GLubyte texId;
	TextureType texType;

	Model() : renderer(NULL), texImg(NULL), texWidth(0), texHeight(0), texId(0), texType(TextureType::NONE) {}

	vector<Face> faces;

	mat4 spinScaleMtx; // spinning and scaling are done in model spcae
	mat4 spinScaleInvMtx;
	mat4 rotateTranslateMtx; // moves the spinned and scaled object in the world
	mat4 rotateTranslateInvMtx;
	
	//mat3 _normal_transform;

	const mat4 genRotationMatrix(const GLfloat deg, const Axes axis) const;
	const mat4 genTranslationMatrix(const GLfloat tx, const GLfloat ty, const GLfloat tz) const;
	const mat4 genScaleMatrix(const GLfloat sx, const GLfloat sy, const GLfloat sz) const;

	bool isActive;

public:
	virtual ~Model() {}
	const mat4 getModelMatrix() const;
	const mat4 getModelNormalMatrix() const;
	const vector<Face>& getFaces() const;

	virtual void reset();
	virtual void setMaterial(const Material& mat);
	virtual void setRandomMaterial();

	void spin(const GLfloat deg, const Axes axis);
	virtual void scale(const GLfloat sx, const GLfloat sy, const GLfloat sz);
	void translate(const GLfloat tx, const GLfloat ty, const GLfloat tz);
	void rotate(const GLfloat deg, const Axes axis);

	void setActive(const bool isActive);
	bool getActive() const;

	void setRenderer(Renderer* renderer);
	void setTexture(string textureFilename);
	void removeTexture();
	void draw();
};