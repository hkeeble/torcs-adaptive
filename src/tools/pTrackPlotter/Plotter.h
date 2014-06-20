#ifndef _PLOTTER_H_
#define _PLOTTER_H_

#include <GL/glut.h>
#include <iostream>
#include "TrackPlot.h"

class Plotter
{
public:
	Plotter();
	~Plotter();

	void Run(int argc, char **argv);

	bool renderOptimalLine, renderActualLine;

private:
	// Window width and height
	float width, height;

	// Pointer to self
	static Plotter* instance;

	// The track plot
	TrackPlot plot;

	// Initialization Functions
	void InitGLUT(int argc, char **argv);
	void InitGL();

	// Concrete callback functions
	void render();
	void keyPress(unsigned char key, int x, int y);
	void specialKey(int key, int x, int y);
	void mouse(int button, int state, int x, int y);
		
	// Static callback functions
	static void renderWrapper() { instance->render(); }
	static void idleWrapper() { instance->render(); }
	static void specialKeyWrapper(int key, int x, int y) { instance->specialKey(key, x, y); }
	static void keyPressWrapper(unsigned char key, int x, int y) { instance->keyPress(key, x, y); }
	static void mouseWrapper(int button, int state, int x, int y) { instance->mouse(button, state, x, y); }
};

#endif _PLOTTER_H_