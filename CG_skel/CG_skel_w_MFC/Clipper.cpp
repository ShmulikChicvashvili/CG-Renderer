#include "stdafx.h"
#include "Renderer.h"
#include <time.h>
#include <algorithm>
#include <functional>


const GLfloat W_CLIPPING_PLANE = 0.00001;

inline vector<LitVertex> clipOnPlain(const vector<LitVertex>& p, 
	const std::function<GLfloat(const vec4&, const vec4&)>& calcIntersection, 
	const std::function<bool(const vec4&)>& isInside);

inline vector<LitVertex> clipOnWAxis(const Triangle& t);
inline vector<LitVertex> clipOnAxis(const vector<LitVertex>& p, int axis);
inline LitVertex interpolateLitVertex(const LitVertex& v1, const LitVertex& v2, const GLfloat t);
inline vec4 interpolateVector(const vec4& v1, const vec4& v2, const GLfloat t);
inline vec4 interpolateDirection(const vec4& n1, const vec4& n2, const GLfloat t);
inline vector<LitVertex> clipTriangle(const Triangle& t);
inline short genBCCode(const vec4& coords);

bool isTriangleFullyClipped(const Triangle& t){
	for (int i = 0; i < 3; i++){
		const vec4& v = t[i].getCoords();
		if (v.w == 0){
			return true;
		}
		for (int j = 0; j<3; j++){
			if (v[j] > v.w || v[j] < -v.w){
				return true;
			}
		}
	}
	return false;
}

void Renderer::clipper(vector<Triangle>& triangles, const vector<shared_ptr<Light>>& lights){
	vector<Triangle> toDraw;

	clock_t begin = clock();
	for (Triangle& t : triangles){
		short bc[3];
		for (int i = 0; i < 3; i++){
			bc[i] = genBCCode(t[i].getCoords());
		}
		//trivial reject
		if ((bc[0] & bc[1] & bc[2]) != 0){
			continue;
		}
		//trivial accept
		if ((bc[0] | bc[1] | bc[2]) == 0){
			for (int i = 0; i < 3; i++){
				t[i].setCoords(vec4(windowCoordinates(divideByW(t[i].getCoords())), 1));
			}
			toDraw.push_back(t);
		}


		vector<LitVertex>& clipped = clipTriangle(t);
		if (clipped.size() < 3){
			continue;
		}

		// transfer to window coordinates and create triangles
		assert(clipped[0].getCoords().w > 0);
		assert(clipped[1].getCoords().w > 0);
		clipped[0].setCoords(vec4(windowCoordinates(divideByW(clipped[0].getCoords())), 1));
		clipped[1].setCoords(vec4(windowCoordinates(divideByW(clipped[1].getCoords())), 1));
		for (int i = 1; i < clipped.size() - 1; i++){
			assert(clipped[i + 1].getCoords().w > 0);
			clipped[i+1].setCoords(vec4(windowCoordinates(divideByW(clipped[i+1].getCoords())), 1));
			Triangle newT;
			newT[0] = clipped[0];
			newT[1] = clipped[i];
			newT[2] = clipped[i + 1];
			newT.setDrawWireframe(t.getDrawWireframe());
			toDraw.push_back(newT);
		}
	}

#ifdef DEBUG_PRINT
	cout << "clipper transfer to window Coords time: " << (double)((clock() - begin)) / CLOCKS_PER_SEC << " secs" << endl;
#endif

	zBuffer(toDraw, lights);
}

short genBCCode(const vec4& coords){
	short res = 0;
	for (int i = 0; i < 3; i++){
		if (coords.w - coords[i] < 0){
			res |= 1 << (i * 2);
		}
		if (coords.w + coords[i] < 0){
			res |= 1 << (i * 2 + 1);
		}
	}
	if (coords.w < 0){
		res |= 1 << 6;
	}
	return res;
}

inline vector<LitVertex> clipOnPlain(const vector<LitVertex>& p,
	const std::function<GLfloat(const vec4&, const vec4&)>& calcIntersection,
	const std::function<bool(const vec4&)>& isInside){
	vector<LitVertex> clipped;

	for (int i = 0; i < p.size(); i++){
		const LitVertex& prev = p[i];
		const LitVertex& curr = p[(i + 1) % p.size()];
		
		const vec4& prevCoords = prev.getCoords();
		const vec4& currCoords = curr.getCoords();
		assert(prevCoords != currCoords);
		
		bool prevIsIn = isInside(prevCoords);
		bool currIsIn = isInside(currCoords);
		if (prevIsIn != currIsIn){
			const GLfloat t = calcIntersection(prevCoords, currCoords);
			LitVertex& interception = interpolateLitVertex(prev, curr, t);
			clipped.push_back(interception);
		}
		if (currIsIn){
			clipped.push_back(curr);
		}
	}

	return std::move(clipped);
}

inline vector<LitVertex> clipOnWAxis(const Triangle& t){
	vector<LitVertex> vertices;
	for (int i = 0; i < 3; i++){
		vertices.push_back(t[i]);
	}

	vector<LitVertex>& clipped = clipOnPlain(vertices,
		[](const vec4& v1, const vec4& v2) {return (v1.w - W_CLIPPING_PLANE) / (v1.w - v2.w); },
		[](const vec4& v) {return v.w >= W_CLIPPING_PLANE; });

	return std::move(clipped);

	/*vector<LitVertex> clipped;

	for (int i = 0; i < 3; i++){
		const LitVertex& prev = t[i];
		const LitVertex& curr = t[(i + 1) % 3];
		const vec4& prevCoords = prev.getCoords();
		const vec4& currCoords = curr.getCoords();
		bool prevIsIn = prevCoords.w >= W_CLIPPING_PLANE;
		bool currIsIn = currCoords.w >= W_CLIPPING_PLANE;

		if (prevIsIn != currIsIn){
			const GLfloat t = (prevCoords.w - W_CLIPPING_PLANE) / (prevCoords.w - currCoords.w);
			LitVertex& interception = interpolateLitVertex(prev, curr, t);
			clipped.push_back(interception);
		}
		if (currIsIn){
			clipped.push_back(curr);
		}
	}

	return std::move(clipped);*/
}

inline vector<LitVertex> clipOnAxis(const vector<LitVertex>& p,const int axis){
	assert(axis >= 0 && axis <= 2);
	vector<LitVertex> clipped = std::move(
		clipOnPlain(p,
		[axis](const vec4& v1, const vec4& v2) {return (v1.w - v1[axis]) / ((v1.w - v1[axis]) - (v2.w - v2[axis])); },
		[axis](const vec4& v) {return (v.w - v[axis]) >= 0; }));

	clipped = std::move(
		clipOnPlain(clipped,
		[axis](const vec4& v1, const vec4& v2) {return (v1.w + v1[axis]) / ((v1.w + v1[axis]) - (v2.w + v2[axis])); },
		[axis](const vec4& v) {return (v.w + v[axis]) >= 0; }));


	return std::move(clipped);
}

vector<LitVertex> clipTriangle(const Triangle& t){
	vector<LitVertex> clipped = std::move(clipOnWAxis(t));
	clipped = std::move(clipOnAxis(clipped, 0));
	clipped = std::move(clipOnAxis(clipped, 1));
	clipped = std::move(clipOnAxis(clipped, 2));

	return std::move(clipped);
}

LitVertex interpolateLitVertex(const LitVertex& v1, const LitVertex& v2, const GLfloat t){
	assert(t >= 0 && t <= 1);
	LitVertex v;

	// interpolate coordinates
	const vec4& v1Coords = v1.getCoords();
	const vec4& v2Coords = v2.getCoords();
	v.setCoords(v1Coords + t*(v2Coords - v1Coords));

	// interpolate material
	const Material& v1Mat = v1.getMaterial();
	const Material& v2Mat = v2.getMaterial();
	v.setMaterial(v1Mat + t*(v2Mat - v1Mat));

	// interpolate normal
	assert(v1.hasNormal());
	assert(v2.hasNormal());
	v.setNorm(interpolateDirection(v1.getNorm(), v2.getNorm(), t));

	// interpolate eye vector
	v.setEyeVec(interpolateDirection(v1.getEyeVec(), v2.getEyeVec(), t));

	// interpolate light directions
	assert(v1.getLightDirs().size() == v2.getLightDirs().size());
	const vector<vec4>& v1LightDirs = v1.getLightDirs();
	const vector<vec4>& v2LightDirs = v2.getLightDirs();
	for (int i = 0; i < v1LightDirs.size(); i++){
		v.addLightDir(interpolateDirection(v1LightDirs[i], v2LightDirs[i], t));
	}
	assert(v.getLightDirs().size() == v1.getLightDirs().size());

	return v;
}

inline vec4 interpolateVector(const vec4& v1, const vec4& v2, const GLfloat t){
	assert(v1.w == 0);
	assert(v2.w == 0);
	const vec4& v = v1 + t * (v2 - v1);
	assert(v.w == 0);

	return v;
}

inline vec4 interpolateDirection(const vec4& n1, const vec4& n2, const GLfloat t){
	assert(cmpFloat(length(n1), 1));
	assert(cmpFloat(length(n2), 1));
	const vec4& dir = interpolateVector(n1, n2, t);
	assert(length(dir) != 0);
	return normalize(dir);
}

//void Renderer::clipper(vector<Triangle>& triangles, const vector<shared_ptr<Light>>& lights){
//	clock_t begin = clock();
//	vector<Triangle>::iterator it = std::remove_if(triangles.begin(), triangles.end(), isTriangleFullyClipped);
//	triangles.erase(it, triangles.end());
//	cout << "clipper remove triangles time: " << (double)((clock() - begin)) / CLOCKS_PER_SEC << " secs" << endl;
//
//	begin = clock();
//	for (Triangle& t : triangles){
//		for (int i = 0; i < 3; i++){
//			t[i].setCoords(vec4(windowCoordinates(divideByW(t[i].getCoords())), 1));
//		}
//	}
//	cout << "clipper transfer to window Coords time: " << (double)((clock() - begin)) / CLOCKS_PER_SEC << " secs" << endl;
//
//
//	zBuffer(triangles, lights);
//}