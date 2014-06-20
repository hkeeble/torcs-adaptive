#include "Plotter.h"

Plotter* Plotter::instance = nullptr;

Plotter::Plotter()
{
	instance = this;

	width = 800;
	height = 600;

	renderOptimalLine = true;
	renderActualLine = true;
}


Plotter::~Plotter()
{

}

void Plotter::Run(int argc, char **argv)
{
	if (argc == 1)
	{
		std::cout << "Error, no arguments passed. Usage: pTrackPlotter [data file]" << std::endl;
		exit(0);
	}
	else
		plot = TrackPlot(std::string(argv[1]));

	InitGLUT(argc, argv);
	InitGL();

	glutMainLoop();
}

void Plotter::InitGLUT(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Procedural Track Plotter v1.0");

	glutDisplayFunc(renderWrapper);
	glutIdleFunc(renderWrapper);
	glutKeyboardFunc(keyPressWrapper);
	glutSpecialFunc(specialKeyWrapper);
	glutMouseFunc(mouseWrapper);
}

void Plotter::InitGL()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glViewport(0, 0, width, height);
	glLineWidth(3.f);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(plot.Min().x, plot.Max().x, plot.Max().y, plot.Min().y);
	glMatrixMode(GL_MODELVIEW);
}

void Plotter::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	plot.renderTrack();

	if (renderOptimalLine)
		plot.renderOptimalLine();
	
	if (renderActualLine)
		plot.renderActualLine();

	glutSwapBuffers();
}

void Plotter::keyPress(unsigned char key, int x, int y)
{
	if (key == 'z')
	{
		glMatrixMode(GL_PROJECTION);
		glScalef(1.1f, 1.1f, 1.1f);
		glMatrixMode(GL_MODELVIEW);
	}
	if (key == 'x')
	{
		glMatrixMode(GL_PROJECTION);
		glScalef(0.9f, 0.9f, 0.9f);
		glMatrixMode(GL_MODELVIEW);
	}

	if (key == 'i')
		renderOptimalLine = !renderOptimalLine;
	if (key == 'o')
		renderActualLine = !renderActualLine;
}

void Plotter::specialKey(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT)
	{
		glMatrixMode(GL_MODELVIEW);
		glTranslatef(10, 0, 0);
	}
	if (key == GLUT_KEY_RIGHT)
	{
		glMatrixMode(GL_MODELVIEW);
		glTranslatef(-10, 0, 0);
	}
	if (key == GLUT_KEY_UP)
	{
		glMatrixMode(GL_MODELVIEW);
		glTranslatef(0, 10, 0);
	}
	if (key == GLUT_KEY_DOWN)
	{
		glMatrixMode(GL_MODELVIEW);
		glTranslatef(0, -10, 0);
	}
}

void Plotter::mouse(int button, int state, int x, int y)
{
	if ((button == 3) || (button == 4))
	{
		if (button = 3)
		{
			
		}
	}
}