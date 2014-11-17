#include "stdafx.h"
#include "Face.h"

const vector<vec4>& Face::getVertices() const{
	return vertices;
}

void Face::addVertex(const vec4 v){
	vertices.push_back(v);
}
