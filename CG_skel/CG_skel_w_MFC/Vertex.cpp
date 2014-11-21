#include "stdafx.h"
#include "Vertex.h"

bool Vertex::setNorm(const vec4& v){
	if (!isValidNormal(v)){
		return false;
	}
	norm = v;
	hasNorm = true;
	return true;
}

const vec4& Vertex::getCoords() const{
	return coords;
}

const vec4& Vertex::getNorm() const{
	return norm;
}

bool Vertex::hasNormal() const{
	return hasNorm;
}
