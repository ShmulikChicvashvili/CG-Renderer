#include "stdafx.h"
#include "Scene.h"
#include "MeshModel.h"
#include <string>
#include <math.h>

using namespace std;

void Scene::addModel(const shared_ptr<Model>& pModel){
	assert(pModel);
	models.push_back(pModel);

	if (activeModel >= 0) {
		models[activeModel]->setActive(false);
	}

	activeModel = models.size() - 1;
	models[activeModel]->setActive(true);

}

void Scene::loadMeshModel(const Model& m){
	shared_ptr<Model> pModel = shared_ptr<Model>(new Model(m));
	addModel(pModel);
}

void Scene::loadOBJModel(string fileName)
{
	shared_ptr<Model> pModel = shared_ptr<Model>(new MeshModel(fileName));
	addModel(pModel);
	//models.push_back(pModel);
	//activeModel = models.size() - 1;
	//models[activeModel]->setActive(true);
	//if (activeModel - 1 >= 0) {
	//	models[activeModel - 1]->setActive(false);
	//}
}

void Scene::draw()
{
	// 1. Send the renderer the current camera transform and the projection
	// 2. Tell all models to draw themselves
	m_renderer->InitializeBuffer();
	cout << "View Matrix while drawing : " << cameras[getActiveCamera()]->getViewMatrix() << endl;
	cout << "Projection Matrix while drawing : " << cameras[getActiveCamera()]->getProjectionMatrix() << endl;
	m_renderer->setBuffer(models, *cameras[getActiveCamera()], lights);
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
	shared_ptr<Camera> pCam = shared_ptr<Camera>(new Camera());
	//shared_ptr<Camera> pCam = dynamic_pointer_cast<Camera, Model>(pModel);
	models.push_back(pCam);
	cameras.push_back(pCam);
	activeCamera = cameras.size() - 1;
}


vector<shared_ptr<Model>>& Scene::getModels() {
	return std::move(this->models);
}

vector<shared_ptr<Camera>>& Scene::getCameras() {
	return std::move(this->cameras);
}

void Scene::setActiveCameraLookAtModel(int index){
	if (index >= models.size()){
		cout << "ERROR: Active model is " << index << ". only " << models.size() << " models" << endl;
		return;
	}
	if (isCameraType(models[index].get())){
		cout << "Active model is a camera" << endl;
		return;
	}

	assert(activeCamera >= 0 && activeCamera < cameras.size());

	Camera& cam = *cameras[activeCamera];
	Model& m = *models[index];

	vec4 center = m.getModelMatrix() * vec4(0, 0, 0, 1);
	CameraLookAtError err = cam.LookAt(center + vec4(0, 0, 5, 0), center, vec4(0, 1, 0, 1));
	if (CameraLookAtError::OK != err){
		cout << "Error: setActiveCameraLookAtModel look at returned error" << endl;
	}
}

int Scene::setCameraAsActiveModel(int cameraIndex) {
	shared_ptr<Camera> c = cameras[cameraIndex];
	for (int i = 0; i < models.size(); i++) {
		if (models[i] == c) {
			return i;
		}
	}
	assert(false);
}

void Scene::deleteCamera(int index){
	if (index >= cameras.size() || cameras.size() == 1){
		return;
	}
	shared_ptr<Camera> pCam = cameras[index];
	cameras.erase(cameras.begin() + index);
	for (vector<shared_ptr<Model>>::iterator it = models.begin(); it != models.end(); ++it){
		if (it->get() == pCam.get()){
			models.erase(it);
			return;
		}
	}
	assert(false);
}



///////////////////////////////////