#pragma once
#include <vector>
#include "mat.h"
#include "Face.h"

using std::vector;

enum Axes {X, Y, Z, ALL};

class Model {
protected:
	vector<Face> faces;
	mat4 spinScaleMtx; // spinning and scaling are done in model spcae
	mat4 rotateTranslateMtx; // moves the spinned and scaled object in the world
	mat3 _normal_transform;

	const mat4 genRotationMatrix(const GLfloat teta, const Axes axis) const;
	const mat4 genTranslationMatrix(const GLfloat tx, const GLfloat ty, const GLfloat tz) const;
	const mat4 genScaleMatrix(const GLfloat sx, const GLfloat sy, const GLfloat sz) const;

public:
	virtual ~Model() {}
	virtual const mat4 getModelMatrix() const;
	virtual const vector<Face>& getFaces() const;

	virtual void spin(const GLfloat deg, const Axes axis);
	virtual void scale(const GLfloat sx, const GLfloat sy, const GLfloat sz);
	virtual void translate(const GLfloat tx, const GLfloat ty, const GLfloat tz);
	virtual void rotate(const GLfloat deg, const Axes axis);
};