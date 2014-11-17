#pragma once
#include "vec.h"
#include <vector>

using std::vector;

class Face {
private:
	vector<vec4> vertices;
public:
	const vector<vec4>& getVertices() const;
	void addVertex(const vec4 v);
};

