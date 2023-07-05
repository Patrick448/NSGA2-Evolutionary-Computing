//
// Created by patri on 12/01/2023.
//

#ifndef UTIL_H
#define UTIL_H

#include "defines.hpp"
#include "Individual.hpp"
#include "Factory.hpp"

class Util {

private:
    static vector<Individual*> allocatedIndividuals;

public:
    static vector<vector<Individual*>> fastNonDominatedSort(vector<Individual*> population);
    static float DMetric(vector<Individual*> &PF, vector<Individual*> &A);
    static float SMetric(vector<Individual*> &PF, vector<Individual*> &A);
    static float GDMetric(vector<Individual*> &PF, vector<Individual*> &A);
    static float IGDMetric(vector<Individual*> &PF, vector<Individual*> &A);
    static string generateCSV(Factory* factory);
    static void allocate(Individual* sol);
    static void deallocate();
    static Individual* maxTECSol(vector<Individual*> &v);
    static Individual* minTECSol(vector<Individual*> &v);
    static Individual* maxTFTSol(vector<Individual*> &v);
    static Individual* minTFTSol(vector<Individual*> &v);
    static float meanIGDMetric(vector<vector<Individual*>> &paretoArchive, vector<Individual*> &PF);
    static float meanGDMetric(vector<vector<Individual*>> &paretoArchive, vector<Individual*> &PF);
    static float meanSMetric(vector<vector<Individual*>> &paretoArchive, vector<Individual*> &PF);
    static float meanDMetric(vector<vector<Individual*>> &paretoArchive, vector<Individual*> &PF);
    static vector<Individual*> joinFronts(vector<vector<Individual*>> fronts);
    static void outputToFile(string path, string text, bool append);
    static void checkDuplicateIndividualsAtFile(string path);


    };

#endif //UTIL_H
