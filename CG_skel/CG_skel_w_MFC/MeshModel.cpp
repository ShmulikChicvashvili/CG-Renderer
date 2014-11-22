#include "StdAfx.h"
#include "MeshModel.h"
#include "vec.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

struct FaceIdcs
{
	int v[4];
	int vn[4];
	int vt[4];

	FaceIdcs()
	{
		for (int i=0; i<4; i++)
			v[i] = vn[i] = vt[i] = 0;
	}

	FaceIdcs(std::istream & aStream)
	{
		for (int i=0; i<4; i++)
			v[i] = vn[i] = vt[i] = 0;

		char c;
		for(int i = 0; i < 3; i++)
		{
			aStream >> std::ws >> v[i] >> std::ws;
			if (aStream.peek() != '/')
				continue;
			aStream >> c >> std::ws;
			if (aStream.peek() == '/')
			{
				aStream >> c >> std::ws >> vn[i];
				continue;
			}
			else
				aStream >> vt[i];
			if (aStream.peek() != '/')
				continue;
			aStream >> c >> vn[i];
		}
	}
};

vec4 vec4fFromStream(std::istream & aStream, GLfloat w)
{
	float x, y, z;
	aStream >> x >> std::ws >> y >> std::ws >> z;
	return vec4(x, y, z, w);
}

vec3 vec3fFromStream(std::istream & aStream)
{
	float x, y, z;
	aStream >> x >> std::ws >> y >> std::ws >> z;
	return vec3(x, y, z);
}

vec2 vec2fFromStream(std::istream & aStream)
{
	float x, y;
	aStream >> x >> std::ws >> y;
	return vec2(x, y);
}

MeshModel::MeshModel(string fileName)
{
	loadFile(fileName);
}

MeshModel::~MeshModel(void)
{
}

void MeshModel::loadFile(string fileName)
{
	ifstream ifile(fileName.c_str());
	vector<FaceIdcs> facesIdcs;
	vector<vec4> vertices;
	vector<vec4> normals;
	// while not end of file
	while (!ifile.eof())
	{
		// get line
		string curLine;
		getline(ifile, curLine);

		// read type of the line
		istringstream issLine(curLine);
		string lineType;

		issLine >> std::ws >> lineType;

		// based on the type parse data
		if (lineType == "v")
			vertices.push_back(vec4fFromStream(issLine, 1));
		else if (lineType == "f")
			facesIdcs.push_back(issLine);
		else if (lineType == "#" || lineType == "")
		{
			// comment / empty line
		}
		else if (lineType == "vn"){
			normals.push_back(vec4fFromStream(issLine, 1));
			cout << "read normal: " << normals.at(normals.size() - 1) << endl;;
		}
		else
		{
			cout << "Found unknown line Type \"" << lineType << "\"" << endl;
		}
	}
	//Vertex_positions is an array of vec3. Every three elements define a triangle in 3D.
	//If the face part of the obj is
	//f 1 2 3
	//f 1 3 4
	//Then vertex_positions should contain:
	//vertex_positions={v1,v2,v3,v1,v3,v4}

	// iterate through all stored faces and create triangles

	for each (const FaceIdcs& faceIdc in facesIdcs)
	{
		Face f;
		for (int i = 0; i < 3; i++){
			const vec4& v = vertices[faceIdc.v[i] - 1];
			if (faceIdc.vn[i] == 0){
				f.addVertex(v);
			}
			else {
				cout << "Vertex: " << v << " Loaded with normal: " << normals[faceIdc.vn[i] - 1] << endl;
				f.addVertex(v, normals[faceIdc.vn[i] - 1]);
			}
		}
		this->faces.push_back(f);
	}

	cout << "Loaded mesh model: " << fileName << " Successfully";
}

///////////////////////////////////////////////
// Shmulik & Eyal stuff
///////////////////////////////////////////////