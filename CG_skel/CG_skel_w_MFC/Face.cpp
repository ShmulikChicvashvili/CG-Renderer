#include "stdafx.h"
#include "Face.h"

const vector<Vertex>& Face::getVertices() const{
	return vertices;
}

void Face::addVertex(const vec4& v){
	vertices.push_back(Vertex(v));
	hasMid = false;
	hasNorm = false;
}

void Face::addVertex(const vec4& v, const vec4& n){
	vertices.push_back(Vertex(v, n));
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
	if (vertices.size() != 3){
		std::cout << "Tried to calc normal for non triangle face" << std::endl;
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