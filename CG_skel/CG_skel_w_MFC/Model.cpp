#include "stdafx.h"
#include "Model.h"

const mat4 Model::getModelMatrix() const{
	return rotateTranslateMtx * spinScaleMtx;
}

const vector<Face>& Model::getFaces() const{
	return faces;
}

void Model::spin(const GLfloat teta, const Axes axis){

}

void Model::scale(const GLfloat sx, const GLfloat sy, const GLfloat sz){

}

void Model::translate(const GLfloat tx, const GLfloat ty, const GLfloat tz){
	mat4 transMatrix = mat4(1, 0, 0, tx,
							0, 1, 0, ty,
							0, 0, 1, tz,
							0, 0, 0, 1);
	rotateTranslateMtx = transMatrix * rotateTranslateMtx;
	std::cout << "Rotation matrix: " << transMatrix << std::endl;
	std::cout << "Current matrix: " << rotateTranslateMtx << std::endl;
}

void Model::rotate(const GLfloat teta, const Axes axis){

}