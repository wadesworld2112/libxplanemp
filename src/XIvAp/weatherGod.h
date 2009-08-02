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

#ifndef WEATHER_GOD
#define WEATHER_GOD

#include "weatherPosition.h"

//#define WX_DEBUG

#define CLOUD_LAYERS 3
#define WIND_LAYERS 3

// max. altitude of thermals agl
#define THERMALS_MAX_AGL	2000

// max. visibility in m
#define MAX_VISIBILITY		40000 // 40 KM

#define QNH_ALTIMETER_FACTOR (1013.25f/29.92f)

#define WEATHER_TRANSITION_INTERVAL 1.0f;

class XPWeatherSituation {
public:
	XPWeatherSituation();

	int cloud_type[CLOUD_LAYERS];
	float cloud_base_msl_m[CLOUD_LAYERS];
	float cloud_tops_msl_m[CLOUD_LAYERS];

	float visibility_reported_m, rain_percent, thunderstorm_percent,
		wind_turbulence_percent, barometer_sealevel_inhg,
		microburst_probability, rate_change_percent;
	int use_real_weather_bool;

	float wind_altitude_msl_m[WIND_LAYERS];
	float wind_direction_degt[WIND_LAYERS];
	float wind_speed_kt[WIND_LAYERS];
	float shear_direction_degt[WIND_LAYERS];
	float shear_speed_kt[WIND_LAYERS];
	float turbulence[WIND_LAYERS];

	float wave_amplitude, wave_length, wave_speed, wave_dir,
		temperature_sealevel_c, dewpoi_sealevel_c,
		thermal_percent, thermal_rate_ms, thermal_altitude_msl_m;

	void init();
	void getWeather();

	// call once per second
	// gradually sets the current situation to target
	// and resets the current weather situation in xplane
	void transit(XPWeatherSituation& target, XPWeatherSituation& lastset);
	void printDebug() const;

private:
	bool initialized;

	void setWeather(const XPWeatherSituation& target, XPWeatherSituation& lastset);

	// data refs
	XPLMDataRef ref_cloud_type[CLOUD_LAYERS],
				ref_cloud_base_msl_m[CLOUD_LAYERS],
				ref_cloud_tops_msl_m[CLOUD_LAYERS];

	XPLMDataRef ref_wind_altitude_msl_m[WIND_LAYERS],
				ref_wind_direction_degt[WIND_LAYERS],
				ref_wind_speed_kt[WIND_LAYERS],
				ref_shear_direction_degt[WIND_LAYERS],
				ref_shear_speed_kt[WIND_LAYERS],
				ref_turbulence[WIND_LAYERS];

	XPLMDataRef ref_visibility_reported_m, ref_rain_percent,
		ref_thunderstorm_percent, ref_wind_turbulence_percent, 
		ref_barometer_sealevel_inhg, ref_runway_friction,
		ref_microburst_probability, ref_rate_change_percent, ref_use_real_weather_bool,
		ref_wave_amplitude, ref_wave_length, ref_wave_speed, ref_wave_dir,
		ref_temperature_sealevel_c, ref_dewpoi_sealevel_c,
		ref_thermal_percent, ref_thermal_rate_ms, ref_thermal_altitude_msl_m;
};

class WeatherGod {
public:
	WeatherGod(): initialized(false), haveWeather(false) {};

	void init();
	void setWeather(const WxStation& station, float aircraftAltm, float groundAltm);

	// call once per second
	// call this periodically, it smoothly sets the current weather situation
	// to the target weather situation
	void transit() { if(initialized && haveWeather) current.transit(target, lastset); };

	void dump() const;

private:
	// synchronizes the current weather station with reality
	void syncWx();

	bool initialized;
	bool haveWeather;
	float aircraftAlt;

	float thunderstormPercent;

	bool rain;
	float rainPercent;
	float turbulencePercent;

	void setClouds(const WxStation& s);
	void setWinds(const WxStation& s);
	float getVisibility(const WxStation& s);

	XPWeatherSituation current;		// currently active wx
	XPWeatherSituation target;		// desired wx (the one we're moving towards)
	XPWeatherSituation lastset;		// the wx settings we did write to XP

	// used for console printout only
	string oldWxStation;
};

#endif