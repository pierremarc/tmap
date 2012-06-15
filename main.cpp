
#include <mapnik/map.hpp>
#include <mapnik/load_map.hpp>
#include <mapnik/datasource_cache.hpp>
#include <mapnik/image_util.hpp>
#include <string>
#include <iostream>
#include <map>
#include <exception>
#include <iomanip>

#include "Utils.h"
#include "Options.h"




int main(int ac, char ** av)
{
	tmap::Options params;
	for(int i(1); i < ac; ++i)
	{
		params.Add(std::string(av[i]));
	}

	double denom(params.Get(tmap::Options::ScaleDenominator, 100000.0));

	/// prepare the map
	mapnik::datasource_cache::instance()->register_datasources(tmap::mapnik_input_dir());
	mapnik::Map m;
	mapnik::load_map(m, params.GetString(tmap::Options::MapnikStyle));

	/// get the extent
	mapnik::box2d<double> bb;
	int lc(m.layer_count());
	std::map<std::string, unsigned int> lnames;
	for(int i(0); i < lc; ++i)
	{
		mapnik::layer& l(m.layers().at(i));
		l.set_active(true);
		bb.expand_to_include(l.envelope());
		lnames[l.name()]=i;
		std::cerr<<l.name()<< ": " <<l.envelope()<<std::endl;
	}

	if(params.Has(tmap::Options::RefLayer))
	{
		bb = m.layers().at(lnames[params.GetString(tmap::Options::RefLayer)]).envelope();
		std::cerr<< params.GetString(tmap::Options::RefLayer)<< " envelope => "<<bb<<std::endl;
	}

	double trans = params.Get(tmap::Options::Transverse, 1000.0);
	if(params.Has(tmap::Options::Latitude)) // trans is height
	{
		// FIXME

		double lat(params.Get(tmap::Options::Latitude, bb.miny() + (bb.height() / 2.0)));
		// we are looking for max and min y
		double centerOnX(bb.minx() + (bb.width() / 2.0));

		double maxy(tmap::follow(mapnik::coord2d(centerOnX, lat), trans / 2.0, AZI_NORTH, m.srs()).y);
		double miny(tmap::follow(mapnik::coord2d(centerOnX, lat), trans / 2.0, AZI_SOUTH, m.srs()).y);
		bb = mapnik::box2d<double>(bb.minx(),miny, bb.maxx(), maxy);

	}
	else if(params.Has(tmap::Options::Longitude))
	{
		double lon(params.Get(tmap::Options::Longitude, bb.minx() + (bb.width() / 2.0)));

		double maxx(std::max(tmap::follow(mapnik::coord2d(lon, bb.miny()), trans / 2.0, AZI_WEST, m.srs()).x,
				     tmap::follow(mapnik::coord2d(lon, bb.maxy()), trans / 2.0, AZI_WEST, m.srs()).x));

		double minx(std::min(tmap::follow(mapnik::coord2d(lon, bb.miny()), trans / 2.0, AZI_EAST, m.srs()).x,
				     tmap::follow(mapnik::coord2d(lon, bb.maxy()), trans / 2.0, AZI_EAST, m.srs()).x));
		bb = mapnik::box2d<double>(minx,bb.miny(),maxx,bb.maxy());
	}
	else
	{
		throw std::string("No lat or lon to center the map on!");
	}

	/// Now compute map size to fit this bounding box at required scale than resize map

	double t0(tmap::bb_width(bb, 0, m.srs()));
	double t1(tmap::bb_width(bb, 1, m.srs()));
	double mapW(std::min(t0,t1) / denom);

	double mapH(tmap::distance(mapnik::coord2d(bb.minx(), bb.miny()), mapnik::coord2d(bb.minx(), bb.maxy()), m.srs()) / denom);

//	m.set_width(mapW);
//	m.set_height(mapH);
	mapnik::Map targetMap(tmap::m2pt(mapW), tmap::m2pt(mapH));
	targetMap.zoom_to_box(bb);
	std::cerr<<"Map Width: "<< targetMap.width() <<std::endl;

	std::string out(params.GetString(tmap::Options::OutputFile));
	std::string type(*(mapnik::type_from_filename(out)));
	if(type == std::string("png"))
		type = std::string("ARGB32");
	mapnik::save_to_cairo_file(m, out, type);



//	tmap::bb_width(bb, 1, m.srs());


//	double oW(tmap::m2pt());
//	double oH(0);
	// Et voila!


//	mapnik::datasource_cache::instance()->register_datasources(tmap::mapnik_input_dir());
//	int edge(1000);
//	double denom(100000.0);
//	if(ac > 4)
//		edge = atoi(av[4]);
//	if(ac > 5)
//		denom = double(atoi(av[5]));
//	mapnik::Map m(edge,edge);
//	std::string xml(av[1]);
//	mapnik::load_map(m,xml);
//	std::cerr<<"Loaded "<<m.layer_count()<<" layers. Map SRS: "<<m.srs()<<std::endl;
//	int lc(m.layer_count());
//	std::map<std::string, unsigned int> lnames;
//	for(int i(0); i < lc; ++i)
//	{
//		mapnik::layer& l(m.layers().at(i));
//		l.set_active(true);
//		lnames[l.name()]=i;
//		std::cerr<<l.name()<< ": " <<l.envelope()<<std::endl;
//	}
//	mapnik::layer& mainLayer(m.layers().at(lnames[std::string("planetosmroads")]));
//	mapnik::box2d<double> bb(mainLayer.envelope());
//	mapnik::coord2d center(bb.center());


//	mapnik::coord2d a(bb.minx(),bb.miny());
//	mapnik::coord2d b(bb.maxx(),bb.maxy());

//	double d(tmap::distance(a,b,mainLayer.srs()));

//	std::cerr<<"Distance0: "<<(bb.maxy()-bb.minx())<<std::endl;
//	std::cerr<<"Distance1: "<<d<<std::endl;

//	double medge(tmap::pt2m(edge));

//	double tw(medge * denom);
//	double th(medge * denom);

//	// look for a kind of top left
//	mapnik::coord2d tmpc(tmap::follow(center, tw/ 2.0, AZI_WEST, mainLayer.srs()));
//	mapnik::coord2d tl(tmap::follow(tmpc, tw/ 2.0, AZI_NORTH, mainLayer.srs()));

//	std::cerr<<"Check0 "<<std::fixed<<tmap::distance(center, tmpc, mainLayer.srs())<<std::endl;
//	std::cerr<<"Check1 "<<std::fixed<<tmap::distance(tl, tmpc, mainLayer.srs())<<std::endl;
//	std::cerr<<"Check2 "<<std::fixed<<tmap::distance(tl, center, mainLayer.srs())<<std::endl;

//	// look for a kind of bottom right
//	mapnik::coord2d tmpd(tmap::follow(center, tw/ 2.0, AZI_EAST, mainLayer.srs()));
//	mapnik::coord2d br(tmap::follow(tmpd, tw/ 2.0, AZI_SOUTH, mainLayer.srs()));

//	mapnik::box2d<double> tbb(tl, br);

//	std::cerr<<"Check3 "<<std::fixed<<tmap::distance(tl, br, mainLayer.srs())<< " " << tbb.width() <<std::endl;


//	m.zoom_to_box(tbb);
//	std::cerr<<"Scale: 1/"<<m.scale_denominator() << " scale: "<<m.scale()<< "Map Width: "<< m.width() <<std::endl;
////	mapnik::save_to_cairo_file(m, std::string("/home/pierre/cairo.png"), std::string("ARGB32"));
//	std::string out(av[3]);
//	std::string type(*(mapnik::type_from_filename(out)));
//	if(type == std::string("png"))
//		type = std::string("ARGB32");
//	mapnik::save_to_cairo_file(m, out, type);
	return 0;
}

