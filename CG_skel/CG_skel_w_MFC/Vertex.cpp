#include "stdafx.h"
#include "Vertex.h"

void Vertex::setCoords(const vec4& _coords){
	coords = _coords;
}

bool Vertex::setNorm(const vec4& v){
	if (!isValidNormal(v)){
		return false;
	}
	norm = v;
	hasNorm = true;
	return true;
}

void Vertex::setTangent(const vec4& v){
	tangent = v;
}

void Vertex::setBitangent(const vec4& v){
	bitangent = v;
}

void Vertex::setTexCoords(GLfloat x, GLfloat y){
	texCoords = vec2(x, y);
	hasSetTexCoords = true;
}

void Vertex::setTexCoords(const vec2& coords){
	setTexCoords(coords.x, coords.y);
}

void Vertex::setMaterial(const Material& mat){
	material = mat;
}

const Material& Vertex::getMaterial() const{
	return material;
}

const vec4& Vertex::getCoords() const{
	return coords;
}

const vec4& Vertex::getNorm() const{
	assert(hasNorm);
	return norm;
}

const vec4& Vertex::getTangent() const{
	return tangent;
}

const vec4& Vertex::getBitangent() const{
	return bitangent;
}

const vec2& Vertex::getTexCoords() const{
	assert(hasSetTexCoords);
	return texCoords;
}

bool Vertex::hasNormal() const{
	return hasNorm;
}

bool Vertex::hasTexCoords() const{
	return hasSetTexCoords;
}
