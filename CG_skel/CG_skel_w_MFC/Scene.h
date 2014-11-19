#pragma once

#include "gl/glew.h"
#include <vector>
#include <string>
#include "Renderer.h"
#include "Model.h"
using namespace std;



class Light {

};

class Camera {
	mat4 viewTransform;
	mat4 projection;


public:
	//void setTransformation(const mat4& transform);
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
};

class Scene {

	vector<Model> models;
	vector<Light*> lights;
	vector<Camera*> cameras;
	Renderer *m_renderer;

public:
	Scene() {};
	Scene(Renderer *renderer) : m_renderer(renderer) {};
	void loadOBJModel(string fileName);
	void draw();
	void drawDemo();

	int activeModel;
	int activeLight;
	int activeCamera;

	vector<Model>& getModels();
};