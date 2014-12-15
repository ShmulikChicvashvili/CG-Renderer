#include "stdafx.h"
#include "ParallelLight.h"


ParallelLight::ParallelLight(const Material& mat, const vec4& dir):
Light(mat), direction(normalize(dir))
{
	vec4 temp = dir;
	Face firstTriangle;
	firstTriangle.addVertex(temp);
	temp.x -= 0.5;
	firstTriangle.addVertex(temp);
	temp.y -= 0.25;
	firstTriangle.addVertex(temp);
	faces.push_back(firstTriangle);

	temp = dir;
	Face secondTriangle;
	secondTriangle.addVertex(temp);
	temp.y -= 0.25;
	secondTriangle.addVertex(temp);
	temp.x -= 0.5;
	secondTriangle.addVertex(temp);
	faces.push_back(secondTriangle);
}


ParallelLight::~ParallelLight()
{
}

vec4 ParallelLight::getDirectionFromPoint(const vec4& p) const{
	assert(direction.w == 0);
	assert(length(direction) == 1);
	return -direction;
}

bool isParallelLight(const Light* p){
	return typeid(*p) == typeid(ParallelLight);
}