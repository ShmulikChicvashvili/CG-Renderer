#pragma once
#include "vec.h"
#include <assert.h>

class Vertex
{
	vec4 coords;
	vec4 norm;
	bool hasNorm;

	bool isValidNormal(const vec4& v){
		return length(vec3(coords.x, coords.y, coords.z)) != 0;
	}
public:
	
	Vertex(const vec4& _coords) : coords(_coords), norm(), hasNorm(false){}
	Vertex(const vec4& _coords, const vec4& _norm) : coords(_coords), norm(_norm), hasNorm(true) { assert(isValidNormal(coords)); }

	bool setNorm(const vec4& v){
		if (!isValidNormal(v)){
			return false;
		}
		norm = v;
		hasNorm = true;
		return true;
	}

	const vec4& getCoords() const;
	const vec4& getNorm() const;
	bool hasNormal() const;
};

