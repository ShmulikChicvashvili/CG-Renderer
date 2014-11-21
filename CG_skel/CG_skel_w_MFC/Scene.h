#pragma once

#include "gl/glew.h"
#include <vector>
#include <string>
#include "Renderer.h"
#include "Model.h"
#include "Camera.h"
using namespace std;


#define ALL_MODELS_ACTIVE -1


class Light {

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

	void setActiveModel(const int activeModel) {
		this->activeModel = activeModel;
	}

	void setActiveLight(const int activeLight) {
		this->activeLight = activeLight;
	}

	void setActiveCamera(const int activeCamera) {
		this->activeCamera = activeCamera;
	}

	vector<Model>& getModels();
	vector<Camera*>& getCameras();
};