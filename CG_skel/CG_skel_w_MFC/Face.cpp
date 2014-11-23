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

}
