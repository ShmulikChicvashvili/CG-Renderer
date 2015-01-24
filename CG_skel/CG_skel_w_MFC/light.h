#pragma once
#include "Model.h"
class Light :
	public Model
{
	Material material;

public:
	Light(const Material& mat, Renderer* _renderer);
	virtual ~Light();

	virtual vec4 getDirectionFromPoint(const vec4& p) const = 0;

	void setMaterial(const Material& mat);

	void setRandomMaterial();

	const Material& getMaterial() const { return material; }
};

