
#include "perfMeasurement\CurvatureApproximation.h"

namespace perfMeasurement
{
	void CurvatureApproximation::Update(tdble deltaTimeIncrement, tdble currentTime)
	{
		// Retrieve current damage
		curDmg = car->priv.dammage;

		// Check for collisions or leaving the track
		if (curDmg > prevDmg)
			collision = true;
		if (car->pub.trkPos.toMiddle > (car->pub.trkPos.seg->width / 2))
			offTrack = true;
		
		// Assign previous damage
		prevDmg = curDmg;

		// When neccesary, generate a new outlook
		if (currentOutlook.IsClear() == true)
		{
			if (car->pub.trkPos.seg->next->type != TR_STR)
			{
				currentOutlook.GenerateOutlook(car->pub.trkPos.seg, car->info.dimension.x, car->info.dimension.z);
				pmOut("Calculated optimal radius for upcoming corner as: %s", std::to_string(currentOutlook.optimalRadius));
				
				// Output the current optimal points
				OutputOptimalPoints(currentOutlook.optimalPoints, 3);
			}
		}
		else // Otherwise, update the current outlook
		{
			currentOutlook.Update(car, dataSet, currentTime);

			if (dataSet.size() == 3) // If the dataset is ready to be evaluated, do so
			{
				Evaluate();

				// Clear data set
				currentOutlook.Clear();
				dataSet.clear();

				// Reset booleans 
				offTrack = false;
				collision = false;
			}
		}
	}

	void CurvatureApproximation::Evaluate()
	{
		pmOut("Evaluating skill level using race line evaluation...\n");

		if (!offTrack && !collision) // Only evaluate if no collisions or track leaves
		{
			// Retrieve the actual passed points from the dataset
			PMPoint2D* actualPoints = new PMPoint2D[dataSet.size()];
			for (int i = 0; i < dataSet.size(); i++)
			{
				actualPoints[i] = PMPoint2D();
				actualPoints[i].x = dataSet[i].GetData().GlobalPosition().x;
				actualPoints[i].y = dataSet[i].GetData().GlobalPosition().y;
			}

			// Calculate the actual radius
			currentOutlook.actualRadius = abs(radius(actualPoints[0].x, actualPoints[0].y, actualPoints[1].x, actualPoints[1].y, actualPoints[2].x, actualPoints[2].y));

			pmOut("Actual Radius: %s", std::to_string(currentOutlook.actualRadius));
			pmOut("Optimal Radius: %s", std::to_string(currentOutlook.optimalRadius));

			// Calculate ratios
			tdble radiusRatio = currentOutlook.actualRadius / currentOutlook.optimalRadius;
			currentEstimate = radiusRatio;

			// In order for lerp to work when generating segments, value cannot exceed 1
			if (currentEstimate > 1)
				currentEstimate = 1.0f;

			delete[] actualPoints;
		}
		else // If collision or off track, discount this evaluation
			currentEstimate = 0.0;

		// Calculate new average skill level
		nEstimates++;
		totalEstimate += currentEstimate;
		avgEstimate = totalEstimate / nEstimates;

		// Add to estimates collection
		estimates.push_back(currentEstimate);
	}

	void CurvatureApproximation::OutputOptimalPoints(PMPoint2D* points, int count)
	{
		std::fstream output = std::fstream(optimalOutputFile, std::ios::app);
		for (int i = 0; i < count; i++)
			output << points[i].x << "," << points[i].y << "\n";
		output.close();
	}
}