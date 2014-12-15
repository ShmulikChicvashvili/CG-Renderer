#pragma once
#include "vec.h"
#include <assert.h>

class Material{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

public:
	Material() : ambient(1, 1, 1, 1), diffuse(1, 1, 1, 1), specular(1, 1, 1, 1){}
	void setAmbient(const vec4& ambient) {
		this->ambient = ambient;
	}
	void setDiffuse(const vec4& diffuse) {
		this->diffuse = diffuse;
	}
	void setSpecular(const vec4& specular) {
		this->specular = specular;
	}
	const vec4& getAmbient() const {
		return ambient;
	}
	const vec4& getDiffuse() const {
		return diffuse;
	}
	const vec4& getSpecular() const {
		return specular;
	}
};

class Vertex
{
	vec4 coords;
	vec4 norm;
	bool hasNorm;

	Material material;

	bool isValidNormal(const vec4& v){
		return length(vec3(coords.x, coords.y, coords.z)) != 0;
	}
public:
	virtual ~Vertex(){}
	
	Vertex(const vec4& _coords) : coords(_coords), norm(), hasNorm(false){}
	Vertex(const vec4& _coords, const vec4& _norm) : coords(_coords), norm(_norm), hasNorm(true) { assert(isValidNormal(coords)); }

	void setCoords(const vec4& _coords);
	bool setNorm(const vec4& v);

	void setMaterial(const Material& mat);
	const Material& getMaterial() const;


	const vec4& getCoords() const;
	const vec4& getNorm() const;
	bool hasNormal() const;
};

