#include "stdafx.h"
#include "PointLight.h"


PointLight::PointLight(const Material& mat, const vec4& p) :
Light(mat), point(p)
{
	vec4 temp = p;
	Face firstTriangle;
	firstTriangle.addVertex(temp);
	temp.x -= 0.1;
	firstTriangle.addVertex(temp);
	temp.y -= 0.1;
	firstTriangle.addVertex(temp);
	faces.push_back(firstTriangle);

	temp = p;
	Face secondTriangle;
	secondTriangle.addVertex(temp);
	temp.y -= 0.1;
	secondTriangle.addVertex(temp);
	temp.x -= 0.1;
	secondTriangle.addVertex(temp);
	faces.push_back(secondTriangle);
}


PointLight::~PointLight()
{
}

vec4 PointLight::getDirectionFromPoint(const vec4& p) const{
	assert(p.w == 1);
	assert(point.w == 1);

	return normalize(point - p);
}

bool isPointLight(const Light* p){
	return typeid(*p) == typeid(PointLight);
}