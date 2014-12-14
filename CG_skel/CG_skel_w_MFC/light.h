#pragma once
#include "Model.h"
class Light :
	public Model
{
	Material material;

public:
	Light(const Material& mat);
	virtual ~Light();

	virtual vec4 getDirectionFromPoint(const vec4& p) const = 0;

	Material& getMatrial() { return material; }
	const Material& getMaterial() const { return material; }
};

