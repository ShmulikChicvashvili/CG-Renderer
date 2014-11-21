#pragma once
#include "vec.h"
#include <vector>
#include "Vertex.h"

using std::vector;

class Face {
private:
	vector<Vertex> vertices;
public:
	const vector<Vertex>& getVertices() const;
	void addVertex(const vec4& v);
};

