#include "stdafx.h"
#include "ParallelLight.h"


ParallelLight::ParallelLight(const Material& mat, const vec4& dir):
Light(mat), direction(normalize(dir))
{
	assert(dir.w == 0);
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