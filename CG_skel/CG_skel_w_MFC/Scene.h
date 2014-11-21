#pragma once

#include "gl/glew.h"
#include <vector>
#include <string>
#include "Renderer.h"
#include "Model.h"
using namespace std;



class Light {

};

class Camera : Model {
	mat4 spinScaleInvMtx;
	mat4 rotateTranslateInvMtx;
	mat4 projection;


public:
	void LookAt(const vec4& eye, const vec4& at, const vec4& up);
	void Ortho(const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar);
	void Frustum(const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar);
	void Perspective(const float fovy, const float aspect,
		const float zNear, const float zFar);
	const mat4& getViewMatrix() const;
	const mat4& getProjectionMatrix() const;

	virtual void spin(const GLfloat teta, const Axes axis);
	virtual void scale(const GLfloat sx, const GLfloat sy, const GLfloat sz);
	virtual void translate(const GLfloat tx, const GLfloat ty, const GLfloat tz);
	virtual void rotate(const GLfloat teta, const Axes axis);
};

class Scene {

	vector<Model> models;
	vector<Light*> lights;
	vector<Camera*> cameras;
	Renderer *m_renderer;
	int activeModel;
	int activeLight;
	int activeCamera;


public:
	Scene() {};
	Scene(Renderer *renderer) : m_renderer(renderer) {};
	void loadOBJModel(string fileName);
	void loadCamera();
	void draw();
	void drawDemo();

	const int getActiveModel() {
		return activeModel;
	}

	const int getActiveLight() {
		return activeLight;
	}

	const int getActiveCamera() {
		return activeCamera;
	}

	vector<Model>& getModels();
	vector<Camera*>& getCameras();
};