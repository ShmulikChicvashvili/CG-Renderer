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

const mat4 Model::getModelNormalMatrix() const{
	return transpose(spinScaleInvMtx * rotateTranslateInvMtx);
}

const vector<Face>& Model::getFaces() const{
	return faces;
}

void Model::spin(const GLfloat deg, const Axes axis){
	const mat4& mtx = genRotationMatrix(deg, axis);
	spinScaleMtx = mtx * spinScaleMtx;
	spinScaleInvMtx = spinScaleInvMtx * transpose(mtx);

	//std::cout << "Spin matrix: " << spinMatrix << std::endl;
	//std::cout << "Current Spin Scale matrix: " << spinScaleMtx << std::endl;
}

void Model::scale(const GLfloat sx, const GLfloat sy, const GLfloat sz){
	spinScaleMtx = genScaleMatrix(sx, sy, sz) * spinScaleMtx;
	spinScaleInvMtx = spinScaleInvMtx * Model::genScaleMatrix(1 / sx, 1 / sy, 1 / sz);

	//std::cout << "Scale matrix: " << scaleMatrix << std::endl;
	//std::cout << "Current Spin Scale matrix: " << spinScaleMtx << std::endl;
}

void Model::translate(const GLfloat tx, const GLfloat ty, const GLfloat tz){
	rotateTranslateMtx = genTranslationMatrix(tx, ty, tz) * rotateTranslateMtx;
	rotateTranslateInvMtx = rotateTranslateInvMtx * genTranslationMatrix(-tx, -ty, -tz);

	//std::cout << "Translation matrix: " << transMatrix << std::endl;
	//std::cout << "Current Rotation Translation matrix: " << rotateTranslateMtx << std::endl;
}

void Model::rotate(const GLfloat deg, const Axes axis){
	const mat4& mtx = genRotationMatrix(deg, axis);
	rotateTranslateMtx = mtx * rotateTranslateMtx;
	rotateTranslateInvMtx = rotateTranslateInvMtx * transpose(mtx);

	/*std::cout << "Spin matrix: " << rotateMatrix << std::endl;
	std::cout << "Current Rotation Translation matrix: " << rotateTranslateMtx << std::endl;*/
}

void Model::reset(){
	const mat4 eye;
	spinScaleMtx = eye;
	spinScaleInvMtx = eye;
	rotateTranslateMtx = eye;
	rotateTranslateInvMtx = eye;
}

void Model::setMaterial(const Material& mat){
	for (auto& f : faces){
		f.setMaterial(mat);
	}
}

void Model::setRandomMaterial(){
	for (auto& f : faces){
		f.setMaterial(randomizeMaterial(0.2, 0.5, 0.6));
	}
}

void Model::setActive(const bool isActive) {
	this->isActive = isActive;
}
bool Model::getActive() const {
	return this->isActive;
}

void Model::setRenderer(Renderer* renderer){
	this->renderer = renderer;
	vao = renderer->addModel(getFaces());
}

void Model::draw() const{
	renderer->drawModel(vao, faces.size() * 3, getModelMatrix(), getModelNormalMatrix());
}

