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
	activeModel = models.size() - 1;
}

void Scene::draw()
{
	// 1. Send the renderer the current camera transform and the projection
	// 2. Tell all models to draw themselves
	m_renderer->InitializeBuffer();
	cout << "View Matrix while drawing : " << cameras[0]->getViewMatrix() << endl;
	cout << "Projection Matrix while drawing : " << cameras[0]->getProjectionMatrix() << endl;
	m_renderer->setBuffer(models, *cameras[0]);
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



vector<Model>& Scene::getModels() {
	return std::move(this->models);
}

vector<Camera*>& Scene::getCameras() {
	return std::move(this->cameras);
}



///////////////////////////////////