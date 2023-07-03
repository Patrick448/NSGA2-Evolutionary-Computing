#ifndef PROBLEM_HPP
#define PROBLEM_HPP

#include "defines.hpp"

class Problem
{
private:
    int F;                 // Number of factories
    int n;                 // Number of jobs
    int m;                 // Number of machines
    vector<vector<int>> t; // Standard processing time matrix (line = job, column = machine)
    vector<float> speeds;  // Available processing speeds

public:
    Problem(int F, int n, int m, vector<vector<int>> t); // Constructor (F = number of factories, n = number of jobs, m = number of machines)
    ~Problem();

    int getF();
    int getN();
    int getM();
    vector<vector<int>> getAllT();
    vector<float> getAllSpeeds();

    int getT(int machine_id, int job_id);
    int getSpeed(int i);
};

#endif // PROBLEM_HPP