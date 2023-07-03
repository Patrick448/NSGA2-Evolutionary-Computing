#include "defines.hpp"
#include "Individual.hpp"
#include "Util.hpp"
#include "Problem.hpp"


void outputToFile(string path, string text, bool append){
    ofstream outputf;

    if(append){
        outputf.open(path, std::ios_base::app);
    }else{
        outputf.open(path);
    }

    outputf << text;
    outputf.close();
}

vector<Individual*> joinFronts(vector<vector<Individual*>> fronts){
    vector<Individual*> result ;

    for(int i=0; i< fronts.size(); i++){
        for(int j=0; j< fronts[i].size(); j++){
            result.push_back(fronts[i][j]);
        }
    }

    return result;
}

float meanDMetric(vector<vector<Individual*>> &paretoArchive, vector<Individual*> &PF){

    float sum = 0;
    for(int i=0; i< paretoArchive.size(); i++){
        sum+= Util::DMetric(PF, paretoArchive[i]);
    }

    return sum/paretoArchive.size();
}

float meanSMetric(vector<vector<Individual*>> &paretoArchive, vector<Individual*> &PF){

    float sum = 0;
    for(int i=0; i< paretoArchive.size(); i++){
        sum+= Util::SMetric(PF, paretoArchive[i]);
    }

    return sum/paretoArchive.size();
}

float meanGDMetric(vector<vector<Individual*>> &paretoArchive, vector<Individual*> &PF){

    float sum = 0;
    for(int i=0; i< paretoArchive.size(); i++){
        sum+= Util::GDMetric(PF, paretoArchive[i]);
    }

    return sum/paretoArchive.size();
}

float meanIGDMetric(vector<vector<Individual*>> &paretoArchive, vector<Individual*> &PF){

    float sum = 0;
    for(int i=0; i< paretoArchive.size(); i++){
        sum+= Util::IGDMetric(PF, paretoArchive[i]);
    }

    return sum/paretoArchive.size();
}


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

    cout << "\nFIM\n";

    return 0;
}
