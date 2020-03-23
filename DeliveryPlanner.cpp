#include "provided.h"
#include <vector>
using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;
private:
    const StreetMap* streets;
    PointToPointRouter router;
    DeliveryOptimizer optimizer;
    string getDirection(double angle) const;
    void generateCommands(list<StreetSegment> segmentList, vector<DeliveryCommand>& commands) const;
};

string DeliveryPlannerImpl::getDirection(double angle) const
{
    while (angle < 0)
        angle += 360;
    if ((0 <= angle && angle < 22.5) || angle >= 337.5)
        return "east";
    else if (22.5 <= angle && angle < 67.5)
        return "northeast";
    else if (67.5 <= angle && angle < 112.5)
        return "north";
    else if (112.5 <= angle && angle < 157.5)
        return "northwest";
    else if (157.5 <= angle && angle < 202.5)
        return "west";
    else if (202.5 <= angle && angle < 247.5)
        return "southwest";
    else if (247.5 <= angle && angle < 292.5)
        return "south";
    else if (292.5 <= angle && angle < 337.5)
        return "southeast";
    else
        return "invalid angle";
}

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
    :router(sm), optimizer(sm)
{
    streets = sm;
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
}

void DeliveryPlannerImpl::generateCommands(list<StreetSegment> segmentList, vector<DeliveryCommand>& commands) const
{
    //this function generates proceed and turn commands, deliver commands should be handled outside of it

    //initialize iterators and tracker variables
    
    auto segmentIt = segmentList.begin();
    StreetSegment startSegment = *segmentIt;
    StreetSegment prevSegment = *segmentIt;
    string startSegmentDirection = getDirection(angleOfLine(startSegment));
    double distanceTracker = distanceEarthMiles((startSegment).start, (startSegment).end);
    
    for (segmentIt++;segmentIt != segmentList.end();segmentIt++)
    {
        if ((*segmentIt).name == startSegment.name && (*segmentIt).start != segmentList.back().start && (*segmentIt).end != segmentList.back().end)
        {
            //add the distance of the current segment to the running total for this proceed command
            distanceTracker += distanceEarthMiles((*segmentIt).start, (*segmentIt).end);
            prevSegment = *segmentIt;
            continue;
        }
        else
        {
            //if last segment in the segment list
            if ((*segmentIt).start == segmentList.back().start && (*segmentIt).end == segmentList.back().end)
                distanceTracker += distanceEarthMiles((*segmentIt).start, (*segmentIt).end);
            //proceed command for the path traveled along street
            DeliveryCommand proceed = DeliveryCommand();
            proceed.initAsProceedCommand(startSegmentDirection, startSegment.name, distanceTracker);
            commands.push_back(proceed);
            //if last segment, dont make a turn command
            if ((*segmentIt).start != segmentList.back().start && (*segmentIt).end != segmentList.back().end)
            {
                //turn or proceed command for next street
                auto turnAngle = angleBetween2Lines(prevSegment, *segmentIt);
                if (turnAngle < 1 || turnAngle > 359)
                {
                    //begin generation of next proceed command
                    startSegment = *segmentIt;
                    distanceTracker = distanceEarthMiles((startSegment).start, (startSegment).end);
                    startSegmentDirection = getDirection(angleOfLine(startSegment));
                    prevSegment = *segmentIt;
                    continue;
                }
                else if (turnAngle >= 1 && turnAngle < 180)
                {
                    //turn left
                    DeliveryCommand turn = DeliveryCommand();
                    turn.initAsTurnCommand("left", (*segmentIt).name);
                    commands.push_back(turn);
                }
                else if (turnAngle >= 180 && turnAngle <= 359)
                {
                    //turn right
                    DeliveryCommand turn = DeliveryCommand();
                    turn.initAsTurnCommand("right", (*segmentIt).name);
                    commands.push_back(turn);
                }
            }
            

            //set start segment, start seg direction, distance tracker
            startSegment = *segmentIt;
            distanceTracker = distanceEarthMiles((startSegment).start, (startSegment).end);
            startSegmentDirection = getDirection(angleOfLine(startSegment));
        }

        //update previous segment
        prevSegment = *segmentIt;
    }

    

}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    //cannot operate on empty deliveries
    if (deliveries.empty())
        return BAD_COORD;

    //clear commands vector in case it contains junk
    commands.clear();
    
    totalDistanceTravelled = 0.0;
    double tempDistanceTracker = 0.0;

    //initialize crow distances to pass into optimizeDeliveryOrder
    double oldcrowdistance = 0.0;
    double newcrowdistance = 0.0;
    //potentially reorder deliveries using optimizer
    auto orderedDeliveries = deliveries;
    optimizer.optimizeDeliveryOrder(depot, orderedDeliveries, oldcrowdistance, newcrowdistance);
    //create a list to temporarily hold streetsegments from router
    //list contents are cleared when processed by generatePointToPointRoute
    list<StreetSegment> segmentList;

    //for each delivery
    auto it = orderedDeliveries.begin();
    auto itPrev = orderedDeliveries.end();
    for (; it != orderedDeliveries.end(); it++)
    {
        if (it == orderedDeliveries.begin())
        {
            //for the first route, start from the depot and route to the first delivery location
            if (router.generatePointToPointRoute(depot, (*it).location, segmentList, tempDistanceTracker) != DELIVERY_SUCCESS)
                return router.generatePointToPointRoute(depot, (*it).location, segmentList, tempDistanceTracker);
            totalDistanceTravelled += tempDistanceTracker;
        }
        else
        {
            //find route from previous delivery location to next one
            if (router.generatePointToPointRoute((*itPrev).location, (*it).location, segmentList, tempDistanceTracker) != DELIVERY_SUCCESS)
                return router.generatePointToPointRoute((*itPrev).location, (*it).location, segmentList, tempDistanceTracker);
            totalDistanceTravelled += tempDistanceTracker;
        }
        
        //if the segment list is not empty, generate commands for the route that was generated
        if (!segmentList.empty())
            generateCommands(segmentList, commands);

        //finally, generate a deliver command
        DeliveryCommand deliver = DeliveryCommand();
        deliver.initAsDeliverCommand((*it).item);
        commands.push_back(deliver);

        //update the previous iterator
        itPrev = it;
    }

    //iterate back to the final delivery location
    it--; 
    //route back to depot once finished with deliveries
    if (router.generatePointToPointRoute((*it).location, depot, segmentList, tempDistanceTracker) != DELIVERY_SUCCESS)
        return router.generatePointToPointRoute((*it).location, depot, segmentList, tempDistanceTracker);
    totalDistanceTravelled += tempDistanceTracker;

    //convert sgementList into proceed or turn commands
    if (!segmentList.empty())
        generateCommands(segmentList, commands);

    
    return DELIVERY_SUCCESS;
}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}
