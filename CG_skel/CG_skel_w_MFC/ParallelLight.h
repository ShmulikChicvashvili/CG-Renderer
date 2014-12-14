#pragma once
#include "Light.h"
class ParallelLight :
	public Light
{
	vec4 direction;
public:
	ParallelLight(const Material& mat, const vec4& dir);
	~ParallelLight();

	vec4 getDirection(const vec4& p) const;
};

