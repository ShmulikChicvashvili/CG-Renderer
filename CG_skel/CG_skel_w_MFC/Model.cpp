#include "stdafx.h"
#include "Model.h"
#include "png.h"
#include "PngWrapper.h"

#pragma import libpng.lib

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
	renderer->fillColorVBO(colorVbo, faces);
}

void Model::setRandomMaterial(){
	for (auto& f : faces){
		f.setMaterial(randomizeMaterial(0.2, 0.5, 0.6));
	}
	renderer->fillColorVBO(colorVbo, faces);
}

void Model::setActive(const bool isActive) {
	this->isActive = isActive;
}
bool Model::getActive() const {
	return this->isActive;
}

void Model::setRenderer(Renderer* renderer){
	this->renderer = renderer;
	for (auto& face : this->faces) {
		face.calcMidPoint();
	}
	renderer->addModel(getFaces(), vao, colorVbo);
}

void Model::setTexture(string textureFilename) {
	if (typeid(this) != typeid(Model)) {
		return;
	}

	PngWrapper png(textureFilename.c_str());
	if (!png.ReadPng()) {
		std::cout << "Couldnt read the texture file" << endl;
		return;
	}

	this->textureWidth = png.GetWidth();
	this->textureHeight = png.GetHeight();
	this->textureImg = new GLuint(textureWidth * textureHeight * 3);
	for (int x = 0; x < textureWidth; x++) {
		for (int y = 0; y < textureHeight; y++) {
			int pixelColor = png.GetValue(x, y);
			textureImg[INDEX(textureWidth, x, y, 0)] = GET_R(pixelColor);
			textureImg[INDEX(textureWidth, x, y, 1)] = GET_G(pixelColor);
			textureImg[INDEX(textureWidth, x, y, 2)] = GET_B(pixelColor);
		}
	}
}

void Model::draw() const{
	if (this->isActive) {
		renderer->drawActiveModel(vao, faces.size() * 3, getModelMatrix(), getModelNormalMatrix());
	}
	renderer->drawModel(vao, faces.size() * 3, getModelMatrix(), getModelNormalMatrix());
}

