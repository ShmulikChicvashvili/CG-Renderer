#pragma once
#include "vec.h"
#include <assert.h>

class Material{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

public:
	Material(const vec3& a, const vec3& d, const vec3& s) : ambient(a), diffuse(d), specular(s){}
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

inline
Material operator*(const GLfloat& f, const Material& mat){
	return Material(f * mat.getAmbient(), f*mat.getDiffuse(), f*mat.getSpecular());
}

inline
Material randomizeMaterial(const GLfloat maxA, const GLfloat maxD, const GLfloat maxS){
	return Material(randomizeVec3(maxA), randomizeVec3(maxD), randomizeVec3(maxS));
}

inline
Material operator+(const Material& m1, const Material& m2){
	return Material(m1.getAmbient() + m2.getAmbient(), m1.getDiffuse() + m2.getDiffuse(), m1.getSpecular() + m2.getSpecular());
}

inline
Material operator-(const Material& m1, const Material& m2){
	return Material(m1.getAmbient() - m2.getAmbient(), m1.getDiffuse() - m2.getDiffuse(), m1.getSpecular() - m2.getSpecular());
}

class Vertex
{
	vec4 coords;
	vec4 norm;
	bool hasNorm;

	vec2 texCoords;
	bool hasSetTexCoords;

	Material material;

	bool isValidNormal(const vec4& v){
		return length(vec3(v.x, v.y, v.z)) != 0;
	}
public:
	virtual ~Vertex(){}
	
	Vertex(const vec4& _coords) : coords(_coords), norm(), hasNorm(false), hasSetTexCoords(false){}
	Vertex(const vec4& _coords, const vec4& _norm) : 
		coords(_coords), norm(_norm), hasNorm(true), hasSetTexCoords(false) { assert(isValidNormal(coords)); }

	void setCoords(const vec4& _coords);
	bool setNorm(const vec4& v);
	void setTexCoords(GLfloat x, GLfloat y);
	void setTexCoords(const vec2& coords);

	void setMaterial(const Material& mat);
	const Material& getMaterial() const;


	const vec4& getCoords() const;
	const vec4& getNorm() const;
	const vec2& getTexCoords() const;
	bool hasNormal() const;
	bool hasTexCoords() const;
};

