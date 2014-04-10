/**
 *	@file 	PSeg.h
 *	@author Henri Keeble
 *	@brief  Declares a class used to represent a procedural segment, and factory functions to ease instantiation of different types of segment.
*/

#ifndef _P_SEG_H_
#define _P_SEG_H_

#include <string>
#include <iostream>
#include <ctime>
#include "tgf.h"
#include "PRange.h"
#include "PDefs.h"

namespace procedural
{
	class PSegFactory;

	/*! Represents segment data internally for the procedural library. This is converted into a TORCS track segment structure when added to the track. */
	class PSeg
	{
	public:	
		PSeg();
		virtual ~PSeg();

		int	id;		/*!< ID of the track segment. */
		int type;	/*!< Type of the track segment, left, right or straight. */

		float length; /*!< Length of the segment in meters. */
		float radius; /*!< Radius of the segment in meters. */
		float arc; 	  /*!< Arc of the segment in radians. */
	};

	//! Represents a type of corner in the procedural library.
	enum PCornerType
	{
		CTRight = 1, /*!< A right turn. */
		CTLeft = 2 	 /*!< A left turn. */
	};

	/*! Factory class for all segments. This factory is a singleton. */
	class PSegFactory
	{
	private:
		PSegFactory();
		virtual ~PSegFactory();
		static PSegFactory* instance;

		float cornerChance; /*!< Percentage chance of a corner being generated. Set externally with <SetChances>"("<float corner, float straight>")" */
		float straightChance; /*!< Percentage change of a straight being generated. Set externally with <SetChances>"("<float corner, float straight>")" */

		/** Generate random floating point value between min and max. */
		float RandBetween(float min, float max);

		/** Generate a random segment type based on the chances currently set */
		PSegType RandomSegType();


		PCornerType previousCornerType; /*!< The previously used corner type */
	public:
		/** Get the instance of the segment factory. If a factory has not been created, create one and return a pointer to it. */
		static PSegFactory* GetInstance();

		PSegmentRanges ranges; 	/*!< The ranges between which segment parameters may currently reside. */
		
		//! Updates the safe ranges for corner arcs, call this after each time a corner segment is added.
		 /*!
			\param angle The previous corner angle in radians.
			\param cType The previous corner type that was added, so that ranges can be adjusted accordingly.
		 */
		void UpdateRanges(tdble angle, PCornerType cType);

		//! Sets the percentage chances or corners and straights being generated. Values should sum to 100.
		 /*!
			\param corner	Percentage chance of a corner being generated.
			\param straight Percentage chance of a straight being generated.
		 */
		void SetChances(float corner, float straight);

		/** Generates a random segment based upon chances and ranges set. */
		PSeg CreateRandomSeg(int id);

		/** Generates a random straight segment. */
		PSeg CreateRandomStr(int id);

		/** Generates a random corner segment. */
		PSeg CreateRandomCnr(int id);

		/** Creates a straight segment. */
		PSeg CreateSegStr(int id, float length);

		/** Creates a corner segment. */
		PSeg CreateSegCnr(int id, PCornerType cType, float radius, float arc);
	};
}
#endif // _P_SEG_H_