#include "Job.hpp"

Job::Job(Job *j) {
    this->id = j->id;
    this->factoryId = j->factoryId;
    //this->position = j->position;
    this->t = j->t;
    this->v = j->v;
    this->C = j->C;
    this->m = j->m;
    this->startTimes = j->startTimes;
}

Job::Job(int id, int m)
{
    this->id = id;
    this->t.resize(m);
    this->v.resize(m);
    this->startTimes.resize(m);
}

int Job::getId()
{
    return this->id;
}

float Job::getTotalP()
{
    float total = 0.0;

    for (int m = 0; m < this->t.size(); m++)
    {
        total += getP(m);
    }

    return total;
}

float Job::getP(int j) { return this->t[j] / this->v[j]; }

float Job::getV(int j)
{
    return this->v[j];
}

vector<float> Job::getAllV()
{
    return this->v;
}

void Job::setVForMachine(int j, float v)
{
    this->v[j] = v;
}

float Job::getT_j(int j)
{
    return this->t[j];
}

void Job::setStartTime(int j, float start_time)
{
    this->startTimes[j] = start_time;
}

float Job::getStartTime(int j)
{
    return this->startTimes[j];
}

vector<float> Job::getStartTimes()
{
    return this->startTimes;
}
