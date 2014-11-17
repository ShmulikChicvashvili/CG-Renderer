#include <vector>
#include "mat.h"

using std::vector;


class Face {
private:
	vector<vec4> vertices;

public:
	const vector<vec4> getVertices() const;
};

class Model {
protected:
	vector<Face> faces;
	virtual ~Model() {}

public:
	virtual const mat4 getModelMatrix() const = 0;
	virtual const vector<Face> getFaces() const = 0;
};