#ifndef NSGAII_HPP
#define NSGAII_HPP

#include "defines.hpp"
#include "Individual.hpp"
#include "Problem.hpp"

class NSGAII
{
private:
    Problem *problem;

    vector<Individual *> population; // Population of Individuals
    vector<Individual *> new_individuals;
    vector<Individual *> paretoArchive;
    vector<vector<Individual *>> dominationFronts;

    struct timeval begin, end;

public:
    NSGAII(Problem *problem);
    ~NSGAII();

    vector<Individual *> getPopulation();
    Individual *getIndividual(int i);

    // Utilities
    void assignCrowdingDistance();
    vector<vector<Individual *>> fastNonDominatedSort();

    // Operators
    void construtivo();

    Individual *maxSMinTFT();
    Individual *randSMinTFT(int seed);
    Individual *minSMinTEC();
    Individual *randSMinTEC(int seed);

    void balancedRandomIndividualGenerator(int s);
    void totalRandomIndividualGenerator(int s);

    void printPopulation();
    string generatePopulationCSVString();
    void NSGA2NextGen(int seed);
    void NSGA2NextGen_operators(int seed);
    void NSGA2NextGen_operators_ND(int seed);
    // vector<Individual*> makeNewPop(vector<Individual*> parents, int seed);
    int nMetric();

    vector<Individual *> getParetoFront();
    vector<Individual *> getParetoArchive();
    void updateArchive(Individual *sol);
    static vector<vector<Individual *>> fastNonDominatedSort(vector<Individual *> population);

    Individual *INGM(Individual *sol, int seed); // Insertion-based new-individual generation method
    Individual *SNGM(Individual *sol, int seed); // Swap-based new-individual generation method
    Individual *HNGM(Individual *sol, int seed); // Hybrid-based new-individual generation method
    vector<Individual *> makenewpop_operators(vector<Individual *> parents, int seed);

    Individual *INGM_ND(Individual *sol, int seed); // Insertion-based new-individual generation method
    Individual *SNGM_ND(Individual *sol, int seed); // Swap-based new-individual generation method
    Individual *HNGM_ND(Individual *sol, int seed); // Hybrid-based new-individual generation method
    vector<Individual *> makenewpop_operators_ND(vector<Individual *> parents, int seed);
};

#endif // NSGAII_HPP