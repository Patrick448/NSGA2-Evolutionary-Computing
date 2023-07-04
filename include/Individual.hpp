#ifndef Individual_HPP
#define Individual_HPP

#include "defines.hpp"
#include "Factory.hpp"
#include "Job.hpp"

class Individual
{
private:
    vector<vector<int>> distribution; // Sequence of jobs in each factory
    vector<vector<float>> V;          // Speed matrix of each job in each machine
    float TEC;                        // Total energy consumption
    float TFT;                        // Total flow time
    vector<float> EC_f;               // Energy consumption of each factory
    vector<float> FT_f;               // Flow time of each factory
    vector<int> jobAllocation;       // Factory id of each job
    vector<Factory *> factories;
    int n;
    int m;
    int F;                   // List of factories
    vector<vector<float>> p; // Actual processing time of each job in each machine
    vector<float> PP;        // Energy consumption when processing of each machine
    vector<float> SP;        // Energy consumption when stand-by of each machine

    // auxiliar attributes
    int dominationRank;
    int dominationCounter;
    float crowdingDistance;

public:
    Individual(int n, int m, int F); // Constructor (n = number of jobs, m = number of machines, F = number of factories)
    Individual(Individual* sol);
    ~Individual();
    float getTEC();
    float getTFT();
    float getTFTUsingMatrix();
    Factory *getFactory(int factoryId);          // not implemented    
    int getNumFactories();

    bool dominates(Individual *other); // true if this instance dominates other
    bool crowdedCompare(Individual *other);

    void replaceFactory(int factoryId, Factory *factory);
    void setV(int jobId, int machineId, float v);
    void setDominationRank(int val);
    int getDominationRank();
    void setDominationCounter(int val);
    int getDominationCounter();
    void setCrowdingDistance(float val);
    float getCrowdingDistance();
    void incrementCrowdingDistance(float val);
    void incrementDominationCounter(int val);
    vector<Factory*> getFactories();

    void insert(int fromFactoryId, int toFactoryId, Job *job, int pos);
    void swap(int f1Id, int f2Id, Job *job1, Job *job2);

    // Auxiliar functions
    void printIndividual();

    // New methods for crossover
    vector<vector<float>> getAllV();
    void updateAllV(vector<vector<float>> newV);
};

#endif // Individual_HPP