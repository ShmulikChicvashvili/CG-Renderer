#include "stdafx.h"
#include "Model.h"

GLfloat deg2radian(const GLfloat deg){
	return (GLfloat)(deg * M_PI / 180);
}

const mat4 Model::genRotationMatrix(const GLfloat deg, const Axes axis) const{
	GLfloat teta = deg2radian(deg);
	GLfloat cosTeta = cos(teta);
	GLfloat sinTeta = sin(teta);
	
	switch (axis){
	case X: return mat4(1, 0, 0, 0,
						0, cosTeta, -sinTeta, 0,
						0, sinTeta, cosTeta, 0,
						0, 0, 0, 1);
	case Y: return mat4(cosTeta, 0, sinTeta, 0,
						0, 1, 0, 0,
						-sinTeta, 0, cosTeta, 0,
						0, 0, 0, 1);
	case Z: return mat4(cosTeta, -sinTeta, 0, 0,
						sinTeta, cosTeta, 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1);
	default: return mat4(); // We don't rotate in all axes
	}
}

const mat4 Model::genTranslationMatrix(const GLfloat tx, const GLfloat ty, const GLfloat tz) const{
	return mat4(1, 0, 0, tx,
				0, 1, 0, ty,
				0, 0, 1, tz,
				0, 0, 0, 1);
}

const mat4 Model::genScaleMatrix(const GLfloat sx, const GLfloat sy, const GLfloat sz) const{
	return mat4(sx, 0, 0, 0,
				0, sy, 0, 0,
				0, 0, sz, 0,
				0, 0, 0, 1);
}

const mat4 Model::getModelMatrix() const{
	return rotateTranslateMtx * spinScaleMtx;
}

const vector<Face>& Model::getFaces() const{
	return faces;
}

void Model::spin(const GLfloat deg, const Axes axis){
	const mat4& spinMatrix = genRotationMatrix(deg, axis);
	spinScaleMtx = spinMatrix * spinScaleMtx;
	std::cout << "Spin matrix: " << spinMatrix << std::endl;
	std::cout << "Current Spin Scale matrix: " << spinScaleMtx << std::endl;
}

void Model::scale(const GLfloat sx, const GLfloat sy, const GLfloat sz){
	const mat4& scaleMatrix = genScaleMatrix(sx, sy, sz);
	spinScaleMtx = scaleMatrix * spinScaleMtx;
	std::cout << "Scale matrix: " << scaleMatrix << std::endl;
	std::cout << "Current Spin Scale matrix: " << spinScaleMtx << std::endl;
}

void Model::translate(const GLfloat tx, const GLfloat ty, const GLfloat tz){
	const mat4& transMatrix = genTranslationMatrix(tx, ty, tz);
	rotateTranslateMtx = transMatrix * rotateTranslateMtx;
	std::cout << "Translation matrix: " << transMatrix << std::endl;
	std::cout << "Current Rotation Translation matrix: " << rotateTranslateMtx << std::endl;
}

void Model::rotate(const GLfloat deg, const Axes axis){
	const mat4& rotateMatrix = genRotationMatrix(deg, axis);
	rotateTranslateMtx = rotateMatrix * spinScaleMtx;
	std::cout << "Spin matrix: " << rotateMatrix << std::endl;
	std::cout << "Current Spin Scale matrix: " << spinScaleMtx << std::endl;
}