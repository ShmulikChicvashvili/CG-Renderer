#pragma once
#include "Vertex.h"
#include <vector>

using std::vector;

class LitVertex :
	public Vertex
{
	vector<vec4> lightDirs;
	vec4 eyeVec;
public:
	LitVertex();
	//LitVertex(const vec4& _coords);
	~LitVertex();

	void addLightDir(const vec4&);
	void setEyeVec(const vec4&);

	const vector<vec4>& getLightDirs() const;
	const vec4& getEyeVec() const;
};
