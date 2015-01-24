#include "stdafx.h"
#include "Light.h"


Light::Light(const Material& mat) : material(mat)
{

}


Light::~Light()
{
}

void Light::setMaterial(const Material& mat) {
	Model::setMaterial(mat);
	material = mat;
}

void Light::setRandomMaterial(){
	Material mat = randomizeMaterial(0.3, 0.5, 0.6);
	setMaterial(mat);
}


