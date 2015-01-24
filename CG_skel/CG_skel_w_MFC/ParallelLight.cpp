#include "stdafx.h"
#include "ParallelLight.h"


ParallelLight::ParallelLight(const Material& mat, const vec4& dir, Renderer* _renderer) :
Light(mat, _renderer), direction(normalize(dir))
{
	assert(dir.w == 0);
	assert(length(dir) != 0);
	vec4 temp(-0.2, -0.1, 0, 1);
	Face firstTriangle;
	firstTriangle.addVertex(temp);
	temp.x = 0.2;
	firstTriangle.addVertex(temp);
	temp.y = 0.1;
	firstTriangle.addVertex(temp);
	
	firstTriangle.calcNorm();
	faces.push_back(firstTriangle);

	temp = vec4(0.2, 0.1, 0, 1);
	Face secondTriangle;
	secondTriangle.addVertex(temp);
	temp.x = -0.2;
	secondTriangle.addVertex(temp);
	temp.y = -0.1;
	secondTriangle.addVertex(temp);
	
	secondTriangle.calcNorm();
	faces.push_back(secondTriangle);

	const vec3& w = vec3(direction.x, direction.y, direction.z);
	const vec3& u = normalize(w.x != 0 ? vec3(w.y, -w.x, 0) : vec3(0, w.z, -w.y));
	const vec3& v = cross(w, u);

	assert(cmpFloat(length(u), 1));
	assert(cmpFloat(length(v), 1));
	assert(cmpFloat(length(w), 1));
	assert(cmpFloat(dot(u,v), 0));
	assert(cmpFloat(dot(u,w), 0));
	assert(cmpFloat(dot(w,v), 0));

	this->spinScaleMtx = mat4(	u.x, v.x, w.x, 0,
								u.y, v.y, w.y, 0,
								u.z, v.z, w.z, 0,
								0, 0, 0, 1);
	this->spinScaleInvMtx = transpose(this->spinScaleMtx);
}


ParallelLight::~ParallelLight()
{
}

vec4 ParallelLight::getDirectionFromPoint(const vec4& p) const{
	assert(direction.w == 0);
	assert(cmpFloat(length(direction), 1));
	return -direction;
}

bool isParallelLight(const Light* p){
	return typeid(*p) == typeid(ParallelLight);
}