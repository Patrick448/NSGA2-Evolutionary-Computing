#include "Individual.hpp"
#include "Util.hpp"

Individual::Individual(Individual *sol) {
    //this->EC_f = sol->EC_f;
    //this->FT_f = sol->FT_f;
    this->n = sol->n;
    this->m = sol->m;
    this->F = sol->F;
    this->factories.reserve(F);
    for(int i=0; i< sol->factories.size(); i++){
        this->factories.push_back(new Factory(sol->factories[i]));
    }
    Util::allocate(this);

}

Individual::Individual(int n, int m, int F)
{
    this->F = F;
    this->n = n;
    this->m = m;

    this->distribution.resize(F);
    this->EC_f.resize(F);
    this->FT_f.resize(F);
    this->jobAllocation.resize(n);
    this->p.resize(n);

    for (int i = 0; i < m; i++)
    {
        this->PP.push_back(0);
        this->SP.push_back(0);
    }

    for (int i = 0; i < F; i++)
    {
        this->EC_f[i] = 0;
        this->FT_f[i] = 0;

        Factory *f = new Factory(i, m);
        this->factories.push_back(f);
    }

    for (int i = 0; i < n; i++)
    {
        this->p[i].resize(m);

        this->jobAllocation[i] = -1;
    }

    this->TEC = 0;
    this->TFT = 0;

    Util::allocate(this);
}

Individual::~Individual()
{

    for(Factory* f: this->factories){
        vector<Job*> jobs =  f->getJobs();

        for(Job * j: jobs){
            delete j;
        }

        delete f;
    }

}

float Individual::getTEC()
{
    float tec = 0.0;
    for (int i = 0; i < this->factories.size(); i++)
    {
        tec += this->factories[i]->getTEC();
    }

    return tec;
}

float Individual::getTFT()
{
    float tft = 0.0;
    for (int i = 0; i < this->factories.size(); i++)
    {
        tft += this->factories[i]->getTFT();
    }

    return tft;
}

float Individual::getTFTUsingMatrix(){
    float tft = 0.0;
    for (int i = 0; i < this->factories.size(); i++)
    {   this->factories[i]->initializeJobsStartTimes();
        tft += this->factories[i]->getTFTAfterStartTimesSet();
    }

    return tft;
}
void Individual::setDominationRank(int val)
{
    this->dominationRank = val;
}
void Individual::setDominationCounter(int val)
{
    this->dominationCounter = val;
}
void Individual::setCrowdingDistance(float val)
{
    this->crowdingDistance = val;
}

int Individual::getDominationRank()
{
    return dominationRank;
}
int Individual::getDominationCounter()
{
    return dominationCounter;
}
float Individual::getCrowdingDistance()
{
    return crowdingDistance;
}

void Individual::incrementCrowdingDistance(float val)
{
    this->crowdingDistance += val;
}

void Individual::incrementDominationCounter(int val)
{
    this->dominationCounter += val;
}

Factory *Individual::getFactory(int factoryId)
{
    return this->factories[factoryId];
}


void Individual::replaceFactory(int factoryId, Factory *factory)
{
    //this->factories[factoryId]->clearJobs();
    delete this->factories[factoryId];
    this->factories[factoryId] = factory;
}

bool Individual::dominates(Individual *other)
{
    if (this->getTFT() < other->getTFT() && this->getTEC() < other->getTEC())
    {
        return true;
    }

    return false;
}

bool Individual::crowdedCompare(Individual *other)
{

    if (this->getDominationRank() < other->getDominationRank())
    {
        return true;
    }
    else if (this->getDominationRank() == other->getDominationRank())
    {
        if (this->getCrowdingDistance() > other->getCrowdingDistance())
        {
            return true;
        }
        return false;
    }

    return false;
}

void Individual::printIndividual()
{
    for (int f = 0; f < this->F; f++)
    {
        cout << "Factory " << f << " -> ";

        for (int i = 0; i < this->factories[f]->getTotalJobs(); i++)
        {
            cout << this->factories[f]->getJobs()[i]->getId() << " ";
        }

        cout << endl;
    }
}

void Individual::insert(int fromFactoryId, int toFactoryId, Job *job, int pos)
{
    this->factories[fromFactoryId]->removeJob(job->getId());
    this->factories[toFactoryId]->insertJobAtPos(job, pos);

    // Reinitialize the start times
    this->factories[fromFactoryId]->initializeJobsStartTimes();
    this->factories[toFactoryId]->initializeJobsStartTimes();
}

void Individual::swap(int f1Id, int f2Id, Job *job1, Job *job2)
{
    if(job1->getId() == job2->getId()){
        return;
    }

    int pos1 = this->factories[f1Id]->getJobPosAtSeq(job1->getId());
    int pos2 = this->factories[f2Id]->getJobPosAtSeq(job2->getId());

    this->factories[f1Id]->removeJob(job1->getId());
    this->factories[f2Id]->removeJob(job2->getId());

    this->factories[f1Id]->insertJobAtPos(job2, pos1);
    this->factories[f2Id]->insertJobAtPos(job1, pos2);
}

int Individual::getNumFactories() {
    return this->factories.size();
}

vector<Factory*> Individual::getFactories(){
    return this->factories;
}

vector<vector<float>> Individual::getAllV(){
    vector<vector<float>> allV;

    // Iterate over jobs to get their V
    for(Factory* f: this->factories)
    {
        vector<Job*> jobs = f->getJobs();

        for(Job* n: jobs)
        {
            allV.push_back(n->getAllV());
        }
    }

    return allV;
}

void Individual::updateAllV(vector<vector<float>> newV){
    
    // Update each job of each factory
    for(Factory* f: this->factories)
    {
        vector<Job*> jobs = f->getJobs();

        for(Job* n: jobs)
        {
            n->setV(newV[n->getId()]);
        }

        // Reinitialize jobs start times in the factory
        f->initializeJobsStartTimes();

        // Inform factory that TFT and TEC need to be recalculated
        f->setTECTFTChanged();
    }
}