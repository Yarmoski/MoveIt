#include "provided.h"
#include "ExpandableHashMap.h"
#include <list>
#include <vector>
#include <queue>
#include <memory>
using namespace std;

class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
private:
    struct SearchCell
    {
        bool visited = false;
        GeoCoord coord;
        shared_ptr<SearchCell> parent;
        double L = INFINITY; //local goal
        double G = INFINITY; //global goal
        StreetSegment segment;

        SearchCell(GeoCoord coord, shared_ptr<SearchCell> parent)
            :coord(coord), parent(parent)
        {
        }
        SearchCell(GeoCoord coord, shared_ptr<SearchCell> parent, StreetSegment segment)
            :coord(coord), parent(parent), segment(segment)
        {
        }
    };

    const StreetMap* streets;
};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
    streets = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    //pre checks
    route.clear();
    totalDistanceTravelled = 0;

    vector<StreetSegment> garbage;
    if (!(streets->getSegmentsThatStartWith(start, garbage)))
        return BAD_COORD;
    if (!(streets->getSegmentsThatStartWith(end, garbage)))
        return BAD_COORD;

    if (start == end)
        return DELIVERY_SUCCESS;

    //structure to track and store search cells
    ExpandableHashMap<GeoCoord, shared_ptr<SearchCell>> cellMap; 

    //initialize the start and end cells and add them to the structure
    shared_ptr<SearchCell> cellStart = shared_ptr<SearchCell>(new SearchCell(start, nullptr));
    shared_ptr<SearchCell> cellEnd = shared_ptr<SearchCell>(new SearchCell(end, nullptr));;
    cellMap.associate(start, cellStart);
    cellMap.associate(end, cellEnd);
    //create and set current cell to the start
    shared_ptr<SearchCell> cellCurrent = cellStart;
    //set the starting cell's local and global distances
    cellStart->L = 0.0;
    cellStart->G = distanceEarthMiles(cellStart->coord, cellEnd->coord);
    //create the open list and add the starting/current cell
    list<shared_ptr<SearchCell>> notTestedList;
    notTestedList.push_back(cellStart);

    //while the open list contains cells and the current cell isnt at the end
    while (!notTestedList.empty() && cellCurrent != cellEnd)
    {
        //sort the open list from lowest to highest global distance
        notTestedList.sort([](const shared_ptr<SearchCell> lhs, const shared_ptr<SearchCell> rhs) {return lhs->G < rhs->G;});

        //pop visited cells off the list
        while (!notTestedList.empty() && notTestedList.front()->visited)
            notTestedList.pop_front();
        //since cells may have been popped, check again if the open list is empty
        if (notTestedList.empty())
            break;

        //condition for final cell's segment
        shared_ptr<SearchCell> previous = nullptr;
        //if the current cell is about to be set to the final cell
        //track the second to last cell
        if (notTestedList.front() == cellEnd)
        {
            previous = cellCurrent;
        }
        
        //set current cell to the first cell on the open list
        cellCurrent = notTestedList.front();
        //mark this cell as visited
        cellCurrent->visited = true;


        //create a vector to store the segments that start with the current cell's coord
        vector<StreetSegment> temp;
        streets->getSegmentsThatStartWith(cellCurrent->coord, temp);
        //for every segment in the vector
        vector<StreetSegment>::iterator it = temp.begin();
        for (; it != temp.end();it++)
        {
            //try to locate a cell in the cell map
            auto neighborPointer = cellMap.find((*it).end);
            
            //if this cell exists
            if (neighborPointer != nullptr && *neighborPointer != nullptr)
            {
                //set neighbor to a neighboring cell
                auto neighbor = *neighborPointer;
                
                //if this is the final cell and neighbor is the cell before it on the path
                if (cellCurrent == cellEnd && neighbor == previous)
                {
                    //generate and associate the segment connecting the two
                    StreetSegment newSeg;
                    newSeg.start = (*it).end;
                    newSeg.end = (*it).start;
                    newSeg.name = (*it).name;
                    cellEnd->segment = newSeg;
                }

                //if the neighbor has not been visited, add it to the open list
                if (!(neighbor->visited))
                    notTestedList.push_back(neighbor);

                //set a variable goal to the current cell's local distance plus the distance from itself to the neighbor
                double goal = cellCurrent->L + distanceEarthMiles(cellCurrent->coord, neighbor->coord);
                //if goal is less than the neighbor's local distance
                if (goal < neighbor->L)
                {
                    //use it on the path and update its information in the cell map
                    neighbor->parent = cellCurrent;
                    neighbor->L = goal;
                    neighbor->G = neighbor->L + distanceEarthMiles(neighbor->coord, cellEnd->coord);
                    cellMap.associate((*it).end, shared_ptr<SearchCell>(new SearchCell((*it).end, cellCurrent, *it)));
                }
                
                    
            }
            else //geocoord is not in the map
            {
                //add the cell to the map
                cellMap.associate((*it).end, shared_ptr<SearchCell>(new SearchCell((*it).end, cellCurrent, *it)));
                //find neighbors
                auto newNeighborPointer = cellMap.find((*it).end);
                shared_ptr<SearchCell> newNeighbor = *newNeighborPointer;
                notTestedList.push_back(newNeighbor);
                //set a variable goal to the current cell's local distance plus the distance from itself to the neighbor
                double goal = cellCurrent->L + distanceEarthMiles(cellCurrent->coord, newNeighbor->coord);
                //if goal is less than the neighbor's local distance
                if (goal < newNeighbor->L)
                {
                    //update the neighbor's members with the new distances
                    newNeighbor->parent = cellCurrent;
                    newNeighbor->L = goal;
                    newNeighbor->G = newNeighbor->L + distanceEarthMiles(newNeighbor->coord, cellEnd->coord);
                }
            }
        }

    }

    //if the last cell is not nullptr
    if (cellEnd != nullptr)
    {
        //set p to the last cell
        auto p = cellEnd; 
        //trace back down the path until reach the first cell
        while (p != cellStart)
        {
            //for each cell along the path, add its segment member to the route vector
            route.push_back(p->segment);
            // Set next node to this node's parent
            p = p->parent;
        }

        //reverse the route to get the directions from the start
        route.reverse();
        //add the final cell's L distance to total distance
        //since the final cell's L distance will equal the distances along the path
        totalDistanceTravelled += cellEnd->L;
        return DELIVERY_SUCCESS;
    }

    //if the last cell is nullptr, return no_route
    return NO_ROUTE; 
}


//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}
