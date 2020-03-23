#include "provided.h"
#include <vector>
#include <algorithm>
#include <random>
#include <math.h>  
using namespace std;

class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;
private:
    double totalDist(const GeoCoord& depot, vector<DeliveryRequest>& deliveries) const;
};

double DeliveryOptimizerImpl::totalDist(const GeoCoord& depot, vector<DeliveryRequest>& deliveries) const
{
    //calculates total crow distance between all deliveries points including from and back to depot

    double crowDist = 0;
    //iterate through deliveries
    auto deliverIt = deliveries.begin();
    auto deliverItPrev = deliveries.begin();
    for (; deliverIt != deliveries.end(); ++deliverIt)
    {
        //add depot to first location
        if (deliverIt == deliveries.begin())
            crowDist += distanceEarthMiles(depot, (*deliverIt).location);
        //location to location
        else
            crowDist += distanceEarthMiles((*deliverItPrev).location, (*deliverIt).location);
        //update prev iterator
        deliverItPrev = deliverIt;
    }
    //from last location back to depot
    crowDist += distanceEarthMiles((*deliverItPrev).location, depot);

    return crowDist;
}

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{

}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    //set old crow dist to initial order's total
    oldCrowDistance = totalDist(depot, deliveries);

    //Set initial temp
    double temp = 1000000;

    //Cooling rate
    double coolingRate = 0.003;

    // Assume best solution is the current solution
    vector<DeliveryRequest> best = deliveries;

    // Loop until system has cooled
    while (temp > 1) {
        
        //make new deliveries path
        vector<DeliveryRequest> randomDeliveries = deliveries;
        //randomize deliveries order
        random_shuffle(randomDeliveries.begin(), randomDeliveries.end());

        // Get energy of solutions
        double currentDistance = totalDist(depot, deliveries);
        double newDistance = totalDist(depot, randomDeliveries);

        //generate random double uniformly distributed between 0 and 1
        random_device rd;
        mt19937 e2(rd());
        uniform_real_distribution<> dist(0, 1);
        double rand = dist(e2);

        //calculate acceptance probabilty based on annealing method
        double acceptanceProbability = 0.0;
        //if the new solution is better than the current one, guarantee acceptance since 2.0 is certainly > (0-1)
        if (newDistance < currentDistance)
            acceptanceProbability = 2.0; 
        // If the new solution is worse, calculate an acceptance probability
        else
            acceptanceProbability = exp((currentDistance - newDistance) / temp);

        // Decide if we should accept the new path using annealing method
        if (acceptanceProbability > rand)
            deliveries = randomDeliveries;

        // Keep track of the best solution found
        if (totalDist(depot, deliveries) < totalDist(depot, best)) {
            best = deliveries;
        }

        // Cool system
        temp *= 1 - coolingRate;
    }

    newCrowDistance = totalDist(depot, best);
    //set deliveries to the best solution found
    deliveries = best;
}



//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}
