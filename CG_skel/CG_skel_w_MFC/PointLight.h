#pragma once
#include "Light.h"
class PointLight :
	public Light
{
	vec4 point;
public:
	PointLight(const Material& mat, const vec4& p);
	~PointLight();

	vec4 getDirection(const vec4& p) const;
};

