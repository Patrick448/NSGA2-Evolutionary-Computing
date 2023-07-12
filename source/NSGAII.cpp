#include "NSGAII.hpp"
#include "Util.hpp"
#include <bits/stdc++.h>

NSGAII::NSGAII(Problem *problem, float crossoverRate, float mutationRate) {
    this->problem = problem;
    this->crossoverRate = crossoverRate;
    this->mutationRate = mutationRate;
}

NSGAII::~NSGAII() {

    if (this->population.size() > 0) {
        for (int i = 0; i < this->population.size(); i++) {
            delete this->population[i];
        }
    }
}

vector<Individual *> NSGAII::getPopulation() {
    return this->population;
}

Individual *NSGAII::getIndividual(int i) {
    return this->population[i];
}

vector<Individual *> NSGAII::getParetoArchive() {
    return this->paretoArchive;
}

void NSGAII::updateArchive(Individual *individual) {
    for (int i = 0; i < this->paretoArchive.size(); i++) {
        if (paretoArchive[i]->dominates(individual)) {
            return;
        }
    }

    for (int i = 0; i < this->paretoArchive.size(); i++) {
        if (individual->dominates(paretoArchive[i])) {
            paretoArchive.erase(paretoArchive.begin() + i);
        }
    }

    paretoArchive.push_back(individual);
}

void NSGAII::construtivo() {

    for (int i = 0; i < 4; i++) {
        this->population.push_back(this->balancedRandomIndividualGenerator(i));
    }
}

bool compareJobsByTotalProcessingTime(Job *a, Job *b) {
    return a->getTotalP() > b->getTotalP();
}

bool compareIndividualsByTFT(Individual *a, Individual *b) {
    return a->getTFT() < b->getTFT();
}

bool compareIndividualsByTEC(Individual *a, Individual *b) {
    return a->getTEC() < b->getTEC();
}

Individual *NSGAII::maxSMinTFT() {
    // Individual *individual = new Individual(this->problem->getN(), this->problem->getM(), this->problem->getF());
    // vector<Job *> jobs(this->problem->getN());
    // int highestSpeed = this->problem->getAllSpeeds().size() - 1;

    // for (int i = 0; i < this->problem->getN(); i++) {
    //     jobs[i] = new Job(i, this->problem->getM());
    //     vector<int> jobTimeByMachine(this->problem->getM());
    //     vector<float> jobSpeedByMachine(this->problem->getM());

    //     for (int j = 0; j < this->problem->getM(); j++) {
    //         jobTimeByMachine[j] = this->problem->getAllT()[i][j];
    //         jobSpeedByMachine[j] = this->problem->getAllSpeeds()[highestSpeed];
    //     }
    //     jobs[i]->setT(jobTimeByMachine);
    //     jobs[i]->setV(jobSpeedByMachine);
    // }

    // // NEH procedure
    // // generate job permutation lambda according to DESCENDING order of Ti (total job i processing time)
    // sort(jobs.begin(), jobs.end(), compareJobsByTotalProcessingTime); 

    // // assign the first f jobs to each one of the factories
    // for (int i = 0; i < this->problem->getF(); i++) { 
    //     // factories[i] = new Factory(i, this->problem->get_m());
    //     individual->getFactory(i)->addJobAtLastPosition(jobs[i]);
    // }

    // // remove the assigned jobs from lambda
    // Factory *testFactory;
    // float testFactoryTFT;
    // float tftVariation;
    // float previousTFT;
    // Factory *minTFTFactory;

    // // for each factory f
    // // test job j at all the possible positions of PI_k (the factory)
    // for (int j = this->problem->getF(); j < jobs.size(); j++) { 
    //     float minIncreaseTFT = INFINITY;
    //     float minTFTPos = 0;

    //     for (int f = 0; f < this->problem->getF(); f++) {
    //         testFactory = individual->getFactory(f)->minTFTAfterInsertion(jobs[j]);
    //         testFactoryTFT = testFactory->getTFT();
    //         previousTFT = individual->getFactory(f)->getTFT();
    //         tftVariation = testFactoryTFT - previousTFT;

    //         if (tftVariation < minIncreaseTFT) {
    //             minIncreaseTFT = tftVariation;
    //             minTFTFactory = testFactory;
    //             minTFTPos = f;
    //         } else {
    //             // testFactory->clearJobs();
    //             delete testFactory;
    //         }
    //     }

    //     // factories[minTFTPos] = minTFTFactory;
    //     individual->replaceFactory(minTFTPos, minTFTFactory); // replaces old factory and deletes it
    // }

    // for (int i = 0; i < this->problem->getF(); i++) {
    //     individual->getFactory(i)->speedDown();
    // }

    // // Initialize the jobs start times of each factory
    // for (int f = 0; f < this->problem->getF(); f++) {
    //     individual->getFactory(f)->initializeJobsStartTimes();
    // }

    // return individual;

    // New version
    // 1st: all the jobs are processed at the maximum speed

    // Empty jobs vector
    vector<Job *> jobs(this->problem->getN());

    // Set all the speeds to the maximum value
    int highestSpeed = this->problem->getAllSpeeds().size() - 1;
    for (int i = 0; i < this->problem->getN(); i++) {
        jobs[i] = new Job(i, this->problem->getM());
        vector<int> jobTimeByMachine(this->problem->getM());
        vector<float> jobSpeedByMachine(this->problem->getM());

        // Iterate over all the machines
        for (int j = 0; j < this->problem->getM(); j++) {
            jobTimeByMachine[j] = this->problem->getAllT()[i][j];
            jobSpeedByMachine[j] = this->problem->getAllSpeeds()[highestSpeed];
        }

        // Set the standard time and speed of the job
        jobs[i]->setT(jobTimeByMachine);
        jobs[i]->setV(jobSpeedByMachine);
    }

    // 2nd: sort the jobs according to DESCENDING order of total processing time (NEH procedure)
    sort(jobs.begin(), jobs.end(), compareJobsByTotalProcessingTime);

    // 3rd: take the jobs out one by one and insert them in the position of the factory that leads to the minimum increase of total flow time
    Individual *individual = new Individual(this->problem->getN(), this->problem->getM(), this->problem->getF());
    for(int i = 0; i < jobs.size(); i++)
    {
        // Get the job
        Job *job = jobs[i];

        // Auxiliary variables
        float minTFTIncrease = INFINITY;
        int minTFTFactoryId = -1;
        int minTFTFactoryPos = -1;

        // Iterate over all the factories
        for(int f = 0; f < this->problem->getF(); f++)
        {
            int possiblePositions = individual->getFactory(f)->getJobs().size() + 1; // +1 because the job can be inserted at the end of the sequence
            for(int pos = 0; pos < possiblePositions; pos++)
            {
                // Create a twin individual to test the insertion
                Individual *twinIndividual = new Individual(individual);

                // Insert the job in the position pos of the factory f
                twinIndividual->getFactory(f)->addJobAtPosition(new Job(job), pos);
                twinIndividual->getFactory(f)->initializeJobsStartTimes();

                // Calculate the TFT increase
                float tftIncrease = twinIndividual->getTFT() - individual->getTFT();

                // If the TFT increase is the minimum, update the auxiliary variables
                if(tftIncrease < minTFTIncrease)
                {
                    minTFTIncrease = tftIncrease;
                    minTFTFactoryId = f;
                    minTFTFactoryPos = pos;
                }

                delete twinIndividual;
            }
        }

        // Insert the job in the position of the factory that leads to the minimum increase of total flow time
        individual->getFactory(minTFTFactoryId)->addJobAtPosition(job, minTFTFactoryPos);
        individual->getFactory(minTFTFactoryId)->initializeJobsStartTimes();
    }

    // 4th: initialize the start_times of each factory and then speed down
    for (int f = 0; f < this->problem->getF(); f++)
    {
        individual->getFactory(f)->initializeJobsStartTimes();
        individual->getFactory(f)->speedDown();
    }

    return individual;
}

Individual *NSGAII::randSMinTFT(int seed) {
    Individual *individual = new Individual(this->problem->getN(), this->problem->getM(), this->problem->getF());
    vector<Job *> jobs(this->problem->getN());
    Xoshiro256plus rand(/*time(NULL) +*/ seed);
    int highestSpeed = this->problem->getAllSpeeds().size() - 1;

    for (int i = 0; i < this->problem->getN(); i++) {

        jobs[i] = new Job(i, this->problem->getM());
        vector<int> jobTimeByMachine(this->problem->getM());
        vector<float> jobSpeedByMachine(this->problem->getM());

        for (int j = 0; j < this->problem->getM(); j++) {
            int randomIndex = rand.next() % this->problem->getAllSpeeds().size();
            jobTimeByMachine[j] = this->problem->getAllT()[i][j];
            jobSpeedByMachine[j] = this->problem->getAllSpeeds()[randomIndex];
        }
        jobs[i]->setT(jobTimeByMachine);
        jobs[i]->setV(jobSpeedByMachine);
    }

    sort(jobs.begin(), jobs.end(),
         compareJobsByTotalProcessingTime); // generate job permutation lambda according to non-descending order of Ti (total job i processing time)

    for (int i = 0; i < this->problem->getF(); i++) { // assign the first f jobs to each one of the factories
        // factories[i] = new Factory(i, this->problem->get_m());
        individual->getFactory(i)->addJobAtLastPosition(jobs[i]);
    }

    // remove the assigned jobs from lambda

    Factory *testFactory;
    float testFactoryTFT;
    float tftVariation;
    float previousTFT;


    // for each factory f
    for (int j = this->problem->getF();
         j < jobs.size(); j++) { // test job j at all the possible positions of PI_k (the factory)
        float minIncreaseTFT = INFINITY;
        float minTFTPos = 0;
        Factory *minTFTFactory = new Factory();

        for (int f = 0; f < this->problem->getF(); f++) {
            testFactory = individual->getFactory(f)->minTFTAfterInsertion(jobs[j]);
            testFactoryTFT = testFactory->getTFT();
            previousTFT = individual->getFactory(f)->getTFT();
            tftVariation = testFactoryTFT - previousTFT;

            if (tftVariation < minIncreaseTFT) {
                minIncreaseTFT = tftVariation;
                minTFTFactory->clearJobs();
                delete minTFTFactory;
                minTFTFactory = testFactory;
                minTFTPos = f;
            } else {
                testFactory->clearJobs();
                delete testFactory;
            }
        }

        // factories[minTFTPos] = minTFTFactory;
        individual->replaceFactory(minTFTPos, minTFTFactory); // replaces old factory and deletes it
    }

    for (int i = 0; i < this->problem->getF(); i++) {
        individual->getFactory(i)->speedDown();
    }
    // this->population.push_back(individual);

    // Initialize the of each factory
    for (int f = 0; f < this->problem->getF(); f++) {
        individual->getFactory(f)->initializeJobsStartTimes();
    }

    return individual;
}

Individual *NSGAII::minSMinTEC()
{
    // Individual *individual = new Individual(this->problem->getN(), this->problem->getM(), this->problem->getF());
    // vector<Job *> jobs(this->problem->getN());

    // // First, all the speeds set to the minimum value
    // for (int i = 0; i < this->problem->getN(); i++) {
    //     jobs[i] = new Job(i, this->problem->getM());
    //     vector<int> jobTimeByMachine(this->problem->getM());
    //     vector<float> jobSpeedByMachine(this->problem->getM());

    //     for (int j = 0; j < this->problem->getM(); j++) {
    //         jobTimeByMachine[j] = this->problem->getAllT()[i][j];
    //         jobSpeedByMachine[j] = this->problem->getAllSpeeds()[0]; // minimum speed
    //     }
    //     jobs[i]->setT(jobTimeByMachine);
    //     jobs[i]->setV(jobSpeedByMachine);
    // }

    // // Sort the jobs according to DESCENDING order of total processing time
    // // NEH procedure
    // sort(jobs.begin(), jobs.end(), compareJobsByTotalProcessingTime);

    // // Assign the first f jobs to each one of the factories
    // for (int k = 0; k < this->problem->getF(); k++)
    // {
    //     individual->getFactory(k)->addJobAtLastPosition(jobs[k]);
    // }

    // // Remove the assigned jobs from lambda = access the jobs vector from the f position to the end
    // // Insert remaining jobs in the position of the factory that can lead to the minimum increse of total energy consumption
    // Factory *testFactory;
    // float testFactoryTEC;
    // float tecVariation;
    // float previousTEC;
    // Factory *minTECFactory;

    // // For each factory f
    // for (int i = this->problem->getF(); i < jobs.size(); i++)
    // {
    //     float minIncreaseTEC = INFINITY;
    //     float minTECPos = 0;

    //     // Test job j at all the possible positions of PI_k (the factory)
    //     for (int k = 0; k < this->problem->getF(); k++) {
    //         testFactory = individual->getFactory(k)->minTECAfterInsertion(jobs[i]);
    //         testFactoryTEC = testFactory->getTEC();
    //         previousTEC = individual->getFactory(k)->getTEC();
    //         tecVariation = testFactoryTEC - previousTEC;

    //         if (tecVariation < minIncreaseTEC) {
    //             minIncreaseTEC = tecVariation;
    //             minTECFactory = testFactory;
    //             minTECPos = k;
    //         } else {
    //             // testFactory->clearJobs();
    //             delete testFactory;
    //         }
    //     }

    //     // Add the job to the factory that leads to the minimum increase of total energy consumption
    //     individual->replaceFactory(minTECPos, minTECFactory); // replaces old factory and deletes it
    // }

    // // Initialize the start_times of each factory and then right shift
    // for (int f = 0; f < this->problem->getF(); f++)
    // {
    //     individual->getFactory(f)->initializeJobsStartTimes();
    //     individual->getFactory(f)->rightShift();
    // }

    // return individual;

    // New version
    // 1st: all the jobs are processed at the minimum speed

    // Empty jobs vector
    vector<Job *> jobs(this->problem->getN());

    // Set all the speeds to the minimum value
    for (int i = 0; i < this->problem->getN(); i++)
    {
        jobs[i] = new Job(i, this->problem->getM());
        vector<int> jobTimeByMachine(this->problem->getM());
        vector<float> jobSpeedByMachine(this->problem->getM());

        // Iterate over all the machines
        for (int j = 0; j < this->problem->getM(); j++) {
            jobTimeByMachine[j] = this->problem->getAllT()[i][j];
            jobSpeedByMachine[j] = this->problem->getAllSpeeds()[0]; // minimum speed
        }

        // Set the standard time and speed of the job
        jobs[i]->setT(jobTimeByMachine);
        jobs[i]->setV(jobSpeedByMachine);
    }

    // 2nd: sort the jobs according to DESCENDING order of total processing time (NEH procedure)
    sort(jobs.begin(), jobs.end(), compareJobsByTotalProcessingTime);

    // 3rd: take the jobs out one by one and insert them in the position of the factory that leads to the minimum increase of total energy consumption
    Individual *individual = new Individual(this->problem->getN(), this->problem->getM(), this->problem->getF());
    for(int i = 0; i < jobs.size(); i++)
    {
        // Get the job and make a vector of its tentatives
        Job *job = jobs[i];

        // Auxiliary variables
        float minTECIncrease = INFINITY;
        int minTECFactoryId = -1;
        int minTECFactoryPos = -1;

        // Iterate over all the factories
        for(int f = 0; f < this->problem->getF(); f++)
        {
            int possiblePositions = individual->getFactory(f)->getJobs().size() + 1; // +1 because the job can be inserted at the end of the sequence
            for(int pos = 0; pos < possiblePositions; pos++)
            {
                // Create a twin individual to test the insertion
                Individual *twinIndividual = new Individual(individual);

                // Insert the job in the position pos of the factory f
                twinIndividual->getFactory(f)->addJobAtPosition(new Job(job), pos);

                // Initialize the start_times
                twinIndividual->getFactory(f)->initializeJobsStartTimes();
                

                // Calculate the TEC increase
                float tecIncrease = twinIndividual->getTEC() - individual->getTEC();

                // If the TEC increase is the minimum, update the auxiliary variables
                if(tecIncrease < minTECIncrease)
                {
                    minTECIncrease = tecIncrease;
                    minTECFactoryId = f;
                    minTECFactoryPos = pos;
                }

                delete twinIndividual;
            }
        }

        // Insert the job in the position of the factory that leads to the minimum increase of total energy consumption
        individual->getFactory(minTECFactoryId)->addJobAtPosition(job, minTECFactoryPos);
        individual->getFactory(minTECFactoryId)->initializeJobsStartTimes();
    }

    // 4th: initialize the start_times of each factory and then right shift
    for (int f = 0; f < this->problem->getF(); f++)
    {
        individual->getFactory(f)->initializeJobsStartTimes();
        individual->getFactory(f)->rightShift();
    }

    return individual;
}

Individual *NSGAII::randSMinTEC(int seed) {
    Xoshiro256plus rand(/*time(NULL) +*/ seed);

    Individual *individual = new Individual(this->problem->getN(), this->problem->getM(), this->problem->getF());
    vector<Job *> jobs(this->problem->getN());

    // First, all the speeds set to the minimum value
    for (int i = 0; i < this->problem->getN(); i++) {
        jobs[i] = new Job(i, this->problem->getM());
        vector<int> jobTimeByMachine(this->problem->getM());
        vector<float> jobSpeedByMachine(this->problem->getM());

        for (int j = 0; j < this->problem->getM(); j++) {
            int randomSpeedIndex = rand.next() % this->problem->getAllSpeeds().size();
            jobTimeByMachine[j] = this->problem->getAllT()[i][j];
            jobSpeedByMachine[j] = this->problem->getAllSpeeds()[randomSpeedIndex]; // random speed
        }
        jobs[i]->setT(jobTimeByMachine);
        jobs[i]->setV(jobSpeedByMachine);
    }

    // Sort the jobs according to non-descending order of total processing time
    sort(jobs.begin(), jobs.end(), compareJobsByTotalProcessingTime);

    // Assign the first f jobs to each one of the factories
    for (int k = 0; k < this->problem->getF(); k++) {
        individual->getFactory(k)->addJobAtLastPosition(jobs[k]);
    }

    // Remove the assigned jobs from lambda = access the jobs vector from the f position to the end

    // Insert remaining jobs in the position of the factory that can lead to the minimum increse of total energy consumption
    Factory *testFactory;
    float testFactoryTEC;
    float tecVariation;
    float previousTEC;


    // For each factory f
    for (int i = this->problem->getF(); i < jobs.size(); i++) {
        float minIncreaseTEC = INFINITY;
        float minTECPos = 0;
        Factory *minTECFactory = new Factory();

        // Test job j at all the possible positions of PI_k (the factory)
        for (int k = 0; k < this->problem->getF(); k++) {
            testFactory = individual->getFactory(k)->minTECAfterInsertion(jobs[i]);
            testFactoryTEC = testFactory->getTEC();
            previousTEC = individual->getFactory(k)->getTEC();
            tecVariation = testFactoryTEC - previousTEC;

            if (tecVariation < minIncreaseTEC) {
                minIncreaseTEC = tecVariation;
                minTECFactory->clearJobs();
                delete minTECFactory;
                minTECFactory = testFactory;
                minTECPos = k;
            } else {
                testFactory->clearJobs();
                delete testFactory;
            }
        }

        // Add the job to the factory that leads to the minimum increase of total energy consumption
        individual->replaceFactory(minTECPos, minTECFactory); // replaces old factory and deletes it
    }

    // Initialize the start_times of each factory and then right shift
    for (int f = 0; f < this->problem->getF(); f++) {
        individual->getFactory(f)->initializeJobsStartTimes();
        individual->getFactory(f)->rightShift();
    }

    return individual;
}

Individual *NSGAII::balancedRandomIndividualGenerator(int s) {
    Xoshiro256plus rand(/*time(NULL) +*/ s);

    Individual *individual = new Individual(this->problem->getN(), this->problem->getM(), this->problem->getF());

    // Vector that indicates if a job (id = index) has already been allocated
    vector<bool> jobAllocated(this->problem->getN(), false);

    vector<int> jobsToAllocate;
    for (size_t i = 0; i < this->problem->getN(); i++) {
        jobsToAllocate.push_back(i);
    }

    // Allocate the jobs equally to the factories
    int factoryId = 0;
    while (jobsToAllocate.size()) {
        int randomNum = rand.next() % jobsToAllocate.size();
        Job *job = new Job(jobsToAllocate[randomNum], this->problem->getM());
        job->setT(this->problem->getAllT()[jobsToAllocate[randomNum]]);

        // Set a random speed for each machine
        for (int j = 0; j < this->problem->getM(); j++) {
            int randomNum = rand.next() % this->problem->getAllSpeeds().size();
            // individual->setV(job->getId(), j, this->problem->getAllSpeeds()[randomNum]);
            job->setVForMachine(j, this->problem->getAllSpeeds()[randomNum]);
        }

        // Select the factory
        if (factoryId == this->problem->getF()) {
            factoryId = 0;
        }

        // Choose a random position to allocate the job
        int random_position = rand.next() % (individual->getFactory(factoryId)->getJobs().size() + 1);

        // Allocate the job
        individual->getFactory(factoryId)->addJobAtPosition(job, random_position);

        // Erase the allocated job from the list
        jobsToAllocate.erase(jobsToAllocate.begin() + randomNum);
        factoryId++;
    }

    // Initialize the of each factory
    for (int f = 0; f < this->problem->getF(); f++) {
        individual->getFactory(f)->initializeJobsStartTimes();
    }

    return individual;
}

Individual *NSGAII::totalRandomIndividualGenerator(int s) {
    Xoshiro256plus rand(/*time(NULL) +*/ s);

    Individual *individual = new Individual(this->problem->getN(), this->problem->getM(), this->problem->getF());

    // Vector that indicates if a job (id = index) has already been allocated
    vector<bool> jobAllocated(this->problem->getN(), false);

    vector<int> jobsToAllocate;
    for (size_t i = 0; i < this->problem->getN(); i++) {
        jobsToAllocate.push_back(i);
    }

    // Allocate one job in each factory
    for (int i = 0; i < this->problem->getF(); i++) {
        int randomNum = rand.next() % jobsToAllocate.size();
        Job *job = new Job(jobsToAllocate[randomNum], this->problem->getM());
        job->setT(this->problem->getAllT()[jobsToAllocate[randomNum]]);

        // Set a random speed for each machine
        for (int j = 0; j < this->problem->getM(); j++) {
            int randomNum = rand.next() % this->problem->getAllSpeeds().size();
            // individual->setV(job->getId(), j, this->problem->getAllSpeeds()[randomNum]);
            job->setVForMachine(j, this->problem->getAllSpeeds()[randomNum]);
        }
        individual->getFactory(i)->addJobAtLastPosition(job);
        jobsToAllocate.erase(jobsToAllocate.begin() + randomNum);
    }

    // Allocate the remaining jobs randomly
    while (jobsToAllocate.size()) {
        int randomNum = rand.next() % jobsToAllocate.size();
        Job *job = new Job(jobsToAllocate[randomNum], this->problem->getM());
        job->setT(this->problem->getAllT()[jobsToAllocate[randomNum]]);

        // Set a random speed for each machine
        for (int j = 0; j < this->problem->getM(); j++) {
            int randomNum = rand.next() % this->problem->getAllSpeeds().size();
            // individual->setV(job->getId(), j, this->problem->getAllSpeeds()[randomNum]);
            job->setVForMachine(j, this->problem->getAllSpeeds()[randomNum]);
        }

        // Choose a random factory to allocate the job
        int factoryId = rand.next() % this->problem->getF();

        // Choose a random position to allocate the job
        int random_position = rand.next() % (individual->getFactory(factoryId)->getJobs().size() + 1);

        // Allocate the job
        individual->getFactory(factoryId)->addJobAtPosition(job, random_position);

        // Erase the allocated job from the list
        jobsToAllocate.erase(jobsToAllocate.begin() + randomNum);
    }

    // Initialize the of jobs strat times each factory
    for (int f = 0; f < this->problem->getF(); f++) {
        individual->getFactory(f)->initializeJobsStartTimes();
    }

    return individual;
}

void NSGAII::printPopulation() {
    cout << "\nPOPULATION "
         << "(with " << this->population.size() << " individuals)" << endl;

    for (size_t i = 0; i < this->population.size(); i++) {
        cout << "\nIndividual " << i << endl;
        this->population[i]->printIndividual();
    }
}

string NSGAII::generatePopulationCSVString() {
    string str = "id, TFT, TEC, level, cd\n";
    Individual *individual;

    for (size_t i = 0; i < this->population.size(); i++) {
        individual = this->population[i];
        str += to_string(i) + "," + to_string(individual->getTFT()) + "," + to_string(individual->getTEC()) + "," +
               to_string(individual->getDominationRank()) + "," + to_string(individual->getCrowdingDistance()) + "\n";
    }
    return str;
}

vector<vector<Individual *>> NSGAII::fastNonDominatedSort(vector<Individual *> population) {

    vector<vector<int>> fronts(1);
    vector<vector<int>> dominatedBy(population.size());

    for (int i = 0; i < population.size(); i++) {
        population[i]->setDominationCounter(0);
        // population[i]->setDominationRank(-1);

        for (int j = 0; j < population.size(); j++) {
            if (population[i]->dominates(population[j])) {
                dominatedBy[i].push_back(j);
            } else if (population[j]->dominates(population[i])) {
                population[i]->incrementDominationCounter(1);
            }
        }

        if (population[i]->getDominationCounter() == 0) {
            population[i]->setDominationRank(1);
            fronts[0].push_back(i);
        }
    }

    int i = 0;
    while (!fronts[i].empty()) {
        vector<int> nextFront;
        for (int j = 0; j < fronts[i].size(); j++) {
            int frontSolId = fronts[i][j]; // id (indices) de cada individualução na fronteira atual

            for (int k = 0;
                 k <
                 dominatedBy[frontSolId].size(); k++) {                                                       // itera por cada individualução dominada pela de indice frontSolId
                int dominatedSolIndex = dominatedBy[frontSolId][k]; // id de cada individualução dominada por frontSolId

                Individual *s = population[dominatedSolIndex]; // cada individualução dominada por frontSolId

                s->incrementDominationCounter(-1);

                if (s->getDominationCounter() == 0) {
                    s->setDominationRank(i + 2);
                    nextFront.push_back(dominatedSolIndex);
                }
            }
        }
        i++;
        fronts.push_back(nextFront);
    }

    vector<vector<Individual *>> individualFronts(fronts.size());
    for (int i = 0; i < fronts.size(); i++) {
        vector<Individual *> front(fronts[i].size());
        for (int j = 0; j < fronts[i].size(); j++) {
            front[j] = population[fronts[i][j]];
        }
        individualFronts[i] = front;
    }

    individualFronts.pop_back();
    // this->dominationFronts = individualFronts;
    return individualFronts;
}

vector<vector<Individual *>> NSGAII::fastNonDominatedSort() {

    vector<vector<int>> fronts(1);
    vector<vector<int>> dominatedBy(population.size());

    for (int i = 0; i < population.size(); i++) {
        population[i]->setDominationCounter(0);
        // population[i]->setDominationRank(-1);

        for (int j = 0; j < population.size(); j++) {
            if (population[i]->dominates(population[j])) {
                dominatedBy[i].push_back(j);
            } else if (population[j]->dominates(population[i])) {
                population[i]->incrementDominationCounter(1);
            }
        }

        if (population[i]->getDominationCounter() == 0) {
            population[i]->setDominationRank(1);
            fronts[0].push_back(i);
        }
    }

    int i = 0;
    while (!fronts[i].empty()) {
        vector<int> nextFront;
        for (int j = 0; j < fronts[i].size(); j++) {
            int frontSolId = fronts[i][j]; // id (indices) de cada individualução na fronteira atual

            for (int k = 0;
                 k <
                 dominatedBy[frontSolId].size(); k++) {                                                       // itera por cada individualução dominada pela de indice frontSolId
                int dominatedSolIndex = dominatedBy[frontSolId][k]; // id de cada individualução dominada por frontSolId

                Individual *s = population[dominatedSolIndex]; // cada individualução dominada por frontSolId

                s->incrementDominationCounter(-1);

                if (s->getDominationCounter() == 0) {
                    s->setDominationRank(i + 2);
                    nextFront.push_back(dominatedSolIndex);
                }
            }
        }
        i++;
        fronts.push_back(nextFront);
    }

    vector<vector<Individual *>> individualFronts(fronts.size());
    for (int i = 0; i < fronts.size(); i++) {
        vector<Individual *> front(fronts[i].size());
        for (int j = 0; j < fronts[i].size(); j++) {
            front[j] = population[fronts[i][j]];
        }
        individualFronts[i] = front;
    }

    individualFronts.pop_back();
    this->dominationFronts = individualFronts;
    return individualFronts;
}

void NSGAII::assignCrowdingDistance() {
    int numObjectives = 2;

    for (int i = 0; i < population.size(); i++) {
        population[i]->setCrowdingDistance(0);
    }

    for (int i = 0; i < numObjectives; i++) {

        if (i == 0) {
            sort(population.begin(), population.end(), compareIndividualsByTFT);
            population[0]->setCrowdingDistance(INFINITY);
            population[population.size() - 1]->setCrowdingDistance(INFINITY);
            float maxTFT = population[population.size() - 1]->getTFT();
            float minTFT = population[0]->getTFT();
            for (int j = 1; j < population.size() - 1; j++) {
                float normalizedDistance =
                        (population[j + 1]->getTFT() - population[j - 1]->getTFT()) / (maxTFT - minTFT);
                population[j]->incrementCrowdingDistance(normalizedDistance);
            }
        } else {
            sort(population.begin(), population.end(), compareIndividualsByTEC);
            population[0]->setCrowdingDistance(INFINITY);
            population[population.size() - 1]->setCrowdingDistance(INFINITY);
            float maxTEC = population[population.size() - 1]->getTEC();
            float minTEC = population[0]->getTEC();

            for (int j = 1; j < population.size() - 1; j++) {
                float normalizedDistance =
                        (population[j + 1]->getTEC() - population[j - 1]->getTEC()) / (maxTEC - minTEC);
                population[j]->incrementCrowdingDistance(normalizedDistance);
            }
        }
    }
}

void assignCrowdingDistance(vector<Individual *> population) {
    int numObjectives = 2;
    int infinity = std::numeric_limits<int>::max();

    for (int i = 0; i < population.size(); i++) {
        population[i]->setCrowdingDistance(0);
    }

    for (int i = 0; i < numObjectives; i++) {

        if (i == 0) {
            sort(population.begin(), population.end(), compareIndividualsByTFT);
            population[0]->setCrowdingDistance(INFINITY);
            population[population.size() - 1]->setCrowdingDistance(INFINITY);
            float maxTFT = population[population.size() - 1]->getTFT();
            float minTFT = population[0]->getTFT();
            for (int j = 1; j < population.size() - 1; j++) {
                float normalizedDistance =
                        (population[j + 1]->getTFT() - population[j - 1]->getTFT()) / (maxTFT - minTFT);
                population[j]->incrementCrowdingDistance(normalizedDistance);
            }
        } else {
            sort(population.begin(), population.end(), compareIndividualsByTEC);
            population[0]->setCrowdingDistance(INFINITY);
            population[population.size() - 1]->setCrowdingDistance(INFINITY);
            float maxTEC = population[population.size() - 1]->getTEC();
            float minTEC = population[0]->getTEC();

            for (int j = 1; j < population.size() - 1; j++) {
                float normalizedDistance =
                        (population[j + 1]->getTEC() - population[j - 1]->getTEC()) / (maxTEC - minTEC);
                population[j]->incrementCrowdingDistance(normalizedDistance);
            }
        }
    }
}

bool crowdedCompare(Individual *s1, Individual *s2) {

    if (s1->getDominationRank() < s2->getDominationRank()) {
        return true;
    } else if (s1->getDominationRank() == s2->getDominationRank()) {
        if (s1->getCrowdingDistance() > s2->getCrowdingDistance()) {
            return true;
        }
        return false;
    }

    return false;
}

vector<Individual *> makeNewPop(vector<Individual *> parents, int seed, int n) {
    vector<Individual *> children;
    Xoshiro256plus rand(seed);

    vector<int> prob{1, 1, 1, 1, 0, 0, 0, 0, 0, 0}; // 1 chance of swap 0 chance of insertion

    for (int i = 0; i < parents.size(); i++) {

        Individual *individual = new Individual(parents[i]);

        for (int j = 0; j < n / 4; j++) {
            int factory1Id = rand.next() % individual->getNumFactories();
            int factory2Id = rand.next() % individual->getNumFactories();
            Factory *factory1 = individual->getFactory(factory1Id);
            Factory *factory2 = individual->getFactory(factory2Id);
            int job1 = rand.next() % factory1->getNumJobs();
            int job2 = rand.next() % factory2->getNumJobs();

            int choice = rand.next() % prob.size();
            // choice = 1;
            if (choice == 1) {
                individual->swap(factory1Id, factory2Id, factory1->getJob(job1), factory2->getJob(job2));
                // factory1->speedUp();
                // factory1->speedDown();
                // factory2->speedUp();
                // factory1->speedDown();
            } else {
                if (factory1->getNumJobs() - 1 > 0) {
                    individual->insert(factory1Id, factory2Id, factory1->getJob(job1), job2);
                } else if (factory2->getNumJobs() - 1 > 0) {
                    individual->insert(factory2Id, factory1Id, factory2->getJob(job2), job1);
                } else
                    individual->swap(factory1Id, factory2Id, factory1->getJob(job1), factory2->getJob(job2));
            }
            // factory1->initializeJobsStartTimes();
            // factory2->initializeJobsStartTimes();
        }

        children.push_back(individual);
    }

    return children;
}

vector<Individual *> makeNewPopV2(vector<Individual *> parents, int seed, int n) {
    vector<Individual *> children;
    Xoshiro256plus rand(seed);

    vector<int> prob{1, 1, 1, 1, 0, 0, 0, 0, 0, 0}; // 1 chance of swap 0 chance of insertion

    for (int i = 0; i < parents.size(); i++) {

        Individual *individual = new Individual(parents[i]);

        for (int j = 0; j < n / 4; j++) {
            int factory1Id = rand.next() % individual->getNumFactories();
            int factory2Id = rand.next() % individual->getNumFactories();
            Factory *factory1 = individual->getFactory(factory1Id);
            Factory *factory2 = individual->getFactory(factory2Id);
            int job1 = rand.next() % factory1->getNumJobs();
            int job2 = rand.next() % factory2->getNumJobs();

            int choice = rand.next() % prob.size();
            choice = 1;
            if (choice == 1) {
                individual->swap(factory1Id, factory2Id, factory1->getJob(job1), factory2->getJob(job2));
                factory1->speedUp();
                factory1->speedDown();
                factory2->speedUp();
                factory1->speedDown();
            } else {
                if (factory1->getNumJobs() - 1 > 0) {
                    individual->insert(factory1Id, factory2Id, factory1->getJob(job1), job2);
                } else if (factory2->getNumJobs() - 1 > 0) {
                    individual->insert(factory2Id, factory1Id, factory2->getJob(job2), job1);
                } else
                    individual->swap(factory1Id, factory2Id, factory1->getJob(job1), factory2->getJob(job2));
            }
            // factory1->initializeJobsStartTimes();
            // factory2->initializeJobsStartTimes();
        }

        children.push_back(individual);
    }

    return children;
}

vector<Individual *> makeNewPopV3(vector<Individual *> parents, int seed, int n) {
    vector<Individual *> children;
    Xoshiro256plus rand(seed);

    vector<int> prob{1, 1, 1, 1, 0, 0, 0, 0, 0, 0}; // 1 chance of swap 0 chance of insertion

    for (int i = 0; i < parents.size(); i++) {

        Individual *individual = new Individual(parents[i]);

        for (int j = 0; j < n / 4; j++) {
            int factory1Id = rand.next() % individual->getNumFactories();
            int factory2Id = rand.next() % individual->getNumFactories();
            Factory *factory1 = individual->getFactory(factory1Id);
            Factory *factory2 = individual->getFactory(factory2Id);
            int job1 = rand.next() % factory1->getNumJobs();
            int job2 = rand.next() % factory2->getNumJobs();

            int choice = rand.next() % prob.size();
            individual->swap(factory1Id, factory2Id, factory1->getJob(job1), factory2->getJob(job2));

            if (prob[choice] == 1) {
                factory1->speedUp();
                factory2->speedUp();
            } else {
                factory1->speedDown();
                factory2->speedDown();
            }
        }

        children.push_back(individual);
    }

    return children;
}

/**
 * Creates a new population of children from the parents
 * based on crossover and mutation. Probability of crossover and mutation
 * is determined by the crossover_rate and mutation_rate respectively.
 */

int roulette(vector<vector<Individual *>> fronts, int seed) {
    // Calculate arithmetic progression of the fronts
    int sumWeights = 0;
    for (int j = 0; j < fronts.size(); j++) {
        sumWeights += j + 1;
    }

    // Calculate probabilities of each front
    float sumProbabilities = 0.0;
    vector<float> probabilities;
    for (int j = fronts.size(); j > 0; j--) {
        float probability = j / sumWeights;
        probabilities.push_back(probability);
        sumProbabilities += probability;
    }

    // Normalize probabilities
    for (int j = 0; j < probabilities.size(); j++) {
        probabilities[j] /= sumProbabilities;
    }

    // Choose a front
    float random = ((double) rand()) / RAND_MAX;
    float sum = 0.0;
    int frontIndex = 0;
    for (int j = 0; j < probabilities.size(); j++) {
        sum += probabilities[j];
        if (random < sum) {
            frontIndex = j;
            break;
        }
    }

    return frontIndex;
}

vector<Individual *> NSGAII::makeChildren(int seed) {
    srand(seed);

    vector<Individual *> children;

    // Crossover
     for (int i = 0; i < this->population.size(); i++) {
        float probability = (float) rand() / (float) RAND_MAX;

        // If probability is less than crossover rate, then crossover
        if (probability < this->crossoverRate) {
            // One parent is the current parent
            int parent1Index = i;

            // Choose from what front the other parent will be using a roulette wheel
            int frontIndex = roulette(this->dominationFronts, seed);

            // Choose a random individual from the chosen front
            int parent2Index = rand() % this->dominationFronts[frontIndex].size();

            Individual* parent1 = this->population[parent1Index];
            Individual* parent2 = this->dominationFronts[frontIndex][parent2Index];

            // Crossover parents: // TODO
            Individual *child1 = new Individual(parent1);
            Individual *child2 = new Individual(parent2);

            vector<vector<float>> parent1Speeds = parent1->getAllV();
            vector<vector<float>> parent2Speeds = parent2->getAllV();

            child1->updateAllV(parent2Speeds);
            child2->updateAllV(parent1Speeds);


            children.push_back(child1);
            children.push_back(child2);
        }
    }

    // Mutation
   for (int i = 0; i < this->population.size(); i++) {
         float probability = (float) rand() / (float) RAND_MAX;

        // If probability is less than mutation rate, then mutate
         if (probability < this->mutationRate) {
             // TODO: é preciso lembrar se o INGM está mesmo funcionando
             // children.push_back(this->INGM(this->population[i], seed));
             // Eu acredito que poderíamos deixar o híbrido ao invés de só INGM

             // Criei uma nova função INGM chamada INGM_V2 que é uma versão que acredito estar mais correta do que
             // a implementação anterior e talvez não fique demorando tanto para achar uma solução que domine o pai
            //  Individual *child = this->INGM_V2(this->population[i], rand() % RAND_MAX);
             Individual *child = this->INGM_ND(this->population[i], rand());

             if (child != nullptr) {
                 children.push_back(child);
             }
         }
    }
    

    return children;
}

// Criei esse método só para não ter que apagar o NSGA2NextGen. Porém, eles são basicamente iguais. Só muda o makeNewPop que aqui é makeChildren
void NSGAII::NSGA2NextGeneration(int seed) {
    Xoshiro256plus rand(seed);

    // Parents and next generation
    vector<Individual *> parents = this->population;
    vector<Individual *> nextGen;

    // Recombine and mutate parents into this vector

    vector<Individual *> children = this->makeChildren(seed);

    // join parents and children into this vector
    vector<Individual *> all = parents;
    all.reserve(this->population.size() + children.size());
    all.insert(all.end(), children.begin(), children.end());

    // Update population
    this->population = all;


    vector<vector<Individual *>> fronts = this->fastNonDominatedSort();

    int inserted = 0;
    int n = parents.size();
    nextGen.reserve(n);

    // insere enquanto o numero de elementos inseridos for menor q n
    int l = 0;
    for (int i = 0; inserted < n && i < fronts.size() - 1; i++) {
        // nextGen.reserve(nextGen.size() + fronts[i].size());
        ::assignCrowdingDistance(fronts[i]);

        if (inserted + fronts[i].size() > n) {
            l = i;
            break;
        }

        for (int j = 0; j < fronts[i].size(); j++) {
            nextGen.push_back(fronts[i][j]);
            l=i;
            inserted++;
        }
    }

    int lastIndIndex=fronts[l].size()-1;
    if (nextGen.size() < n) {
        sort(fronts[l].begin(), fronts[l].end(), crowdedCompare);
        for (int i = 0; nextGen.size() < n; i++) {
            nextGen.push_back(fronts[l][i]);
            lastIndIndex = i;
        }
    }

    //int count = 0;
    for(int j=lastIndIndex+1; j<fronts[l].size(); j++){
        delete fronts[l][j];
    }

    for(int i=l+1; i<fronts.size(); i++){
        for(int j=0; j<fronts[i].size(); j++){
            delete fronts[i][j];
        }
    }

   // cout << "Deleted " << count << " individuals out of " << this->population.size() << endl;
    this->population = nextGen;
    this->fastNonDominatedSort();
}

void NSGAII::NSGA2NextGen(int seed) {
    Xoshiro256plus rand(seed);

    // Select parents
    vector<Individual *> parents = this->population;
    vector<Individual *> nextGen;

    // Recombine and mutate parents into this vector
    vector<Individual *> children = makeNewPopV3(parents, rand.next() % 30000, parents.size());

    // join parents and children into this vector
    vector<Individual *> all = parents;
    all.reserve(this->population.size() + children.size());
    all.insert(all.end(), children.begin(), children.end());
    this->population = all;

    vector<vector<Individual *>> fronts = this->fastNonDominatedSort();

    int inserted = 0;
    int n = parents.size();
    nextGen.reserve(n);

    // insere enquanto o numero de elementos inseridos for menor q n
    int l = 0;
    for (int i = 0; inserted < n && i < fronts.size() - 1; i++) {
        // nextGen.reserve(nextGen.size() + fronts[i].size());
        ::assignCrowdingDistance(fronts[i]);

        if (inserted + fronts[i].size() > n) {
            l = i;
            break;
        }

        for (int j = 0; j < fronts[i].size(); j++) {
            nextGen.push_back(fronts[i][j]);
            l=i;
            inserted++;
        }
    }

    int lastIndIndex=fronts[l].size()-1;
    if (nextGen.size() < n) {
        sort(fronts[l].begin(), fronts[l].end(), crowdedCompare);
        for (int i = 0; nextGen.size() < n; i++) {
            nextGen.push_back(fronts[l][i]);
            lastIndIndex = i;
        }
    }

    for(int j=lastIndIndex+1; j<fronts[l].size(); j++){
        delete fronts[l][j];
    }

    for(int i=l+1; i<fronts.size(); i++){
        for(int j=0; j<fronts[i].size(); j++){
            delete fronts[i][j];
        }
    }

    this->population = nextGen;
}

void NSGAII::NSGA2NextGen_operators(int seed) {
    vector<Individual *> parents = this->population;
    vector<Individual *> nextGen;

    // Recombine and mutate parents into this vector
    vector<Individual *> children = makenewpop_operators(parents, seed);

    // join parents and children into this vector
    vector<Individual *> all = parents;
    all.reserve(this->population.size() + children.size());
    all.insert(all.end(), children.begin(), children.end());
    this->population = all;

    vector<vector<Individual *>> fronts = this->fastNonDominatedSort();

    int inserted = 0;
    int n = parents.size();
    nextGen.reserve(n);

    // insere enquanto o numero de elementos inseridos for menor q n
    int l = 0;
    for (int i = 0; inserted < n && i < fronts.size() - 1; i++) {
        // nextGen.reserve(nextGen.size() + fronts[i].size());
        ::assignCrowdingDistance(fronts[i]);

        if (inserted + fronts[i].size() > n) {
            l = i;
            break;
        }

        for (int j = 0; j < fronts[i].size(); j++) {
            nextGen.push_back(fronts[i][j]);
            l=i;
            inserted++;
        }
    }

    int lastIndIndex=fronts[l].size()-1;
    if (nextGen.size() < n) {
        sort(fronts[l].begin(), fronts[l].end(), crowdedCompare);
        for (int i = 0; nextGen.size() < n; i++) {
            nextGen.push_back(fronts[l][i]);
            lastIndIndex = i;
        }
    }

    for(int j=lastIndIndex+1; j<fronts[l].size(); j++){
        delete fronts[l][j];
    }

    for(int i=l+1; i<fronts.size(); i++){
        for(int j=0; j<fronts[i].size(); j++){
            delete fronts[i][j];
        }
    }

    this->population = nextGen;
}

void NSGAII::NSGA2NextGen_operators_ND(int seed) {
    vector<Individual *> parents = this->population;
    vector<Individual *> nextGen;
    
    // Recombine and mutate parents into this vector
    vector<Individual *> children = makenewpop_operators_ND(parents, seed);
    
    // join parents and children into this vector
    vector<Individual *> all = parents;
    all.reserve(this->population.size() + children.size());
    all.insert(all.end(), children.begin(), children.end());
    this->population = all;

    vector<vector<Individual *>> fronts = this->fastNonDominatedSort();

    int inserted = 0;
    int n = parents.size();
    nextGen.reserve(n);

    // insere enquanto o numero de elementos inseridos for menor q n
    int l = 0;
    for (int i = 0; inserted < n && i < fronts.size() - 1; i++) {
        // nextGen.reserve(nextGen.size() + fronts[i].size());
        ::assignCrowdingDistance(fronts[i]);

        if (inserted + fronts[i].size() > n) {
            l = i;
            break;
        }

        for (int j = 0; j < fronts[i].size(); j++) {
            nextGen.push_back(fronts[i][j]);
            l=i;
            inserted++;
        }
    }

    int lastIndIndex=fronts[l].size()-1;
    if (nextGen.size() < n) {
        sort(fronts[l].begin(), fronts[l].end(), crowdedCompare);
        for (int i = 0; nextGen.size() < n; i++) {
            nextGen.push_back(fronts[l][i]);
            lastIndIndex = i;
        }
    }

    for(int j=lastIndIndex+1; j<fronts[l].size(); j++){
        delete fronts[l][j];
    }

    for(int i=l+1; i<fronts.size(); i++){
        for(int j=0; j<fronts[i].size(); j++){
            delete fronts[i][j];
        }
    }


    this->population = nextGen;
}

void normalize(vector<Individual *> individuals,
               vector<tuple<float, float, int, int>> &refPoints,
               vector<tuple<float, float, Individual *>> &normalizedIndividuals) {
    int numPoints = 10;
    refPoints.reserve(numPoints);
    // TFT, TEC, Niche Count, id
    // vector<tuple<float, float, int, int>> refPoints(numPoints);
    // TFT, TEC
    // vector<tuple<float, float, Individual*>> normalizedIndividuals(numPoints);

    // vector<float> refPointTFTs(numPoints);
    // vector<float> refPointTECs(numPoints);
    // vector<float> TFTs(individuals.size(), 0.0);
    // vector<float> TECs(individuals.size(), 0.0);
    float minTFT = Util::minTFTSol(individuals)->getTFT();
    float minTEC = Util::minTECSol(individuals)->getTEC();
    float maxTFT = Util::maxTFTSol(individuals)->getTFT();
    float maxTEC = Util::maxTECSol(individuals)->getTEC();

    for (int i = 0; i < individuals.size(); i++) {
        float nTFT = (individuals[i]->getTFT() - minTFT) / (maxTFT - minTFT);
        float nTEC = (individuals[i]->getTEC() - minTEC) / (maxTEC - minTEC);
        tuple<float, float, Individual *> ns = make_tuple(nTFT, nTEC, individuals[i]);
        normalizedIndividuals.push_back(ns);

        // TFTs[i] = (individuals[i]->getTFT() - minTFT) / maxTFT;
        // TECs[i] = (individuals[i]->getTEC() - minTEC) / maxTEC;
    }

    for (int i = 0; i < numPoints; i++) {
        float x = ((float) 1 / (float) numPoints) * (float) i;
        float y = 1 - ((float) 1 / (float) numPoints) * (float) i;
        tuple<float, float, int, int> ref = make_tuple(x, y, 0, 0);
        refPoints.push_back(ref);
        // refPointTECs.push_back((1 / numPoints) * i);
        // refPointTFTs.push_back((1 / numPoints) * i);
    }
}

void associate(vector<tuple<float, float, int, int>> &refPoints,
               vector<tuple<float, float, Individual *>> &normSol,
               vector<tuple<Individual *, float, int>> &assocVec) {

    int numIndividuals = normSol.size();
    int numRefPoints = refPoints.size();

    vector<int> associationVector(numIndividuals, 0);
    for (int i = 0; i < numIndividuals; i++) {
        float minDistance = INFINITY;
        int refPointPos;

        for (int j = 0; j < numRefPoints; j++) {
            // compute distance of individual from each line
            float a = get<1>(refPoints[j]);
            float b = -get<0>(refPoints[j]);
            float x0 = get<0>(normSol[i]);
            float y0 = get<1>(normSol[i]);
            float ax0 = a * x0;
            float by0 = b * y0;
            float distance = fabsf(ax0 + by0) / sqrtf(powf(a, 2) + powf(b, 2));

            if (distance < minDistance) {
                minDistance = distance;
                refPointPos = j;
            }
        }

        tuple<Individual *, float, int> assoc = make_tuple(get<2>(normSol[i]), minDistance, refPointPos);
        assocVec.push_back(assoc);
        // associationVector[i] = refPointPos;

        // assign PI(s) = line w closest from s
        // assign d(s) distance of individual. s to the closest line w
    }
}

bool nicheCompare(vector<tuple<Individual *, float>> &a, vector<tuple<Individual *, float>> &b) {
    return a.size() < b.size();
}

bool distanceCompare(tuple<Individual *, float> &a, tuple<Individual *, float> &b) {
    return get<0>(a) < get<0>(b);
}

vector<tuple<Individual *, float>>
getIntersection(vector<Individual *> &individualV, vector<tuple<Individual *, float>> &niche) {
    vector<tuple<Individual *, float>> intersection;

    for (Individual *s: individualV) {
        for (tuple<Individual *, float> n: niche) {
            if (s == get<0>(n)) {
                intersection.push_back(n);
            }
        }
    }

    return intersection;
}

int minDistanceIndex(vector<tuple<Individual *, float>> v) {

    float minDistance = INFINITY;
    int minDistancePos = 0;
    for (tuple<Individual *, float> t: v) {
        if (get<1>(t) < minDistance) {
            minDistance = get<1>(t);
        }
    }

    return minDistancePos;
}

vector<int> getLeastNicheCountPoints(vector<tuple<float, float, int, int>> &refPoints) {
    vector<int> leastNicheCountPoints;
    int leastNicheCount = std::numeric_limits<int>::max();
    for (tuple<float, float, int, int> p: refPoints) {
        if (get<2>(p) < leastNicheCount && get<3>(p) != -1) {
            leastNicheCount = get<2>(p);
        }
    }

    for (int i = 0; i < refPoints.size(); i++) {
        if (get<2>(refPoints[i]) == leastNicheCount && get<3>(refPoints[i]) != -1) {
            leastNicheCountPoints.push_back(i);
        }
    }
    return leastNicheCountPoints;
}

int minDistancePointIndex(vector<tuple<Individual *, float, int>> &assoc) {
    float minDistance = INFINITY;
    int minDistancePoint;

    for (int i = 0; i < assoc.size(); i++) {
        tuple<Individual *, float, int> p = assoc[i];
        if (get<1>(p) < minDistance) {
            minDistance = get<1>(p);
            minDistancePoint = i;
        }
    }

    return minDistancePoint;
}

void niching(int K, int seed, vector<tuple<float, float, int, int>> &refPoints,
             vector<tuple<Individual *, float, int>> &assoc,
             vector<Individual *> &lastFront,
             vector<Individual *> &selected) {

    // vector<Individual *> selected(K);
    Xoshiro256plus rand(seed);

    int k = 0;
    while (k < K) {
        // conjunto J = indices dos pontos com menor niche count
        vector<int> J = getLeastNicheCountPoints(refPoints);
        // escolhe elemento j aleatório em J
        int j = J[rand.next() % J.size()];

        // conjunto I = elementos de Fl que estão associados a j
        vector<tuple<Individual *, float, int>> I;
        for (tuple<Individual *, float, int> el: assoc) {
            if (get<2>(el) == j) {
                for (Individual *s: lastFront) {
                    if (get<0>(el) == s) {
                        I.push_back(el);
                    }
                }
            }
        }

        // se I não está vazio
        if (!I.empty()) {
            Individual *s;
            int pos;
            if (get<2>(refPoints[j]) == 0) {
                // seleciona elemento de I com o menor distância ao ponto de referência
                pos = minDistancePointIndex(I);
            } else {
                // seleciona elemento aleatório de I
                pos = rand.next() % I.size();
            }
            s = get<0>(I[pos]);
            selected.push_back(s);
            // incrementa niche count de j
            get<2>(refPoints[j])++;
            // remove elemento selecionado de Fl
            // std::remove(lastFront.begin(), lastFront.end(), s);
            lastFront.erase(std::remove(lastFront.begin(), lastFront.end(), s), lastFront.end());
            k++;
        } else {
            get<3>(refPoints[j]) = -1;
            // refPoints.erase(refPoints.begin()+j);
        }
    }
}

int NSGAII::nMetric() {
    return this->dominationFronts[0].size();
}

vector<Individual *> NSGAII::getParetoFront() {
    return this->dominationFronts[0];
}

Individual *NSGAII::INGM(Individual *individual, int seed) {
    Xoshiro256plus rand(seed);
    Individual *newIndividual = new Individual(individual);

    // Randomly choose the objective for optimization
    int randomObjective = rand.next() % 2; // 0 = TFT, 1 = TEC

    // Get the factory with the largest TFT or TEC
    float largestFactory = 0;
    int largestFactoryIndex = -1;

    if (randomObjective == 0) // Optimize TFT
    {
        for (int f = 0; f < this->problem->getF(); f++) {
            float fTFT = newIndividual->getFactory(f)->getTFT();
            if (fTFT > largestFactory) {
                largestFactory = fTFT;
                largestFactoryIndex = f;
            }
        }
    } else // Optimize TEC
    {
        for (int f = 0; f < this->problem->getF(); f++) {
            float fTEC = newIndividual->getFactory(f)->getTEC();
            if (fTEC > largestFactory) {
                largestFactory = fTEC;
                largestFactoryIndex = f;
            }
        }
    }

    vector<Job *> jobsToTry = newIndividual->getFactory(largestFactoryIndex)->getJobs();
    int largestFactoryFactoryTotalJobs = newIndividual->getFactory(largestFactoryIndex)->getNumJobs();

    while (jobsToTry.size() > largestFactoryFactoryTotalJobs / 2) {
        // Get a random job and extract from the factory
        int randomJobIndex = rand.next() % jobsToTry.size();
        Job *job = jobsToTry[randomJobIndex];
        jobsToTry.erase(jobsToTry.begin() + randomJobIndex);

        // Change the origin factory
        if (randomObjective == 0) // Optimize TFT
        {
            newIndividual->getFactory(largestFactoryIndex)->randSpeedUp(seed);
            newIndividual->getFactory(largestFactoryIndex)->speedUp();
        } else // Optimize TEC
        {
            newIndividual->getFactory(largestFactoryIndex)->randSpeedDown(seed);
            newIndividual->getFactory(largestFactoryIndex)->speedDown();
            newIndividual->getFactory(largestFactoryIndex)->rightShift();
        }

        // Try inserting the job to every position of every factory until the individual dominates the original one
        for (int f = 0; f < this->problem->getF(); f++) {
            int factoryNumJobs = newIndividual->getFactory(f)->getNumJobs();
            for (int pos = 0; pos < factoryNumJobs; pos++) {
                // Insert the job to the factory
                newIndividual->insert(largestFactoryIndex, f, job, pos);

                // Change the factory
                if (randomObjective == 0) // Optimize TFT
                {
                    newIndividual->getFactory(f)->randSpeedUp(seed);
                    newIndividual->getFactory(f)->speedUp();
                } else // Optimize TEC
                {
                    newIndividual->getFactory(f)->randSpeedDown(seed);
                    newIndividual->getFactory(f)->speedDown();
                    newIndividual->getFactory(f)->rightShift();
                }
                if (newIndividual->getTFT() < individual->getTFT() &&
                    newIndividual->getTEC() < individual->getTEC()) // If newIndividual dominates individual
                {
                    return newIndividual;
                } else if (newIndividual->getTFT() < individual->getTFT() ||
                           newIndividual->getTEC() < individual->getTEC()) {

                    this->updateArchive(newIndividual);
                }
                newIndividual->insert(f, largestFactoryIndex, job, randomJobIndex);
            }
        }
    }
    return nullptr;
}

// Second version of INGM with some changes (Guilherme - 03/07)
Individual *NSGAII::INGM_V2(Individual *individual, int seed) {
    Xoshiro256plus rand(seed);

    // Randomly choose the objective for optimization
    int randomObjective = rand.next() % 2; // 0 = TFT, 1 = TEC

    // Get the factory with the largest TFT or TEC
    float largestFactory = 0;
    int largestFactoryIndex = -1;

    if (randomObjective == 0) // Optimize TFT
    {
        for (int f = 0; f < this->problem->getF(); f++) {
            float fTFT = individual->getFactory(f)->getTFT();
            if (fTFT > largestFactory) {
                largestFactory = fTFT;
                largestFactoryIndex = f;
            }
        }
    } else // Optimize TEC
    {
        for (int f = 0; f < this->problem->getF(); f++) {
            float fTEC = individual->getFactory(f)->getTEC();
            if (fTEC > largestFactory) {
                largestFactory = fTEC;
                largestFactoryIndex = f;
            }
        }
    }

    vector<Job *> jobsToTry = individual->getFactory(largestFactoryIndex)->getJobs();
    int largestFactoryFactoryTotalJobs = individual->getFactory(largestFactoryIndex)->getNumJobs();

    while (jobsToTry.size() > largestFactoryFactoryTotalJobs / 2) {
        // Get a random job and extract from the factory
        int randomJobIndex = rand.next() % jobsToTry.size();
        Job *job = jobsToTry[randomJobIndex];
        jobsToTry.erase(jobsToTry.begin() + randomJobIndex);

        // Try inserting the job to every position of every factory until the individual dominates the original one
        for (int f = 0; f < this->problem->getF(); f++) {
            int factoryNumJobs = individual->getFactory(f)->getNumJobs();
            for (int pos = 0; pos < factoryNumJobs; pos++) {
                Individual *auxNewIndividual = new Individual(individual);

                // Insert the job to the factory
                auxNewIndividual->insert(largestFactoryIndex, f, job, pos);

                // Change the factory
                if (randomObjective == 0) // Optimize TFT
                {
                    // Change the origin factory
                    auxNewIndividual->getFactory(largestFactoryIndex)->randSpeedUp(seed);
                    auxNewIndividual->getFactory(largestFactoryIndex)->speedUp();

                    // Change the destination factory
                    auxNewIndividual->getFactory(f)->randSpeedUp(seed);
                    auxNewIndividual->getFactory(f)->speedUp();
                } else // Optimize TEC
                {
                    // Change the origin factory
                    auxNewIndividual->getFactory(largestFactoryIndex)->randSpeedDown(seed);
                    auxNewIndividual->getFactory(largestFactoryIndex)->speedDown();
                    auxNewIndividual->getFactory(largestFactoryIndex)->rightShift();

                    // Change the destination factory
                    auxNewIndividual->getFactory(f)->randSpeedDown(seed);
                    auxNewIndividual->getFactory(f)->speedDown();
                    auxNewIndividual->getFactory(f)->rightShift();
                }

                // Compare the new individual with the original one
                // If auxNewIndividual dominates individual
                if (auxNewIndividual->getTFT() < individual->getTFT() &&
                    auxNewIndividual->getTEC() < individual->getTEC()) {
                    return auxNewIndividual;
                }
                 // If auxNewIndividual dominates individual in one objective
                /* else  if (auxNewIndividual->getTFT() < individual->getTFT() ||
                         auxNewIndividual->getTEC() < individual->getTEC()) {
                    return auxNewIndividual;
                }*/
            }
        }
    }
    return nullptr;
}

Individual *NSGAII::SNGM(Individual *individual, int seed) {
    Xoshiro256plus rand(seed);
    Individual *newIndividual = new Individual(individual);

    // Randomly choose the objective for optimization
    int randomObjective = rand.next() % 2; // 0 = TFT, 1 = TEC

    // Get the factory with the largest TFT or TEC
    float largestFactory = 0;
    int largestFactoryIndex = -1;

    if (randomObjective == 0) // Optimize TFT
    {
        for (int f = 0; f < this->problem->getF(); f++) {
            float fTFT = newIndividual->getFactory(f)->getTFT();
            if (fTFT > largestFactory) {
                largestFactory = fTFT;
                largestFactoryIndex = f;
            }
        }
    } else // Optimize TEC
    {
        for (int f = 0; f < this->problem->getF(); f++) {
            float fTEC = newIndividual->getFactory(f)->getTEC();
            if (fTEC > largestFactory) {
                largestFactory = fTEC;
                largestFactoryIndex = f;
            }
        }
    }

    vector<Job *> jobsToTry = newIndividual->getFactory(largestFactoryIndex)->getJobs();
    int largestFactoryFactoryTotalJobs = newIndividual->getFactory(largestFactoryIndex)->getNumJobs();
    while (jobsToTry.size() > largestFactoryFactoryTotalJobs / 2) {
        // Get a random job
        int randomJobIndex = rand.next() % jobsToTry.size();
        Job *job = jobsToTry[randomJobIndex];
        jobsToTry.erase(jobsToTry.begin() + randomJobIndex);

        // Try inserting the job to every position of every factory until the individual dominates the original one
        for (int f = 0; f < this->problem->getF(); f++) {
            bool tag = false;
            int factoryNumJobs = newIndividual->getFactory(f)->getNumJobs();
            for (int pos = 0; pos < factoryNumJobs; pos++) {
                // Swap the job to the factory
                Job *job2 = newIndividual->getFactory(f)->getJobs().at(pos);
                while (job2->getId() == job->getId()) // get a different job to swap
                {
                    // if all possibilities of positions on f have been tried
                    if (pos + 1 == factoryNumJobs) {
                        tag = true;
                        break;
                    } else {
                        pos++;
                        job2 = newIndividual->getFactory(f)->getJobs().at(pos);
                    }
                }
                // Needs to change factory
                if (tag)
                    break;

                newIndividual->swap(largestFactoryIndex, f, job, job2);

                // Change the factories
                if (randomObjective == 0) // Optimize TFT
                {
                    newIndividual->getFactory(largestFactoryIndex)->randSpeedUp(seed);
                    newIndividual->getFactory(largestFactoryIndex)->speedUp();

                    newIndividual->getFactory(f)->randSpeedUp(seed);
                    newIndividual->getFactory(f)->speedUp();
                } else // Optimize TEC
                {
                    newIndividual->getFactory(largestFactoryIndex)->randSpeedDown(seed);
                    newIndividual->getFactory(largestFactoryIndex)->speedDown();
                    newIndividual->getFactory(largestFactoryIndex)->rightShift();

                    newIndividual->getFactory(f)->randSpeedDown(seed);
                    newIndividual->getFactory(f)->speedDown();
                    newIndividual->getFactory(f)->rightShift();
                }
                if (newIndividual->getTFT() < individual->getTFT() &&
                    newIndividual->getTEC() < individual->getTEC()) // If newIndividual dominates individual
                {
                    return newIndividual;
                } else if (newIndividual->getTFT() < individual->getTFT() ||
                           newIndividual->getTEC() < individual->getTEC()) {

                    // this->updateArchive(newIndividual);
                }
                newIndividual->swap(f, largestFactoryIndex, job, job2);
            }
        }
    }
    return nullptr;
}

Individual *NSGAII::HNGM(Individual *individual, int seed) {
    Xoshiro256plus rand(seed/*time(NULL)*/);

    // Randomly choose INGM or SNGM
    int random_gen = rand.next() % 2; // 0 = INGM, 1 = SNGM

    if (random_gen == 0)
        return this->INGM(individual, seed);
    // else
    return this->SNGM(individual, seed);
}

vector<Individual *> NSGAII::makenewpop_operators(vector<Individual *> parents, int seed) {
    Xoshiro256plus rand(seed);

    // clear the newIndividuals vector
    vector<Individual *> children;
    children.clear();
    Individual *individual_ptr = nullptr;

    // Generate the same number of new individuals as parents size
    // For each individual in parents, generate a neighbour
    int i = 0;
    int cont = 0;
    while (i < parents.size()) {
        // Randomly choose which operator will be used to generate a neighbour
        int rand_op = rand.next() % 3; // 0 = INGM, 1 = SNGM, 2 = HNGM

        if (rand_op == 0) {
            individual_ptr = this->INGM(parents[i], rand.next() % 30000);
        } else if (rand_op == 1) {
            individual_ptr = this->SNGM(parents[i], rand.next() % 30000);
        } else {
            individual_ptr = this->HNGM(parents[i], rand.next() % 30000);
        }
        i++;
        if (individual_ptr != nullptr) {
            children.push_back(individual_ptr);
        }

        // if(cont == 3*parents.size()){
        //    break;
        //}

        cont++;
    }

    // Return children
    return children;
}

// INGM Non-Dominated: can generates an individual dominated by the original one
Individual *NSGAII::INGM_ND(Individual *individual, int seed) {
    Xoshiro256plus rand(seed);
    Individual *newIndividual = new Individual(individual);

    // Randomly choose the objective for optimization
    int randomObjective = rand.next() % 2; // 0 = TFT, 1 = TEC

    // Get the factory with the largest TFT or TEC
    float largestFactory = 0;
    int largestFactoryIndex = -1;

    if (randomObjective == 0) // Optimize TFT
    {
        for (int f = 0; f < this->problem->getF(); f++) {
            float fTFT = newIndividual->getFactory(f)->getTFT();
            if (fTFT > largestFactory) {
                largestFactory = fTFT;
                largestFactoryIndex = f;
            }
        }
    } else // Optimize TEC
    {
        for (int f = 0; f < this->problem->getF(); f++) {
            float fTEC = newIndividual->getFactory(f)->getTEC();
            if (fTEC > largestFactory) {
                largestFactory = fTEC;
                largestFactoryIndex = f;
            }
        }
    }

    vector<Job *> jobsToTry = newIndividual->getFactory(largestFactoryIndex)->getJobs();
    int largestFactoryFactoryTotalJobs = newIndividual->getFactory(largestFactoryIndex)->getNumJobs();

    while (jobsToTry.size() > largestFactoryFactoryTotalJobs / 2) {
        // Get a random job and extract from the factory
        int randomJobIndex = rand.next() % jobsToTry.size();
        Job *job = jobsToTry[randomJobIndex];
        jobsToTry.erase(jobsToTry.begin() + randomJobIndex);

        // Change the origin factory
        /* if (randomObjective == 0) // Optimize TFT
        {
            // newIndividual->getFactory(largestFactoryIndex)->randSpeedUp(seed);
            // newIndividual->getFactory(largestFactoryIndex)->speedUp();
        } else // Optimize TEC
        {
            // newIndividual->getFactory(largestFactoryIndex)->randSpeedDown(seed);
            // newIndividual->getFactory(largestFactoryIndex)->speedDown();
            // newIndividual->getFactory(largestFactoryIndex)->rightShift();
        } */

        // Try inserting the job to every position of every factory until the individual dominates the original one
        for (int f = 0; f < this->problem->getF(); f++) {
            int factoryNumJobs = newIndividual->getFactory(f)->getNumJobs();
            for (int pos = 0; pos < factoryNumJobs; pos++) {
                // Insert the job to the factory
                newIndividual->insert(largestFactoryIndex, f, job, pos);

                // Change the factory
                if (randomObjective == 0) // Optimize TFT
                {
                    newIndividual->getFactory(largestFactoryIndex)->randSpeedUp(seed);
                    newIndividual->getFactory(largestFactoryIndex)->speedUp();

                    newIndividual->getFactory(f)->randSpeedUp(seed);
                    newIndividual->getFactory(f)->speedUp();
                } else // Optimize TEC
                {
                    newIndividual->getFactory(largestFactoryIndex)->randSpeedDown(seed);
                    newIndividual->getFactory(largestFactoryIndex)->speedDown();
                    newIndividual->getFactory(largestFactoryIndex)->rightShift();

                    newIndividual->getFactory(f)->randSpeedDown(seed);
                    newIndividual->getFactory(f)->speedDown();
                    newIndividual->getFactory(f)->rightShift();
                }
                if (newIndividual->getTFT() < individual->getTFT() ||
                    newIndividual->getTEC() < individual->getTEC()) // If newIndividual is non dominated
                {
                    // this->updateArchive(newIndividual);
                    return newIndividual;
                }
                newIndividual->insert(f, largestFactoryIndex, job, randomJobIndex);
            }
        }
    }

    delete newIndividual;
    return nullptr;
}

Individual *NSGAII::SNGM_ND(Individual *individual, int seed) {
    Xoshiro256plus rand(seed);
    Individual *newIndividual = new Individual(individual);

    // Randomly choose the objective for optimization
    int randomObjective = rand.next() % 2; // 0 = TFT, 1 = TEC

    // Get the factory with the largest TFT or TEC
    float largestFactory = 0;
    int largestFactoryIndex = -1;

    if (randomObjective == 0) // Optimize TFT
    {
        for (int f = 0; f < this->problem->getF(); f++) {
            float fTFT = newIndividual->getFactory(f)->getTFT();
            if (fTFT > largestFactory) {
                largestFactory = fTFT;
                largestFactoryIndex = f;
            }
        }
    } else // Optimize TEC
    {
        for (int f = 0; f < this->problem->getF(); f++) {
            float fTEC = newIndividual->getFactory(f)->getTEC();
            if (fTEC > largestFactory) {
                largestFactory = fTEC;
                largestFactoryIndex = f;
            }
        }
    }

    vector<Job *> jobsToTry = newIndividual->getFactory(largestFactoryIndex)->getJobs();
    int largestFactoryFactoryTotalJobs = newIndividual->getFactory(largestFactoryIndex)->getNumJobs();
    while (jobsToTry.size() > largestFactoryFactoryTotalJobs / 2) {
        // Get a random job
        int randomJobIndex = rand.next() % jobsToTry.size();
        Job *job = jobsToTry[randomJobIndex];
        jobsToTry.erase(jobsToTry.begin() + randomJobIndex);

        // Try inserting the job to every position of every factory until the individual dominates the original one
        for (int f = 0; f < this->problem->getF(); f++) {
            bool tag = false;
            int factoryNumJobs = newIndividual->getFactory(f)->getNumJobs();
            for (int pos = 0; pos < factoryNumJobs; pos++) {
                // Swap the job to the factory
                Job *job2 = newIndividual->getFactory(f)->getJobs().at(pos);
                while (job2->getId() == job->getId()) // get a different job to swap
                {
                    // if all possibilities of positions on f have been tried
                    if (pos + 1 == factoryNumJobs) {
                        tag = true;
                        break;
                    } else
                        job2 = newIndividual->getFactory(f)->getJobs().at(pos++);
                }
                // Needs to change factory
                if (tag)
                    break;

                newIndividual->swap(largestFactoryIndex, f, job, job2);

                // Change the factories
                if (randomObjective == 0) // Optimize TFT
                {
                    newIndividual->getFactory(largestFactoryIndex)->randSpeedUp(seed);
                    newIndividual->getFactory(largestFactoryIndex)->speedUp();

                    newIndividual->getFactory(f)->randSpeedUp(seed);
                    newIndividual->getFactory(f)->speedUp();
                } else // Optimize TEC
                {
                    newIndividual->getFactory(largestFactoryIndex)->randSpeedDown(seed);
                    newIndividual->getFactory(largestFactoryIndex)->speedDown();
                    newIndividual->getFactory(largestFactoryIndex)->rightShift();

                    newIndividual->getFactory(f)->randSpeedDown(seed);
                    newIndividual->getFactory(f)->speedDown();
                    newIndividual->getFactory(f)->rightShift();
                }
                if (newIndividual->getTFT() < individual->getTFT() ||
                    newIndividual->getTEC() < individual->getTEC()) // If newIndividual is non dominated by individual
                {
                    // this->updateArchive(newIndividual);
                    return newIndividual;
                }
                newIndividual->swap(f, largestFactoryIndex, job, job2);
            }
        }
    }
    return nullptr;
}

Individual *NSGAII::HNGM_ND(Individual *individual, int seed) {
    Xoshiro256plus rand(seed/*time(NULL)*/);

    // Randomly choose INGM or SNGM
    int random_gen = rand.next() % 2; // 0 = INGM, 1 = SNGM

    if (random_gen == 0)
        return this->INGM_ND(individual, seed);
    // else
    return this->SNGM_ND(individual, seed);
}

vector<Individual *> NSGAII::makenewpop_operators_ND(vector<Individual *> parents, int seed) {
    Xoshiro256plus rand(seed);

    // clear the newIndividuals vector
    vector<Individual *> children;
    children.clear();
    Individual *individual_ptr = nullptr;

    // Generate the same number of new individuals as parents size
    // For each individual in parents, generate a neighbour
    int i = 0;
    int cont = 0;
    
    while (children.size() < parents.size()) {
        // Randomly choose which operator will be used to generate a neighbour
        int rand_op = rand.next() % 3; // 0 = INGM, 1 = SNGM, 2 = HNGM
        
        if (rand_op == 0) {
            individual_ptr = this->INGM_ND(parents[i], rand.next() % 30000);
        } else if (rand_op == 1) {
            individual_ptr = this->SNGM_ND(parents[i], rand.next() % 30000);
        } else {
            individual_ptr = this->HNGM_ND(parents[i], rand.next() % 30000);
        }
        i++;
        if (individual_ptr != nullptr) {
            children.push_back(individual_ptr);
            cont = 0;
        }
        if (i == parents.size())
            i = 0;

        if (cont == 10 * parents.size()) {
            break;
        }
        cont++;
    }

    // Return children
    return children;
}

// string runExperiment(string path, int iterations, float stopTime, int baseSeed)
// {

//     string csv = "";
//     // vector<vector<Solution*>> paretoArchive;
//     vector<Problem *> instances;
//     clock_t start, end;

//     for (int j = 0; j < 10; j++)
//     {
//         instance->balancedRandomSolutionGenerator(j + baseSeed);
//         instance->randSMinTEC(j + baseSeed);
//         instance->randSMinTFT(j + baseSeed);
//     }
//     instance->minSMinTEC();
//     instance->maxSMinTFT();

//     instance->assignCrowdingDistance();

//     int nsgaIterationsSum = 0;
//     instances.push_back(instance);

//     start = clock();

//     int counter = 0;
//     while (true)
//     {
//         end = clock();
//         double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
//         if (time_taken > instance->get_n() / 2)
//         {
//             cout << "Time's up! " << counter << " iterations in " << time_taken << " seconds" << endl;
//             break;
//         }

//         instance->NSGA2NextGen(nsgaIterationsSum + baseSeed);

//         nsgaIterationsSum++;
//         counter++;
//     }

//     Util::deallocate();

//     return csv;
// }

// string runExperiment(string path, int iterations, float stopTime, int baseSeed)
// {

//     // string csv = "id,baseSeed,iterations,nsgaIterations,N,D(antiga), GD, IGD, S\n";
//     string csv = "";
//     vector<vector<Solution *>> paretoArchive;
//     vector<Problem *> instances;
//     clock_t start, end;

//     int nsgaIterationsSum = 0;
//     for (int i = 0; i < iterations; i++)
//     {
//         Problem *instance = readFile(path);
//         instances.push_back(instance);

//         start = clock();

//         for (int j = 0; j < 10; j++)
//         {
//             instance->balancedRandomSolutionGenerator(j + baseSeed);
//             instance->randSMinTEC(j + baseSeed);
//             instance->randSMinTFT(j + baseSeed);
//         }
//         instance->minSMinTEC();
//         instance->maxSMinTFT();

//         instance->assignCrowdingDistance();

//         int counter = 0;
//         while (true)
//         {
//             end = clock();
//             double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
//             if (time_taken > instance->get_n() / 2)
//             {
//                 cout << "Time's up! " << counter << " iterations in " << time_taken << " seconds" << endl;
//                 break;
//             }

//             instance->NSGA2NextGen(nsgaIterationsSum + baseSeed);

//             nsgaIterationsSum++;
//             counter++;
//         }

//         instance->fastNonDominatedSort();
//         paretoArchive.push_back(instance->getParetoFront());
//         // csv += path + "," + to_string(baseSeed) + "," + to_string(nsgaIterations) + "," + to_string(instance->nMetric()) + "\n";

//         // delete instance;
//     }

//     vector<Solution *> joinedParetoArchive = joinFronts(paretoArchive);
//     vector<Solution *> archiveParetoFront = Util::fastNonDominatedSort(joinedParetoArchive)[0];

//     csv += path + "," + to_string(baseSeed) + "," + to_string(iterations) + "," + to_string((float)nsgaIterationsSum / (float)iterations) + "," + to_string(archiveParetoFront.size()) + "," + to_string(meanDMetric(paretoArchive, archiveParetoFront)) + "," + to_string(meanGDMetric(paretoArchive, archiveParetoFront)) + "," + to_string(meanIGDMetric(paretoArchive, archiveParetoFront)) + "," + to_string(meanSMetric(paretoArchive, archiveParetoFront)) + "\n";

//     // for(Instance* i:instances){
//     //     delete i;
//     // }

//     Util::deallocate();

//     return csv;
// }

void NSGAII::run(int seed, int iterationsLimit, float timeLimit, int option) {
    //cout << "running experiment " << seed << endl;
    clock_t start, end;

    // Initialize the population PS = 30 individuals (26 random + 4 balanced)
    for (int j = 0; j < 26; j++) {
        this->population.push_back(this->totalRandomIndividualGenerator(j + seed));
    }
    this->population.push_back(this->randSMinTEC(seed));
    this->population.push_back(this->randSMinTFT(seed));
    this->population.push_back(this->minSMinTEC());
    this->population.push_back(this->maxSMinTFT());
    this->fastNonDominatedSort();

    //cout << "initial pop generated " << seed << endl;
    // Assign crowding distance to individuals to facilitate the selection process
    this->assignCrowdingDistance();
    string experimentDir = "../analysis/exp/"+to_string(seed);
    string experimentCSVDir = "../analysis/exp/"+to_string(seed)+"/csv";

    if (this->outputEnabled) {
        // If directory ../analysis/exp/ does not exist, create it
        struct stat buffer;
        if (stat(experimentDir.c_str(), &buffer) != 0)
        {
            string createDirName = "mkdir " + experimentDir;
            system(createDirName.c_str());
        }

        if (stat(experimentCSVDir.c_str(), &buffer) != 0)
        {
            string createDirName = "mkdir " + experimentCSVDir;
            system(createDirName.c_str());
        }
        
        // Clear and create directory ../analysis/exp/seed/
        string clearCreateDirName = "rm -rf "+experimentCSVDir+" && mkdir " + experimentCSVDir;
        system(clearCreateDirName.c_str());

        Util::outputToFile(experimentCSVDir + "/after_0.csv", this->generatePopulationCSVString(), false);
    }

    int counter = 0;

    start = clock();
    while (true) {
        end = clock();
        double time_taken = double(end - start) / double(CLOCKS_PER_SEC);

        //cout << "counter: " << counter << " time: " << time_taken << endl;
        if (time_taken > timeLimit || counter >= iterationsLimit) {
            cout << "Time's up! " << counter << " iterations in " << time_taken << " seconds" << endl;
            break;
        }

        if(option==0){
            this->NSGA2NextGeneration(counter + seed);
        }else if(option==1){
            this->NSGA2NextGen(counter + seed);}
        else if(option==2){
            this->NSGA2NextGen_operators_ND(counter + seed);
        }

        counter++;

        if (this->outputEnabled) {
            Util::outputToFile(experimentCSVDir + "/after_" + to_string(counter) + ".csv", this->generatePopulationCSVString(), false);
        }
    }

    if (this->outputEnabled) {
        // Check duplicates at last iteration file
        Util::checkDuplicateIndividualsAtFile(experimentCSVDir + "/after_" + to_string(counter) + ".csv");
    }


    this->fastNonDominatedSort();
    this->makeMinimalParetoFront();
    // Add the pareto front to the archive
    // for (Individual *i: this->getParetoFront()) {
    //     this->updateArchive(i);
    // }
    this->numIterations = counter;

   // Util::deallocate();
}

void NSGAII::setOutputEnabled(bool outputEnabled) {
    this->outputEnabled = outputEnabled;
}

int NSGAII::getNumIterations() {
    return this->numIterations;
}

void NSGAII::makeMinimalParetoFront() {
    this->minimalParetoFront.reserve(this->getParetoFront().size());

    for(Individual* i:this->getParetoFront()){
       MinimalIndividual* mi = new MinimalIndividual(i);
       minimalParetoFront.push_back(mi);
    }
}

vector<MinimalIndividual *> NSGAII::getMinimalParetoFront() {
    return this->minimalParetoFront;
}
