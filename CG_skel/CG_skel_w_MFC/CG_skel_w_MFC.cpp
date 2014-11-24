// CG_skel_w_MFC.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CG_skel_w_MFC.h"
#include <algorithm>



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

void doTranslation(vector<shared_ptr<Model>>& currentModels, int activeModel, float intensity) {
	if (activeModel != ALL_MODELS_ACTIVE) {
		cout << "Translating by : " << intensity << " In " << selectedAxis << " Axis Model number" << activeModel << endl;
		currentModels[activeModel]->translate((allAxesBool + xAxisBool)*intensity,
			(allAxesBool + yAxisBool)*intensity,
			(allAxesBool + zAxisBool)*intensity);
	}
	else {
		for (auto &m : currentModels)
		{
			cout << "Translating by : " << intensity << " In " << selectedAxis << " Axis" << endl;
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
		cout << " Scaling by : " << intensity << "The model :" << activeModel << endl;
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
			cout << " Scaling by : " << intensity << endl;
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
		cout << "spinning by : " << intensity << " In" << selectedAxis << " Axis The model : " << activeModel << endl;
		currentModels[activeModel]->spin(intensity, selectedAxis);
	}
	else {
		for (auto& m : currentModels) {
			cout << "spinning by : " << intensity << " In" << selectedAxis << " Axis" << endl;
			m->spin(intensity, selectedAxis);
		}
	}
	scene->draw();
}

void doRotate(vector<shared_ptr<Model>>& currentModels, int activeModel, float intensity) {
	if (activeModel != ALL_MODELS_ACTIVE) {
		cout << "Rotating By : " << intensity << "In " << selectedAxis << " Axis The model : " << activeModel << endl;
		currentModels[activeModel]->rotate(intensity, selectedAxis);
	}
	else {
		for (auto &m : currentModels) {
			cout << "Rotating By : " << intensity << "In " << selectedAxis << " Axis" << endl;
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
	for (auto& c : currentCameras) {
		c->reset();
	}
	for (auto& m : currentModels) {
		m->reset();
	}
	scene->draw();
}

void keyboard(unsigned char key, int x, int y)
{	
	vector<shared_ptr<Model>>& currentModels = scene->getModels();
	vector<shared_ptr<Camera>>& currentCameras = scene->getCameras();
	
	int index = scene->getActiveModel();
	int cameraIndex = scene->getActiveCamera();

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
		currentCameras[cameraIndex]->Ortho(-1.0, 1.0, -1.0, 1.0, 0.5, 10.0);
		scene->draw();
		break;
	case 0x70:
		// Keyboard : p
		cout << "Perspective projection: " << endl;
		currentCameras[cameraIndex]->Frustum(-1.0, 1.0, -1.0, 1.0, 0.5, 10.0);
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
	last_dx = abs(dx);
	last_dy = abs(dy);

	//cout << "dx_diff:" << dx_diff << ", dy_diff:" << dy_diff << endl;
	if (dx_diff > MOUSE_SMOOTH || dy_diff > MOUSE_SMOOTH){
		//cout << "Smoothed" << endl;
		return;
	}

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

void initMenu()
{
	int menuFile = glutCreateMenu(fileMenu);
	glutAddMenuEntry("Open..", FILE_OPEN);

	// my addition
	int menuAxis = glutCreateMenu(axisMenu);
	glutAddMenuEntry("x", X_AXIS);
	glutAddMenuEntry("y", Y_AXIS);
	glutAddMenuEntry("z", Z_AXIS);
	glutAddMenuEntry("all", ALL_AXES);

	// my addition
	int menuMeshModel = glutCreateMenu(meshMenu);
	glutAddMenuEntry("Cube", LOAD_CUBE);

	// my addition
	int menuCamera = glutCreateMenu(cameraMenu);
	glutAddMenuEntry("Load Camera", LOAD_CAMERA);

	// my addition
	int menuAction = glutCreateMenu(actionMenu);
	glutAddMenuEntry("Translation", TRANSLATION_ACTION);
	glutAddMenuEntry("Spin", SPIN_ACTION);
	glutAddMenuEntry("Scaling", SCALING_ACTION);
	glutAddMenuEntry("Rotation", ROTATE_ACTION);

	glutCreateMenu(mainMenu);
	glutAddSubMenu("File", menuFile);
	glutAddSubMenu("Predefined models", menuMeshModel);
	glutAddSubMenu("Cameras",menuCamera);
	glutAddMenuEntry("Demo", MAIN_DEMO);
	glutAddSubMenu("Choose Axis", menuAxis);
	glutAddSubMenu("Choose Action", menuAction);
	glutAddMenuEntry("About", MAIN_ABOUT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
//----------------------------------------------------------------------------



int my_main(int argc, char **argv)
{
	//----------------------------------------------------------------------------
	// Initialize window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(INIT_WIDTH, INIT_HEIGHT);
	glutInitContextVersion(3, 2);
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
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));



	renderer = new Renderer(INIT_WIDTH, INIT_HEIGHT);
	scene = new Scene(renderer);
	scene->loadCamera();
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
