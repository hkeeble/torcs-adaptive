#include "perfMeasurement\Pathfinder.h"

namespace perfMeasurement
{
	void Pathfinder::Output(std::string file)
	{
		std::fstream output = std::fstream(file, std::ios::app);
		for (int i = pointsOnLastOutput; i < segments.size(); i++)
			output << segments[i].getLoc().x << "," << segments[i].getLoc().y << std::endl;
		pointsOnLastOutput = segments.size();
		output.close();
	}
}