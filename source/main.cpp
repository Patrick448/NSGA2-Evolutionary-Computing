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
    Problem *instance = readFile("./instances/294/3-4-20__15.txt");
    cout << "Instância lida com sucesso!\n" << endl;

    cout << "F = " << instance->getF() << endl;
    cout << "n = " << instance->getN() << endl;
    cout << "m = " << instance->getM() << endl;

    // Build NSGA-II
    NSGAII nsgaii = NSGAII(instance);

    // Run NSGA-II
    nsgaii.run();

    cout << "\nFIM\n";

    return 0;
}
