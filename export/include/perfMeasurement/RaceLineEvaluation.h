/*
	File: RaceLineEvaluation.h
	Author: Henri Keeble
	Desc: Declares an evaluator class designed to evaluate the racing line of the target car.
*/

#ifndef _RACE_LINE_EVAL_H_
#define _RACE_LINE_EVAL_H_

#include "PMEvaluator.h"

namespace perfMeasurement
{
	/* Evaluates performance based upon the car's deviation from the optimal racing line within the given data set. */
	class RaceLineEvaluation : public PMEvaluator
	{
	private:
		
		/* Nested class used to represent an outlook on a corner. */
		class CornerOutlook
		{
		private:
			bool clear;

			// The number of optimal points
			const int NUMB_OF_OPTIMAL_POINTS;

			/*
			 * Used to calculate the optimal points the optimal arc must pass through.
			 * carWidth Width of the cuurent car.
			 * carDepth Depth (Z dimension) of the current car.
			 */
			PMPoint2D* CalculateOptimalPoints(tdble carWidth, tdble carDepth);

			std::vector<tdble> speedData; /* Speed is recorded on each update, to calculate the average speed on the corner */
		public:
			CornerOutlook() : 
				entrance(Segment()), corner(Segment()), exit(Segment()), optimalRadius(0.f), actualRadius(0.f), clear(true), NUMB_OF_OPTIMAL_POINTS(3), speedData(std::vector<tdble>()) { };
			~CornerOutlook();

			/* Represents a single segment in the current outlook */
			struct Segment
			{
			public:
				Segment() : maxRange(0.f), minRange(0.f), seg(nullptr) { }
				tTrackSeg* seg;
				
				/*
				 * Update this segment, updating the dataset passed in if the car's toStart position is between maxRange and minRange.
				 * data    		Current car data.
				 * dataSet 		The dataset being used. Appended to where necessary.
				 * currentTime  The current time stamp to use when recording data.
				 */
				void Update(CarData& data, std::vector<PMData>& dataSet, const tdble& currentTime);

				float maxRange, minRange;
			} entrance, corner, exit; // Each outlook stores three segments

			tdble optimalRadius;
			tdble actualRadius;

			/*
			 * Generate an outlook consisting of a straight, corner, and exit straight.
			 * baseSeg  The base segment for the outlook, this should be a pointer to the entry segment.
			 * carWidth Width of the current car.
			 * carDepth Depth (Z dimension) of the current car.
			 */
			void GenerateOutlook(tTrackSeg* baseSeg, tdble carWidth, tdble carDepth);

			/*
			 * Updates the current outlook, appending data where necessary.
			 * data        The current car data.
			 * dataSet     The current data set. Appended to where necessary.
			 * currentTime The current time stamp to use when recording new data.
			 */
			void Update(CarData& data, std::vector<PMData>& dataSet, const tdble& currentTime);

			/* Returns whether or not the current outlook is empty */
			bool IsClear() { return clear; }

			/* Clears the outlook of all data. */
			void Clear() { entrance = corner = exit = Segment(); actualRadius = optimalRadius = 0.f; speedData.clear(); clear = true; }

			/* Calculates the average speed the car has up until this point */
			tdble AverageSpeed();
		} currentOutlook;

		bool offTrack; // Whether or not the car has left the main track in this evaluation segment
		bool collision; // Whether or not the car has collided with another object in this evaluation segment

		tdble curDmg; // Damage on current update
		tdble prevDmg; // Damage on previous update

		const tdble offTrackPenalty;

		virtual void Evaluate() override final;
	public:
		RaceLineEvaluation() : currentOutlook(CornerOutlook()), offTrackPenalty(0.5), offTrack(false), collision(false), curDmg(0), prevDmg(0) { }
		virtual void Update(tdble deltaTimeIncrement, tdble currentTime) override final;
	};
}

#endif //_RACE_LINE_EVAL_H_