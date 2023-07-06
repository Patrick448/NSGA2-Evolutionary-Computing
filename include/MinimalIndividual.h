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
public:
    //implemented methods here
    MinimalIndividual(Individual *ind);
    ~MinimalIndividual();
    float getTEC();
    float getTFT();
};

#endif //GREEN_SCHEDULING_DISTRIBUTED_FLOWSHOP_PROBLEM_MINIMALINDIVIDUAL_H
