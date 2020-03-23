#include "provided.h"
#include "ExpandableHashMap.h"

#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

unsigned int hasher(const GeoCoord& g)
{
    return std::hash<string>()(g.latitudeText + g.longitudeText);
}

class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
private:
    ExpandableHashMap<GeoCoord, vector<StreetSegment>> m_map;
};

StreetMapImpl::StreetMapImpl()
{

}

StreetMapImpl::~StreetMapImpl()
{
}

bool StreetMapImpl::load(string mapFile)
{
	ifstream inf(mapFile);   

	if (!inf) // Did opening the file fail?
	{
		return false;
	}
	else
	{
		string streetName;
		// Read each line.  The return value of getline is treated
		// as true if a line was read, false otherwise (e.g., because
		// the end of the file was reached).
		string line;
		while (getline(inf, line))
		{
			// To extract the information from the line, we'll
			// create an input stringstream from it, which acts
			// like a source of input for operator >>
			istringstream iss(line);

			//geocoord lat/longs
			string lat1;
			string long1;
			string lat2;
			string long2;
			string dummy;

			//if segment
			if (iss >> lat1 >> long1 >> lat2 >> long2 && !(iss >> dummy) && isdigit(line[0]))
			{
				
				string lat1s = lat1;
				string long1s = long1;
				string lat2s = lat2;
				string long2s = long2;

				auto startCoord = GeoCoord(lat1s, long1s);
				auto endCoord = GeoCoord(lat2s, long2s);

				auto segment = StreetSegment(startCoord, endCoord, streetName);
				//check if this key is already mapped
				auto segVectPointer = m_map.find(startCoord);
				//if mapped, then add to the existing vector
				if (segVectPointer != nullptr)
				{
					(*segVectPointer).push_back(segment);
					m_map.associate(startCoord, *segVectPointer);
				}
				//if not mapped, created a vector and map it
				else
				{
					vector<StreetSegment> temp;
					temp.push_back(segment);
					m_map.associate(startCoord, temp);
				}

				//swap start/end coords in segment
				segment = StreetSegment(endCoord, startCoord, streetName);
				//check if this key is already mapped
				segVectPointer = m_map.find(endCoord);
				//if mapped, then add to the existing vector
				if (segVectPointer != nullptr)
				{
					(*segVectPointer).push_back(segment);
					m_map.associate(endCoord, *segVectPointer);
				}
				//if not mapped, created a vector and map it
				else
				{
					vector<StreetSegment> temp;
					temp.push_back(segment);
					m_map.associate(endCoord, temp);
				}
			}
			else
			{
				bool proceed = true;
				if (line.size() <= 3)
					proceed = false;

				//if street name
				if (proceed)
				{
					streetName = line;
				}

			}
		}
	}
	return true;
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
	auto segResults = m_map.find(gc);
	//if no segments found, return false leave segs unchanged
	if (segResults == nullptr)
		return false;

	//if segments found, discard any values in segs and replace with results
	segs.clear();
	segs = *segResults;

    return true;
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
   return m_impl->getSegmentsThatStartWith(gc, segs);
}
