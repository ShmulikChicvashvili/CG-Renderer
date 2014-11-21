#include "stdafx.h"
#include "Camera.h"
#include <assert.h>

void Camera::LookAt(const vec4& eye, const vec4& at, const vec4& up) {
	const vec3 eyeNotHomogenic = divideByW(eye);
	const vec3 atNotHomogenic = divideByW(at);
	const vec3 upNotHomogenic = divideByW(up);
	const vec3 zAxis = normalize(eyeNotHomogenic - atNotHomogenic); // The forward vector
	const vec3 xAxis = normalize(cross(upNotHomogenic, zAxis)); // The right vector
	const vec3 yAxis = cross(zAxis, xAxis); // the upper vector
	this->spinScaleInvMtx = mat4(xAxis.x, xAxis.y, xAxis.z, 0,
		yAxis.x, yAxis.y, yAxis.z, 0,
		zAxis.x, zAxis.y, zAxis.z, 0,
		0, 0, 0, 1);
	this->spinScaleMtx = transpose(this->spinScaleInvMtx);
	assert(spinScaleMtx * spinScaleInvMtx == mat4());
	this->rotateTranslateInvMtx = mat4(1, 0, 0, -eyeNotHomogenic.x,
										0, 1, 0, -eyeNotHomogenic.y,
										0, 0, 1, -eyeNotHomogenic.z,
										0, 0, 0, 1);
	this->rotateTranslateMtx = mat4(1, 0, 0, eyeNotHomogenic.x,
									0, 1, 0, eyeNotHomogenic.y,
									0, 0, 1, eyeNotHomogenic.z,
									0, 0, 0, 1);
	assert(rotateTranslateMtx * rotateTranslateInvMtx == mat4());
	//@TODO camera is a model. date modelMatrix (the inverse of the above)
}

void Camera::Frustum(const float left, const float right,
	const float bottom, const float top,
	const float zNear, const float zFar) {
	const float entry11 = ((2 * zNear) / (right - left));
	const float entry13 = ((right + left) / (right - left));
	const float entry22 = ((2 * zNear) / (top - bottom));
	const float entry23 = ((top + bottom) / (top - bottom));
	const float entry33 = ((-zFar - zNear) / (zFar - zNear));
	const float entry34 = ((-2 * zFar*zNear) / (zFar - zNear));

	this->projection = mat4(entry11, 0, entry13, 0,
		0, entry22, entry23, 0,
		0, 0, entry33, entry34,
		0, 0, -1, 0);
}

void Camera::Perspective(const float fovy, const float aspect,
	const float zNear, const float zFar) {

	const double DEG2RAD = 3.14159265 / 180;

	double tangent = tan(fovy / 2 * DEG2RAD);   // tangent of half fovY
	double height = zNear * tangent;          // half height of near plane
	double width = height * aspect;      // half width of near plane

	// params: left, right, bottom, top, near, far
	Frustum(-width, width, -height, height, zNear, zFar);
}

void Camera::Ortho(const float left, const float right,
	const float bottom, const float top,
	const float zNear, const float zFar) {
	const float entry11 = (2 / (right - left));
	const float entry14 = ((-right - left) / (right - left));
	const float entry22 = (2 / (top - bottom));
	const float entry24 = ((-top - bottom) / (top - bottom));
	const float entry33 = (-2 / (zFar - zNear));
	const float entry34 = ((-zFar - zNear) / (zFar - zNear));
	this->projection = mat4(entry11, 0, 0, entry14,
		0, entry22, 0, entry24,
		0, 0, entry33, entry34,
		0, 0, 0, 1);
}

const mat4 Camera::getProjectionMatrix() const {
	return this->projection;
}

const mat4 Camera::getViewMatrix() const {
	return spinScaleInvMtx * rotateTranslateInvMtx;
}