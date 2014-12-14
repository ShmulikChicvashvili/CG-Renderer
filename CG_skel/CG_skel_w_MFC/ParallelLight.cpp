#include "stdafx.h"
#include "ParallelLight.h"


ParallelLight::ParallelLight(const Material& mat, const vec4& dir):
Light(mat), direction(normalize(dir))
{
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