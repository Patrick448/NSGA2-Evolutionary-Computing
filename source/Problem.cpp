#include "Problem.hpp"
#include "Factory.hpp"

Problem::Problem(int F, int n, int m, vector<vector<int>> t) {
    this->F = F;
    this->n = n;
    this->m = m;
    this->t = t;

    this->speeds.push_back(1.0);
    this->speeds.push_back(1.3);
    this->speeds.push_back(1.55);
    this->speeds.push_back(1.75);
    this->speeds.push_back(2.1);
    Factory::setSpeeds(this->speeds);
}

Problem::~Problem() {
}

int Problem::getF() {
    return this->F;
}

int Problem::getN() {
    return this->n;
}

int Problem::getM() {
    return this->m;
}

vector<vector<int>> Problem::getAllT() {
    return this->t;
}

vector<float> Problem::getAllSpeeds() {
    return this->speeds;
}

int Problem::getT(int machine_id, int job_id) {
    return t[machine_id][job_id];
}

int Problem::getSpeed(int i) {
    return this->speeds[i];
}