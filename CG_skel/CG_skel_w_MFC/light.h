#pragma once
#include "Model.h"
class light :
	public Model
{
	vec3 ka;
	vec3 kd;
	vec3 ks;

public:
	light();
	virtual ~light();
};

