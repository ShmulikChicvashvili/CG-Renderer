#pragma once
#include "Model.h"

using namespace std;

class MeshModel : public Model
{
protected :
	MeshModel() {}

public:

	MeshModel(string fileName);
	~MeshModel(void);
	void loadFile(string fileName);

	///////////////////////////////
	// Shmulik & Eyal stuff

	const vector<Face>& getFaces() const;
	virtual const mat4 getModelMatrix() const;
	//////////////////////////////
};
