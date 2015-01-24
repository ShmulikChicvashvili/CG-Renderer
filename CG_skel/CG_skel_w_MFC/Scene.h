#pragma once

#include "gl/glew.h"
#include <vector>
#include <string>
#include "Renderer.h"
#include "Model.h"
#include "Camera.h"
#include "Light.h"
#include <memory>

using namespace std;

#define ALL_MODELS_ACTIVE -1



class Scene {
	
	vector<shared_ptr<Model>> models;
	vector<shared_ptr<Light>> lights;
	vector<shared_ptr<Camera>> cameras;
	Renderer *m_renderer;
	int activeModel;
	int activeLight;
	int activeCamera;

	void addModel(const shared_ptr<Model>& pModel);

public:
	Scene() :activeModel(), activeLight(), activeCamera() {};
	Scene(Renderer *renderer) : m_renderer(renderer), activeModel(), activeLight(), activeCamera() {};
	void loadOBJModel(string fileName);
	void loadMeshModel(const Model& m);
	void loadCamera();
	void deleteCamera(int index);
	void draw();
	void drawDemo();

	void setActiveCameraLookAtModel(int index);
	int setCameraAsActiveModel(int cameraIndex);

	void loadLight(const shared_ptr<Light>& l);

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

	vector<shared_ptr<Model>>& getModels();
	vector<shared_ptr<Camera>>& getCameras();
	Renderer* getRenderer();
};