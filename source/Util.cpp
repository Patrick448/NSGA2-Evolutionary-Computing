//
// Created by patri on 12/01/2023.
//

#include "Util.hpp"
#include "MinimalIndividual.h"

vector<Individual*> Util::allocatedIndividuals;

vector<vector<MinimalIndividual*>> Util::fastNonDominatedSort(vector<MinimalIndividual*> population) {

    vector<vector<int>> fronts(1);
    vector<vector<int>> dominatedBy(population.size());


    for(int i=0; i< population.size(); i++){
        population[i]->setDominationCounter(0);
        //population[i]->setDominationRank(-1);

        for(int j=0; j< population.size(); j++){
            if(population[i]->dominates(population[j])){
                dominatedBy[i].push_back(j);
            }else if(population[j]->dominates(population[i])){
                population[i]->incrementDominationCounter(1);
            }
        }

        if(population[i]->getDominationCounter()==0){
            population[i]->setDominationRank(1);
            fronts[0].push_back(i);
        }

    }

    int i =0;
    while(!fronts[i].empty()){
        vector<int> nextFront;
        for(int j=0; j<fronts[i].size(); j++){
            int frontSolId =  fronts[i][j]; //id (indices) de cada solução na fronteira atual

            for(int k=0; k<dominatedBy[frontSolId].size(); k++){ //itera por cada solução dominada pela de indice frontSolId
                int dominatedSolIndex = dominatedBy[frontSolId][k]; // id de cada solução dominada por frontSolId

                MinimalIndividual *s = population[dominatedSolIndex]; // cada solução dominada por frontSolId

                s->incrementDominationCounter(-1);

                if(s->getDominationCounter()==0){
                    s->setDominationRank(i+2);
                    nextFront.push_back(dominatedSolIndex);
                }
            }
        }
        i++;
        fronts.push_back(nextFront);
    }

    vector<vector<MinimalIndividual*>> IndividualFronts(fronts.size());
    for(int i=0; i< fronts.size(); i++){
        vector<MinimalIndividual*> front(fronts[i].size());
        for(int j=0; j < fronts[i].size(); j++){
            front[j] = population[fronts[i][j]];
        }
        IndividualFronts[i] = front;
    }

    IndividualFronts.pop_back();
    return IndividualFronts;
}
MinimalIndividual* extremeTEC(vector<MinimalIndividual*> &v, bool maxExtreme)
{
    float extreme;
    MinimalIndividual* extremeIndividual;

    if(maxExtreme){
        extreme=0;
    }else{
        extreme=INFINITY;
    }

    for(int i=0; i<v.size(); i++){
        if(maxExtreme){
            if(v[i]->getTEC() > extreme){
                extreme = v[i]->getTEC();
                extremeIndividual = v[i];
            }
        }else{
            if(v[i]->getTEC() < extreme){
                extreme = v[i]->getTEC();
                extremeIndividual = v[i];
            }
        }
    }

    return extremeIndividual;
}


MinimalIndividual* Util::maxTECSol(vector<MinimalIndividual*> &v){
    float max=0;
    MinimalIndividual* sol;

    for(int i=0; i<v.size(); i++){
        if(v[i]->getTEC() > max){
            max = v[i]->getTEC();
            sol = v[i];
        }
    }
    return sol;
}

MinimalIndividual* Util::maxTFTSol(vector<MinimalIndividual*> &v){
    float max=0;
    MinimalIndividual* sol;

    for(int i=0; i<v.size(); i++){
        if(v[i]->getTFT() > max){
            max = v[i]->getTFT();
            sol = v[i];
        }
    }
    return sol;
}


MinimalIndividual* Util::minTECSol(vector<MinimalIndividual*> &v){
    float min=INFINITY;
    MinimalIndividual* sol;

    for(int i=0; i<v.size(); i++){
        if(v[i]->getTEC() < min){
            min = v[i]->getTEC();
            sol = v[i];
        }
    }
    return sol;
}

MinimalIndividual* Util::minTFTSol(vector<MinimalIndividual*> &v){
    float min=INFINITY;
    MinimalIndividual* sol;

    for(int i=0; i<v.size(); i++){
        if(v[i]->getTFT() < min){
            min = v[i]->getTFT();
            sol = v[i];
        }
    }
    return sol;
}


Individual* Util::maxTECSol(vector<Individual*> &v){
    float max=0;
    Individual* sol;

    for(int i=0; i<v.size(); i++){
        if(v[i]->getTEC() > max){
            max = v[i]->getTEC();
            sol = v[i];
        }
    }
    return sol;
}

Individual* Util::maxTFTSol(vector<Individual*> &v){
    float max=0;
    Individual* sol;

    for(int i=0; i<v.size(); i++){
        if(v[i]->getTFT() > max){
            max = v[i]->getTFT();
            sol = v[i];
        }
    }
    return sol;
}


Individual* Util::minTECSol(vector<Individual*> &v){
    float min=INFINITY;
    Individual* sol;

    for(int i=0; i<v.size(); i++){
        if(v[i]->getTEC() < min){
            min = v[i]->getTEC();
            sol = v[i];
        }
    }
    return sol;
}

Individual* Util::minTFTSol(vector<Individual*> &v){
    float min=INFINITY;
    Individual* sol;

    for(int i=0; i<v.size(); i++){
        if(v[i]->getTFT() < min){
            min = v[i]->getTFT();
            sol = v[i];
        }
    }
    return sol;
}


// Functions that came from main

float euclideanDistance(MinimalIndividual* sol1, MinimalIndividual* sol2){
    float sol1Y = sol1->getTEC();
    float sol1X = sol1->getTFT();
    float sol2Y = sol2->getTEC();
    float sol2X = sol2->getTFT();

    float distance = sqrtf(pow(sol1Y-sol2Y, 2) + pow(sol1X - sol2X, 2));
    return distance;
}

float normalizedEuclideanDistance(MinimalIndividual* sol1, MinimalIndividual* sol2, float maxTFT, float minTFT, float maxTEC, float minTEC){
    float sol1TEC = sol1->getTEC();
    float sol1TFT = sol1->getTFT();
    float sol2TEC = sol2->getTEC();
    float sol2TFT = sol2->getTFT();

    float distance =
            sqrtf(
                pow((sol1TEC - sol2TEC) / (maxTEC - minTEC), 2) +
                pow((sol1TFT - sol2TFT) / (maxTFT - minTFT), 2)
            );
    return distance;
}

float Util::DMetric(vector<MinimalIndividual*> &PF, vector<MinimalIndividual*> &A){

    float sum = 0;
    for(int i=0; i < PF.size(); i++){
        float min =INFINITY;
        for(int j=0; j < A.size(); j++){
            float distance = euclideanDistance(PF[i], A[j]);
            if(distance<min){
                min = distance;
            }
        }
        sum+=min;
    }

    return sum/PF.size();
}

float Util::GDMetric(vector<MinimalIndividual*> &PF, vector<MinimalIndividual*> &A){
    float minTEC = minTECSol(PF)->getTEC();
    float minTFT = minTFTSol(PF)->getTFT();
    float maxTEC = maxTECSol(PF)->getTEC();
    float maxTFT = maxTFTSol(PF)->getTFT();

    float sum = 0;
    for(int i=0; i < A.size(); i++){
        float min =INFINITY;
        for(int j=0; j < PF.size(); j++){
            float distance = normalizedEuclideanDistance(PF[j], A[i], maxTFT, minTFT, maxTEC, minTEC);
            if(distance<min){
                min = distance;
            }
        }
        sum+=pow(min, 2);
    }

    return sqrtf(sum)/A.size();
}

float Util::IGDMetric(vector<MinimalIndividual*> &PF, vector<MinimalIndividual*> &A){
    float minTEC = minTECSol(PF)->getTEC();
    float minTFT = minTFTSol(PF)->getTFT();
    float maxTEC = maxTECSol(PF)->getTEC();
    float maxTFT = maxTFTSol(PF)->getTFT();


    float sum = 0;
    for(int i=0; i < PF.size(); i++){
        float min =INFINITY;
        for(int j=0; j < A.size(); j++){
            float distance = normalizedEuclideanDistance(PF[i], A[j], maxTFT, minTFT, maxTEC, minTEC);
            if(distance<min){
                min = distance;
            }
        }
        sum+=pow(min, 2);
    }

    return sqrtf(sum)/PF.size();
}



float Util::SMetric(vector<MinimalIndividual*> &PF, vector<MinimalIndividual*> &A) {

    float diSum, dMean = 0;
    vector<float> d;
    d.reserve(A.size());

    for(int i=0; i < A.size(); i++){
        float min =INFINITY;
        for(int j=0; j < A.size(); j++){
            float distance = euclideanDistance(A[i], A[j]);
            if(j!=i && distance<min){
                min = distance;
            }
        }
        diSum+=min;
        d.push_back(min);
    }

    dMean = diSum / A.size();
    float term2Sum = 0;
    for(int i=0; i< d.size(); i++){
        term2Sum+= fabsf(d[i] - dMean);
    }


    float term1Sum = euclideanDistance(minTECSol(PF), minTECSol(A))
            + euclideanDistance(minTFTSol(PF), minTFTSol(A));

    float result = (term1Sum + term2Sum)/(term1Sum + A.size()*dMean);

    return result;
}

string Util::generateCSV(Factory* factory)
{
    factory->initializeJobsStartTimes();
    string csvString = "";

    for (int i = 0; i < factory->getNumJobs(); i++)
    {
        csvString += "tj" + to_string(i) + ",td" + to_string(i) + ",";
    }
    csvString.pop_back();
    csvString += "\n";

    for (int i = 0; i < factory->getNumMachines(); i++)
    {
        vector<Job*> jobs = factory->getJobs();
        for (int j = 0; j < factory->getNumJobs(); j++)
        {
            csvString += to_string(jobs[j]->getStartTime(i)) + "," + to_string(jobs[j]->getP(i)) + ",";
        }
        csvString.pop_back();
        csvString += "\n";
    }

    return csvString;
}

void Util::allocate(Individual* sol){
    Util::allocatedIndividuals.push_back(sol);
}

void Util::deallocate(){
    for(Individual* s: Util::allocatedIndividuals){
        delete s;
    }

    Util::allocatedIndividuals.clear();
}

void Util::outputToFile(string path, string text, bool append){
    ofstream outputf;

    if(append){
        outputf.open(path, std::ios_base::app);
    }else{
        outputf.open(path);
    }

    outputf << text;
    outputf.close();
}

vector<MinimalIndividual*> Util::joinFronts(vector<vector<MinimalIndividual*>> fronts){
    vector<MinimalIndividual*> result ;

    for(int i=0; i< fronts.size(); i++){
        for(int j=0; j< fronts[i].size(); j++){
            result.push_back(fronts[i][j]);
        }
    }

    return result;
}

float Util::meanDMetric(vector<vector<MinimalIndividual*>> &paretoArchive, vector<MinimalIndividual*> &PF){

    float sum = 0;
    for(int i=0; i< paretoArchive.size(); i++){
        sum+= Util::DMetric(PF, paretoArchive[i]);
    }

    return sum/paretoArchive.size();
}

float Util::meanSMetric(vector<vector<MinimalIndividual*>> &paretoArchive, vector<MinimalIndividual*> &PF){

    float sum = 0;
    for(int i=0; i< paretoArchive.size(); i++){
        sum+= Util::SMetric(PF, paretoArchive[i]);
    }

    return sum/paretoArchive.size();
}

float Util::meanGDMetric(vector<vector<MinimalIndividual*>> &paretoArchive, vector<MinimalIndividual*> &PF){

    float sum = 0;
    for(int i=0; i< paretoArchive.size(); i++){
        sum+= Util::GDMetric(PF, paretoArchive[i]);
    }

    return sum/paretoArchive.size();
}

float Util::meanIGDMetric(vector<vector<MinimalIndividual*>> &paretoArchive, vector<MinimalIndividual*> &PF){

    float sum = 0;
    for(int i=0; i< paretoArchive.size(); i++){
        sum+= Util::IGDMetric(PF, paretoArchive[i]);
    }

    return sum/paretoArchive.size();
}

void Util::checkDuplicateIndividualsAtFile(string path)
{
    // Read file
    ifstream file(path);
    string str;
    vector<string> lines;
    while (getline(file, str))
    {
        lines.push_back(str);
    }
    file.close();

    // Check duplicates ignoring the first column
    vector<string> uniqueLines;
    bool duplicateFound = false;
    cout << "\nAnalyzing " << path << "... " << endl;
    for (int i = 0; i < lines.size(); i++)
    {
        bool isUnique = true;
        for (int j = 0; j < uniqueLines.size(); j++)
        {
            if (lines[i].substr(lines[i].find(",") + 1) == uniqueLines[j].substr(uniqueLines[j].find(",") + 1))
            {
                cout << "Duplicate found: " << uniqueLines[j] << " == " << lines[i] << endl;
                isUnique = false;
                duplicateFound = true;
                break;
            }
        }
        if (isUnique)
        {
            uniqueLines.push_back(lines[i]);
        }
    }
    if (!duplicateFound)
    {
        cout << "No duplicates found at " << path << endl;
    }
}

// Hypervolume metric
float Util::hypervolumeMetric(vector<MinimalIndividual*> &PF)
{
    // Get the reference point (Nadir point)
    // The approximate Nadir point is the worst value of each objective
    // The approx Nadir point will have the TFT of the best solution in TEC and the TEC of the best solution in TFT
    float tftAux = PF[0]->getTFT();
    float tecAux = PF[0]->getTEC();
    int indexMinTFT = 0;
    int indexMinTEC = 0;
    for(int i = 0; i < PF.size(); i++)
    {
        if(PF[i]->getTFT() < tftAux)
        {
            tftAux = PF[i]->getTFT();
            indexMinTFT = i;
        }
        if(PF[i]->getTEC() < tecAux)
        {
            tecAux = PF[i]->getTEC();
            indexMinTEC = i;
        }
    }
    float referencePointTFT = PF[indexMinTEC]->getTFT();
    float referencePointTEC = PF[indexMinTFT]->getTEC();

    // Create a new PF with the normalized values
    vector<MinimalIndividual> normalizedPF;
    for(int i = 0; i < PF.size(); i++)
    {
        float normalizedTFT = (PF[i]->getTFT() - PF[indexMinTFT]->getTFT()) / (referencePointTFT - PF[indexMinTFT]->getTFT());
        float normalizedTEC = (PF[i]->getTEC() - PF[indexMinTEC]->getTEC()) / (referencePointTEC - PF[indexMinTEC]->getTEC());
        
        // Clone the individual
        MinimalIndividual normalizedIndividual = *PF[i];
        normalizedIndividual.setTFT(normalizedTFT);
        normalizedIndividual.setTEC(normalizedTEC);
        normalizedPF.push_back(normalizedIndividual);
    }

    // Sort the normalized PF by TEC
    sort(normalizedPF.begin(), normalizedPF.end(), [](MinimalIndividual &a, MinimalIndividual &b) {
        return a.getTEC() < b.getTEC();
    });

    // Calculate the hypervolume
    float hypervolume = 0;
    float width = 0;
    float height = 0;
    for(int i = 0; i < normalizedPF.size()-1; i++)
    {
        width = normalizedPF[i+1].getTEC() - normalizedPF[i].getTEC();
        height = referencePointTFT - normalizedPF[i].getTFT();
        hypervolume += width * height;
    }
    // Add the last rectangle
    width = referencePointTEC - normalizedPF[normalizedPF.size()-1].getTEC();
    height = referencePointTFT - normalizedPF[normalizedPF.size()-1].getTFT();
    hypervolume += width * height;

    // Delete normalized individuals
    // for(int i = 0; i < normalizedPF.size(); i++)
    // {
    //     delete normalizedPF[i];
    // }

    return hypervolume;
}

float Util::meanHypervolumeMetric(vector<vector<MinimalIndividual*>> &paretoArchive)
{
    float sum = 0;
    for(int i=0; i< paretoArchive.size(); i++){
        sum+= Util::hypervolumeMetric(paretoArchive[i]);
    }

    return sum/paretoArchive.size();
}