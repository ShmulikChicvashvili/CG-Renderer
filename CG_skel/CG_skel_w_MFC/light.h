#pragma once
#include "Model.h"
class light :
	public Model
{
	Material material;

public:
	light();
	virtual ~light();

	Material& getMatrial();
	const Material& getMaterial() const;
};

