
#include "perfMeasurement\RaceLineEvaluation.h"

namespace perfMeasurement
{
	void RaceLineEvaluation::Update(tdble deltaTimeIncrement, tdble currentTime)
	{
		car.Update();

		// Retrieve current damage
		curDmg = car.GetCar()->priv.dammage;

		// Check for collisions or leaving the track
		if (curDmg > prevDmg)
			collision = true;
		if (car.LocalPosition().toMiddle > (car.LocalPosition().seg->width / 2))
			offTrack = true;
		
		// Assign previous damage
		prevDmg = curDmg;

		// When neccesary, generate a new outlook
		if (currentOutlook.IsClear() == true)
		{
			if (car.CurrentSeg()->next->type != TR_STR)
			{
				currentOutlook.GenerateOutlook(car.CurrentSeg(), car.GetCar()->info.dimension.x, car.GetCar()->info.dimension.z);
				pmOut("Calculated optimal radius for upcoming corner as: " + std::to_string(currentOutlook.optimalRadius) + "\n");
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

	void RaceLineEvaluation::Evaluate()
	{
		pmOut("Evaluating skill level using race line evaluation...\n");

		if (!offTrack && !collision) // Only evaluate if no collisions or track leaves
		{
			// Retrieve the actual passed points from the dataset
			PMPoint2D* actualPoints = new PMPoint2D[dataSet.size()];
			for (int i = 0; i < dataSet.size(); i++)
			{
				actualPoints[i] = PMPoint2D();
				actualPoints[i].x = dataSet.at(i).Data().GlobalPosition().x;
				actualPoints[i].y = dataSet.at(i).Data().GlobalPosition().y;
			}

			// Calculate the actual radius
			currentOutlook.actualRadius = abs(radius(actualPoints[0].x, actualPoints[0].y, actualPoints[1].x, actualPoints[1].y, actualPoints[2].x, actualPoints[2].y));

			pmOut("Actual Radius: " + std::to_string(currentOutlook.actualRadius) + "\n");
			pmOut("Optimal Radius: " + std::to_string(currentOutlook.optimalRadius) + "\n");

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
}