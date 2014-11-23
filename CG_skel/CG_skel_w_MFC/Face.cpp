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
