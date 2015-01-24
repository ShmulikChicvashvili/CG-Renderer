#pragma once
#include "Model.h"

using namespace std;

class MeshModel : public Model
{
protected :
	MeshModel(Renderer* _renderer) : Model(_renderer) {}

public:

	MeshModel(string fileName, Renderer* _renderer);
	~MeshModel(void);
	void loadFile(string fileName);

	///////////////////////////////
	// Shmulik & Eyal stuff
	//////////////////////////////
};
