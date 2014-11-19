// CG_skel_w_MFC.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CG_skel_w_MFC.h"


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
#include <string>

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

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

Scene *scene;
Renderer *renderer;

int last_x, last_y;
bool lb_down, rb_down, mb_down;

// My addition

enum Action { translate, rotatee, spin, scale };

bool allAxesBool, yAxisBool, zAxisBool = false;
bool xAxisBool = true;

Axes selectedAxis = X;

Action selectedAction = translate;

void applyTransformation(float intensity) {
	vector<Model>& currentModels = scene->getModels();
	switch (selectedAction)
	{
	case translate:
		for (auto &m : currentModels)
		{
			cout << "Translating by : " << intensity << " In " << selectedAxis << " Axis";
			m.translate((allAxesBool + xAxisBool)*intensity,
				(allAxesBool + yAxisBool)*intensity,
				(allAxesBool + zAxisBool)*intensity);
		}
		scene->draw();
		break;
	case scale:
		for (auto &m : currentModels)
		{
			cout << "Scaling by : " << intensity << "In " << selectedAxis << " Axis";
			m.scale((allAxesBool + xAxisBool)*intensity,
				(allAxesBool + yAxisBool)*intensity,
				(allAxesBool + zAxisBool)*intensity);
		}
		scene->draw();
		break;
	case spin:
		for (auto &m : currentModels) {
			cout << "Rotating by : " << intensity << " In" << selectedAxis << " Axis";
			m.spin(intensity, selectedAxis);
		}
		scene->draw();
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

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 0x1B:
		exit(EXIT_SUCCESS);
		break;
	case GLUT_KEY_UP:
		applyTransformation(10.0);
		break;
	case GLUT_KEY_DOWN:
		applyTransformation(-10.0);
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

	// my addition
	float sensitivity = 1.f;
	GLfloat pixels = sensitivity * ((GLfloat)dx / glutGet(GLUT_WINDOW_WIDTH) + (GLfloat)dy / glutGet(GLUT_WINDOW_HEIGHT));
	applyTransformation(pixels);
	// update last x,y
	last_x = x;
	last_y = y;
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
	int menuAction = glutCreateMenu(actionMenu);
	glutAddMenuEntry("Translation", TRANSLATION_ACTION);
	glutAddMenuEntry("Rotation", SPIN_ACTION);
	glutAddMenuEntry("Scaling", SCALING_ACTION);

	glutCreateMenu(mainMenu);
	glutAddSubMenu("File", menuFile);
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
	glutInitWindowSize(512, 512);
	glutInitContextVersion(2, 0);
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



	renderer = new Renderer(512, 512);
	scene = new Scene(renderer);
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
