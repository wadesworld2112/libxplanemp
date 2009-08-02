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

#include "weatherGod.h"
#include "xivap.h"
#include "helpers.h"

#include <math.h> // for fabs()

XPWeatherSituation::XPWeatherSituation():
initialized(false)
{
}

void XPWeatherSituation::init()
{
	// those are all 660+ refs
	ref_cloud_type[0]				= XPLMFindDataRef("sim/weather/cloud_type[0]");
	ref_cloud_type[1]				= XPLMFindDataRef("sim/weather/cloud_type[1]");
	ref_cloud_type[2]				= XPLMFindDataRef("sim/weather/cloud_type[2]");
	ref_cloud_base_msl_m[0]			= XPLMFindDataRef("sim/weather/cloud_base_msl_m[0]");
	ref_cloud_base_msl_m[1]			= XPLMFindDataRef("sim/weather/cloud_base_msl_m[1]");
	ref_cloud_base_msl_m[2]			= XPLMFindDataRef("sim/weather/cloud_base_msl_m[2]");
	ref_cloud_tops_msl_m[0]			= XPLMFindDataRef("sim/weather/cloud_tops_msl_m[0]");
	ref_cloud_tops_msl_m[1]			= XPLMFindDataRef("sim/weather/cloud_tops_msl_m[1]");
	ref_cloud_tops_msl_m[2]			= XPLMFindDataRef("sim/weather/cloud_tops_msl_m[2]");
	ref_visibility_reported_m		= XPLMFindDataRef("sim/weather/visibility_reported_m");
	ref_rain_percent				= XPLMFindDataRef("sim/weather/rain_percent");
	ref_thunderstorm_percent		= XPLMFindDataRef("sim/weather/thunderstorm_percent");
	ref_wind_turbulence_percent		= XPLMFindDataRef("sim/weather/wind_turbulence_percent");
	ref_barometer_sealevel_inhg		= XPLMFindDataRef("sim/weather/barometer_sealevel_inhg");
	ref_microburst_probability		= XPLMFindDataRef("sim/weather/microburst_probability");
	ref_rate_change_percent			= XPLMFindDataRef("sim/weather/rate_change_percent");
	ref_use_real_weather_bool		= XPLMFindDataRef("sim/weather/use_real_weather_bool");
	ref_wind_altitude_msl_m[0]		= XPLMFindDataRef("sim/weather/wind_altitude_msl_m[0]");
	ref_wind_altitude_msl_m[1]		= XPLMFindDataRef("sim/weather/wind_altitude_msl_m[1]");
	ref_wind_altitude_msl_m[2]		= XPLMFindDataRef("sim/weather/wind_altitude_msl_m[2]");
	ref_wind_direction_degt[0]		= XPLMFindDataRef("sim/weather/wind_direction_degt[0]");
	ref_wind_direction_degt[1]		= XPLMFindDataRef("sim/weather/wind_direction_degt[1]");
	ref_wind_direction_degt[2]		= XPLMFindDataRef("sim/weather/wind_direction_degt[2]");
	ref_wind_speed_kt[0]			= XPLMFindDataRef("sim/weather/wind_speed_kt[0]");
	ref_wind_speed_kt[1]			= XPLMFindDataRef("sim/weather/wind_speed_kt[1]");
	ref_wind_speed_kt[2]			= XPLMFindDataRef("sim/weather/wind_speed_kt[2]");
	ref_shear_direction_degt[0]		= XPLMFindDataRef("sim/weather/shear_direction_degt[0]");
	ref_shear_direction_degt[1]		= XPLMFindDataRef("sim/weather/shear_direction_degt[1]");
	ref_shear_direction_degt[2]		= XPLMFindDataRef("sim/weather/shear_direction_degt[2]");
	ref_shear_speed_kt[0]			= XPLMFindDataRef("sim/weather/shear_speed_kt[0]");
	ref_shear_speed_kt[1]			= XPLMFindDataRef("sim/weather/shear_speed_kt[1]");
	ref_shear_speed_kt[2]			= XPLMFindDataRef("sim/weather/shear_speed_kt[2]");
	ref_turbulence[0]				= XPLMFindDataRef("sim/weather/turbulence[0]");
	ref_turbulence[1]				= XPLMFindDataRef("sim/weather/turbulence[1]");
	ref_turbulence[2]				= XPLMFindDataRef("sim/weather/turbulence[2]");
	ref_wave_amplitude				= XPLMFindDataRef("sim/weather/wave_amplitude");
	ref_wave_length					= XPLMFindDataRef("sim/weather/wave_length");
	ref_wave_speed					= XPLMFindDataRef("sim/weather/wave_speed");
	ref_wave_dir					= XPLMFindDataRef("sim/weather/wave_dir");
	ref_runway_friction				= XPLMFindDataRef("sim/weather/runway_friction");
	ref_temperature_sealevel_c		= XPLMFindDataRef("sim/weather/temperature_sealevel_c");
	ref_dewpoi_sealevel_c			= XPLMFindDataRef("sim/weather/dewpoi_sealevel_c");
	ref_thermal_percent				= XPLMFindDataRef("sim/weather/thermal_percent");
	ref_thermal_rate_ms				= XPLMFindDataRef("sim/weather/thermal_rate_ms");
	ref_thermal_altitude_msl_m		= XPLMFindDataRef("sim/weather/thermal_altitude_msl_m");

	initialized = true;
}

void XPWeatherSituation::getWeather()
{
	if(!initialized) return;

	for(int i = 0; i < CLOUD_LAYERS; ++i) {
		cloud_type[i] = XPLMGetDatai(ref_cloud_type[i]);
		cloud_base_msl_m[i] = XPLMGetDataf(ref_cloud_base_msl_m[i]);
		cloud_tops_msl_m[i] = XPLMGetDataf(ref_cloud_tops_msl_m[i]);
	}

	/*
	wave_amplitude = XPLMGetDataf(ref_wave_amplitude);
	wave_length = XPLMGetDataf(ref_wave_length);
	wave_speed = XPLMGetDataf(ref_wave_speed);
	wave_dir = XPLMGetDataf(ref_wave_dir);
	*/

	for(int i = 0; i < WIND_LAYERS; ++i) {
		wind_altitude_msl_m[i] = XPLMGetDataf(ref_wind_altitude_msl_m[i]);
		wind_direction_degt[i] = XPLMGetDataf(ref_wind_direction_degt[i]);
		wind_speed_kt[i] = XPLMGetDataf(ref_wind_speed_kt[i]);
		shear_direction_degt[i] = XPLMGetDataf(ref_shear_direction_degt[i]);
		shear_speed_kt[i] = XPLMGetDataf(ref_shear_speed_kt[i]);
		turbulence[i] = XPLMGetDataf(ref_turbulence[i]);
	}

	temperature_sealevel_c = XPLMGetDataf(ref_temperature_sealevel_c);
	dewpoi_sealevel_c = XPLMGetDataf(ref_dewpoi_sealevel_c);
	barometer_sealevel_inhg = XPLMGetDataf(ref_barometer_sealevel_inhg);
	wind_turbulence_percent = XPLMGetDataf(ref_wind_turbulence_percent);
	thunderstorm_percent = XPLMGetDataf(ref_thunderstorm_percent);
	microburst_probability = XPLMGetDataf(ref_microburst_probability);	
	rate_change_percent = XPLMGetDataf(ref_rate_change_percent);
	use_real_weather_bool = XPLMGetDatai(ref_use_real_weather_bool);
	rain_percent = XPLMGetDataf(ref_rain_percent);
	visibility_reported_m = XPLMGetDataf(ref_visibility_reported_m);
	thermal_percent = XPLMGetDataf(ref_thermal_percent);
	thermal_rate_ms = XPLMGetDataf(ref_thermal_rate_ms);
	thermal_altitude_msl_m = XPLMGetDataf(ref_thermal_altitude_msl_m);
}

void XPWeatherSituation::setWeather(const XPWeatherSituation& target, XPWeatherSituation& lastset)
{

#define WXi(ref, x) { if(lastset.x != x && ref != NULL) { XPLMSetDatai(ref, x); lastset.x = x; } }
#define WXf(ref, x) { if(lastset.x != x && ref != NULL) { XPLMSetDataf(ref, x); lastset.x = x; } }

// debug version
#define DEB_WXf(text, ref, x) {												\
	if(lastset.x != x) {													\
		XPLMSetDataf(ref, x); lastset.x = x;								\
		xivap.addText(colWhite, text + ": " + ftoa(x), true, true);	\
	}																		\
}

	if(!initialized) return;

	for(int i = 0; i < CLOUD_LAYERS; ++i) {
		WXi(ref_cloud_type[i], cloud_type[i]);
		WXf(ref_cloud_base_msl_m[i], cloud_base_msl_m[i]);
		WXf(ref_cloud_tops_msl_m[i], cloud_tops_msl_m[i]);
/*
#ifdef WX_DEBUG
		xivap.addText(colWhite, "set cloud layer " + itostring(i) + ": type " + itostring(cloud_type[i])
			+ " base " + ftoa(cloud_base_msl_m[i]) + " tops " + ftoa(cloud_tops_msl_m[i]),
			false, true); // log to file only
#endif
*/
	}

/*
	WXf(ref_wave_amplitude, wave_amplitude);
	WXf(ref_wave_length, wave_length);
	WXf(ref_wave_speed, wave_speed);
	WXf(ref_wave_dir, wave_dir);
*/

	for(int i = 0; i < WIND_LAYERS; ++i) {

		DEB_WXf(string("wind alt ") + itostring(i), ref_wind_altitude_msl_m[i], wind_altitude_msl_m[i]);
		DEB_WXf(string("wind dir ") + itostring(i), ref_wind_direction_degt[i], wind_direction_degt[i]);
		DEB_WXf(string("wind spd ") + itostring(i), ref_wind_speed_kt[i], wind_speed_kt[i]);
		DEB_WXf(string("shear dir ") + itostring(i), ref_shear_direction_degt[i], shear_direction_degt[i]);
		DEB_WXf(string("shear spd ") + itostring(i), ref_shear_speed_kt[i], shear_speed_kt[i]);
		DEB_WXf(string("turb ") + itostring(i), ref_turbulence[i], turbulence[i]);

/*
#ifdef WX_DEBUG
		if(i == 1) {
		xivap.addText(colWhite, "set wind layer " + itostring(i) + ": alt " + ftoa(wind_altitude_msl_m[i])
			+ " dir " + ftoa(wind_direction_degt[i]) + " speed " + ftoa(wind_speed_kt[i])
			+ " shear " + ftoa(shear_direction_degt[i]) + " gusts " + ftoa(shear_speed_kt[i])
			+ " turb " + ftoa(turbulence[i]),
			false, true); // log to file only
		}
#endif
*/
	}

	WXf(ref_temperature_sealevel_c, temperature_sealevel_c);
	WXf(ref_dewpoi_sealevel_c, dewpoi_sealevel_c);
	WXf(ref_barometer_sealevel_inhg, barometer_sealevel_inhg);
	WXf(ref_wind_turbulence_percent, wind_turbulence_percent);
	WXf(ref_thunderstorm_percent, thunderstorm_percent);
	WXf(ref_microburst_probability, microburst_probability);	
	WXf(ref_rain_percent, rain_percent);
	WXf(ref_runway_friction, rain_percent);
	WXf(ref_visibility_reported_m, visibility_reported_m);
	WXf(ref_thermal_percent, thermal_percent);
	WXf(ref_thermal_rate_ms, thermal_rate_ms);
	WXf(ref_thermal_altitude_msl_m, thermal_altitude_msl_m);

	XPLMSetDatai(ref_use_real_weather_bool, 0);
	XPLMSetDataf(ref_rate_change_percent, rate_change_percent);

/*
#ifdef WX_DEBUG
	if(set) {
	xivap.addText(colWhite, "temp " + ftoa(temperature_sealevel_c) + " dewp " + ftoa(dewpoi_sealevel_c)
		+ " press " + ftoa(barometer_sealevel_inhg) + " rain " + ftoa(rain_percent)
		+ " thdst " + ftoa(thunderstorm_percent) + " vis " + ftoa(visibility_reported_m),
		false, true); // log to file only
	}
	set = false;
#endif
*/
}

void WeatherGod::init()
{
	current.init();
	lastset.init();
	current.getWeather();
	target = current;
	haveWeather = false;
	initialized = true;
	oldWxStation = "";
}

void WeatherGod::syncWx()
{
	current.getWeather();
}

void WeatherGod::setClouds(const WxStation& s)
{
	rainPercent = 0;
	turbulencePercent = 0;
	thunderstormPercent = 0;

	//
	// set the two cloud layers with the highest cloud coverage
	//
	int maxCoverage = 0; // remember max. coverage
	for(size_t i = 0; i < s.cloudLayers.size(); ++i) {
		if(s.cloudLayers[i].coverage > maxCoverage) maxCoverage = s.cloudLayers[i].coverage;

		// thunderstorm?
		// no thunderstorms above 4500m. otherwise XP will draw flashes and thunder at FL330
		if(aircraftAlt < 4500) {
			if(s.cloudLayers[i].type == FSD::CLOUD_THUNDERSTORM) {
				if(s.cloudLayers[i].coverage > 4) { // thunderstorms only if there is actually enough cloud
						                            // coverage to be able to have a storm
					if(thunderstormPercent < (s.cloudLayers[i].coverage - 4) / 3.0f)
						thunderstormPercent = (s.cloudLayers[i].coverage - 4) / 3.0f;
				}
			}
		}

		// check for rain
		if(s.cloudLayers[i].precip != FSD::PRECIP_NONE) {
			if(s.cloudLayers[i].precipbase < (int)aircraftAlt) {
				if(s.cloudLayers[i].tops > aircraftAlt) {
					rain = true;
					if(rainPercent < s.cloudLayers[i].preciprate / 5.0f)
						rainPercent = s.cloudLayers[i].preciprate / 5.0f;
				}
			}
		}

		// check for turbulences inside a cloud layer the aircraft is flying in
		if(s.cloudLayers[i].base <= aircraftAlt && s.cloudLayers[i].tops >= aircraftAlt)
			turbulencePercent = static_cast<float>(s.cloudLayers[i].turbulence) / 4.0f;
	}

	size_t layersSet = 0;
	while(maxCoverage > 0 && layersSet < CLOUD_LAYERS && layersSet < s.cloudLayers.size()) {
		for(size_t i = 0; i < s.cloudLayers.size() && layersSet < CLOUD_LAYERS; ++i) {
			if(s.cloudLayers[i].coverage == maxCoverage) {
				// Clear = 0, High Cirrus = 1, Scattered = 2, Broken = 3, Overcast = 4
				int cloudType = 0;

				switch(s.cloudLayers[i].coverage) {
					case 1:	case 2:	case 3:
						cloudType = 2;
						break;
					case 4: case 5: case 6:
						cloudType = 3;
						break;
					case 7: case 8: default:
						cloudType = 4;
						break;
				}
				// if clouds are high and cirrus, set cloud type to cirrus
				if((s.cloudLayers[i].type == FSD::CLOUD_CIRRUS ||
					s.cloudLayers[i].type == FSD::CLOUD_CIRROCUMULUS ||
					s.cloudLayers[i].type == FSD::CLOUD_CIRROSTRATUS)
					&& s.cloudLayers[i].base > 6000)
					cloudType = 1;

				target.cloud_type[layersSet] = cloudType;
				target.cloud_base_msl_m[layersSet] = s.cloudLayers[i].base;
				target.cloud_tops_msl_m[layersSet] = s.cloudLayers[i].tops;

				++layersSet;
			}
		}

		--maxCoverage; // use the next cloud layer
	}

	// set remaining cloud layers to clear clouds
	while(layersSet < CLOUD_LAYERS) {
		target.cloud_base_msl_m[layersSet] = 0;
		target.cloud_tops_msl_m[layersSet] = 0;
		target.cloud_type[layersSet++] = 0;
	}
}

void WeatherGod::setWinds(const WxStation& station)
{

	// i need at least 3 wind layers
	// FIXME - maybe do something smarter here?
	if(station.windLayers.size() < 3)
		return;

	/* no waves for now...

	target.wave_amplitude = station.windLayers[0].speed / 30.0f;
	target.wave_length = 10.0f;
	target.wave_speed = station.windLayers[0].speed / 30.0f;
	target.wave_dir = station.windLayers[0].direction * 1.0f;
	*/

	//
	// surface wind (first wind layer)
	//
	target.wind_altitude_msl_m[0] = station.windLayers[0].alt;
	target.wind_direction_degt[0] = station.windLayers[0].direction;
	target.wind_speed_kt[0] = station.windLayers[0].speed;
	target.shear_direction_degt[0] = station.windLayers[0].variance;
	target.turbulence[0] = station.windLayers[0].turbulence / 4.0f;
	if(station.windLayers[0].gusts > 0)
		target.shear_speed_kt[0] = station.windLayers[0].gusts - station.windLayers[0].speed;
	else
		target.shear_speed_kt[0] = 0;

	// check for turbulences in wind layer
	if(aircraftAlt <= station.windLayers[0].alt && turbulencePercent < target.turbulence[0])
		turbulencePercent = target.turbulence[0];

	//
	// top wind layer (last wind layer)
	//
	target.wind_altitude_msl_m[2] = station.windLayers[station.windLayers.size() - 1].alt;
	target.wind_direction_degt[2] = station.windLayers[station.windLayers.size() - 1].direction;
	target.wind_speed_kt[2] = station.windLayers[station.windLayers.size() - 1].speed;
	target.shear_direction_degt[2] = station.windLayers[station.windLayers.size() - 1].variance;
	target.turbulence[2] = station.windLayers[station.windLayers.size() - 1].turbulence / 4.0f;
	if(station.windLayers[2].gusts > 0)
		target.shear_speed_kt[2] = station.windLayers[2].gusts - station.windLayers[2].speed;
	else
		target.shear_speed_kt[2] = 0;

	// check for turbulences in wind layer
	if(aircraftAlt >= station.windLayers[2].alt && turbulencePercent < target.turbulence[2])
		turbulencePercent = target.turbulence[2];

	//
	// the layer in between
	//
	if(aircraftAlt <= station.windLayers[1].alt) {
		// aircraft is below the second wind layer
		target.wind_altitude_msl_m[1] = station.windLayers[1].alt;
		target.wind_direction_degt[1] = station.windLayers[1].direction;
		target.wind_speed_kt[1] = station.windLayers[1].speed;
		target.shear_direction_degt[1] = station.windLayers[1].variance;
		target.turbulence[1] = station.windLayers[1].turbulence / 4.0f;
		if(station.windLayers[1].gusts > 0)
			target.shear_speed_kt[1] = station.windLayers[1].gusts - station.windLayers[1].speed;
		else
			target.shear_speed_kt[1] = 0;

	} else if(aircraftAlt >= station.windLayers[station.windLayers.size() - 2].alt) {
		// aircraft is above the last wind layer
		target.wind_altitude_msl_m[1] = station.windLayers[station.windLayers.size() - 2].alt;
		target.wind_direction_degt[1] = station.windLayers[station.windLayers.size() - 2].direction;
		target.wind_speed_kt[1] = station.windLayers[station.windLayers.size() - 2].speed;
		target.shear_direction_degt[1] = station.windLayers[station.windLayers.size() - 2].variance;
		target.turbulence[1] = station.windLayers[station.windLayers.size() - 2].turbulence / 4.0f;
		if(station.windLayers[1].gusts > 0)
			target.shear_speed_kt[1] = station.windLayers[station.windLayers.size() - 2].gusts
										- station.windLayers[station.windLayers.size() - 2].speed;
		else
			target.shear_speed_kt[1] = 0;


	} else {
		// aircraft is somewhere in between - interpolate the wind layers
		WxStation s = station;
		s.sortWindLayers(aircraftAlt, 2);

		// interpolate between those two layers
		float altDiff = s.windLayers[1].alt - s.windLayers[0].alt;
		float diff0 = aircraftAlt - s.windLayers[0].alt;
		float diff1 = s.windLayers[1].alt - aircraftAlt;

		target.wind_altitude_msl_m[1] = aircraftAlt;
		target.wind_speed_kt[1] =
			(s.windLayers[0].speed * diff0 + s.windLayers[1].speed * diff1) / altDiff;
		target.wind_direction_degt[1] = 
			(s.windLayers[0].direction * diff0 + s.windLayers[1].direction * diff1) / altDiff;
		while(target.wind_direction_degt[1] > 360.0f) target.wind_direction_degt[1] -= 360.0f;
		while(target.wind_direction_degt[1] < 0.0f) target.wind_direction_degt[1] += 360.0f;

		target.shear_direction_degt[1] =
			(s.windLayers[0].variance * diff0 + s.windLayers[1].variance * diff1) / altDiff;
		target.turbulence[1] =
			(s.windLayers[0].turbulence * diff0 + s.windLayers[1].turbulence * diff1) / altDiff / 4.0f;

		float gusts = (s.windLayers[0].gusts * diff0 + s.windLayers[1].gusts * diff1) / altDiff;
		if(gusts > 0)
			target.shear_speed_kt[1] = gusts - target.wind_speed_kt[1];
		else
			target.shear_speed_kt[1] = 0;

	}

	// check for turbulences in wind layer
	if(aircraftAlt > station.windLayers[0].alt && aircraftAlt < station.windLayers[2].alt 
		&& turbulencePercent < target.turbulence[1]) {
		turbulencePercent = target.turbulence[1];
	}
}

float WeatherGod::getVisibility(const WxStation& s)
{
	float result = MAX_VISIBILITY;
	for(size_t i = 0; i < s.visLayers.size(); ++i) {
		if(s.visLayers[i].base <= aircraftAlt
			&& s.visLayers[i].tops >= aircraftAlt
			&& s.visLayers[i].visibility < result)
			result = s.visLayers[i].visibility;
	}
	return result;
}

void WeatherGod::dump() const
{
	xivap.addText(colWhite, "Target WX:", true, true);
	target.printDebug();
	xivap.addText(colWhite, "Current WX:", true, true);
	current.printDebug();
}

void WeatherGod::setWeather(const WxStation& s, float aircraftAltm, float groundAltm)
{
	if(!initialized) return;
	if(!s.isValid()) return;


#ifdef WX_DEBUG
	xivap.addText(colWhite, "Set Weather to this wx station profile ----", false, true);
	s.debugDump();
#endif

	haveWeather = true;

	aircraftAlt = aircraftAltm;

	thunderstormPercent = 0;
	rain = false;
	rainPercent = 0;
	turbulencePercent = 0;

	// preserve this sequence of calls.
	// setClouds and setWinds calculate additional parameters like rain and turbulence
	setClouds(s);
	setWinds(s);

	// fixme - calculate templayer to sealevel
	target.temperature_sealevel_c = s.tempLayers[0].day * 1.0f;

	// Splendor-requested fix for mach number calculation above FL280
	if(aircraftAlt > 28000 / 3.2808399)
		target.temperature_sealevel_c = 15.0f;

	target.dewpoi_sealevel_c = s.tempLayers[0].dewpoint * 1.0f;

	target.barometer_sealevel_inhg = s.qnh / QNH_ALTIMETER_FACTOR;
	target.wind_turbulence_percent = turbulencePercent;
	target.thunderstorm_percent = thunderstormPercent;
	
	float microburst = (thunderstormPercent + turbulencePercent / 3.0f) / 1.33f;
	target.microburst_probability = microburst;
	
	target.rate_change_percent = 0.0f;
	target.use_real_weather_bool = 0;
	target.rain_percent = rainPercent;

	float visibility = getVisibility(s);
	target.visibility_reported_m = visibility;

	float thermalPercent = (s.tempLayers[0].day + 20.0f) / 45.0f; // more thermals on hot days
	thermalPercent *= (visibility > 20000 ? 20000 : visibility) / 7000;	// more thermals with good vis
	thermalPercent *= 0.25;
	if(thermalPercent < 0) thermalPercent = 0;
	if(thermalPercent > 0.25f) thermalPercent = 0.25f;
	target.thermal_percent = thermalPercent;

	target.thermal_rate_ms = thermalPercent * visibility / 10000.0f * 350.0f;
	if(target.thermal_rate_ms <= 0) target.thermal_rate_ms = 3;
	if(target.thermal_rate_ms > 1500) target.thermal_rate_ms = 1500;

	target.thermal_altitude_msl_m = THERMALS_MAX_AGL + groundAltm;

	// synchronize internal wx state with the simulator
	syncWx();

	// don't show the same weather station name every 30 secs
	if(s.name != oldWxStation) {
		xivap.addText(colYellow, "Setting weather to station " + s.name, true, true);
		oldWxStation = s.name;
	}

#ifdef WX_DEBUG
	xivap.addText(colYellow, "Using WX for " + s.name, true, true);
	target.printDebug();
	xivap.addText(colYellow, "Current WX follows:", true, true);
	current.printDebug();
#endif

}

void XPWeatherSituation::transit(XPWeatherSituation& target, XPWeatherSituation& lastset)
{

#define TRANSIT(x, step) {						\
	if(x < target.x) {							\
		x += step * stepMultiply;				\
		if(x > target.x) x = target.x;			\
	} else if(x > target.x) {					\
		x -= step * stepMultiply;				\
		if(x < target.x) x = target.x;			\
	}											\
}

#define TRANSIT_DIR(x, step) {						\
	if(stepMultiply != 1) {	x = target.x; }			\
	else {											\
		float f = target.x - x;						\
		if(f < -180) f += 360;						\
		else if(f > 180) f -= 360;					\
		if(fabs(f) < step) { x = target.x; }		\
		else {										\
			if(f > 0) x += step * stepMultiply;		\
			else x -= step * stepMultiply;			\
			while(x < 0) x += 360;					\
			while(x > 360) x -= 360;				\
		}											\
	}												\
}

	// if this is called for the first time - immediately set
	// the target weather, without transition
	static bool firstTime = true;
	float stepMultiply = 1;
	if(firstTime) {
		stepMultiply = 1000;
		firstTime = false;
	}

	for(int i = 0; i < CLOUD_LAYERS; ++i) {
		// no transit for cloud layers. it looks too bad in the sim (slight pause, clouds change suddently)

		cloud_type[i] = target.cloud_type[i];	// no transition for cloud types possible
		cloud_base_msl_m[i] = target.cloud_base_msl_m[i];	
		cloud_tops_msl_m[i] = target.cloud_tops_msl_m[i];	
	}

	/*
	TRANSIT(wave_amplitude, 0.1f);
	TRANSIT(wave_length, 0.1f);
	TRANSIT(wave_speed, 0.1f);
	TRANSIT_DIR(wave_dir, 1);
	*/

	for(int i = 0; i < WIND_LAYERS; ++i) {
		TRANSIT(wind_altitude_msl_m[i], 50.0f);	// let the wind layer altitude change quickly, so that
													// it stays current even with high vertical speeds

		TRANSIT_DIR(wind_direction_degt[i], 1.0f);	// wind direction change in degrees per second
		TRANSIT(shear_direction_degt[i], 1.0f);

		TRANSIT(wind_speed_kt[i], 0.6f);		// change speed by 0.6kt per second
		TRANSIT(shear_speed_kt[i], 0.6f);
		TRANSIT(turbulence[i], 0.02f);			// 2% turbulence change per second
	}

	TRANSIT(temperature_sealevel_c, 0.05f);
	TRANSIT(dewpoi_sealevel_c, 0.05f);
	TRANSIT(barometer_sealevel_inhg, 0.001f);
	TRANSIT(wind_turbulence_percent, 2.0f);
	TRANSIT(thunderstorm_percent, 2.0f);
	TRANSIT(microburst_probability, 2.0f);	
	TRANSIT(rain_percent, 2.0f);
	TRANSIT(visibility_reported_m, 200.0f);
	TRANSIT(thermal_percent, 0.1f);
	TRANSIT(thermal_rate_ms, 1.0f);
	TRANSIT(thermal_altitude_msl_m, 5.0f);

	rate_change_percent = target.rate_change_percent;
	use_real_weather_bool = target.use_real_weather_bool;

	setWeather(target, lastset);
}

void XPWeatherSituation::printDebug() const
{
	for(int i = 0; i < CLOUD_LAYERS; ++i) {
		string str = "cloud layer " + itostring(i) + ": ";
		switch(cloud_type[i]) {
			case 1: str += "HighCirrus "; break;
			case 2: str += "Scattered "; break;
			case 3: str += "Broken "; break;
			case 4: str += "Overcast "; break;
		}
		str += "base: " + ftoa(cloud_base_msl_m[i] * 3.2808399f)
			+ " top: " + ftoa(cloud_tops_msl_m[i] * 3.2808399f) + " ";
		xivap.addText(colLightGray, str, true, true);
	}

	for(int i = 0; i < WIND_LAYERS; ++i) {
		string str = "wind layer " + itostring(i) + ": alt " + ftoa(wind_altitude_msl_m[i] * 3.2808399f)
			+ " dir " + ftoa(wind_direction_degt[i]) 
			+ " speed " + ftoa(wind_speed_kt[i])
			+ " shearDir " + ftoa(shear_direction_degt[i])
			+ " shearGain " + ftoa(shear_speed_kt[i])
			+ " turb " + ftoa(turbulence[i]);
		xivap.addText(colLightGray, str, true, true);
	}

	string str = "temp.sea " + ftoa(temperature_sealevel_c)
			+ " dewp.sea " + ftoa(dewpoi_sealevel_c);
	str += " ALTI " + ftoa(barometer_sealevel_inhg*QNH_ALTIMETER_FACTOR)
		+ " turb " + ftoa(wind_turbulence_percent)
		+ " thdstrm " + ftoa(thunderstorm_percent)
		+ " microb " + ftoa(microburst_probability);
	xivap.addText(colLightGray, str, true, true);
	str = "rain " + ftoa(rain_percent)
		+ " therm " + ftoa(thermal_percent)
		+ " therm.alt " + ftoa(thermal_altitude_msl_m)
		+ " vis " + ftoa(visibility_reported_m);
	xivap.addText(colLightGray, str, true, true);
}
