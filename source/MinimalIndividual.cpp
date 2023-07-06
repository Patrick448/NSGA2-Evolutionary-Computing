//
// Created by patrick on 06/07/23.
//

#include "MinimalIndividual.h"

MinimalIndividual::MinimalIndividual(Individual *ind)
{
    this->tec = ind->getTEC();
    this->tft = ind->getTFT();
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