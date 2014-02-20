/*
	File: PMEvaluationFunctions.h
	Author: Henri Keeble
	Desc: Declarations for all evaluation functor types.
*/

#ifndef _PM_EVALUATION_FUNCTIONS_H_
#define _PM_EVALUATION_FUNCTIONS_H_

#include "PMData.h"
#include "PMDefs.h"
#include "PMMath.h"
#include "robottools.h"

namespace perfMeasurement
{
	/* 
		Base Evaluation class. The idea is to derive from this class with bespoke update and
		evaluation behaviours, such that a data set is maintained and evaluated correctly for each method
	*/
	class PMEvaluator
	{
	protected:
		CarData car;
		PMDataCollection dataSet;
		virtual tdble Evaluate() = 0;
	public:
		PMEvaluator() { }
		virtual float Update(tdble deltaTimeIncrement, tdble currentTime) = 0;
		tCarElt* GetCar() { return car.GetCar(); }
		void ClearData() { dataSet.Clear(); }
		void Init(tCarElt* carElt) { car = carElt; dataSet = PMDataCollection(); }
	};
	
	/* Evaluates performance based upon the mean deviation from the car's top speed. */
	class MeanDeviationFromTopSpeed : public PMEvaluator
	{
	private:
		tdble cumulativeTime;
		tdble timeOnLastUpdate;
		const tdble UPDATE_INTERVAL;
		virtual tdble Evaluate() override final;
	public:
		MeanDeviationFromTopSpeed() : UPDATE_INTERVAL(0.1f), timeOnLastUpdate(0), cumulativeTime(0) { }
		virtual float Update(tdble deltaTimeIncrement, tdble currentTime) override final;
	};
	
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
			const int NUMB_OF_OPTIMAL_POINTS = 3;

			/* Used to calculate the optimal radius of the current corner */
			tdble CalculateOptimalRadius(tdble carWidth, tdble carDepth);

			/* Used to calculate the optimal points the optimal arc must pass through */
			PMPoint2D* CalculateOptimalPoints(tdble carWidth, tdble carDepth);
		public:
			CornerOutlook() : 
				entrance(Segment()), corner(Segment()), exit(Segment()), optimalRadius(0.f), actualRadius(0.f), clear(true) { };
			~CornerOutlook();

			/* Represents a single segment in the current outlook */
			struct Segment
			{
			public:
				Segment() : maxRange(0.f), minRange(0.f), seg(nullptr), dataRecorded(false) { }
				tTrackSeg* seg;
				bool dataRecorded;
				
				/* Update this segment, updating the dataset passed in if the car's toStart position is between maxRange and minRange */
				void Update(CarData currentData, PMDataCollection& dataSet, tdble currentTime);

				float maxRange, minRange;
			} entrance, corner, exit; // Each outlook stores three segments

			tdble optimalRadius;
			tdble actualRadius;

			/* Generate an outlook consisting of a straight, corner, and exit straight */
			void GenerateOutlook(tTrackSeg* baseSeg, tdble carWidth, tdble carDepth);

			/* Updates the current outlook, appending data where neccesary */
			void Update(CarData data, PMDataCollection& dataSet, tdble currentTime);

			/* Returns whether or not the current outlook is empty */
			bool IsClear() { return clear; }

			/* Clears the outlook of all data. */
			void Clear() { entrance = corner = exit = Segment(); actualRadius = optimalRadius = 0.f; clear = true; }

			/* Returns whether or not all data is recorded within this outlook */
			bool IsComplete() { return entrance.dataRecorded && corner.dataRecorded && exit.dataRecorded; }
		} currentOutlook;

		virtual tdble Evaluate() override final;
	public:
		RaceLineEvaluation() : currentOutlook(CornerOutlook()) { }
		virtual float Update(tdble deltaTimeIncrement, tdble currentTime) override final;
	};
}

#endif //_PM_EVALUATION_FUNCTIONS_H_