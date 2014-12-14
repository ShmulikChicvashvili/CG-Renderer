#include "stdafx.h"
#include "PointLight.h"


PointLight::PointLight(const Material& mat, const vec4& p) :
Light(mat), point(p)
{
}


PointLight::~PointLight()
{
}

vec4 PointLight::getDirectionFromPoint(const vec4& p) const{
	assert(p.w == 1);
	assert(point.w == 1);

	return normalize(point - p);
}

bool isPointLight(const Light* p){
	return typeid(*p) == typeid(PointLight);
}