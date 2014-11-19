#include "stdafx.h"
#include "Model.h"

const mat4 Model::getModelMatrix() const{
	return rotateTranslateTransform * spinScaleTransform;
}

const vector<Face>& Model::getFaces() const{
	return faces;
}

void Model::spin(const GLfloat teta, const Axes axis){

}

void Model::scale(const GLfloat sx, const GLfloat sy, const GLfloat sz){

}

void Model::translate(const GLfloat tx, const GLfloat ty, const GLfloat tz){

}

void Model::rotate(const GLfloat teta, const Axes axis){

}