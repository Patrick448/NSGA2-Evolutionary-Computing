#include "defines.hpp"
#include "Individual.hpp"
#include "Util.hpp"
#include "Problem.hpp"
#include "read.hpp"
#include "NSGAII.hpp"

int main()
{
    cout << "Hello" << endl;

    cout << "Iniciando leitura da instância...\n" << endl;
    Problem *instance = readFile("../instances/294/3-4-20__75.txt");
    cout << "Instância lida com sucesso!\n" << endl;

    cout << "F = " << instance->getF() << endl;
    cout << "n = " << instance->getN() << endl;
    cout << "m = " << instance->getM() << endl;

    // Build NSGA-II
    float crossoverProbability = 0.8;
    float mutationProbability = 0.2;
    NSGAII nsgaii = NSGAII(instance, crossoverProbability, mutationProbability);
    nsgaii.setOutputEnabled(true);
    
    // Run NSGA-II
    int seed = 111;
    nsgaii.run(seed);

    cout << "\nFIM\n";

    return 0;
}
