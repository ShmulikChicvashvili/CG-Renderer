#include "stdafx.h"
#include "PointLight.h"


PointLight::PointLight(const Material& mat, const vec4& p) :
Light(mat), point(p)
{
}


PointLight::~PointLight()
{
}

vec4 PointLight::getDirection(const vec4& p) const{
	assert(p.w == 0);
	assert(point.w == 0);

	return normalize(point - p);
}