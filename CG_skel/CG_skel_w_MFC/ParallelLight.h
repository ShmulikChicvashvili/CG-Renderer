#pragma once
#include "Light.h"
class ParallelLight :
	public Light
{
	vec4 direction;
public:
	ParallelLight(const Material& mat, const vec4& dir);
	~ParallelLight();

	const vec4& getDirection() const { return direction; }

	vec4 getDirectionFromPoint(const vec4& p) const;
};

bool isParallelLight(const Light* p);