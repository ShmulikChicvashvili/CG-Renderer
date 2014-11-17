#pragma once
#include "Model.h"

using namespace std;

class MeshModel : public Model
{
protected :
	MeshModel() {}
	vec3 *vertex_positions;
	//add more attributes
	mat4 _world_transform;
	mat3 _normal_transform;

public:

	MeshModel(string fileName);
	~MeshModel(void);
	void loadFile(string fileName);

	///////////////////////////////
	// Shmulik & Eyal stuff

	const vector<Face> getFaces() const;
	virtual const mat4 getModelMatrix() const;
	//////////////////////////////
};
