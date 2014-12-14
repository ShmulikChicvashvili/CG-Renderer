#pragma once
#include "vec.h"
#include <assert.h>

class Material{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

public:
	Material() : ambient(1, 1, 1), diffuse(1, 1, 1), specular(1, 1, 1){}
	void setAmbient(const vec3&);
	void setDiffuse(const vec3&);
	void setSpecular(const vec3&);
	const vec3& getAmbient() const;
	const vec3& getDiffuse() const;
	const vec3& getSpecular() const;
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

	bool setNorm(const vec4& v);

	Material& getMatrial();
	const Material& getMaterial() const;

	const vec4& getCoords() const;
	const vec4& getNorm() const;
	bool hasNormal() const;
};

