#pragma once
#include <vector>
#include "mat.h"
#include "Face.h"

using std::vector;

enum Axes {x, y, z};

class Model {
protected:
	vector<Face> faces;
	mat4 spinScaleTransform; // spinning and scaling are done in model spcae
	mat4 rotateTranslateTransform; // moves the spinned and scaled object in the world
	mat3 _normal_transform;

public:
	virtual ~Model() {}
	virtual const mat4 getModelMatrix() const;
	virtual const vector<Face>& getFaces() const;

	virtual void spin(const GLfloat teta, const Axes axis);
	virtual void scale(const GLfloat s, const Axes axis);
	virtual void translate(const GLfloat t, const Axes axis);
	virtual void rotate(const GLfloat teta, const Axes axis);
};