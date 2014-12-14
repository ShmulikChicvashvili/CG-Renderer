#include "stdafx.h"
#include "Light.h"


Light::Light(const Material& mat) :material(mat)
{
}


Light::~Light()
{
}

Material& Light::getMatrial(){
	return material;
}

const Material& Light::getMaterial() const{
	return material;
}
