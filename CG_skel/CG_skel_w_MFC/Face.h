#pragma once
#include "vec.h"
#include <vector>
#include "Vertex.h"

using std::vector;

class Face {
private:
	vector<Vertex> vertices;
	vec4 midPoint;
	vec4 norm;

	bool hasMid;
	bool hasNorm;

public:
	vector<Vertex>& getVertices();
	const vector<Vertex>& getVertices() const;
	void addVertex(const vec4& v);
	void addVertex(const vec4& v, const vec4& n);
	void addVertex(const Vertex& v);

	bool hasMidPoint() const;
	bool hasNormal() const;
	void calcMidPoint();
	void calcNorm();
	void calcTangents();

	const vec4& getMidPoint() const;
	const vec4& getNorm() const;

	void setMaterial(const Material& mat);

};

