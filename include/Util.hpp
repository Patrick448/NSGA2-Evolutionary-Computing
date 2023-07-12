//
// Created by patri on 12/01/2023.
//

#ifndef UTIL_H
#define UTIL_H

#include "defines.hpp"
#include "Individual.hpp"
#include "Factory.hpp"
#include "MinimalIndividual.h"
#include "Problem.hpp"

class Util {

private:
    static vector<Individual*> allocatedIndividuals;

public:
    static vector<vector<MinimalIndividual*>> fastNonDominatedSort(vector<MinimalIndividual*> population);
    static float DMetric(vector<MinimalIndividual*> &PF, vector<MinimalIndividual*> &A);
    static float SMetric(vector<MinimalIndividual*> &PF, vector<MinimalIndividual*> &A);
    static float GDMetric(vector<MinimalIndividual*> &PF, vector<MinimalIndividual*> &A);
    static float IGDMetric(vector<MinimalIndividual*> &PF, vector<MinimalIndividual*> &A);
    static string generateCSV(Factory* factory);
    static void allocate(Individual* sol);
    static void deallocate();
    static MinimalIndividual* maxTECSol(vector<MinimalIndividual*> &v);
    static MinimalIndividual* minTECSol(vector<MinimalIndividual*> &v);
    static MinimalIndividual* maxTFTSol(vector<MinimalIndividual*> &v);
    static MinimalIndividual* minTFTSol(vector<MinimalIndividual*> &v);
    static Individual* maxTECSol(vector<Individual*> &v);
    static Individual* minTECSol(vector<Individual*> &v);
    static Individual* maxTFTSol(vector<Individual*> &v);
    static Individual* minTFTSol(vector<Individual*> &v);


    static float meanIGDMetric(vector<vector<MinimalIndividual*>> &paretoArchive, vector<MinimalIndividual*> &PF);
    static float meanGDMetric(vector<vector<MinimalIndividual*>> &paretoArchive, vector<MinimalIndividual*> &PF);
    static float meanSMetric(vector<vector<MinimalIndividual*>> &paretoArchive, vector<MinimalIndividual*> &PF);
    static float meanDMetric(vector<vector<MinimalIndividual*>> &paretoArchive, vector<MinimalIndividual*> &PF);
    static vector<MinimalIndividual*> joinFronts(vector<vector<MinimalIndividual*>> fronts);
    static void outputToFile(string path, string text, bool append);
    static void checkDuplicateIndividualsAtFile(string path);

    static float hypervolumeMetric(vector<MinimalIndividual*> &PF, float maxTFT, float maxTEC);
    static float meanHypervolumeMetric(vector<vector<MinimalIndividual*>> &paretoArchive);
    };

#endif //UTIL_H
