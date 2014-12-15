#include "stdafx.h"
#include "ParallelLight.h"


ParallelLight::ParallelLight(const Material& mat, const vec4& dir):
Light(mat), direction(normalize(dir))
{
	assert(dir.w == 0);
	vec4 temp(0,0,0,1);
	Face firstTriangle;
	firstTriangle.addVertex(temp);
	temp.x -= 0.5;
	firstTriangle.addVertex(temp);
	temp.y -= 0.25;
	firstTriangle.addVertex(temp);
	firstTriangle.calcNorm();
	faces.push_back(firstTriangle);

	temp = vec4(0, 0, 0, 1);
	Face secondTriangle;
	secondTriangle.addVertex(temp);
	temp.y -= 0.25;
	secondTriangle.addVertex(temp);
	temp.x -= 0.5;
	secondTriangle.addVertex(temp);
	secondTriangle.calcNorm();
	faces.push_back(secondTriangle);
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