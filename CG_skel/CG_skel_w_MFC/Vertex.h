#pragma once
#include "vec.h"
#include <assert.h>

class Material{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

public:
	Material() : ambient(1, 1, 1), diffuse(1, 1, 1), specular(1, 1, 1){}
	void setAmbient(const vec3& ambient) {
		this->ambient = ambient;
	}
	void setDiffuse(const vec3& diffuse) {
		this->diffuse = diffuse;
	}
	void setSpecular(const vec3& specular) {
		this->specular = specular;
	}
	const vec3& getAmbient() const {
		return ambient;
	}
	const vec3& getDiffuse() const {
		return diffuse;
	}
	const vec3& getSpecular() const {
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

