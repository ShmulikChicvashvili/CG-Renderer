#include "stdafx.h"
#include "Face.h"
#include <exception>

vector<Vertex>& Face::getVertices(){
	return vertices;
}

const vector<Vertex>& Face::getVertices() const{
	return vertices;
}

void Face::addVertex(const vec4& v){
	addVertex(Vertex(v));
}

void Face::addVertex(const vec4& v, const vec4& n){
	addVertex(Vertex(v, n));
}

void Face::addVertex(const Vertex& v){
	vertices.push_back(v);
	hasMid = false;
	hasNorm = false;
}

void Face::calcMidPoint(){
	int num = vertices.size();
	if (num == 0){
		std::cout << "Tried to calc mid point for 0 point face" << std::endl;
		assert(false);
	}
	GLfloat x = 0;
	GLfloat y = 0;
	GLfloat z = 0;

	for (auto& v : vertices){
		x += v.getCoords().x;
		y += v.getCoords().y;
		z += v.getCoords().z;
	}

	midPoint = vec4(x / num, y / num, z / num, 1);
	hasMid = true;
}

void Face::calcNorm(){
	assert(vertices.size() == 3);
	if (vertices.size() != 3){
		std::cout << "Tried to calc normal for non triangle face" << std::endl;
		throw std::exception("Tried to calc normal for non triangle face");
	}
	const vec4& v0 = vertices[0].getCoords();
	const vec4& v1 = vertices[1].getCoords();
	const vec4& v2 = vertices[2].getCoords();

	assert(v0.w == 1 && v1.w == 1 && v2.w);

	const vec4& vec1 = v1 - v0;
	const vec4& vec2 = v2 - v1;

	norm = cross(vec1, vec2);
	norm.w = 0;

	hasNorm = length(norm) != 0;
	assert(hasNorm);
	if (hasNorm){
		for (auto& v : vertices){
			if (!v.hasNormal()){
				v.setNorm(norm);
			}
		}
	}
}

void Face::calcTangents(){
	assert(hasNorm);

	const vec4& v0 = vertices[0].getCoords();
	const vec4& v1 = vertices[1].getCoords();
	const vec4& v2 = vertices[2].getCoords();
		
	const vec2& uv0 = vertices[0].getTexCoords();
	const vec2& uv1 = vertices[1].getTexCoords();
	const vec2& uv2 = vertices[2].getTexCoords();

	const vec4& deltaPos1 = v1 - v0;
	const vec4& deltaPos2 = v2 - v0;

	const vec2& deltaUV1 = uv1 - uv0;
	const vec2& deltaUV2 = uv2 - uv0;

	float tmp = deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x;
	float r = 1;
	if (tmp != 0){
		r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	}
	const vec4& tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
	const vec4& bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

	assert(cmpFloat(tangent.w, 0));
	assert(cmpFloat(bitangent.w, 0));

	for (auto& v : vertices){
		v.setTangent(tangent);
		v.setBitangent(bitangent);
	}
}

const vec4& Face::getMidPoint() const{
	assert(hasMid);
	return midPoint;
}

const vec4& Face::getNorm() const{
	assert(hasNorm);
	return norm;
}

bool Face::hasMidPoint() const{
	return hasMid;
}

bool Face::hasNormal() const{
	return hasNorm;
}

void Face::setMaterial(const Material& mat){
	for (auto& v : vertices){
		v.setMaterial(mat);
	}
}