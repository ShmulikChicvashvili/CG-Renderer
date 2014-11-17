#pragma once
#include <vector>
#include "mat.h"
#include "Face.h"

using std::vector;

class Model {
protected:
	vector<Face> faces;
	mat4 rotation;
	mat4 translation;
	mat4 scale;
	mat3 _normal_transform;

public:
	virtual ~Model() {}
	virtual const mat4 getModelMatrix() const;
	virtual const vector<Face>& getFaces() const;
};