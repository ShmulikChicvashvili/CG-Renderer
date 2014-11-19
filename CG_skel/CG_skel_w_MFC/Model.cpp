#include "stdafx.h"
#include "Model.h"

GLfloat deg2radian(const GLfloat deg){
	return deg * M_PI / 180;
}

const mat4 Model::getModelMatrix() const{
	return rotateTranslateMtx * spinScaleMtx;
}

const vector<Face>& Model::getFaces() const{
	return faces;
}

void Model::spin(const GLfloat deg, const Axes axis){
	GLfloat teta = deg2radian(deg);
	GLfloat cosTeta = cos(teta);
	GLfloat sinTeta = sin(teta);


	switch (axis){
	case X: 
	}
}

void Model::scale(const GLfloat sx, const GLfloat sy, const GLfloat sz){
	mat4 scaleMatrix(sx, 0, 0, 0,
					0, sy, 0, 0,
					0, 0, sz, 0,
					0, 0, 0, 1);
	spinScaleMtx = scaleMatrix * spinScaleMtx;
	std::cout << "Scale matrix: " << scaleMatrix << std::endl;
	std::cout << "Current Spin Scale matrix: " << spinScaleMtx << std::endl;
}

void Model::translate(const GLfloat tx, const GLfloat ty, const GLfloat tz){
	mat4 transMatrix	(1, 0, 0, tx,
						0, 1, 0, ty,
						0, 0, 1, tz,
						0, 0, 0, 1);
	rotateTranslateMtx = transMatrix * rotateTranslateMtx;
	std::cout << "Translation matrix: " << transMatrix << std::endl;
	std::cout << "Current Rotation Translation matrix: " << rotateTranslateMtx << std::endl;
}

void Model::rotate(const GLfloat teta, const Axes axis){

}