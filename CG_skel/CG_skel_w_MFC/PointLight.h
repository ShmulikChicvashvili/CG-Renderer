#pragma once
#include "Light.h"
class PointLight :
	public Light
{
	vec4 point;
public:
	PointLight(const Material& mat, const vec4& p, Renderer* _renderer);
	~PointLight();

	const vec4& getPoint() const { return point; }

	vec4 getDirectionFromPoint(const vec4& p) const;
};

bool isPointLight(const Light* p);