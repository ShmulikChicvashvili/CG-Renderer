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

bool Vertex::hasNormal() const{
	return hasNorm;
}
