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
#define ROTATION_ACTION 8
#define SCALING_ACTION 9

Scene *scene;
Renderer *renderer;

int last_x, last_y;
bool lb_down, rb_down, mb_down;

// My addition

enum Action { translate, rotatee, scale };

bool xAxisBool, yAxisBool, zAxisBool = false;
bool allAxesBool = true;

Axes selectedAxis = all;

Action selectedAction = translate;

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
	case 033:
		exit(EXIT_SUCCESS);
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
	int dy = y - last_y;

	// my addition
	vector<Model>& currentModels = scene->getModels(); 
	switch (selectedAction)
	{
	case translate:
		for (vector<Model>::iterator it = currentModels.begin(); it != currentModels.end(); ++it)
		{
			cout << "Translating by : " << dx + dy << "In " << selectedAxis << " Axis";
			it->translate((allAxesBool + xAxisBool)*(dx + dy), (allAxesBool + yAxisBool)*(dx + dy), (allAxesBool + zAxisBool)*(dx + dy));
		}
		scene->draw();
		break;
	case scale:
		for (vector<Model>::iterator it = currentModels.begin(); it != currentModels.end(); ++it)
		{
			cout << "Scaling by : " << dx + dy << "In " << selectedAxis << " Axis";
			it->scale((allAxesBool + xAxisBool)*(dx + dy), (allAxesBool + yAxisBool)*(dx + dy), (allAxesBool + zAxisBool)*(dx + dy));
		}
		break;
	default:
		break;
	}

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
		selectedAxis = x;
		break;
	case Y_AXIS:
		yAxisBool = true;
		xAxisBool = zAxisBool = allAxesBool = false;
		selectedAxis = y;
		break;
	case Z_AXIS:
		zAxisBool = true;
		xAxisBool = yAxisBool = allAxesBool = false;
		selectedAxis = z;
		break;
	case ALL_AXES:
		allAxesBool = true;
		xAxisBool = yAxisBool = zAxisBool = false;
		selectedAxis = all;
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
	case ROTATION_ACTION:
		selectedAction = rotatee;
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
	glutAddMenuEntry("Rotation", ROTATION_ACTION);
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
