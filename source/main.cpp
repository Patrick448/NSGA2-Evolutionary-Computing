#include "defines.hpp"
#include "Individual.hpp"
#include "Util.hpp"
#include "Problem.hpp"
#include "read.hpp"
#include "NSGAII.hpp"

int main()
{
    cout << "Hello" << endl;

    // inicializa o construtivo

    // Leitura retorna uma instancia

    // cout << "Iniciando leitura da instância...\n" << endl;
    // Problem *inst = readFile("./instances/294/3-4-20__15.txt");
    // cout << "Instância lida com sucesso!\n" << endl;

    // cout << "F = " << inst->getF() << endl;
    // cout << "n = " << inst->getN() << endl;
    // cout << "m = " << inst->getM() << endl;

    // inst->construtivo();
    // inst->printPopulation();

    // Read instance
    cout << "Iniciando leitura da instância...\n" << endl;
    Problem *instance = readFile("../instances/294/3-4-20__15.txt");
    cout << "Instância lida com sucesso!\n" << endl;

    cout << "F = " << instance->getF() << endl;
    cout << "n = " << instance->getN() << endl;
    cout << "m = " << instance->getM() << endl;

    // Build NSGA-II
    float crossoverProbability = 0.9;
    float mutationProbability = 0.8;
    NSGAII nsgaii = NSGAII(instance, crossoverProbability, mutationProbability);
    nsgaii.setOutputEnabled(true);
    // Run NSGA-II
    int seed = 0;
    nsgaii.run(seed);

    cout << "\nFIM\n";

    return 0;
}
