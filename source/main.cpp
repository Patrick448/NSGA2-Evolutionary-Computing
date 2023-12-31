#include "defines.hpp"
#include "Individual.hpp"
#include "Util.hpp"
#include "Problem.hpp"
#include "read.hpp"
#include "NSGAII.hpp"

template <typename T>
vector<T*> joinFronts(vector<vector<T*>> fronts){
    vector<T*> result ;

    for(int i=0; i< fronts.size(); i++){
        for(int j=0; j< fronts[i].size(); j++){
            result.push_back(fronts[i][j]);
        }
    }

    return result;
}

vector<vector<MinimalIndividual*>> runAlgorithmGetAchives(string path, int iterations, int baseSeed, int &totalIterations, float crossoverProbability, float mutationProbability, int option){

    //string csv = "id,baseSeed,iterations,nsgaIterations,N,D(antiga), GD, IGD, S\n";
    string csv = "";
    vector<vector<MinimalIndividual*>> paretoArchive;

    int nsgaIterationsSum = 0;
    Problem *instance = readFile(path);


    for(int i=0; i<iterations; i++){
        NSGAII nsgaii = NSGAII(instance, crossoverProbability, mutationProbability);
        nsgaii.run(baseSeed+i, 1000, INFINITY, option);
        nsgaii.setOutputEnabled(false);
        nsgaIterationsSum += nsgaii.getNumIterations();
        paretoArchive.push_back(nsgaii.getMinimalParetoFront());
    }

    delete instance;
    totalIterations = nsgaIterationsSum;

    return paretoArchive;

}

string algorithmCSV(string path, string alg, int baseSeed, int nsgaIterationsSum, int iterations, int N, vector<MinimalIndividual*> trueParetoFront, vector<vector<MinimalIndividual*>> algParetoArchive){
    return path + ","+alg +"," + to_string(baseSeed)
           + "," + to_string(iterations)
           + "," + to_string((float)nsgaIterationsSum/(float)iterations)
           + "," + to_string(N)
           + "," + to_string(Util::meanDMetric(algParetoArchive, trueParetoFront))
           + "," + to_string(Util::meanGDMetric(algParetoArchive, trueParetoFront))
           + "," + to_string(Util::meanIGDMetric(algParetoArchive, trueParetoFront))
           + "," + to_string(Util::meanSMetric(algParetoArchive, trueParetoFront))
           + "," + to_string(Util::meanHypervolumeMetric(algParetoArchive))
           + "\n";
}

void deallocateMinimalIndivduals(vector<MinimalIndividual*> individuals){
    for(int i=0; i<individuals.size(); i++){
        delete individuals[i];
    }
}

string runCrossoverExperiment(string path, int iterations, int baseSeed){
    string csv = "";

    vector<vector<MinimalIndividual*>> alg1ParetoFronts;
    vector<vector<MinimalIndividual*>> alg2ParetoFronts;
    vector<vector<MinimalIndividual*>> alg3ParetoFronts;
    vector<vector<MinimalIndividual*>> alg4ParetoFronts;
    vector<vector<MinimalIndividual*>> alg5ParetoFronts;

    int alg1Its=0;
    alg1ParetoFronts = runAlgorithmGetAchives(path, iterations, baseSeed, alg1Its, 0.0, 1.0, 0);
    int alg2Its=0;
    alg2ParetoFronts = runAlgorithmGetAchives(path, iterations, baseSeed, alg2Its, 0.25, 1.0, 0);
    int alg3Its=0;
    alg3ParetoFronts = runAlgorithmGetAchives(path, iterations, baseSeed, alg3Its, 0.5, 1.0, 0);
    int alg4Its=0;
    alg4ParetoFronts = runAlgorithmGetAchives(path, iterations, baseSeed, alg4Its, 0.75, 1.0, 0);
    int alg5Its=0;
    alg5ParetoFronts = runAlgorithmGetAchives(path, iterations, baseSeed, alg5Its, 1.0, 1.0, 0);

    vector<MinimalIndividual*> alg1UnifiedParetoArchive = joinFronts(alg1ParetoFronts);
    vector<vector<MinimalIndividual*>> alg1FirstFront = Util::fastNonDominatedSort(alg1UnifiedParetoArchive);

    vector<MinimalIndividual*> alg2UnifiedParetoArchive = joinFronts(alg2ParetoFronts);
    vector<vector<MinimalIndividual*>> alg2FirstFront = Util::fastNonDominatedSort(alg2UnifiedParetoArchive);

    vector<MinimalIndividual*> alg3UnifiedParetoArchive = joinFronts(alg3ParetoFronts);
    vector<vector<MinimalIndividual*>> alg3FirstFront = Util::fastNonDominatedSort(alg3UnifiedParetoArchive);

    vector<MinimalIndividual*> alg4UnifiedParetoArchive = joinFronts(alg4ParetoFronts);
    vector<vector<MinimalIndividual*>> alg4FirstFront = Util::fastNonDominatedSort(alg4UnifiedParetoArchive);

    vector<MinimalIndividual*> alg5UnifiedParetoArchive = joinFronts(alg5ParetoFronts);
    vector<vector<MinimalIndividual*>> alg5FirstFront = Util::fastNonDominatedSort(alg5UnifiedParetoArchive);


    vector<vector<MinimalIndividual*>> allAlgsFronts{alg1UnifiedParetoArchive, alg2UnifiedParetoArchive, alg3UnifiedParetoArchive, alg4UnifiedParetoArchive, alg5UnifiedParetoArchive};
    vector<MinimalIndividual*> trueParetoFront = joinFronts(allAlgsFronts);
    trueParetoFront = Util::fastNonDominatedSort(trueParetoFront)[0];

    csv += algorithmCSV(path, "alg1", baseSeed, alg1Its, iterations, alg1FirstFront[0].size(), trueParetoFront,
                        alg1ParetoFronts);

    csv += algorithmCSV(path, "alg2", baseSeed, alg2Its, iterations, alg2FirstFront[0].size(), trueParetoFront,
                        alg2ParetoFronts);

    csv += algorithmCSV(path, "alg3", baseSeed, alg3Its, iterations, alg3FirstFront[0].size(), trueParetoFront,
                        alg3ParetoFronts);

    csv += algorithmCSV(path, "alg4", baseSeed, alg4Its, iterations, alg4FirstFront[0].size(), trueParetoFront,
                        alg4ParetoFronts);

    csv += algorithmCSV(path, "alg5", baseSeed, alg5Its, iterations, alg5FirstFront[0].size(), trueParetoFront,
                        alg5ParetoFronts);


    deallocateMinimalIndivduals(alg1UnifiedParetoArchive);
    deallocateMinimalIndivduals(alg2UnifiedParetoArchive);
    deallocateMinimalIndivduals(alg3UnifiedParetoArchive);
    deallocateMinimalIndivduals(alg4UnifiedParetoArchive);
    deallocateMinimalIndivduals(alg5UnifiedParetoArchive);
   // Util::deallocate();

    return csv;

}

string runAlgorithmComparisonExperiment(string path, int iterations, int baseSeed){
    string csv = "";

    vector<vector<MinimalIndividual*>> alg1ParetoFronts;
    vector<vector<MinimalIndividual*>> alg2ParetoFronts;
    vector<vector<MinimalIndividual*>> alg3ParetoFronts;
    vector<vector<MinimalIndividual*>> alg4ParetoFronts;

    int alg1Its=0;
    alg1ParetoFronts = runAlgorithmGetAchives(path, iterations, baseSeed, alg1Its, 0.6, 0.8, 0);
    int alg2Its=0;
    alg2ParetoFronts = runAlgorithmGetAchives(path, iterations, baseSeed, alg2Its, 0.0, 1.0, 1);
    int alg3Its=0;
    alg3ParetoFronts = runAlgorithmGetAchives(path, iterations, baseSeed, alg3Its, 0.0, 1.0, 2);
    int alg4Its=0;
    alg4ParetoFronts = runAlgorithmGetAchives(path, iterations, baseSeed, alg4Its, 0.0, 1.0, 0);

    vector<MinimalIndividual*> alg1UnifiedParetoArchive = joinFronts(alg1ParetoFronts);
    vector<vector<MinimalIndividual*>> alg1FirstFront = Util::fastNonDominatedSort(alg1UnifiedParetoArchive);

    vector<MinimalIndividual*> alg2UnifiedParetoArchive = joinFronts(alg2ParetoFronts);
    vector<vector<MinimalIndividual*>> alg2FirstFront = Util::fastNonDominatedSort(alg2UnifiedParetoArchive);

    vector<MinimalIndividual*> alg3UnifiedParetoArchive = joinFronts(alg3ParetoFronts);
    vector<vector<MinimalIndividual*>> alg3FirstFront = Util::fastNonDominatedSort(alg3UnifiedParetoArchive);

    vector<MinimalIndividual*> alg4UnifiedParetoArchive = joinFronts(alg4ParetoFronts);
    vector<vector<MinimalIndividual*>> alg4FirstFront = Util::fastNonDominatedSort(alg4UnifiedParetoArchive);


    vector<vector<MinimalIndividual*>> allAlgsFronts{alg1UnifiedParetoArchive, alg2UnifiedParetoArchive, alg3UnifiedParetoArchive, alg4UnifiedParetoArchive};
    vector<MinimalIndividual*> trueParetoFront = joinFronts(allAlgsFronts);
    trueParetoFront = Util::fastNonDominatedSort(trueParetoFront)[0];

    csv += algorithmCSV(path, "alg1", baseSeed, alg1Its, iterations, alg1FirstFront[0].size(), trueParetoFront,
                        alg1ParetoFronts);

    csv += algorithmCSV(path, "alg2", baseSeed, alg2Its, iterations, alg2FirstFront[0].size(), trueParetoFront,
                        alg2ParetoFronts);

    csv += algorithmCSV(path, "alg3", baseSeed, alg3Its, iterations, alg3FirstFront[0].size(), trueParetoFront,
                        alg3ParetoFronts);

    csv += algorithmCSV(path, "alg4", baseSeed, alg4Its, iterations, alg4FirstFront[0].size(), trueParetoFront,
                        alg4ParetoFronts);


    deallocateMinimalIndivduals(alg1UnifiedParetoArchive);
    deallocateMinimalIndivduals(alg2UnifiedParetoArchive);
    deallocateMinimalIndivduals(alg3UnifiedParetoArchive);
    deallocateMinimalIndivduals(alg4UnifiedParetoArchive);
    // Util::deallocate();

    return csv;

}

int main()
{
    string expResults = "";
    Util::outputToFile("results.csv", "id, alg, baseSeed, iterations, nsgaIterations, N, D(antiga), GD, IGD, S, HV\n", true);

    expResults = runCrossoverExperiment("../instances/566/2-4-60__0.txt", 5, 0);
    Util::outputToFile("results.csv", expResults , true);
    cout << "2-4-60__0.txt done!\n" << endl;

    expResults = runCrossoverExperiment("../instances/566/2-16-60__0.txt", 5, 0);
    Util::outputToFile("results.csv", expResults , true);
    cout << "2-16-60__0.txt done!\n" << endl;

    expResults = runCrossoverExperiment("../instances/566/3-4-60__0.txt", 5, 0);
    Util::outputToFile("results.csv", expResults , true);
    cout << "3-4-60__0.txt done!\n" << endl;

    expResults = runCrossoverExperiment("../instances/566/3-16-60__0.txt", 5, 0);
    Util::outputToFile("results.csv", expResults , true);   
    cout << "3-16-60__0.txt done!\n" << endl;

    expResults = runCrossoverExperiment("../instances/566/4-4-60__0.txt", 5, 0);
    Util::outputToFile("results.csv", expResults , true);
    cout << "4-4-60__0.txt done!\n" << endl;

    expResults = runCrossoverExperiment("../instances/566/4-16-60__0.txt", 5, 0);
    Util::outputToFile("results.csv", expResults , true);
    cout << "4-16-60__0.txt done!\n" << endl;

    expResults = runCrossoverExperiment("../instances/566/5-4-60__0.txt", 5, 0);
    Util::outputToFile("results.csv", expResults , true);
    cout << "5-4-60__0.txt done!\n" << endl;

    expResults = runCrossoverExperiment("../instances/566/5-16-60__0.txt", 5, 0);
    Util::outputToFile("results.csv", expResults , true);
    cout << "5-16-60__0.txt done!\n" << endl;

    return 0;
    cout << "Hello" << endl;

    cout << "Iniciando leitura da instância...\n" << endl;
    Problem *instance = readFile("../instances/294/3-4-20__75.txt");
    cout << "Instância lida com sucesso!\n" << endl;

    cout << "F = " << instance->getF() << endl;
    cout << "n = " << instance->getN() << endl;
    cout << "m = " << instance->getM() << endl;

    // Build NSGA-II
    float crossoverProbability = 0.8;
    float mutationProbability = 0.8;
    NSGAII nsgaii = NSGAII(instance, crossoverProbability, mutationProbability);
    nsgaii.setOutputEnabled(true);

    // Run NSGA-II
    int seed = 111;
    nsgaii.run(seed, 1000, instance->getN()/2, 0);

    cout << "\nFIM\n";
    //Util::deallocate();
    delete instance;
    return 0;
}
