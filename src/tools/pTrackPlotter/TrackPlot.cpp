#include <iostream>
#include <fstream>
#include "TrackPlot.h"
#include "stringutil.h"

TrackPlot::TrackPlot()
{

}

TrackPlot::TrackPlot(std::string fileName)
{
	// Initialize max and minimum constraints
	max = Point2D(0, 0);
	min = Point2D(0, 0);

	// Initialize vertices
	trackPoints = std::vector<Point2D>();
	optimalLine = std::vector<Point2D>();
	actualLine = std::vector<Point2D>();

	// Open the file
	std::ifstream input = std::ifstream(fileName.c_str());

	std::string line;

	std::getline(input, line);

	// Read Track
	do
	{
		std::vector<std::string> coords = split(line, ',');
		trackPoints.push_back(Point2D(std::atof(coords[0].c_str()), std::atof(coords[1].c_str())));
		
		if (trackPoints.back().x < min.y)
			min.x = trackPoints.back().x;
		if (trackPoints.back().y < min.y)
			min.y = trackPoints.back().y;

		if (trackPoints.back().x > max.x)
			max.x = trackPoints.back().x;
		if (trackPoints.back().y > max.y)
			max.y = trackPoints.back().y;

		std::getline(input, line);
	} while (line != "");

	std::getline(input, line);

	// Read Optimal Line
	do
	{
		std::vector<std::string> coords = split(line, ',');
		optimalLine.push_back(Point2D(std::atof(coords[0].c_str()), std::atof(coords[1].c_str())));
		std::getline(input, line);
	} while (line != "");

	std::getline(input, line);

	// Read Actual Line
	do
	{
		std::vector<std::string> coords = split(line, ',');
		actualLine.push_back(Point2D(std::atof(coords[0].c_str()), std::atof(coords[1].c_str())));
		std::getline(input, line);
	} while (!input.eof());
}

Point2D TrackPlot::Max()
{
	return max;
}

Point2D TrackPlot::Min()
{
	return min;
}

TrackPlot::~TrackPlot()
{

}

void TrackPlot::renderTrack()
{
	glColor3f(0.0f, 0.0f, 0.0f);

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < trackPoints.size(); i++)
		glVertex2d(trackPoints[i].x, trackPoints[i].y);
	glEnd();
}

void TrackPlot::renderOptimalLine()
{
	glColor3f(1.0f, 0.5f, 0.0f);

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < optimalLine.size(); i++)
		glVertex2d(optimalLine[i].x, optimalLine[i].y);
	glEnd();
}

void TrackPlot::renderActualLine()
{
	glColor3f(1.0f, 0.0f, 0.0f);

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < actualLine.size(); i++)
		glVertex2d(actualLine[i].x, actualLine[i].y);
	glEnd();
}