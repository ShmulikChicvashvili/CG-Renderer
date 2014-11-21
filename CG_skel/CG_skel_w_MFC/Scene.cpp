#include "stdafx.h"
#include "Scene.h"
#include "MeshModel.h"
#include <string>
#include <math.h>

using namespace std;
void Scene::loadOBJModel(string fileName)
{
	MeshModel model = MeshModel(fileName);
	models.push_back(model);
}

void Scene::draw()
{
	// 1. Send the renderer the current camera transform and the projection
	// 2. Tell all models to draw themselves
	m_renderer->InitializeBuffer();
	m_renderer->setBuffer(models, cameras[0]->getViewMatrix(), cameras[0]->getProjectionMatrix());
	m_renderer->SwapBuffers();
}

void Scene::drawDemo()
{
	m_renderer->SetDemoBuffer();
	m_renderer->SwapBuffers();
}


///////////////////////////////////
// Shmulik & Eyal stuff

void Scene::loadCamera() {
	cameras.push_back(new Camera());
	cout << "ViewMatrix is : " << cameras[0]->getViewMatrix() << endl;
	cout << "ProjectionMatrix is : " << cameras[0]->getProjectionMatrix() << endl;
}

void Camera::spin(const GLfloat teta, const Axes axis) {
	spinScaleInvMtx = spinScaleInvMtx * Model::genRotationMatrix(-teta, axis);
}

void Camera::scale(const GLfloat sx, const GLfloat sy, const GLfloat sz) {
	spinScaleInvMtx = spinScaleInvMtx * Model::genScaleMatrix(1 / sx, 1 / sy, 1 / sz);
}

void Camera::translate(const GLfloat tx, const GLfloat ty, const GLfloat tz) {
	rotateTranslateInvMtx = rotateTranslateInvMtx * genTranslationMatrix(-tx, -ty, -tz);
}

void Camera::rotate(const GLfloat teta, const Axes axis) {
	rotateTranslateInvMtx = rotateTranslateInvMtx * genRotationMatrix(-teta, axis);
}

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
	this->rotateTranslateInvMtx = mat4(0,0,0,-eyeNotHomogenic.x,
									   0,0,0,-eyeNotHomogenic.y,
									   0,0,0,-eyeNotHomogenic.z,
									   0,0,0,1);
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

const mat4& Camera::getProjectionMatrix() const {
	return this->projection;
}

const mat4& Camera::getViewMatrix() const {
	return spinScaleInvMtx * rotateTranslateInvMtx;
}


vector<Model>& Scene::getModels() {
	return std::move(this->models);
}

vector<Camera*>& Scene::getCameras() {
	return std::move(this->cameras);
}

///////////////////////////////////