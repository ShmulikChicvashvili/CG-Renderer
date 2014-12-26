#include "StdAfx.h"
#include "Renderer.h"
#include "CG_skel_w_MFC.h"
#include "InitShader.h"
#include "GL\freeglut.h"
#include <assert.h>
#include "PointLight.h"
#include "ParallelLight.h"
#include <algorithm>
#include <math.h>
#include "Vertex.h"

#include <time.h>


#define INDEX(width,x,y,c) (x+y*width)*3+c
#define INDEXZ(width,x,y) (x+y*width)

#define SPEC_SHININESS 3




Renderer::Renderer() :m_width(512), m_height(512), colorMethod(ColorMethod::FLAT)
{
	InitOpenGLRendering();
	initial_width = 512;
	initial_height = 512;
	CreateBuffers(512, 512);
	drawVertexNormals = false;
	drawFaceNorms = false;
}
Renderer::Renderer(int width, int height) : m_width(width), m_height(height), colorMethod(ColorMethod::FLAT)
{
	InitOpenGLRendering();
	initial_width = width;
	initial_height = height;
	CreateBuffers(width, height);
	drawVertexNormals = false;
	drawFaceNorms = false;
}

Renderer::~Renderer(void)
{
}



void Renderer::CreateBuffers(int width, int height)
{
	m_width = width;
	m_height = height;
	CreateOpenGLBuffer(); //Do not remove this line.
	m_outBuffer = new float[3 * m_width*m_height];
	m_zbuffer = new float[m_width*m_height];
}

void Renderer::SetDemoBuffer()
{
	//vertical line
	for (int i = 0; i < m_width; i++)
	{
		m_outBuffer[INDEX(m_width, 256, i, 0)] = 1;	m_outBuffer[INDEX(m_width, 256, i, 1)] = 0;	m_outBuffer[INDEX(m_width, 256, i, 2)] = 0;

	}
	//horizontal line
	for (int i = 0; i < m_width; i++)
	{
		m_outBuffer[INDEX(m_width, i, 256, 0)] = 1;	m_outBuffer[INDEX(m_width, i, 256, 1)] = 0;	m_outBuffer[INDEX(m_width, i, 256, 2)] = 1;

	}
}

/////////////////////////////////////////////////////
// Shmulik & Eyal stuff

void Renderer::setColorMethod(const ColorMethod& cm){
	colorMethod = cm;
}

void Renderer::reshape(int width, int height){
	resizingMatrix = mat4((float)initial_width / width, 0, 0, 0,
		0, (float)initial_height / height, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
	CreateBuffers(width, height);
}

void Renderer::InitializeBuffer() {
	for (int x = 0; x < m_width; x++) {
		for (int y = 0; y < m_height; y++) {
			m_outBuffer[INDEX(m_width, x, y, 0)] = 0;
			m_outBuffer[INDEX(m_width, x, y, 1)] = 0;
			m_outBuffer[INDEX(m_width, x, y, 2)] = 0;
		}
	}
}

// The algorith for drawing line that is used is Bresenham's line algorithm
void Renderer::drawLine(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, Color c) {
	//InitializeBuffer();
	const bool highOctant = (fabs(y2 - y1) > fabs(x2 - x1));
	if (highOctant == true) {
		// switch between x and y
		std::swap(x1, y1);
		std::swap(x2, y2);
	}
	if (x1 > x2) {
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	const GLint dx = x2 - x1;
	const GLint dx2 = 2 * dx;
	const GLint dy2 = 2 * fabs(y2 - y1);

	GLint d = dy2 - dx;
	const GLint ystep = (y1 < y2) ? 1 : -1;
	GLint y = (GLint)y1;

	const GLint Xmax = (GLint)x2;

	for (GLint x = (GLint)x1; x <= Xmax; x++) {

		if (highOctant == true) {
			drawSinglePixel(y, x, c);
		}
		else {
			drawSinglePixel(x, y, c);
		}

		d += dy2;
		if (d >= 0) {
			y += ystep;
			d -= dx2;
		}

	}
}

void Renderer::drawSinglePixel(GLint x, GLint y, Color c) {
	if (!(x >= 0 && x < m_width && y >= 0 && y < m_height)){
		return;
	}
	m_outBuffer[INDEX(m_width, x, y, 0)] = c.getRed();
	m_outBuffer[INDEX(m_width, x, y, 1)] = c.getGreen();
	m_outBuffer[INDEX(m_width, x, y, 2)] = c.getBlue();
}

vector<shared_ptr<Light>> transferLightsToCamSpace(const vector<shared_ptr<Light>> lights, const mat4& viewMtx, const mat4& normViewMtx){
	vector<shared_ptr<Light>> ret;
	ret.reserve(lights.size());
	
	for (const auto& l : lights){
		Material material = l->getMaterial();
		shared_ptr<Light> newLight;
		if (isParallelLight(l.get())){
			const mat4& normModelView = normViewMtx * l->getModelNormalMatrix();
			vec4& direction = normModelView * vec4(0,0,1,0);
			direction.w = 0;

			newLight = shared_ptr<Light>(new ParallelLight(material, direction));
		}
		else if (isPointLight(l.get())){
			const mat4& modelView = viewMtx * l->getModelMatrix();
			const vec4& point = modelView * vec4(0,0,0,1);

			newLight = shared_ptr<Light>(new PointLight(material, point));
		}
		else {
			assert(false);
		}

		ret.push_back(newLight);
	}

	assert(ret.size() == lights.size());

	return ret;
}

void transferFaceToClipSpace(const Face& face, 
	const mat4& modelView, 
	const mat4& normModelView, 
	const mat4& proj, 
	const vector<shared_ptr<Light>>& lights, 
	bool drawWireframe,
	Triangle& t){

	const vector<Vertex>& vertices = face.getVertices();
	assert(vertices.size() == 3);
	//Triangle t;

	for (int i = 0; i < 3; i++){
		const Vertex& v = vertices[i];
		const vec4 camSpace = modelView * v.getCoords();
		LitVertex& l = t[i];

		// set coordinates to clip space
		l.setCoords(proj * camSpace);
		
		l.setMaterial(v.getMaterial());
		
		if (v.hasNormal()){
			// add normal if exists, and directions to all lights and camera
			vec4& norm = normModelView * v.getNorm();
			norm.w = 0;
			l.setNorm(normalize(norm));
		}
		else {
			assert(face.hasNormal());
			vec4& norm = normModelView * face.getNorm();
			norm.w = 0;
			l.setNorm(normalize(norm));
		}

		vec4& eyeVec = -camSpace;
		eyeVec.w = 0;
		eyeVec = eyeVec == vec4(0, 0, 0, 0) ? eyeVec : normalize(eyeVec);
		l.setEyeVec(eyeVec);
		
		l.clearLightDirs();
		for (const auto& pLight : lights){
			l.addLightDir(pLight->getDirectionFromPoint(camSpace));
		}
	}

	t.setDrawWireframe(drawWireframe);

	//return t;
}

void Renderer::setBuffer(const vector<shared_ptr<Model>>& models, const Camera& cam, const vector<shared_ptr<Light>>& lights) {
	//@TODO: Delete it later
	cout << "The size of models is : " << models.size() << endl;
	const mat4& viewMtx = cam.getViewMatrix();
	const mat4& normViewMtx = cam.getViewNormalMatrix();
	const mat4& projMtx = cam.getProjectionMatrix() * resizingMatrix;

	
	const vector<shared_ptr<Light>>& camSpaceLights = transferLightsToCamSpace(lights, viewMtx, normViewMtx);

	vector<Triangle> clipTriangles;
	int numTriangles = 0;
	for (const auto& pModel : models){
		numTriangles += pModel->getFaces().size();
	}

	clock_t begin = clock();
	clipTriangles.reserve(numTriangles);
	cout << "SetBuffer reserve: " << (double)((clock() - begin)) / CLOCKS_PER_SEC << " secs" << endl;

	begin = clock();

	Triangle t;
	for each (const shared_ptr<Model>& pModel in models)
	{
		const vector<Face>& modelFaces = pModel->getFaces();

		const mat4& modelViewMtx = viewMtx * pModel->getModelMatrix();
		const mat4& normalModelViewMtx = normViewMtx * pModel->getModelNormalMatrix();

		for each (const Face& face in modelFaces)
		{
			transferFaceToClipSpace(face, modelViewMtx, normalModelViewMtx, projMtx, camSpaceLights, pModel->getActive(), t);
			clipTriangles.push_back(t);
			//drawFace(face,normViewMtx * pModel->getModelNormalMatrix(), modelViewMtx, projMtx, projMtx * modelViewMtx,  Color(1,1,1));
		}
		
		bool active = pModel->getActive();
		Color c = active ? Color(1, 1, 0) : Color(1, 1, 1);
	}
	cout << "SetBuffer transfer to cam space: " << (double)((clock() - begin)) / CLOCKS_PER_SEC << " secs" << endl;

	////@TODO remove
	//for (auto& t : clipTriangles){
	//	for (int i = 0; i < 3; i++){
	//		LitVertex& v = t[i];
	//		if (v.hasNormal()){
	//			vec4 norm = v.getNorm();
	//			assert(norm.w == 0);
	//			assert(abs(length(norm) - 1) < 0.0001);
	//			norm *= 0.1;
	//			//norm.w = 1;
	//			const vec3& endPoint = windowCoordinates(divideByW(v.getCoords() + projMtx*norm));
	//			const vec3& startPoint = windowCoordinates(divideByW(v.getCoords()));

	//			drawLine(startPoint.x, startPoint.y, endPoint.x, endPoint.y,Color(0,0,1));
	//		}
	//	} 
	//}

	begin = clock();

	clipper(clipTriangles, camSpaceLights);
	cout << "Clipper to End time: " << (double)((clock() - begin)) / CLOCKS_PER_SEC << " secs" << endl;

	cout << "@@@ clipTriangles size: " << clipTriangles.size() << endl;
	begin = clock();
	clipTriangles.clear();
	cout << "@@@ clipTriangles clear time: " << (double)((clock() - begin)) / CLOCKS_PER_SEC << " secs" << endl;
}







bool Renderer::isClipped(const vector<vec4>& clipCords) const{
	for each (const auto& v in clipCords)
	{
		if (v.w == 0){
			return true;
		}
		for (int i = 0; i<3; i++){
			if (v[i] > v.w || v[i] < -v.w){
				return true;
			}
		}
	}
	return false;
}





const vec3 Renderer::windowCoordinates(const vec3& vector) const {
	GLfloat halfWidth = m_width / 2;
	GLfloat halfHeight = m_height / 2;
	return vec3((halfWidth*vector.x) + halfWidth, (halfHeight*vector.y) + halfHeight, (vector.z / 2) + 0.5);
}


bool Renderer::getDrawNormals() {
	return this->drawVertexNormals;
}
void Renderer::setDrawNormals(const bool drawNormals) {
	this->drawVertexNormals = drawNormals;
}

bool Renderer::getDrawFaceNormals() {
	return this->drawFaceNorms;
}

void Renderer::setDrawFaceNormals(const bool drawFaceNorms) {
	this->drawFaceNorms = drawFaceNorms;
}

const bool Renderer::getBarycentricCoordinates(const int x, const int y, const vec2& a, const vec2& b, const vec2&c, float& u, float& v, float& w) const {
	vec2 p = vec2(x, y);

	float Sabc = cross(b - a, c - b);
	float Sapc = cross(p - a, c - p);
	float Sapb = cross(b - a, p - b);

	v = Sapc / Sabc;
	w = Sapb / Sabc;
	u = 1.0f - v - w;

	if (u >= 0 && u <= 1 && v >= 0 && v <= 1 && w >= 0 && w <= 1) {
		return true;
	}
	return false;
}

void Renderer::zBuffer(const vector<Triangle>& polygons, const vector<shared_ptr<Light>>& lights) {
	cout << "Coloring " << polygons.size() << " triangles" << endl;
	cout << "# of lights: " << lights.size() << endl;
	float xMin, xMax, yMin, yMax = 0;
	float u = 0.0;
	float v = 0.0;
	float w = 0.0;
	vec2 a, b, c;
	float z = 0;
	for (int i = 0; i < m_width; i++){
		for (int j = 0; j < m_height; j++){
			m_zbuffer[INDEXZ(m_width, i, j)] = 1.1f;
		}
	}
	int count = 0;
	clock_t begin = clock();
	clock_t color_time = 0;
	for each (auto &t in polygons)
	{
		const vec4& v1 = t[0].getCoords();
		const vec4& v2 = t[1].getCoords();
		const vec4& v3 = t[2].getCoords();
		xMin = min(v1.x, min(v2.x, v3.x));
		xMax = max(v1.x, max(v2.x, v3.x));
		yMin = min(v1.y, min(v2.y, v3.y));
		yMax = max(v1.y, max(v2.y, v3.y));

		for (int x = xMin; x < xMax; x++) {
			for (int y = yMin; y < yMax; y++) {
				a = vec2(v1.x, v1.y);
				b = vec2(v2.x, v2.y);
				c = vec2(v3.x, v3.y);
				if (!getBarycentricCoordinates(x, y, a, b, c, u, v, w)){ continue; }
			

				z = u * v1.z + v * v2.z + w * v3.z;
				if (z < m_zbuffer[INDEXZ(m_width, x, y)]) {
					color_time -= clock();
					setColor(x, y, t, lights, u, v, w);
					color_time += clock();
					//drawSinglePixel(x, y, count%2==0?Color(1, 0, 0):Color(0,0,1));
					m_zbuffer[INDEXZ(m_width, x, y)] = z;
				}
			}
		}


		count++;
	}
	cout << "zBuffer time: " << (double)((clock() - begin)) / CLOCKS_PER_SEC << " secs" << endl;
	cout << "zBuffer setting color time: " << (double)(color_time) / CLOCKS_PER_SEC << " secs" << endl;

	begin = clock();
	for (const auto& t : polygons){
		if (t.getDrawWireframe()){
			for (int i = 0; i < 3; i++){
				drawLine(t[i].getCoords().x, t[i].getCoords().y, t[(i + 1) % 3].getCoords().x, t[(i + 1) % 3].getCoords().y,Color(1,0,0));
			}
		}
	}
	cout << "Draw wireframe time: " << (double)((clock() - begin)) / CLOCKS_PER_SEC << " secs" << endl;

}

vec4& Renderer::reflect(const vec4& V1, const vec4& V2) {
	return V1 - 2.0 * (dot(V1, V2)) * V2;
}

vec3 Renderer::calculateIlluminationIntensity(const Material& pixelMaterial, const Material& lightMaterial,
	const vec4& lightDirection, const vec4& norm, const vec4& viewDirection) {
	assert(cmpFloat(length(viewDirection), 1) && viewDirection.w == 0);
	assert((cmpFloat(length(lightDirection), 1) || lightDirection == vec4(0, 0, 0, 0)) && lightDirection.w == 0);
	assert(cmpFloat(length(norm), 1) && norm.w == 0);

	vec3 illuminationIntensity = 0.0;
	// Ambient illumination
	illuminationIntensity += pixelMaterial.getAmbient() * lightMaterial.getAmbient();
	// Defuse illumination
	illuminationIntensity += pixelMaterial.getDiffuse() * max((dot(lightDirection, norm)),0)
		* lightMaterial.getDiffuse();
	// Specular illumination
	vec4 R = normalize(reflect(-lightDirection, norm));
	float temp = max(dot(R, viewDirection), 0);
	illuminationIntensity += pixelMaterial.getSpecular() * (pow(temp, SPEC_SHININESS)) * lightMaterial.getSpecular();
	return illuminationIntensity;
}

void Renderer::setColor(const int x, const int y, const Triangle& t, const vector<shared_ptr<Light>>& lights,
	const float& u, const float& v, const float& w) {
	
	vec3 illuminationIntensity = 0.0;

	const LitVertex& v1 = t[0];
	const LitVertex& v2 = t[1];
	const LitVertex& v3 = t[2];

	switch (colorMethod)
	{
	case ColorMethod::FLAT:
		for (int i = 0; i < lights.size(); i++) {
			illuminationIntensity += calculateIlluminationIntensity(v1.getMaterial(), lights[i]->getMaterial(), v1.getLightDirs().at(i),
				v1.getNorm(), v1.getEyeVec());
		}
		drawSinglePixel(x, y, illuminationIntensity);
		break;
	case ColorMethod::GOURAUD:{
		vec3 illuminationIntensityAtVertex1 = 0.0;
		vec3 illuminationIntensityAtVertex2 = 0.0;
		vec3 illuminationIntensityAtVertex3 = 0.0;

		for (int i = 0; i < lights.size(); i++) {
			illuminationIntensityAtVertex1 += calculateIlluminationIntensity(v1.getMaterial(), lights[i]->getMaterial(), v1.getLightDirs().at(i),
				v1.getNorm(), v1.getEyeVec());
			illuminationIntensityAtVertex2 += calculateIlluminationIntensity(v2.getMaterial(), lights[i]->getMaterial(), v2.getLightDirs().at(i),
				v2.getNorm(), v2.getEyeVec());
			illuminationIntensityAtVertex3 += calculateIlluminationIntensity(v3.getMaterial(), lights[i]->getMaterial(), v3.getLightDirs().at(i),
				v3.getNorm(), v3.getEyeVec());
		}
		illuminationIntensity = u * illuminationIntensityAtVertex1 + v * illuminationIntensityAtVertex2 + w * illuminationIntensityAtVertex3;
		drawSinglePixel(x, y, illuminationIntensity);
		break;
	}
	case ColorMethod::PHONG:{
		vec4 phongNorm = 0.0;
		Material phongMaterial;
		vec4 phongLightDir;
		vec4 phongEyeVec;
		illuminationIntensity = 0.0;

		phongNorm = normalize(u * v1.getNorm() + v * v2.getNorm() + w * v3.getNorm());

		phongEyeVec = normalize(u * v1.getEyeVec() + v * v2.getEyeVec() + w * v3.getEyeVec());

		phongMaterial = u * v1.getMaterial() + v * v2.getMaterial() + w * v3.getMaterial();

		for (int i = 0; i < lights.size(); i++) {
			phongLightDir = u * v1.getLightDirs().at(i) + v * v2.getLightDirs().at(i) + w * v3.getLightDirs().at(i);
			phongLightDir = normalize(phongLightDir);

			illuminationIntensity += calculateIlluminationIntensity(phongMaterial, lights[i]->getMaterial(), phongLightDir,
				phongNorm, phongEyeVec);
		}

		drawSinglePixel(x, y, illuminationIntensity);
		break; 
	}
	default:
		assert(false);
		break;
	}
}


/////////////////////////////////////////////////////


/////////////////////////////////////////////////////
//OpenGL stuff. Don't touch.

void Renderer::InitOpenGLRendering()
{
	int a = glGetError();
	a = glGetError();
	glGenTextures(1, &gScreenTex);
	a = glGetError();
	glGenVertexArrays(1, &gScreenVtc);
	GLuint buffer;
	glBindVertexArray(gScreenVtc);
	glGenBuffers(1, &buffer);
	const GLfloat vtc[] = {
		-1, -1,
		1, -1,
		-1, 1,
		-1, 1,
		1, -1,
		1, 1
	};
	const GLfloat tex[] = {
		0, 0,
		1, 0,
		0, 1,
		0, 1,
		1, 0,
		1, 1 };
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc) + sizeof(tex), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);
	GLint  vPosition = glGetAttribLocation(program, "vPosition");

	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0,
		0);

	GLint  vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		(GLvoid *) sizeof(vtc));
	glUniform1i(glGetUniformLocation(program, "texture"), 0);
	a = glGetError();
}

void Renderer::CreateOpenGLBuffer()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gScreenTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_width, m_height, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, m_width, m_height);
}

void Renderer::SwapBuffers()
{

	int a = glGetError();
	glActiveTexture(GL_TEXTURE0);
	a = glGetError();
	glBindTexture(GL_TEXTURE_2D, gScreenTex);
	a = glGetError();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_FLOAT, m_outBuffer);
	glGenerateMipmap(GL_TEXTURE_2D);
	a = glGetError();

	glBindVertexArray(gScreenVtc);
	a = glGetError();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	a = glGetError();
	glutSwapBuffers();
	a = glGetError();
}

//void Renderer::drawFaceNormal(const vec4& norm, const vec4& midPoint, const mat4& normModelViewMtx, const mat4& modelViewMtx, const mat4& projMtx) {
//	vec4 normCords = normModelViewMtx * norm;
//	normCords.w = 0;
//	normCords = 0.1 * normalize(normCords);
//
//	const vec4& midPointCords = modelViewMtx * midPoint;
//	const vec4& endNormViewCoords = vec4(midPointCords.x + normCords.x, midPointCords.y + normCords.y, midPointCords.z + normCords.z, 1);
//	const vec3& endNormWindowCoords = windowCoordinates(divideByW(projMtx * endNormViewCoords));
//	const vec3& midPointWindowCords = windowCoordinates(divideByW(projMtx * midPointCords));
//
//	drawLine(endNormWindowCoords.x, endNormWindowCoords.y, midPointWindowCords.x, midPointWindowCords.y, Color(0.5, 0.5, 0.5));
//}
//
//const vec3 Renderer::normalNDC2Window(const vec4& n) const{
//	vec4& ndc = mat4((GLfloat)2 / m_width, 0, 0, 0,
//		0, (GLfloat)2 / m_height, 0, 0,
//		0, 0, 2, 0,
//		-1, -1, -1, 1) * n;
//	return vec3(ndc.x, ndc.y, ndc.z);
//
//}
//
//void Renderer::drawFace(const Face& face, const mat4& normModelViewMtx, const mat4& modelViewMtx, const mat4& projMtx, const mat4& mvpMtx, Color c){
//	vector<vec4> clipCords;
//	clipCords.reserve(face.getVertices().size());
//
//	for (const auto& v : face.getVertices()){
//		clipCords.push_back(mvpMtx * v.getCoords());
//	}
//	if (isClipped(clipCords)){
//		return;
//	}
//
//
//	vector<vec3> windowCords;
//	windowCords.reserve(face.getVertices().size());
//
//	for (const auto& v : clipCords){
//		windowCords.push_back(windowCoordinates(divideByW(v)));
//	}
//
//	for (int i = 0; i < face.getVertices().size(); i++) {
//		vec3& windowCordsFirstPoint = windowCords[i];
//		vec3& windowCordsSecondPoint = windowCords[(i + 1) % face.getVertices().size()];
//		drawLine(windowCordsFirstPoint.x, windowCordsFirstPoint.y, windowCordsSecondPoint.x, windowCordsSecondPoint.y, c);
//	}
//
//	if (face.hasNormal() && face.hasMidPoint() && drawFaceNorms) {
//		drawFaceNormal(face.getNorm(), face.getMidPoint(), normModelViewMtx, modelViewMtx, projMtx);
//	}
//
//	if (!drawVertexNormals){
//		return;
//	}
//
//	const vector<Vertex>& vertices = face.getVertices();
//	for (int i = 0; i < vertices.size(); i++) {
//		if (vertices[i].hasNormal()) {
//			vec4 normCords = normModelViewMtx * vertices[i].getNorm();
//			normCords.w = 0;
//			normCords = 0.1 * normalize(normCords);
//
//			const vec4& viewPointCords = modelViewMtx * vertices[i].getCoords();
//			const vec4& endNormViewCoords = vec4(viewPointCords.x + normCords.x, viewPointCords.y + normCords.y, viewPointCords.z + normCords.z, 1);
//			const vec3& endNormWindowCoords = windowCoordinates(divideByW(projMtx * endNormViewCoords));
//
//
//			drawLine(windowCords[i].x,
//				windowCords[i].y, endNormWindowCoords.x, endNormWindowCoords.y, Color(1, 0, 0));
//		}
//	}
//
//
//
//}