#include "PTrackDesc.h"

namespace procPathfinder
{
	PTrackDesc::PTrackDesc(const tTrack* track)
	{
		// Initialize variables
		ts = PTrackSegmentCollection();
		nPitEntryStart = 0;
		nPitExitEnd = 0;

		// Initialize state
		stateMngr = PStateManager((tTrack* const)track);

		// Initialize description
		Update();
	}

	void PTrackDesc::Update()
	{
		// Update the state manager first
		stateMngr.Update();

		// Update only if the track has changed since last call to state update
		if (stateMngr.IsUpdateNeeded())
		{
			// Get pointer to the TORCS track structure
			tTrack* track = stateMngr.GetTorcsTrack();

			// Pointers to track segments
			tTrackSeg* end = nullptr;
			tTrackSeg* curSeg = nullptr;

			// Vector to contain segments to add
			std::vector<PTrackSegment> newSegs = std::vector<PTrackSegment>();

			// Initialize length and nsegments, with both corresponding to the new part of the track
			int nOfNewSegs = 0;
			double tracklength = 0.0;

			if (ts.Count() == 0)
			{
				end = stateMngr.GetEnd();
				curSeg = end;

				/* compute the length of the track */
				do {
					tracklength += curSeg->length;
					curSeg = curSeg->next;
				} while ((curSeg != end) && curSeg);
			}
			else
			{
				end = stateMngr.GetLastEnd();
				curSeg = end->next;

				while ((curSeg != stateMngr.GetStart()) && curSeg)
				{
					tracklength += curSeg->length;
					curSeg = curSeg->next;
				}
			}

			// Set number of new segments based on new length
			nOfNewSegs = (int)floor(tracklength);

			/* init all the new segments of the track description */
			v3d l, m, r;
			int currentts = 0;
			double lastseglen = 0.0;
			double curseglen = 0.0;
			curSeg = (tTrackSeg*)end->next;

			for (int i = 0; i < nOfNewSegs; i++)
			{
				if (curSeg->type == TR_STR) {
					double dxl = (curSeg->vertex[TR_EL].x - curSeg->vertex[TR_SL].x) / (curSeg->length / TRACKRES);
					double dyl = (curSeg->vertex[TR_EL].y - curSeg->vertex[TR_SL].y) / (curSeg->length / TRACKRES);
					double dzl = (curSeg->vertex[TR_EL].z - curSeg->vertex[TR_SL].z) / (curSeg->length / TRACKRES);
					double dxr = (curSeg->vertex[TR_ER].x - curSeg->vertex[TR_SR].x) / (curSeg->length / TRACKRES);
					double dyr = (curSeg->vertex[TR_ER].y - curSeg->vertex[TR_SR].y) / (curSeg->length / TRACKRES);
					double dzr = (curSeg->vertex[TR_ER].z - curSeg->vertex[TR_SR].z) / (curSeg->length / TRACKRES);

					for (int i = 0; curseglen < curSeg->length && currentts < nOfNewSegs; i++) {

						l.x = curSeg->vertex[TR_SL].x + dxl*curseglen;
						l.y = curSeg->vertex[TR_SL].y + dyl*curseglen;
						l.z = curSeg->vertex[TR_SL].z + dzl*curseglen;

						r.x = curSeg->vertex[TR_SR].x + dxr*curseglen;
						r.y = curSeg->vertex[TR_SR].y + dyr*curseglen;
						r.z = curSeg->vertex[TR_SR].z + dzr*curseglen;

						m = (l + r) / 2.0;

						newSegs.push_back(PTrackSegment(curSeg->id, curSeg, &l, &m, &r));
						currentts++;

						lastseglen = curseglen;
						curseglen += TRACKRES;
					}
				}
				else {
					double dphi = TRACKRES / curSeg->radius;
					double xc = curSeg->center.x;
					double yc = curSeg->center.y;
					double dzl = (curSeg->vertex[TR_EL].z - curSeg->vertex[TR_SL].z) / (curSeg->length / TRACKRES);
					double dzr = (curSeg->vertex[TR_ER].z - curSeg->vertex[TR_SR].z) / (curSeg->length / TRACKRES);

					dphi = (curSeg->type == TR_LFT) ? dphi : -dphi;
					for (int i = 0; curseglen < curSeg->length && currentts < nOfNewSegs; i++) {
						double phi = curseglen * dphi;
						double cs = cos(phi), ss = sin(phi);
						l.x = curSeg->vertex[TR_SL].x * cs - curSeg->vertex[TR_SL].y * ss - xc * cs + yc * ss + xc;
						l.y = curSeg->vertex[TR_SL].x * ss + curSeg->vertex[TR_SL].y * cs - xc * ss - yc * cs + yc;
						l.z = curSeg->vertex[TR_SL].z + dzl*curseglen;

						r.x = curSeg->vertex[TR_SR].x * cs - curSeg->vertex[TR_SR].y * ss - xc * cs + yc * ss + xc;
						r.y = curSeg->vertex[TR_SR].x * ss + curSeg->vertex[TR_SR].y * cs - xc * ss - yc * cs + yc;
						r.z = curSeg->vertex[TR_SR].z + dzr*curseglen;

						m = (l + r) / 2.0;

						newSegs.push_back(PTrackSegment(curSeg->id, curSeg, &l, &m, &r));
						currentts++;

						lastseglen = curseglen;
						curseglen += TRACKRES;
					}
				}

				curseglen = TRACKRES - (curSeg->length - lastseglen);
				lastseglen = curseglen;
				while (curseglen > TRACKRES) {
					curseglen -= TRACKRES;
				}

				curSeg = curSeg->next;
			}

			if (currentts != nOfNewSegs) printf("error: PTrackDesc::PTrackDesc currentts %d != nOfNewSegs %d.\n", currentts, nOfNewSegs);

			int i;

			/* setting up length */
			for (i = 0; i < nOfNewSegs; i++) {
				int k = (i + nOfNewSegs + 1) % nOfNewSegs;
				v3d* p = newSegs[k].getMiddle();
				newSegs[i].setLength(newSegs[i].distToMiddle2D(p->x, p->y));
			}

			/* init kbeta, for height profile of track */
			v3d *p0, *p1, *p2;
			double dz10, dz21;

			for (i = 0; i < nOfNewSegs; i++) {
				p0 = newSegs[(i + nOfNewSegs - 5) % nOfNewSegs].getMiddle();
				p1 = newSegs[(i + nOfNewSegs) % nOfNewSegs].getMiddle();
				p2 = newSegs[(i + nOfNewSegs + 5) % nOfNewSegs].getMiddle();
				dz10 = p1->z - p0->z;
				dz21 = p2->z - p1->z;

				if (dz21 < dz10) {
					double dl10, dl21, r;
					v3d pr;
					dirVector2D(p1, p0, &pr);
					dl10 = pr.len();
					dirVector2D(p2, p1, &pr);
					dl21 = pr.len();
					r = fabs(radius(0.0, p0->z, dl10, p1->z, dl21 + dl10, p2->z));
					if (r < RREL) {
						newSegs[i].setKbeta(1.0 / r);
					}
					else {
						newSegs[i].setKbeta(0.0);
					}
				}
				else {
					newSegs[i].setKbeta(0.0);
				}
			}

			for (i = 0; i < nOfNewSegs; i++) {
				p0 = newSegs[(i + nOfNewSegs - 3) % nOfNewSegs].getMiddle();
				p1 = newSegs[(i + nOfNewSegs + 3) % nOfNewSegs].getMiddle();
				newSegs[i].setKgamma(atan((p1->z - p0->z) / 6.0));
			}

			// Append the segments to the collection
			ts.Append(newSegs);
		}
	}

	PTrackDesc::~PTrackDesc()
	{

	}

	void PTrackDesc::plot(char* filename)
	{
		FILE *fd = fopen(filename, "w");
		v3d *l, *m, *r;

		/* plot track */
		for (int i = 0; i < segmentCount(); i++) {
			PTrackSegment* p = getSegmentPtr(i);
			
			l = p->getLeftBorder();
			fprintf(fd, "%f,%f\n", l->x, l->y);
			m = p->getMiddle();
			fprintf(fd, "%f,%f\n", m->x, m->y);
			r = p->getRightBorder();
			fprintf(fd, "%f,%f\n", r->x, r->y);
		}

		fclose(fd);
	}

	void PTrackDesc::outputCurvature(char* filename)
	{
		FILE *fd = fopen(filename, "w");

		tTrack* track = GetTorcsTrack();
		tTrackSeg* firstSeg = track->seg->next;
		tTrackSeg* currentSeg = firstSeg;

		do {
			fprintf(fd, "%f \n", currentSeg->radius > 0 ? 1 / currentSeg->radius : 0.f);
			currentSeg = currentSeg->next;
		} while (currentSeg->id != firstSeg->id);

		fclose(fd);
	}

	/* get the segment on which the car is, searching ALL the segments */
	int PTrackDesc::getCurrentSegment(tCarElt* car)
	{
		double d, min = FLT_MAX;
		PTrackSegment* ts;
		int minindex = 0;

		for (int i = 0; i < segmentCount(); i++) {
			ts = getSegmentPtr(i);
			d = ts->distToMiddle3D(car->_pos_X, car->_pos_Y, car->_pos_Z);
			if (d < min) {
				min = d;
				minindex = i;
			}
		}
		return minindex;
	}


	int PTrackDesc::getNearestId(v3d* p)
	{
		double tmp, dist = FLT_MAX;
		int minindex = 0;

		for (int i = 0; i < segmentCount(); i++) {
			tmp = getSegmentPtr(i)->distToMiddle3D(p);
			if (tmp < dist) {
				dist = tmp;
				minindex = i;
			}
		}

		return minindex;
	}
}