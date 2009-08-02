/****************************************************************************

    X-IvAp  -  Martin Domig <martin@domig.net>
    Copyright (C) 2006 by Martin Domig

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/
#include "interpolator.h"
#include "xivap.h"
#include "helpers.h"
#include <math.h>

using namespace Multiplayer;

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

void dist_course(double* d, double* tc, const double lat1d, const double lon1d, const double lat2d, const double lon2d) {
	// returns d in nm
	// everything else is in degrees
	long double lat1 = lat1d * M_PI / 180;
	long double lon1 = lon1d * M_PI / 180;
	long double lat2 = lat2d * M_PI / 180;
	long double lon2 = lon2d * M_PI / 180;
	*d = (double) 2*asinl(sqrtl(powl(sinl((lat1-lat2)/2),2) + 
	                 cosl(lat1)*cosl(lat2)*powl(sinl((lon1-lon2)/2),2)));
	*tc = (double) fmodl(atan2l(sinl(lon1-lon2)*cosl(lat2),
		            cosl(lat1)*sinl(lat2)
		             - sinl(lat1)*cosl(lat2)*cosl(lon1-lon2)), 
		       2*M_PI);	
	*tc *= 180 / M_PI;
	*d *= (180*60)/M_PI;
}

void fly_course(double* lat, double* lon, const double lat1d, const double lon1d, const double dd, const double tcd) {
	// Lat/lon given radial and distance
	// lat1 lon1 is starting point, tc course
	// d in nm.
	long double lat1 = lat1d * M_PI / 180;
	long double lon1 = lon1d * M_PI / 180;
	long double tc = tcd * M_PI / 180;
	long double d = dd * M_PI / (180*60);
    long double latt = asinl(sinl(lat1)*cosl(d) + cosl(lat1)*sinl(d)*cosl(tc));
    long double dlon = atan2l(sinl(tc)*sinl(d)*cosl(lat1), cosl(d) - sinl(lat1)*sinl(latt));
	*lat = (double) latt;
    *lon = (double) fmodl(lon1 - dlon + M_PI, 2*M_PI) - M_PI;
	*lat *= 180 / M_PI;
	*lon *= 180 / M_PI;
}


Interpolator::Interpolator()
{
}

Interpolator::~Interpolator()
{
	clear();
}

void Interpolator::clear()
{
	for(deque<PlanePosition*>::iterator it = positions.begin(); it != positions.end(); ++it)
		delete *it;
}

void Interpolator::takeNewPosition(PlanePosition* new_position, double distance_nm)
{
	if(positions.size() > 4) { // keep a history of 4 packets (for latency averaging)
		delete positions.back();
		positions.pop_back();
	}

	if (positions.size() == 0) {
		// start from the first packet position
		Pos = *new_position;
	}


	if (Pos.pos.heading < 0) {
		Pos.pos.heading +=360;
	}
	if (Pos.pos.heading > 360) {
		Pos.pos.heading -=360;
	}

	if(new_position->p_timestamp == 0) {
		// this is a FSD pos update (without peer timestamp).
		// dont extrapolate for FSD updates - add 5 seconds to the time
		new_position->timestamp += 5000000;
	}

	// new_position:
	//     timestamp = time when we received that packet
	//     latency = avg. ping round-trip time to peer at that point
	//     p_timestamp = peers timestamp on packet

	// re-calculate new_position->timestamp to something reasonable, taking into account
	// the current latency and the time difference of the timestamps

	// if this is not the first update...
	if(!(positions.size() == 0 || lastPos.p_timestamp == 0)) {

		// calculate avg. latencies over all positions we know
		UInt32 avg_latency = new_position->latency;
		for(deque<PlanePosition*>::iterator it = positions.begin(); it != positions.end(); ++it)
			avg_latency += (*it)->latency;
		avg_latency /= ((UInt32)positions.size() + 1);
		
		// not the first packet - find a reasonable value for timestamp
		//		new_position->timestamp = lastPos.timestamp + lastPos.latency	// last pos. time minus last latency...
		//			+ new_position->p_timestamp - lastPos.p_timestamp			// ...plus peers delta time...
		//			- avg_latency;												// ...plus average latency

		// peer told us how much time has passed... believe them
		// may not work if their clock has a massively wrong rate
		new_position->timestamp = lastPos.timestamp
			+ new_position->p_timestamp - lastPos.p_timestamp;

		new_position->latency = avg_latency;

	}

	positions.push_front(new_position);

	// Add in 0.5*(*new_position - Pos), which is the prediction error
	// so that it is nulled out in the long run
	// have to do lat and lon separately
	lastDelta = (*new_position * 2) - lastPos - Pos;

	// how big is the error?
	double de, tce;
	dist_course(&de, &tce, Pos.pos.lat,
					       Pos.pos.lon,
	                       (*new_position).pos.lat,
	                       (*new_position).pos.lon);

#ifdef DEBUG_INTERP
	xivap.addText(colYellow, "de " + ftoa(de) + " tce " + ftoa(tce) + " latency " + ftoa(lastDelta.timestamp/1e3), true, true);
#endif

	// we want to interpolate even with bigger errors if getting 5s updates
	// can't be too big or we bounce all over the sky

	if ((de > 0.16) || ((lastDelta.timestamp < 4000000) && (de > 0.02))) {
		// error has grown too big, just teleport
		Pos = *new_position;
		// have to recalculate, we're now somewhere else
		lastDelta = *new_position - lastPos;
	}

	// assume we can't turn around between updates... reasonable
	if (lastDelta.pos.heading > 180.0) {
		lastDelta.pos.heading -= 360.0;
	}
	if (lastDelta.pos.heading < -180.0) {
		lastDelta.pos.heading += 360.0;
	}

	if (lastDelta.pos.pitch > 90.0) {
		lastDelta.pos.pitch -= 180.0;
	}
	if (lastDelta.pos.pitch < -90.0) {
		lastDelta.pos.pitch += 180.0;
	}

	if (lastDelta.pos.roll > 180.0) {
		lastDelta.pos.roll -= 360.0;
	}
	if (lastDelta.pos.roll < -180.0) {
		lastDelta.pos.roll += 360.0;
	}

	// xivap.addText(colYellow, string("lastPos ") + ftoa(lastPos.pos.heading) + " Pos " + ftoa(Pos.pos.heading)
	// 		+ " new " + ftoa((*new_position).pos.heading) + " delta " + ftoa(lastDelta.pos.heading), true, true);

	// d and tc will be how far and in what direction we moved... 
	dist_course(&d, &tc, lastPos.pos.lat,
					     lastPos.pos.lon,
	                     (*new_position).pos.lat,
	                     (*new_position).pos.lon);
	// if we're turning, need to correct for that... course has changed
	tc -= lastDelta.pos.heading/4;

	// correct distance for the curve
	double theta = lastDelta.pos.heading*M_PI/360;
	// but don't bother if the angle is too small
	if ((theta>1e-5) || (theta<-1e-5))
		d *= 2*sin(theta/2)/theta;
	
	// where are we going?
	double latt, lont;
	fly_course(&latt, &lont, (*new_position).pos.lat, (*new_position).pos.lon, d, tc);
	
	// how to get there from here
	dist_course(&d, &tc, Pos.pos.lat,
					     Pos.pos.lon,
	                     latt,
	                     lont);	
	
	lastPos = *new_position;

#ifdef DEBUG_INTERP
	xivap.addText(colGray, string("lat ") + ftoa(lastPos.pos.lat) + " lon " + ftoa(lastPos.pos.lon)
			+ " elevation " + ftoa(lastPos.pos.elevation) + " heading " + ftoa(lastPos.pos.heading) 
			+ " lastPos.timestamp " + ftoa(lastPos.timestamp), true, true);
#endif

}

bool Interpolator::renderPosition(UInt32 timestamp, PlanePosition& rendered_position)
{
	if(!usable())
		return false;

	// FIXME: latency
	// looks wrong, but the timestamp is subtracted as well in making the delta
	UInt32 timediff = lastDelta.timestamp;

	// lastDelta.heading tells us a turn rate
	// incrementally fly segments (easier than calculating new pos from scratch)
	
	double interp = (((double) timestamp - (double) Pos.timestamp)/(double) timediff);
	
	rendered_position = Pos + lastDelta*interp;

	// assume we can't turn around between updates... reasonable
	if (rendered_position.pos.heading > 180.0) {
		rendered_position.pos.heading -= 360.0;
	}
	if (rendered_position.pos.heading < -180.0) {
		rendered_position.pos.heading += 360.0;
	}

	if (rendered_position.pos.pitch > 90.0) {
		rendered_position.pos.pitch -= 180.0;
	}
	if (rendered_position.pos.pitch < -90.0) {
		rendered_position.pos.pitch += 180.0;
	}

	if (rendered_position.pos.roll > 180.0) {
		rendered_position.pos.roll -= 360.0;
	}
	if (rendered_position.pos.roll < -180.0) {
		rendered_position.pos.roll += 360.0;
	}

	// but actually, calculate lat and lon separately
	// note that we're taking heading rate as turn rate in the ground plane
	// which is an assumption, but should be fine most of the time
	tc -= lastDelta.pos.heading * interp;
	tc = fmod(tc, 360);
	fly_course(&rendered_position.pos.lat, &rendered_position.pos.lon, Pos.pos.lat, Pos.pos.lon, d * interp, tc);

	rendered_position.timestamp = timestamp;

	if(rendered_position.pos.heading < 0)
		rendered_position.pos.heading += 360;

	Pos = rendered_position;

	return true;
}

bool Interpolator::usable() const
{
	// usable if at least two pos. packet
	return positions.size() > 1;
}

bool Interpolator::errorTooLarge(UInt32 timestamp, const PlanePosition& truePosition)
{
	if(!usable())
		return true;

	PlanePosition estimatedPos;
	renderPosition(timestamp, estimatedPos);
	Pos = estimatedPos;

	// allow max. alt difference
	if(fabs(truePosition.pos.elevation - estimatedPos.pos.elevation) > 0.3) {
#ifdef DEBUG_INTERP
		xivap.addText(colGreen, "---> ALT " + ftoa(truePosition.pos.elevation - estimatedPos.pos.elevation), true, true);
#endif
		return true;
		}

	// allow max. heading/pitch/roll difference... small errors not so visible here.
	if(fabs(truePosition.pos.heading - estimatedPos.pos.heading) > 3.0) {
#ifdef DEBUG_INTERP
		xivap.addText(colGreen, "---> HEADING", true, true);
#endif
		return true;
		}
	if(fabs(truePosition.pos.pitch - estimatedPos.pos.pitch) > 3.0) {
#ifdef DEBUG_INTERP
		xivap.addText(colGreen, "---> PITCH", true, true);
#endif
		return true;
		}
	if(fabs(truePosition.pos.roll - estimatedPos.pos.roll) > 3.0) {
#ifdef DEBUG_INTERP
		xivap.addText(colGreen, "---> ROLL", true, true);
#endif
		return true;
		}

	// allow max. distance
	double course, dist;
	dist_course(&dist, &course,
				estimatedPos.pos.lat, estimatedPos.pos.lon,
				truePosition.pos.lat, truePosition.pos.lon);

	double disttol = 0.1/1852.0;

#ifdef DEBUG_INTERP
	xivap.addText((dist>disttol)?colGreen:colRed, "---> " + ftoa(dist), true, true);
#endif

	if(dist > disttol)
		return true;

	return false;
}
