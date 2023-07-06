//
// Created by patrick on 06/07/23.
//

#include "MinimalIndividual.h"

MinimalIndividual::MinimalIndividual(Individual *ind)
{
    this->tec = ind->getTEC();
    this->tft = ind->getTFT();
    this->dominationCounter = ind->getDominationCounter();
    this->dominationRank = ind->getDominationRank();
    this->crowdingDistance = ind->getCrowdingDistance();
}

MinimalIndividual::~MinimalIndividual()
{
}

float MinimalIndividual::getTEC()
{
    return this->tec;
}

float MinimalIndividual::getTFT()
{
    return this->tft;
}

int MinimalIndividual::getDominationCounter()
{
    return dominationCounter;
}


void MinimalIndividual::incrementDominationCounter(int val)
{
    this->dominationCounter += val;
}

bool MinimalIndividual::dominates(MinimalIndividual *other)
{
    if (this->getTFT() < other->getTFT() && this->getTEC() < other->getTEC())
    {
        return true;
    }

    return false;
}

void MinimalIndividual::setDominationRank(int val)
{
    this->dominationRank = val;
}
void MinimalIndividual::setDominationCounter(int val)
{
    this->dominationCounter = val;
}