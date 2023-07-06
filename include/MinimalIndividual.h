//
// Created by patrick on 06/07/23.
//

#ifndef GREEN_SCHEDULING_DISTRIBUTED_FLOWSHOP_PROBLEM_MINIMALINDIVIDUAL_H
#define GREEN_SCHEDULING_DISTRIBUTED_FLOWSHOP_PROBLEM_MINIMALINDIVIDUAL_H

#include "defines.hpp"
#include "Individual.hpp"

class MinimalIndividual
{

private:
    float tec;
    float tft;
    int dominationCounter;
    int dominationRank;
    float crowdingDistance;

public:
    //implemented methods here
    MinimalIndividual(Individual *ind);
    ~MinimalIndividual();
    float getTEC();
    float getTFT();
    void setDominationCounter(int val);
    int getDominationCounter();
    void incrementDominationCounter(int val);
    bool dominates(MinimalIndividual *other); // true if this instance dominates other
    void setDominationRank(int val);


};

#endif //GREEN_SCHEDULING_DISTRIBUTED_FLOWSHOP_PROBLEM_MINIMALINDIVIDUAL_H
