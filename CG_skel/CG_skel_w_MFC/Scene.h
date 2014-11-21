#pragma once

#include "gl/glew.h"
#include <vector>
#include <string>
#include "Renderer.h"
#include "Model.h"
#include "Camera.h"
using namespace std;



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

	vector<Model>& getModels();
	vector<Camera*>& getCameras();
};