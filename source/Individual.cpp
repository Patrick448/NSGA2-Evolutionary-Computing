#include "Individual.hpp"
#include "Util.hpp"

Individual::Individual(Individual *sol) {
    this->V = sol->V;
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
    this->V.resize(n);
    this->EC_f.resize(F);
    this->FT_f.resize(F);
    this->job_allocation.resize(n);
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
        this->V[i].resize(m);
        this->p[i].resize(m);

        this->job_allocation[i] = -1;
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
    float tft = 0.0;
    for (int i = 0; i < this->factories.size(); i++)
    {
        tft += this->factories[i]->getTEC();
    }

    return tft;
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

Factory *Individual::getFactory(int f_id)
{
    return this->factories[f_id];
}

void Individual::setV(int job_id, int mach_id, float v)
{
    this->V[job_id][mach_id] = v;
}

void Individual::replaceFactory(int f_id, Factory *factory)
{
    //this->factories[f_id]->clearJobs();
    delete this->factories[f_id];
    this->factories[f_id] = factory;
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

void Individual::insert(int from_f_id, int to_f_id, Job *job, int pos)
{
    this->factories[from_f_id]->removeJob(job->getId());
    this->factories[to_f_id]->insertJobAtPos(job, pos);
}

void Individual::swap(int f1_id, int f2_id, Job *job1, Job *job2)
{
    if(job1->getId() == job2->getId()){
        return;
    }

    int pos1 = this->factories[f1_id]->getJobPosAtSeq(job1->getId());
    int pos2 = this->factories[f2_id]->getJobPosAtSeq(job2->getId());

    this->factories[f1_id]->removeJob(job1->getId());
    this->factories[f2_id]->removeJob(job2->getId());

    this->factories[f1_id]->insertJobAtPos(job2, pos1);
    this->factories[f2_id]->insertJobAtPos(job1, pos2);
}

int Individual::getNumFactories() {
    return this->factories.size();
}

vector<Factory*> Individual::getFactories(){
    return this->factories;
}