/***************************************************************************
 *   Copyright (C) 2012 by Pierre Marchand   *
 *   pierre@oep-h.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef UTILS_H
#define UTILS_H


#include <mapnik/map.hpp>
#include <proj_api.h>
#include <GeographicLib/Geodesic.hpp>
#include <GeographicLib/Geocentric.hpp>
#include <GeographicLib/GeoCoords.hpp>

#include <vector>

#include <pstreams/pstream.h>

namespace tmap
{
#define AZI_NORTH 0.0
#define AZI_EAST 90.0
#define AZI_WEST 270.0
#define AZI_SOUTH 180.0


	const std::string mapnik_config = std::string("mapnik-config");
	const std::string mapnik_default_plugin= std::string("/usr/lib/mapnik/input");
	const std::string world_mercator = std::string("+proj=merc +lon_0=0 +k=1 +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs");

	double pt2m(const double& v)
	{
		return v / 2834.64567;
	}

	double m2pt(const double& v)
	{
		return v * 2834.64567;
	}


	mapnik::coord2d follow(const mapnik::coord2d& a, const double& dist, const double& azi, const std::string& crs = world_mercator)
	{
		std::cerr<<a<<" "<<dist<<" "<<azi<<std::endl;
		projPJ pj_target = pj_init_plus("+init=epsg:4326"); // # WGS 84
		projPJ pj_merc = pj_init_plus(crs.c_str());

		double x[1];
		double y[1];
		x[0] = a.x;
		y[0] = a.y;

		int res = pj_transform(pj_merc, pj_target, 1, 1, (double*)&x, (double*)&y, NULL);
		if(res)
			throw std::string("Coordinate transformation failed");

		const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84;
		mapnik::coord2d ret;

		geod.Direct(y[0] * RAD_TO_DEG, x[0] * RAD_TO_DEG, azi, dist, ret.y, ret.x);
		double rx[1];
		double ry[1];
		rx[0] = ret.x * DEG_TO_RAD;
		ry[0] = ret.y * DEG_TO_RAD;
		pj_transform(pj_target, pj_merc, 1, 1, (double*)&rx, (double*)&ry, NULL);

		ret.x = rx[0];
		ret.y = ry[0];

		std::cerr<<"\t=>"<<ret<<std::endl;

		return ret;
	}


	double distance(const mapnik::coord2d& a, const mapnik::coord2d& b, const std::string& crs = world_mercator)
	{
		//	std::cerr<<"distanceMerc SRS: "<<crs<<std::endl;
		projPJ pj_target = pj_init_plus("+init=epsg:4326"); // # WGS 84
		projPJ pj_merc = pj_init_plus(crs.c_str());

		double x[2];
		double y[2];
		x[0] = a.x;
		x[1] = b.x;
		y[0] = a.y;
		y[1] = b.y;

		int res = pj_transform(pj_merc, pj_target, 2, 1, (double*)&x, (double*)&y, NULL);
		if(res)
			throw std::string("Coordinate transformation failed");

		//	GeographicLib::GeoCoords ca(/*0, true,*/ x[0], y[0]);
		//	GeographicLib::GeoCoords cb(/*0, true,*/ x[1], y[1]);

		const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84;
		double s12;
		//	geod.Inverse(ca.Latitude(), ca.Longitude(), cb.Latitude(), cb.Longitude(), s12);
		double dd = geod.Inverse(y[0] * RAD_TO_DEG, x[0] * RAD_TO_DEG,
			     y[1] * RAD_TO_DEG, x[1] * RAD_TO_DEG, s12);
		return s12;
	}

	double bb_width(const mapnik::box2d<double>& bbox, unsigned int r, const std::string& crs = world_mercator)
	{
		projPJ pj_target = pj_init_plus("+init=epsg:4326"); // # WGS 84
		projPJ pj_merc = pj_init_plus(crs.c_str());

		double x[4];
		double y[4];
		x[0] = bbox.minx(); y[0] = bbox.maxy(); // top left
		x[1] = bbox.maxx(); y[1] = bbox.maxy(); // top right
		x[2] = bbox.maxx(); y[2] = bbox.miny(); // bottom right
		x[3] = bbox.minx(); y[3] = bbox.miny(); // bottom left

		pj_transform(pj_merc, pj_target, 4, 1, (double*)&x, (double*)&y, NULL);

//		std::cerr<<bbox<< bbox.width() <<std::endl;
		double res(0);
		if(r == 0)
		{
			res = distance(mapnik::coord2d(bbox.minx(),bbox.miny()),
					      mapnik::coord2d(bbox.maxx(),bbox.miny()), crs);
		}
		else if(r == 1)
		{
			res = distance(mapnik::coord2d(bbox.minx(),bbox.miny() + (bbox.height()/2.0)),
				       mapnik::coord2d(bbox.maxx(),bbox.miny() + (bbox.height()/2.0)), crs);
		}
		else if(r == 2)
		{
			res = distance(mapnik::coord2d(bbox.minx(),bbox.maxy()),
				       mapnik::coord2d(bbox.maxx(),bbox.maxy()), crs);
		}

		std::cerr<<std::fixed<<"BB_WIDTH "<< res <<std::endl;
		return res;

	}


	std::string mapnik_input_dir()
	{
		std::vector<std::string> args;
		args.push_back(mapnik_config);
		args.push_back(std::string("--input-plugins"));

		redi::ipstream in(mapnik_config, args);
		std::string out;
		std::getline(in, out);
		return out;
	}

}

#endif // UTILS_H
