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

#include "Options.h"

#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

namespace tmap {

	const std::string Options::Transverse = std::string("trans");
	const std::string Options::OutputFile = std::string("file");
	const std::string Options::ScaleDenominator = std::string("scale");
	const std::string Options::MapnikStyle = std::string("style");
	const std::string Options::Longitude = std::string("lon");
	const std::string Options::Latitude = std::string("lat");
	const std::string Options::Projection = std::string("proj"); // ???
	const std::string Options::RefLayer = std::string("layer");
	
	Options::Options()
	{
	}

	void Options::Add(const std::string &paramstring)
	{
		std::vector<std::string> res;
		boost::algorithm::split( res, paramstring, boost::algorithm::is_any_of("="), boost::algorithm::token_compress_on );
		if(res.size() > 1)
			pData[res.at(0)] = res.at(1);
	}

	void Options::Add(const std::string &key, const std::string &val)
	{
		pData[key] = val;
	}

	bool Options::Has(const std::string &key) const
	{
		if(pData.find(key) != pData.end())
			return true;
		return false;
	}

	bool Options::Has(const std::vector<std::string> &keys) const
	{
		BOOST_FOREACH(const std::string& key , keys)
		{
			if(!Has(key))
				return false;
		}
		return true;
	}

	std::vector<std::string> Options::Keys() const
	{
		std::vector<std::string> ret;
		std::map<std::string, std::string>::const_iterator it;
		for(it = pData.begin(); it != pData.end(); it++)
		{
			ret.push_back(it->first);
		}
		return ret;
	}

	std::string Options::GetString(const std::string &key) const
	{
		if(Has(key))
			return pData.find(key)->second;
		return std::string();
	}

	void Options::Remove(const std::string &key)
	{
		pData.erase(key);
	}


	
} // namespace tmap
