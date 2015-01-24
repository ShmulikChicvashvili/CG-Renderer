#include "stdafx.h"
#include "PointLight.h"


PointLight::PointLight(const Material& mat, const vec4& p) :
Light(mat), point(p)
{
	assert(p.w == 1);
	vec4 temp(-0.1, -0.1, 0, 1);
	Face firstTriangle;
	firstTriangle.addVertex(temp);
	temp.x = 0.1;
	firstTriangle.addVertex(temp);
	temp.y = 0.1;
	firstTriangle.addVertex(temp);
	
	firstTriangle.calcNorm();
	faces.push_back(firstTriangle);

	temp = vec4(0.1, 0.1, 0, 1);
	Face secondTriangle;
	secondTriangle.addVertex(temp);
	temp.x = -0.1;
	secondTriangle.addVertex(temp);
	temp.y = -0.1;
	secondTriangle.addVertex(temp);
	
	secondTriangle.calcNorm();
	faces.push_back(secondTriangle);

	translate(p.x, p.y, p.z);
}


PointLight::~PointLight()
{
}
 
vec4 PointLight::getDirectionFromPoint(const vec4& p) const{
	assert(p.w == 1);
	assert(point.w == 1);

	if (p == point){
		return vec4(0,0,0,0);
	}

	vec4& dir = point - p;

	return normalize(point - p);
}

bool isPointLight(const Light* p){
	return typeid(*p) == typeid(PointLight);
}