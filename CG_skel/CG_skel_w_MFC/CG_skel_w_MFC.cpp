// CG_skel_w_MFC.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GlVersion.h"
#include "CG_skel_w_MFC.h"
#include <algorithm>
#include <string>
#include "PointLight.h"
#include "ParallelLight.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/freeglut_ext.h"
#include "vec.h"
#include "mat.h"
#include "InitShader.h"
#include "Scene.h"
#include "Renderer.h"
#include "Resource.h"
#include "InputDialog.h"
#include "PrimMeshModel.h"
#include <string>
#include <sstream>

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

#define INIT_WIDTH 512
#define INIT_HEIGHT 512

#define FILE_OPEN 1
#define MAIN_DEMO 1
#define MAIN_ABOUT 2
#define X_AXIS 3
#define Y_AXIS 4
#define Z_AXIS 5
#define ALL_AXES 6
#define TRANSLATION_ACTION 7
#define SPIN_ACTION 8
#define SCALING_ACTION 9
#define ROTATE_ACTION 10

#define LOAD_CAMERA 1

#define LOAD_CUBE 1

#define SCALING_FACTOR 1
#define MOUSE_SMOOTH 30

#define FLAT_SHADING 1
#define GOURAUD_SHADING 2
#define PHONG_SHADING 3

#define POINT_LIGHT 1
#define PARALLEL_LIGHT 2

#define LOAD_TEXTURE 1

#define SILHOUETTE 1
#define TOON 2

Scene *scene;
Renderer *renderer;

int last_x, last_y;
int last_dx, last_dy;
bool lb_down, rb_down, mb_down;

// My addition

enum Action { translate, rotatee, spin, scale };

bool xAxisBool, yAxisBool, zAxisBool = false;
bool allAxesBool = true;

Axes selectedAxis = ALL;

Action selectedAction = scale;

void materialDialog(vec3& ambient, vec3& diffuse, vec3& specular) {
	CXyzDialog ambientDlg("Ambient Color");
	CXyzDialog diffuseDlg("Diffuse Color");
	CXyzDialog specularDlg("Specular Color");

	cout << "Choose Ambient Color" << endl;
	if (ambientDlg.DoModal() == IDOK) {
		ambient = ambientDlg.GetXYZ();
	}
	if (ambient.x < 0 || ambient.x > 1 || ambient.y < 0 || ambient.y > 1 || ambient.z < 0 || ambient.z > 1) { return; }
	cout << "Choose Diffuse Color" << endl;
	if (diffuseDlg.DoModal() == IDOK) {
		diffuse = diffuseDlg.GetXYZ();
	}
	if (diffuse.x < 0 || diffuse.x > 1 || diffuse.y < 0 || diffuse.y > 1 || diffuse.z < 0 || diffuse.z > 1) { return; }
	cout << "Choose Specular Color" << endl;
	if (specularDlg.DoModal() == IDOK) {
		specular = specularDlg.GetXYZ();
	}
	if (specular.x < 0 || specular.x > 1 || specular.y < 0 || specular.y > 1 || specular.z < 0 || specular.z > 1) { return; }
}

void doTranslation(vector<shared_ptr<Model>>& currentModels, int activeModel, float intensity) {
	if (activeModel != ALL_MODELS_ACTIVE) {
#ifdef DEBUG_PRINT
		cout << "Translating by : " << intensity << " In " << selectedAxis << " Axis Model number" << activeModel << endl;
#endif
		currentModels[activeModel]->translate((allAxesBool + xAxisBool)*intensity,
			(allAxesBool + yAxisBool)*intensity,
			(allAxesBool + zAxisBool)*intensity);
	}
	else {
		for (auto &m : currentModels)
		{
#ifdef DEBUG_PRINT
			cout << "Translating by : " << intensity << " In " << selectedAxis << " Axis" << endl;
#endif
			m->translate((allAxesBool + xAxisBool)*intensity,
				(allAxesBool + yAxisBool)*intensity,
				(allAxesBool + zAxisBool)*intensity);
		}
	}
	scene->draw();
}

void doScaling(vector<shared_ptr<Model>>& currentModels, int activeModel, float intensity) {
	if (intensity < 0) {
		intensity = 1 / abs(intensity);
	}
	if (activeModel != ALL_MODELS_ACTIVE) {
#ifdef DEBUG_PRINT
		cout << " Scaling by : " << intensity << "The model :" << activeModel << endl;
#endif
		switch (selectedAxis)
		{
		case X:
			currentModels[activeModel]->scale(SCALING_FACTOR*abs(intensity), 1, 1);
			break;
		case Y:
			currentModels[activeModel]->scale(1, SCALING_FACTOR*abs(intensity), 1);
			break;
		case Z:
			currentModels[activeModel]->scale(1, 1, SCALING_FACTOR*abs(intensity));
			break;
		case ALL:
			currentModels[activeModel]->scale(SCALING_FACTOR*abs(intensity), SCALING_FACTOR * abs(intensity), SCALING_FACTOR * abs(intensity));
			break;
		default:
			break;
		}
	}
	else {
		for (auto &m : currentModels)
		{
#ifdef DEBUG_PRINT
			cout << " Scaling by : " << intensity << endl;
#endif
			switch (selectedAxis)
			{
			case X:
				m->scale(SCALING_FACTOR*abs(intensity), 1, 1);
				break;
			case Y:
				m->scale(1, SCALING_FACTOR*abs(intensity), 1);
				break;
			case Z:
				m->scale(1, 1, SCALING_FACTOR*abs(intensity));
				break;
			case ALL:
				m->scale(SCALING_FACTOR*abs(intensity), SCALING_FACTOR * abs(intensity), SCALING_FACTOR * abs(intensity));
				break;
			default:
				break;
			}
		}
	}
	scene->draw();
}

void doSpin(vector<shared_ptr<Model>>& currentModels, int activeModel, float intensity) {
	if (activeModel != ALL_MODELS_ACTIVE) {
#ifdef DEBUG_PRINT
		cout << "spinning by : " << intensity << " In" << selectedAxis << " Axis The model : " << activeModel << endl;
#endif
		currentModels[activeModel]->spin(intensity, selectedAxis);
	}
	else {
		for (auto& m : currentModels) {
#ifdef DEBUG_PRINT
			cout << "spinning by : " << intensity << " In" << selectedAxis << " Axis" << endl;
#endif
			m->spin(intensity, selectedAxis);
		}
	}
	scene->draw();
}

void doRotate(vector<shared_ptr<Model>>& currentModels, int activeModel, float intensity) {
	if (activeModel != ALL_MODELS_ACTIVE) {
#ifdef DEBUG_PRINT
		cout << "Rotating By : " << intensity << "In " << selectedAxis << " Axis The model : " << activeModel << endl;
#endif
		currentModels[activeModel]->rotate(intensity, selectedAxis);
	}
	else {
		for (auto &m : currentModels) {
#ifdef DEBUG_PRINT
			cout << "Rotating By : " << intensity << "In " << selectedAxis << " Axis" << endl;
#endif
			m->rotate(intensity, selectedAxis);
		}
	}
	scene->draw();
}

void applyTransformation(float intensity) {
	int activeModel = scene->getActiveModel();
	vector<shared_ptr<Model>>& currentModels = scene->getModels();
	switch (selectedAction)
	{
	case translate:
		doTranslation(currentModels, activeModel, intensity);
		break;
	case scale:
		doScaling(currentModels, activeModel, intensity);
		break;
	case spin:
		doSpin(currentModels, activeModel, intensity);
		break;
	case rotatee:
		doRotate(currentModels, activeModel, intensity);
		break;
	default:
		break;
	}
}

//----------------------------------------------------------------------------
// Callbacks

void display(void)
{
	//Call the scene and ask it to draw itself
	scene->draw();
}

void reshape(int width, int height)
{
	renderer->reshape(width, height);
	//update the renderer's buffers
}

CameraLookAtError lookAt(Camera& cam) {
	
	CXyzDialog dlgEye("Camera Coordinates");
	CXyzDialog dlgTarget("Target Coordinates");
	CXyzDialog dlgUp("Up Coordinates");
	vec3 eye;
	vec3 target;
	vec3 up;
	CameraLookAtError err;

	cout << "Choose Camera Coordinates" << endl;
	if (dlgEye.DoModal() == IDOK) {
		eye = dlgEye.GetXYZ();
	}
	cout << "Choose Target Coordinates" << endl;
	if (dlgTarget.DoModal() == IDOK) {
		target = dlgTarget.GetXYZ();
	}
	cout << "Choose Up Coordinates" << endl;
	if (dlgUp.DoModal() == IDOK) {
		up = dlgUp.GetXYZ();
	}
	err = cam.LookAt(vec4(eye.x, eye.y, eye.z, 1), vec4(target.x, target.y, target.z, 1), vec4(up.x, up.y, up.z, 1));
	if (CameraLookAtError::OK != err) {
		return err;
	}
	else {
		scene->draw();
		return err;
	}
}

void selectAllAxes() {
	cout << "Selected All Axes" << endl;
	allAxesBool = true;
	xAxisBool = yAxisBool = zAxisBool = false;
	selectedAxis = ALL;
}

void selectXAxis() {
	cout << "Selected X axis" << endl;
	xAxisBool = true;
	yAxisBool = zAxisBool = allAxesBool = false;
	selectedAxis = X;
}

void selectYAxis() {
	cout << "Selected Y axis" << endl;
	yAxisBool = true;
	xAxisBool = zAxisBool = allAxesBool = false;
	selectedAxis = Y;
}

void selectZAxis() {
	cout << "Selected Z axis" << endl;
	zAxisBool = true;
	xAxisBool = yAxisBool = allAxesBool = false;
	selectedAxis = Z;
}

void tempLookAt(Camera& c) {
	CameraLookAtError err = lookAt(c);
	switch (err)
	{
	case CameraLookAtError::EYE_AT_TARGET:
		cout << "The camera chosen is at the target" << endl;
		break;
	case CameraLookAtError::INVALID_UP:
		cout << "The camera's up vector in invalid" << endl;
		break;
	case CameraLookAtError::OK:
		break;
	default:
		break;
	}
}

void deleteModels(vector<shared_ptr<Model>>& models) {
	renderer->InitializeBuffer();
	vector<shared_ptr<Model>>::iterator it = std::remove_if(models.begin(), models.end(), [](const shared_ptr<Model>& p) {return !isCameraType(p.get()); });
	models.erase(it, models.end());
	//for (vector<shared_ptr<Model>>::iterator it = models.begin(); it != models.end(); ++it) {
	//	if (typeid(*(it->get())) != typeid (Camera)) {
	//		it = models.erase(it);
	//	}
	//}
	scene->setActiveCamera(0);
	scene->setActiveLight(0);
	scene->setActiveModel(0);
	scene->getModels()[0]->setActive(true);
	scene->draw();
}

void selectSpecificModel(vector<shared_ptr<Model>>& currentModels, int index, int newIndex) {
	if (scene->getActiveModel() == ALL_MODELS_ACTIVE) {
		scene->setActiveModel(currentModels.size() - 1);
		for (auto &m : currentModels) {
			m->setActive(false);
		}
		currentModels[currentModels.size() - 1]->setActive(true);
	}
	else {
		if (newIndex == -1) {
			newIndex = currentModels.size() - 1;
		}
		cout << "Previous active model is : " << index << endl;
		currentModels[index]->setActive(false);
		scene->setActiveModel(newIndex % scene->getModels().size());
		cout << "Current active model is : " << newIndex << endl;
		currentModels[newIndex % scene->getModels().size()]->setActive(true);
	}
	scene->draw();
}

void selectAllModels(vector<shared_ptr<Model>>& currentModels) {
	scene->setActiveModel(ALL_MODELS_ACTIVE);
	for (auto &m : currentModels) {
		m->setActive(true);
	}
	scene->draw();
}

void selectSpecificCamera(int index, int newIndex) {
	cout << "Previous active Camera is : " << index << endl;
	scene->setActiveCamera(newIndex % scene->getCameras().size());
	cout << "Current active Camera is : " << newIndex << endl;
	scene->draw();
}

void sceneDrawNormals() {
	bool drawNorms = renderer->getDrawNormals();
	cout << "Draw Normals was : " << drawNorms << endl;
	renderer->setDrawNormals(!drawNorms);
	cout << "And now it is : " << renderer->getDrawNormals() << endl;
	scene->draw();
}

void resetScene(vector<shared_ptr<Model>>& currentModels, vector<shared_ptr<Camera>>& currentCameras) {
	for (auto& m : currentModels) {
		m->reset();
	}
	scene->draw();
}

void setFrustrum(Camera& c) {
	CCmdDialog dlg("Insert Left, Right, Bottom, Top, zNear and zFar in this order");
	if (dlg.DoModal() == IDOK) {
		string input = dlg.GetCmd();
		std::istringstream iss(input);
		string params[6];
		for (int i = 0; i < 6; i++) {
			iss >> params[i];
		}
		float left = ::atof(params[0].c_str());
		float right = ::atof(params[1].c_str());
		float bottom = ::atof(params[2].c_str());
		float top = ::atof(params[3].c_str());
		float zNear = ::atof(params[4].c_str());
		float zFar = ::atof(params[5].c_str());
		c.setFrustrumBoundries(left, right, bottom, top, zNear, zFar);
	}
}

void changeMaterial(vector<shared_ptr<Model>>& currentModels, const int activeModel) {
	vec3 ambient = 0.0;
	vec3 diffuse = 0.0;
	vec3 specular = 0.0;

	materialDialog(ambient, diffuse, specular);

	currentModels[activeModel]->setMaterial(Material(ambient, diffuse, specular));
}

void keyboard(unsigned char key, int x, int y)
{	
	vector<shared_ptr<Model>>& currentModels = scene->getModels();
	vector<shared_ptr<Camera>>& currentCameras = scene->getCameras();
	
	int index = scene->getActiveModel();
	int cameraIndex = scene->getActiveCamera();

	int newIndex;

	switch (key) {
	case 0x1B:
		exit(EXIT_SUCCESS);
		break;
	case 0x2E:
		// Keyboard : >
		applyTransformation(1.1);
		break;
	case 0x2C:
		// Keyboard : <
		applyTransformation(-1.1);
		break;
	case 0x61:
		// Keyboard : A
		selectAllAxes();
		break;
	case 0x78:
		// Keyboard : X
		selectXAxis();
		break;
	case 0x79:
		// Keyboard : Y
		selectYAxis();
		break;
	case 0x7A:
		// keyboard : Z
		selectZAxis();
		break;
	case 0x53:
		// Keyboard : Shift + s
		cout << "Selected Spinning Action" << endl;
		selectedAction = spin;
		break;
	case 0x73:
		// Keyboard : s
		cout << "Selected Scaling Action" << endl;
		selectedAction = scale;
		break;
	case 0x72:
		// Keyboard : r
		cout << "Selected Rotation Action" << endl;
		selectedAction = rotatee;
		break;
	case 0x74:
		// Keyboard : t
		cout << "Selected Translation Action" << endl;
		selectedAction = translate;
		break;
	case 0x6C:
		// Keyboard : l
		tempLookAt(*currentCameras[cameraIndex]);
		break;
	case 0x4C:
		scene->setActiveCameraLookAtModel(scene->getActiveModel());
		display();
		break;
	case 0x6F:
		// Keyboard : o
		cout << "Orthographic projection: " << endl;
		currentCameras[cameraIndex]->Ortho();
		scene->draw();
		break;
	case 0x70:
		// Keyboard : p
		cout << "Perspective projection: " << endl;
		currentCameras[cameraIndex]->Frustum();
		scene->draw();
		break;
	case 0x7F:
		// Keyboard : Delete
		deleteModels(currentModels);
		break;
	case 0x60:
		// Keyboard : ~
		resetScene(currentModels, currentCameras);
		break;
	case 0x5B:
		// Keyboard : [
		selectSpecificModel(currentModels, index, index - 1);
		break;
	case 0x5D:
		// Keyboard : ]
		selectSpecificModel(currentModels, index, index + 1);
		break;
	case 0x5C:
		// Keyboard : '\'
		selectAllModels(currentModels);
		break;
	case 0x7B:
		// Keyboard : Shift + [
		selectSpecificCamera(cameraIndex, cameraIndex - 1);
		break;
	case 0x7D:
		// Keyboard : Shift + ]
		selectSpecificCamera(cameraIndex, cameraIndex + 1);
		break;
	case 0x6E:
		// Keyboard : n
		sceneDrawNormals();
		break;
	case 0x66:
		// Keyboard : f
		setFrustrum(*currentCameras[cameraIndex]);
		scene->draw();
		break;
	case 0x63:
		// Keyboard : c
		newIndex = scene->setCameraAsActiveModel(cameraIndex);
		selectSpecificModel(currentModels, index, newIndex);
		index = newIndex;
		break;
	case 0x6D:
		// Keyboard : m
		changeMaterial(currentModels, index);
		scene->draw();
		break;
	case 0x4D:
		// Keyboard : Shift + m
		currentModels[index]->setRandomMaterial();
		scene->draw();
		break;
	case 0x4E:
		// Keyboard : Shift + n
		bool drawFaceNorms = renderer->getDrawFaceNormals();
		renderer->setDrawFaceNormals(!drawFaceNorms);
		scene->draw();
		break;
	}

}

void mouse(int button, int state, int x, int y)
{
	//button = {GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON}
	//state = {GLUT_DOWN,GLUT_UP}

	//set down flags
	switch (button) {
	case GLUT_LEFT_BUTTON:
		lb_down = (state == GLUT_UP) ? 0 : 1;
		break;
	case GLUT_RIGHT_BUTTON:
		rb_down = (state == GLUT_UP) ? 0 : 1;
		break;
	case GLUT_MIDDLE_BUTTON:
		mb_down = (state == GLUT_UP) ? 0 : 1;
		break;
	}

	// add your code
	last_x = x;
	last_y = y;
}

void motion(int x, int y)
{
	// calc difference in mouse movement
	int dx = x - last_x;
	int dy = -(y - last_y);
	int dx_diff = abs(abs(dx) - last_dx);
	int dy_diff = abs(abs(dy) - last_dy);

	last_x = x;
	last_y = y;
	/*last_dx = abs(dx);
	last_dy = abs(dy);

	//cout << "dx_diff:" << dx_diff << ", dy_diff:" << dy_diff << endl;
	if (dx_diff > MOUSE_SMOOTH || dy_diff > MOUSE_SMOOTH){
		//cout << "Smoothed" << endl;
		return;
	}*/

	// my addition
	float sensitivity = 1.f;
	GLfloat pixels = sensitivity * ((GLfloat)dx / glutGet(GLUT_WINDOW_WIDTH) + (GLfloat)dy / glutGet(GLUT_WINDOW_HEIGHT));
	if (selectedAction != scale) {
		applyTransformation(pixels);
	}

}

void fileMenu(int id)
{
	switch (id)
	{
	case FILE_OPEN:
		CFileDialog dlg(TRUE, _T(".obj"), NULL, NULL, _T("*.obj|*.*"));
		if (dlg.DoModal() == IDOK)
		{
			std::string s((LPCTSTR)dlg.GetPathName());
			scene->loadOBJModel((LPCTSTR)dlg.GetPathName());
			display();
		}
		break;
	}
}

void textureMenu(int id) {
	switch (id) {
	case LOAD_TEXTURE:
		CFileDialog dlg(TRUE, _T(".png"), NULL, NULL, _T("*.png|*.*"));
		if (dlg.DoModal() == IDOK)
		{
			std::string s((LPCTSTR)dlg.GetPathName());
			int activeModel = scene->getActiveModel();
			scene->getModels()[activeModel]->setTexture((LPCTSTR)dlg.GetPathName());
			display();
		}
		break;
	}
}

void axisMenu(int id) {
	switch (id)
	{
	case X_AXIS:
		xAxisBool = true;
		yAxisBool = zAxisBool = allAxesBool = false;
		selectedAxis = X;
		break;
	case Y_AXIS:
		yAxisBool = true;
		xAxisBool = zAxisBool = allAxesBool = false;
		selectedAxis = Y;
		break;
	case Z_AXIS:
		zAxisBool = true;
		xAxisBool = yAxisBool = allAxesBool = false;
		selectedAxis = Z;
		break;
	case ALL_AXES:
		allAxesBool = true;
		xAxisBool = yAxisBool = zAxisBool = false;
		selectedAxis = ALL;
		break;
	default:
		break;
	}
}

void actionMenu(int id) {
	switch (id)
	{
	case TRANSLATION_ACTION:
		selectedAction = translate;
		break;
	case SPIN_ACTION:
		selectedAction = spin;
		break;
	case SCALING_ACTION:
		selectedAction = scale;
		break;
	case ROTATE_ACTION:
		selectedAction = rotatee;
		break;
	default:
		break;
	}
}

void meshMenu(int id){
	switch (id){
	case LOAD_CUBE:
		scene->loadMeshModel(PrimMeshModel());
		display();
		break;
	default:
		break;
	}
}

void cameraMenu(int id) {
	CameraLookAtError err;
	vector<shared_ptr<Camera>>& cameras = scene->getCameras();
	switch (id)
	{
	case LOAD_CAMERA:
		scene->loadCamera();
		err = lookAt(*(cameras[scene->getActiveCamera()]));
		if (CameraLookAtError::OK != err) {
			scene->deleteCamera(scene->getActiveCamera());
		}
		else {
			scene->draw();
		}
		break;
	default:
		break;
	}
}

void mainMenu(int id)
{
	switch (id)
	{
	case MAIN_DEMO:
		scene->drawDemo();
		break;
	case MAIN_ABOUT:
		AfxMessageBox(_T("Shmulik and Eyal RULEZ!"));
		break;
	}
}

void shadingMenu(int id) {
	switch (id)
	{
	case FLAT_SHADING:
		renderer->setColorMethod(ColorMethod::FLAT);
		break;
	case GOURAUD_SHADING:
		renderer->setColorMethod(ColorMethod::GOURAUD);
		break;
	case PHONG_SHADING:
		renderer->setColorMethod(ColorMethod::PHONG);
		break;
	default:
		assert(false);
		break;
	}
	scene->draw();
}

void lightMenu(int id) {
	vec3 ambient = 0.0;
	vec3 diffuse = 0.0;
	vec3 specular = 0.0;
	switch (id)
	{
	case POINT_LIGHT:
	{
		CXyzDialog pointDlg("Point");
		vec4 point;
		materialDialog(ambient, diffuse, specular);
		cout << "Choose Camera Point" << endl;
		if (pointDlg.DoModal() == IDOK) {
			vec3 temp = pointDlg.GetXYZ();
			point = vec4(temp.x, temp.y, temp.z, 1);
		}
		shared_ptr<Light> l(new PointLight(Material(ambient,diffuse,specular), point));
		scene->loadLight(l);
	}
		break;
	case PARALLEL_LIGHT:
	{
		CXyzDialog normalDlg("Parallel");
		vec4 normal;

		cout << "Choose Light Direction" << endl;
		if (normalDlg.DoModal() == IDOK) {
			vec3 temp = normalDlg.GetXYZ();
			normal = vec4(temp.x, temp.y, temp.z, 0);
		}
		if (normal == vec4(0, 0, 0, 0)) { return;  }
		shared_ptr<Light> l(new ParallelLight(Material(ambient, diffuse, specular), normal));
		scene->loadLight(l);
	}
		break;
	default:
		assert(false);
		break;
	}
	scene->draw();
}

void effectMenu(int id) {
	switch (id)
	{
	case SILHOUETTE:
	{
		if (renderer->getSilhouette()) {
			renderer->setSilhouette(false);
		}
		else {
			renderer->setSilhouette(true);
		}
		std::cout << "Silhouette is: " << renderer->getSilhouette() << std::endl;
	}
	case TOON:
	{
		if (renderer->getToon()) {
			renderer->setToon(false);
		}
		else {
			renderer->setToon(true);
		}
		std::cout << "Toon is: " << renderer->getToon() << std::endl;
	}
	default:
		break;
	}
	scene->draw();
}

void initMenu()
{
	int menuFile = glutCreateMenu(fileMenu);
	glutAddMenuEntry("Open..", FILE_OPEN);

	// Axis menu
	int menuAxis = glutCreateMenu(axisMenu);
	glutAddMenuEntry("x", X_AXIS);
	glutAddMenuEntry("y", Y_AXIS);
	glutAddMenuEntry("z", Z_AXIS);
	glutAddMenuEntry("all", ALL_AXES);

	// Load Light
	int menuLight = glutCreateMenu(lightMenu);
	glutAddMenuEntry("Point Light", POINT_LIGHT);
	glutAddMenuEntry("Parallel Light", PARALLEL_LIGHT);

	// Shading menu
	int menuShading = glutCreateMenu(shadingMenu);
	glutAddMenuEntry("Flat", FLAT_SHADING);
	glutAddMenuEntry("Gouraud", GOURAUD_SHADING);
	glutAddMenuEntry("Phong", PHONG_SHADING);

	// Predefined shapes
	int menuMeshModel = glutCreateMenu(meshMenu);
	glutAddMenuEntry("Cube", LOAD_CUBE);

	// Load menu
	int menuCamera = glutCreateMenu(cameraMenu);
	glutAddMenuEntry("Load Camera", LOAD_CAMERA);

	// Action menu
	int menuAction = glutCreateMenu(actionMenu);
	glutAddMenuEntry("Translation", TRANSLATION_ACTION);
	glutAddMenuEntry("Spin", SPIN_ACTION);
	glutAddMenuEntry("Scaling", SCALING_ACTION);
	glutAddMenuEntry("Rotation", ROTATE_ACTION);

	// TEXTURES menu
	int menuTextures = glutCreateMenu(textureMenu);
	glutAddMenuEntry("Load texture", LOAD_TEXTURE);

	// Special Effects menu
	int menuEffects = glutCreateMenu(effectMenu);
	glutAddMenuEntry("Silhouette", SILHOUETTE);
	glutAddMenuEntry("Toon", TOON);

	glutCreateMenu(mainMenu);
	glutAddSubMenu("File", menuFile);
	glutAddSubMenu("Predefined models", menuMeshModel);
	glutAddSubMenu("Cameras",menuCamera);
	//glutAddMenuEntry("Demo", MAIN_DEMO);
	//glutAddSubMenu("Choose Axis", menuAxis);
	//glutAddSubMenu("Choose Action", menuAction);
	glutAddSubMenu("Choose Shading Algorithm", menuShading);
	glutAddSubMenu("Load texture", menuTextures);
	glutAddSubMenu("Insert Special Effects", menuEffects);
	glutAddSubMenu("Add Light", menuLight);
	glutAddMenuEntry("About", MAIN_ABOUT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
//----------------------------------------------------------------------------



int my_main(int argc, char **argv)
{

	//----------------------------------------------------------------------------
	// Initialize window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(INIT_WIDTH, INIT_HEIGHT);
	glutInitContextVersion(OPEN_GL_VERSION);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("CG");
	glewExperimental = GL_TRUE;
	glewInit();
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		/*		...*/
	}
	glGetError();
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

#ifdef NDEBUG
	printf("NDEBUG is defined\n");
#else
	printf("NDEBUG is NOT defined\n");
#endif
#ifdef DEBUG_PRINT
	printf("DEBUG_PRINT is defined\n");
#else
	printf("DEBUG_PRINT is NOT defined\n");
#endif
	glEnable(GL_DEPTH_TEST);

	renderer = new Renderer(INIT_WIDTH, INIT_HEIGHT);
	scene = new Scene(renderer);
	scene->loadCamera();
	shared_ptr<Light> p (new PointLight(Material(), vec4(0, 0, 1, 1)));
	//shared_ptr<Light> p(new ParallelLight(Material(), vec4(0, 0, -1, 0)));
	scene->loadLight(p);

	//----------------------------------------------------------------------------
	// Initialize Callbacks

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	initMenu();


	glutMainLoop();
	delete scene;
	delete renderer;
	return 0;
}

CWinApp theApp;

using namespace std;

int main(int argc, char **argv)
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		my_main(argc, argv);
	}

	return nRetCode;
}
