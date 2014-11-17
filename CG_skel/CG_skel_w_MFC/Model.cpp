#include "stdafx.h"
#include "Model.h"

const mat4 Model::getModelMatrix() const{
	return translation * rotation * scale;
}

const vector<Face>& Model::getFaces() const{
	return faces;
}