#include "stdafx.h"
#include "LitVertex.h"


LitVertex::LitVertex() : Vertex(vec4(0,0,0,1))
{
}


LitVertex::~LitVertex()
{
}

void LitVertex::addLightDir(const vec4& lightDir){
	//assert(lightDir.w == 0);
	lightDirs.push_back(lightDir);
}

void LitVertex::setEyeVec(const vec4& _eyeVec){
	assert(_eyeVec.w == 0);
	assert(length(_eyeVec) != 0);
	eyeVec = _eyeVec;
}

const vector<vec4>& LitVertex::getLightDirs() const{
	return lightDirs;
}

const vec4& LitVertex::getEyeVec() const{
	return eyeVec;
}