#include "stdafx.h"
#include "PrimMeshModel.h"


PrimMeshModel::PrimMeshModel(Renderer* _renderer) : Model(_renderer)
{
	const GLfloat side = 0.5;
	for (int i = 0; i < 3; i++){
		Face f;
		for (int j = -1; j <= 1; j += 2){
			f = Face();
			vec4 norm;
			norm[i] = j;
			GLfloat plane = j*side;
			vec4 vertex;
			vertex.w = 1;
			vertex[i] = plane;
			//First triangle
			vertex[(i + 1) % 3] = -side;
			vertex[(i + 2) % 3] = side;
			f.addVertex(vertex, norm);

			vertex[(i + 1) % 3] = side;
			vertex[(i + 2) % 3] = -j*side;
			f.addVertex(vertex,norm);
			
			vertex[(i + 1) % 3] = side;
			vertex[(i + 2) % 3] = j*side;
			f.addVertex(vertex,norm);
			f.calcMidPoint();
			f.calcNorm();
			faces.push_back(f);

			//Second triangle
			f = Face();
			vertex[(i + 1) % 3] = -side;
			vertex[(i + 2) % 3] = side;
			f.addVertex(vertex, norm);

			vertex[(i + 1) % 3] = -j*side;
			vertex[(i + 2) % 3] = -side;
			f.addVertex(vertex, norm);

			vertex[(i + 1) % 3] = j*side;
			vertex[(i + 2) % 3] = -side;
			f.addVertex(vertex, norm);
			f.calcMidPoint();
			f.calcNorm();
			faces.push_back(f);
		}
	}
	assert(faces.size() == 6 * 2);
}
