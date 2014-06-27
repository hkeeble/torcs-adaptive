#ifndef _TRACK_PLOT_H_
#define _TRACK_PLOT_H_

#include <string>
#include <vector>
#include <GL/glut.h>
#include "Point2D.h"

const std::string optimalFile = "optimal.dat";
const std::string actualFile = "actual.dat";

class TrackPlot
{
public:
	TrackPlot();
	TrackPlot(std::string config, std::string track);
	~TrackPlot();

	void renderTrack();
	void renderOptimalLine();
	void renderActualLine();

	Point2D Max();
	Point2D Min();

private:
	std::vector<Point2D> trackPoints;
	std::vector<Point2D> optimalLine;
	std::vector<Point2D> actualLine;



	Point2D max, min;
};

#endif // TRACK_PLOT_H_