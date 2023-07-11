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

vector<vector<MinimalIndividual*>> runAlgorithmGetAchives(string path, int iterations, int baseSeed, int &totalIterations, float crossoverProbability, float mutationProbability){

    //string csv = "id,baseSeed,iterations,nsgaIterations,N,D(antiga), GD, IGD, S\n";
    string csv = "";
    vector<vector<MinimalIndividual*>> paretoArchive;

    int nsgaIterationsSum = 0;
    Problem *instance = readFile(path);


    for(int i=0; i<iterations; i++){
        NSGAII nsgaii = NSGAII(instance, crossoverProbability, mutationProbability);
        nsgaii.run(baseSeed+i);
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
           + "\n";
}

void deallocateMinimalIndivduals(vector<MinimalIndividual*> individuals){
    for(int i=0; i<individuals.size(); i++){
        delete individuals[i];
    }
}

string runExperiment(string path, int iterations,int baseSeed){
    string csv = "";

    vector<vector<MinimalIndividual*>> alg1ParetoFronts;
    vector<vector<MinimalIndividual*>> alg2ParetoFronts;
    vector<vector<MinimalIndividual*>> alg3ParetoFronts;
    vector<vector<MinimalIndividual*>> alg4ParetoFronts;

    int alg1Its=0;
    alg1ParetoFronts = runAlgorithmGetAchives(path, iterations, baseSeed, alg1Its, 0.0, 0.8);
    int alg2Its=0;
    alg2ParetoFronts = runAlgorithmGetAchives(path, iterations, baseSeed, alg2Its, 0.0, 0.8);
    int alg3Its=0;
    alg3ParetoFronts = runAlgorithmGetAchives(path, iterations, baseSeed, alg3Its, 0.0, 0.8);
    int alg4Its=0;
    alg4ParetoFronts = runAlgorithmGetAchives(path, iterations, baseSeed, alg4Its, 0.0, 0.8);

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

    csv += algorithmCSV(path, "alg1", baseSeed, alg1Its, iterations, alg1FirstFront.size(), trueParetoFront,
                        alg1ParetoFronts);

    csv += algorithmCSV(path, "alg2", baseSeed, alg2Its, iterations, alg2FirstFront.size(), trueParetoFront,
                        alg2ParetoFronts);

    csv += algorithmCSV(path, "alg3", baseSeed, alg3Its, iterations, alg3FirstFront.size(), trueParetoFront,
                        alg3ParetoFronts);

    csv += algorithmCSV(path, "alg4", baseSeed, alg4Its, iterations, alg4FirstFront.size(), trueParetoFront,
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
    //string expResults = runExperiment("../instances/294/3-4-20__75.txt", 1, 0);
    //Util::outputToFile("results.csv", "id,alg,baseSeed,iterations,nsgaIterations,N,D(antiga), GD, IGD, S\n", true);
    //Util::outputToFile("results.csv", expResults , true);

   // return 0;
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
    nsgaii.run(seed);

    cout << "\nFIM\n";
    //Util::deallocate();
    delete instance;
    return 0;
}
