#include "NSGAII.hpp"
#include "Util.hpp"

NSGAII::NSGAII(Problem *problem)
{
    this->problem = problem;
}

NSGAII::~NSGAII()
{

    if (this->population.size() > 0)
    {
        for (int i = 0; i < this->population.size(); i++)
        {
            delete this->population[i];
        }
    }
}

vector<Individual *> NSGAII::getPopulation()
{
    return this->population;
}

Individual *NSGAII::getIndividual(int i)
{
    return this->population[i];
}

vector<Individual *> NSGAII::getParetoArchive()
{
    return this->paretoArchive;
}

void NSGAII::updateArchive(Individual *sol)
{
    for (int i = 0; i < this->paretoArchive.size(); i++)
    {
        if (paretoArchive[i]->dominates(sol))
        {
            return;
        }
    }

    for (int i = 0; i < this->paretoArchive.size(); i++)
    {
        if (sol->dominates(paretoArchive[i]))
        {
            paretoArchive.erase(paretoArchive.begin() + i);
        }
    }

    paretoArchive.push_back(sol);
}

void NSGAII::construtivo()
{

    for (int i = 0; i < 4; i++)
    {
        this->balancedRandomIndividualGenerator(i);
    }
}

bool compareJobsByTotalProcessingTime(Job *a, Job *b)
{
    return a->getTotalP() < b->getTotalP();
}

bool compareIndividualsByTFT(Individual *a, Individual *b)
{
    return a->getTFT() < b->getTFT();
}

bool compareIndividualsByTEC(Individual *a, Individual *b)
{
    return a->getTEC() < b->getTEC();
}

Individual *NSGAII::maxSMinTFT()
{
    Individual *individual = new Individual(this->problem->getN(), this->problem->getM(), this->problem->getF());
    vector<Job *> jobs(this->problem->getN());
    int highestSpeed = this->problem->getAllSpeeds().size() - 1;

    for (int i = 0; i < this->problem->getN(); i++)
    {
        jobs[i] = new Job(i, this->problem->getM());
        vector<int> jobTimeByMachine(this->problem->getM());
        vector<float> jobSpeedByMachine(this->problem->getM());

        for (int j = 0; j < this->problem->getM(); j++)
        {
            jobTimeByMachine[j] = this->problem->getAllT()[i][j];
            jobSpeedByMachine[j] = this->problem->getAllSpeeds()[highestSpeed];
        }
        jobs[i]->setT(jobTimeByMachine);
        jobs[i]->setV(jobSpeedByMachine);
    }

    sort(jobs.begin(), jobs.end(),
         compareJobsByTotalProcessingTime); // generate job permutation lambda according to non-descending order of Ti (total job i processing time)

    for (int i = 0; i < this->problem->getF(); i++)
    { // assign the first f jobs to each one of the factories
        // factories[i] = new Factory(i, this->problem->get_m());
        individual->getFactory(i)->addJobAtLastPosition(jobs[i]);
    }

    // remove the assigned jobs from lambda

    Factory *testFactory;
    float testFactoryTFT;
    float tftVariation;
    float previousTFT;
    Factory *minTFTFactory;

    // for each factory f
    for (int j = this->problem->getF(); j < jobs.size(); j++)
    { // test job j at all the possible positions of PI_k (the factory)
        float minIncreaseTFT = INFINITY;
        float minTFTPos = 0;

        for (int f = 0; f < this->problem->getF(); f++)
        {
            testFactory = individual->getFactory(f)->minTFTAfterInsertion(jobs[j]);
            testFactoryTFT = testFactory->getTFT();
            previousTFT = individual->getFactory(f)->getTFT();
            tftVariation = testFactoryTFT - previousTFT;

            if (tftVariation < minIncreaseTFT)
            {
                minIncreaseTFT = tftVariation;
                minTFTFactory = testFactory;
                minTFTPos = f;
            }
            else
            {
                // testFactory->clearJobs();
                delete testFactory;
            }
        }

        // factories[minTFTPos] = minTFTFactory;
        individual->replaceFactory(minTFTPos, minTFTFactory); // replaces old factory and deletes it
    }

    for (int i = 0; i < this->problem->getF(); i++)
    {
        individual->getFactory(i)->speedDown();
    }
    this->population.push_back(individual);

    // Initialize the of each factory
    for (int f = 0; f < this->problem->getF(); f++)
    {
        individual->getFactory(f)->initializeJobsStartTimes();
    }

    return individual;
}

Individual *NSGAII::randSMinTFT(int seed)
{
    Individual *individual = new Individual(this->problem->getN(), this->problem->getM(), this->problem->getF());
    vector<Job *> jobs(this->problem->getN());
    Xoshiro256plus rand(/*time(NULL) +*/ seed);
    int highestSpeed = this->problem->getAllSpeeds().size() - 1;

    for (int i = 0; i < this->problem->getN(); i++)
    {

        jobs[i] = new Job(i, this->problem->getM());
        vector<int> jobTimeByMachine(this->problem->getM());
        vector<float> jobSpeedByMachine(this->problem->getM());

        for (int j = 0; j < this->problem->getM(); j++)
        {
            int randomIndex = rand.next() % this->problem->getAllSpeeds().size();
            jobTimeByMachine[j] = this->problem->getAllT()[i][j];
            jobSpeedByMachine[j] = this->problem->getAllSpeeds()[randomIndex];
        }
        jobs[i]->setT(jobTimeByMachine);
        jobs[i]->setV(jobSpeedByMachine);
    }

    sort(jobs.begin(), jobs.end(),
         compareJobsByTotalProcessingTime); // generate job permutation lambda according to non-descending order of Ti (total job i processing time)

    for (int i = 0; i < this->problem->getF(); i++)
    { // assign the first f jobs to each one of the factories
        // factories[i] = new Factory(i, this->problem->get_m());
        individual->getFactory(i)->addJobAtLastPosition(jobs[i]);
    }

    // remove the assigned jobs from lambda

    Factory *testFactory;
    float testFactoryTFT;
    float tftVariation;
    float previousTFT;
    Factory *minTFTFactory;

    // for each factory f
    for (int j = this->problem->getF(); j < jobs.size(); j++)
    { // test job j at all the possible positions of PI_k (the factory)
        float minIncreaseTFT = INFINITY;
        float minTFTPos = 0;

        for (int f = 0; f < this->problem->getF(); f++)
        {
            testFactory = individual->getFactory(f)->minTFTAfterInsertion(jobs[j]);
            testFactoryTFT = testFactory->getTFT();
            previousTFT = individual->getFactory(f)->getTFT();
            tftVariation = testFactoryTFT - previousTFT;

            if (tftVariation < minIncreaseTFT)
            {
                minIncreaseTFT = tftVariation;
                minTFTFactory = testFactory;
                minTFTPos = f;
            }
            else
            {
                // testFactory->clearJobs();
                delete testFactory;
            }
        }

        // factories[minTFTPos] = minTFTFactory;
        individual->replaceFactory(minTFTPos, minTFTFactory); // replaces old factory and deletes it
    }

    for (int i = 0; i < this->problem->getF(); i++)
    {
        individual->getFactory(i)->speedDown();
    }
    this->population.push_back(individual);

    // Initialize the of each factory
    for (int f = 0; f < this->problem->getF(); f++)
    {
        individual->getFactory(f)->initializeJobsStartTimes();
    }

    return individual;
}

Individual *NSGAII::minSMinTEC()
{
    Individual *individual = new Individual(this->problem->getN(), this->problem->getM(), this->problem->getF());
    vector<Job *> jobs(this->problem->getN());

    // First, all the speeds set to the minimum value
    for (int i = 0; i < this->problem->getN(); i++)
    {
        jobs[i] = new Job(i, this->problem->getM());
        vector<int> jobTimeByMachine(this->problem->getM());
        vector<float> jobSpeedByMachine(this->problem->getM());

        for (int j = 0; j < this->problem->getM(); j++)
        {
            jobTimeByMachine[j] = this->problem->getAllT()[i][j];
            jobSpeedByMachine[j] = this->problem->getAllSpeeds()[0]; // minimum speed
        }
        jobs[i]->setT(jobTimeByMachine);
        jobs[i]->setV(jobSpeedByMachine);
    }

    // Sort the jobs according to non-descending order of total processing time
    sort(jobs.begin(), jobs.end(), compareJobsByTotalProcessingTime);

    // Assign the first f jobs to each one of the factories
    for (int k = 0; k < this->problem->getF(); k++)
    {
        individual->getFactory(k)->addJobAtLastPosition(jobs[k]);
    }

    // Remove the assigned jobs from lambda = access the jobs vector from the f position to the end

    // Insert remaining jobs in the position of the factory that can lead to the minimum increse of total energy consumption
    Factory *testFactory;
    float testFactoryTEC;
    float tecVariation;
    float previousTEC;
    Factory *minTECFactory;

    // For each factory f
    for (int i = this->problem->getF(); i < jobs.size(); i++)
    {
        float minIncreaseTEC = INFINITY;
        float minTECPos = 0;

        // Test job j at all the possible positions of PI_k (the factory)
        for (int k = 0; k < this->problem->getF(); k++)
        {
            testFactory = individual->getFactory(k)->minTECAfterInsertion(jobs[i]);
            testFactoryTEC = testFactory->getTEC();
            previousTEC = individual->getFactory(k)->getTEC();
            tecVariation = testFactoryTEC - previousTEC;

            if (tecVariation < minIncreaseTEC)
            {
                minIncreaseTEC = tecVariation;
                minTECFactory = testFactory;
                minTECPos = k;
            }
            else
            {
                // testFactory->clearJobs();
                delete testFactory;
            }
        }

        // Add the job to the factory that leads to the minimum increase of total energy consumption
        individual->replaceFactory(minTECPos, minTECFactory); // replaces old factory and deletes it
    }

    // Initialize the start_times of each factory and then right shift
    for (int f = 0; f < this->problem->getF(); f++)
    {
        individual->getFactory(f)->initializeJobsStartTimes();
        individual->getFactory(f)->rightShift();
    }

    this->population.push_back(individual);
    return individual;
}

Individual *NSGAII::randSMinTEC(int seed)
{
    Xoshiro256plus rand(/*time(NULL) +*/ seed);

    Individual *individual = new Individual(this->problem->getN(), this->problem->getM(), this->problem->getF());
    vector<Job *> jobs(this->problem->getN());

    // First, all the speeds set to the minimum value
    for (int i = 0; i < this->problem->getN(); i++)
    {
        jobs[i] = new Job(i, this->problem->getM());
        vector<int> jobTimeByMachine(this->problem->getM());
        vector<float> jobSpeedByMachine(this->problem->getM());

        for (int j = 0; j < this->problem->getM(); j++)
        {
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
    for (int k = 0; k < this->problem->getF(); k++)
    {
        individual->getFactory(k)->addJobAtLastPosition(jobs[k]);
    }

    // Remove the assigned jobs from lambda = access the jobs vector from the f position to the end

    // Insert remaining jobs in the position of the factory that can lead to the minimum increse of total energy consumption
    Factory *testFactory;
    float testFactoryTEC;
    float tecVariation;
    float previousTEC;
    Factory *minTECFactory;

    // For each factory f
    for (int i = this->problem->getF(); i < jobs.size(); i++)
    {
        float minIncreaseTEC = INFINITY;
        float minTECPos = 0;

        // Test job j at all the possible positions of PI_k (the factory)
        for (int k = 0; k < this->problem->getF(); k++)
        {
            testFactory = individual->getFactory(k)->minTECAfterInsertion(jobs[i]);
            testFactoryTEC = testFactory->getTEC();
            previousTEC = individual->getFactory(k)->getTEC();
            tecVariation = testFactoryTEC - previousTEC;

            if (tecVariation < minIncreaseTEC)
            {
                minIncreaseTEC = tecVariation;
                minTECFactory = testFactory;
                minTECPos = k;
            }
            else
            {
                // testFactory->clearJobs();
                delete testFactory;
            }
        }

        // Add the job to the factory that leads to the minimum increase of total energy consumption
        individual->replaceFactory(minTECPos, minTECFactory); // replaces old factory and deletes it
    }

    // Initialize the start_times of each factory and then right shift
    for (int f = 0; f < this->problem->getF(); f++)
    {
        individual->getFactory(f)->initializeJobsStartTimes();
        individual->getFactory(f)->rightShift();
    }

    this->population.push_back(individual);
    return individual;
}

void NSGAII::balancedRandomIndividualGenerator(int s)
{
    Xoshiro256plus rand(/*time(NULL) +*/ s);

    Individual *sol = new Individual(this->problem->getN(), this->problem->getM(), this->problem->getF());

    // Vector that indicates if a job (id = index) has already been allocated
    vector<bool> job_allocated(this->problem->getN(), false);

    vector<int> jobs_to_allocate;
    for (size_t i = 0; i < this->problem->getN(); i++)
    {
        jobs_to_allocate.push_back(i);
    }

    // Allocate the jobs equally to the factories
    int f_id = 0;
    while (jobs_to_allocate.size())
    {
        int random_num = rand.next() % jobs_to_allocate.size();
        Job *job = new Job(jobs_to_allocate[random_num], this->problem->getM());
        job->setT(this->problem->getAllT()[jobs_to_allocate[random_num]]);

        // Set a random speed for each machine
        for (int j = 0; j < this->problem->getM(); j++)
        {
            int random_num = rand.next() % this->problem->getAllSpeeds().size();
            sol->setV(job->getId(), j, this->problem->getAllSpeeds()[random_num]);
            job->setVForMachine(j, this->problem->getAllSpeeds()[random_num]);
        }

        // Select the factory
        if (f_id == this->problem->getF())
        {
            f_id = 0;
        }

        // Choose a random position to allocate the job
        int random_position = rand.next() % (sol->getFactory(f_id)->getJobs().size() + 1);

        // Allocate the job
        sol->getFactory(f_id)->addJobAtPosition(job, random_position);

        // Erase the allocated job from the list
        jobs_to_allocate.erase(jobs_to_allocate.begin() + random_num);
        f_id++;

        // Time control
        // gettimeofday(&this->end, 0);
        // long seconds = this->end.tv_sec - this->begin.tv_sec;
        // if (seconds > this->problem->getN()/2)
        //    return;
    }

    // Add individual to population
    this->population.push_back(sol);

    // Initialize the of each factory
    for (int f = 0; f < this->problem->getF(); f++)
    {
        sol->getFactory(f)->initializeJobsStartTimes();
    }
}

void NSGAII::totalRandomIndividualGenerator(int s)
{
    Xoshiro256plus rand(/*time(NULL) +*/ s);

    Individual *sol = new Individual(this->problem->getN(), this->problem->getM(), this->problem->getF());

    // Vector that indicates if a job (id = index) has already been allocated
    vector<bool> job_allocated(this->problem->getN(), false);

    vector<int> jobs_to_allocate;
    for (size_t i = 0; i < this->problem->getN(); i++)
    {
        jobs_to_allocate.push_back(i);
    }

    // Allocate one job in each factory
    for (int i = 0; i < this->problem->getF(); i++)
    {
        int random_num = rand.next() % jobs_to_allocate.size();
        Job *job = new Job(jobs_to_allocate[random_num], this->problem->getM());
        job->setT(this->problem->getAllT()[jobs_to_allocate[random_num]]);

        // Set a random speed for each machine
        for (int j = 0; j < this->problem->getM(); j++)
        {
            int random_num = rand.next() % this->problem->getAllSpeeds().size();
            sol->setV(job->getId(), j, this->problem->getAllSpeeds()[random_num]);
            job->setVForMachine(j, this->problem->getAllSpeeds()[random_num]);
        }
        sol->getFactory(i)->addJobAtLastPosition(job);
        jobs_to_allocate.erase(jobs_to_allocate.begin() + random_num);
    }

    // Allocate the remaining jobs randomly
    while (jobs_to_allocate.size())
    {
        int random_num = rand.next() % jobs_to_allocate.size();
        Job *job = new Job(jobs_to_allocate[random_num], this->problem->getM());
        job->setT(this->problem->getAllT()[jobs_to_allocate[random_num]]);

        // Set a random speed for each machine
        for (int j = 0; j < this->problem->getM(); j++)
        {
            int random_num = rand.next() % this->problem->getAllSpeeds().size();
            sol->setV(job->getId(), j, this->problem->getAllSpeeds()[random_num]);
            job->setVForMachine(j, this->problem->getAllSpeeds()[random_num]);
        }

        // Choose a random factory to allocate the job
        int f_id = rand.next() % this->problem->getF();

        // Choose a random position to allocate the job
        int random_position = rand.next() % (sol->getFactory(f_id)->getJobs().size() + 1);

        // Allocate the job
        sol->getFactory(f_id)->addJobAtPosition(job, random_position);

        // Erase the allocated job from the list
        jobs_to_allocate.erase(jobs_to_allocate.begin() + random_num);
    }

    // Add individual to population
    this->population.push_back(sol);

    // Initialize the of each factory
    for (int f = 0; f < this->problem->getF(); f++)
    {
        sol->getFactory(f)->initializeJobsStartTimes();
    }
}

void NSGAII::printPopulation()
{
    cout << "\nPOPULATION "
         << "(with " << this->population.size() << " individuals)" << endl;

    for (size_t i = 0; i < this->population.size(); i++)
    {
        cout << "\nIndividual " << i << endl;
        this->population[i]->printIndividual();
    }
}

string NSGAII::generatePopulationCSVString()
{
    string str = "id, TFT, TEC, level, cd\n";
    Individual *individual;

    for (size_t i = 0; i < this->population.size(); i++)
    {
        individual = this->population[i];
        str += to_string(i) + "," + to_string(individual->getTFT()) + "," + to_string(individual->getTEC()) + "," + to_string(individual->getDominationRank()) + "," + to_string(individual->getCrowdingDistance()) + "\n";
    }
    return str;
}

vector<vector<Individual *>> NSGAII::fastNonDominatedSort(vector<Individual *> population)
{

    vector<vector<int>> fronts(1);
    vector<vector<int>> dominatedBy(population.size());

    for (int i = 0; i < population.size(); i++)
    {
        population[i]->setDominationCounter(0);
        // population[i]->setDominationRank(-1);

        for (int j = 0; j < population.size(); j++)
        {
            if (population[i]->dominates(population[j]))
            {
                dominatedBy[i].push_back(j);
            }
            else if (population[j]->dominates(population[i]))
            {
                population[i]->incrementDominationCounter(1);
            }
        }

        if (population[i]->getDominationCounter() == 0)
        {
            population[i]->setDominationRank(1);
            fronts[0].push_back(i);
        }
    }

    int i = 0;
    while (!fronts[i].empty())
    {
        vector<int> nextFront;
        for (int j = 0; j < fronts[i].size(); j++)
        {
            int frontSolId = fronts[i][j]; // id (indices) de cada solução na fronteira atual

            for (int k = 0;
                 k < dominatedBy[frontSolId].size(); k++)
            {                                                       // itera por cada solução dominada pela de indice frontSolId
                int dominatedSolIndex = dominatedBy[frontSolId][k]; // id de cada solução dominada por frontSolId

                Individual *s = population[dominatedSolIndex]; // cada solução dominada por frontSolId

                s->incrementDominationCounter(-1);

                if (s->getDominationCounter() == 0)
                {
                    s->setDominationRank(i + 2);
                    nextFront.push_back(dominatedSolIndex);
                }
            }
        }
        i++;
        fronts.push_back(nextFront);
    }

    vector<vector<Individual *>> individualFronts(fronts.size());
    for (int i = 0; i < fronts.size(); i++)
    {
        vector<Individual *> front(fronts[i].size());
        for (int j = 0; j < fronts[i].size(); j++)
        {
            front[j] = population[fronts[i][j]];
        }
        individualFronts[i] = front;
    }

    individualFronts.pop_back();
    // this->dominationFronts = individualFronts;
    return individualFronts;
}

vector<vector<Individual *>> NSGAII::fastNonDominatedSort()
{

    vector<vector<int>> fronts(1);
    vector<vector<int>> dominatedBy(population.size());

    for (int i = 0; i < population.size(); i++)
    {
        population[i]->setDominationCounter(0);
        // population[i]->setDominationRank(-1);

        for (int j = 0; j < population.size(); j++)
        {
            if (population[i]->dominates(population[j]))
            {
                dominatedBy[i].push_back(j);
            }
            else if (population[j]->dominates(population[i]))
            {
                population[i]->incrementDominationCounter(1);
            }
        }

        if (population[i]->getDominationCounter() == 0)
        {
            population[i]->setDominationRank(1);
            fronts[0].push_back(i);
        }
    }

    int i = 0;
    while (!fronts[i].empty())
    {
        vector<int> nextFront;
        for (int j = 0; j < fronts[i].size(); j++)
        {
            int frontSolId = fronts[i][j]; // id (indices) de cada solução na fronteira atual

            for (int k = 0;
                 k < dominatedBy[frontSolId].size(); k++)
            {                                                       // itera por cada solução dominada pela de indice frontSolId
                int dominatedSolIndex = dominatedBy[frontSolId][k]; // id de cada solução dominada por frontSolId

                Individual *s = population[dominatedSolIndex]; // cada solução dominada por frontSolId

                s->incrementDominationCounter(-1);

                if (s->getDominationCounter() == 0)
                {
                    s->setDominationRank(i + 2);
                    nextFront.push_back(dominatedSolIndex);
                }
            }
        }
        i++;
        fronts.push_back(nextFront);
    }

    vector<vector<Individual *>> individualFronts(fronts.size());
    for (int i = 0; i < fronts.size(); i++)
    {
        vector<Individual *> front(fronts[i].size());
        for (int j = 0; j < fronts[i].size(); j++)
        {
            front[j] = population[fronts[i][j]];
        }
        individualFronts[i] = front;
    }

    individualFronts.pop_back();
    this->dominationFronts = individualFronts;
    return individualFronts;
}

void NSGAII::assignCrowdingDistance()
{
    int numObjectives = 2;

    for (int i = 0; i < population.size(); i++)
    {
        population[i]->setCrowdingDistance(0);
    }

    for (int i = 0; i < numObjectives; i++)
    {

        if (i == 0)
        {
            sort(population.begin(), population.end(), compareIndividualsByTFT);
            population[0]->setCrowdingDistance(INFINITY);
            population[population.size() - 1]->setCrowdingDistance(INFINITY);
            float maxTFT = population[population.size() - 1]->getTFT();
            float minTFT = population[0]->getTFT();
            for (int j = 1; j < population.size() - 1; j++)
            {
                float normalizedDistance =
                    (population[j + 1]->getTFT() - population[j - 1]->getTFT()) / (maxTFT - minTFT);
                population[j]->incrementCrowdingDistance(normalizedDistance);
            }
        }
        else
        {
            sort(population.begin(), population.end(), compareIndividualsByTEC);
            population[0]->setCrowdingDistance(INFINITY);
            population[population.size() - 1]->setCrowdingDistance(INFINITY);
            float maxTEC = population[population.size() - 1]->getTEC();
            float minTEC = population[0]->getTEC();

            for (int j = 1; j < population.size() - 1; j++)
            {
                float normalizedDistance =
                    (population[j + 1]->getTEC() - population[j - 1]->getTEC()) / (maxTEC - minTEC);
                population[j]->incrementCrowdingDistance(normalizedDistance);
            }
        }
    }
}

void assignCrowdingDistance(vector<Individual *> population)
{
    int numObjectives = 2;
    int infinity = std::numeric_limits<int>::max();

    for (int i = 0; i < population.size(); i++)
    {
        population[i]->setCrowdingDistance(0);
    }

    for (int i = 0; i < numObjectives; i++)
    {

        if (i == 0)
        {
            sort(population.begin(), population.end(), compareIndividualsByTFT);
            population[0]->setCrowdingDistance(INFINITY);
            population[population.size() - 1]->setCrowdingDistance(INFINITY);
            float maxTFT = population[population.size() - 1]->getTFT();
            float minTFT = population[0]->getTFT();
            for (int j = 1; j < population.size() - 1; j++)
            {
                float normalizedDistance =
                    (population[j + 1]->getTFT() - population[j - 1]->getTFT()) / (maxTFT - minTFT);
                population[j]->incrementCrowdingDistance(normalizedDistance);
            }
        }
        else
        {
            sort(population.begin(), population.end(), compareIndividualsByTEC);
            population[0]->setCrowdingDistance(INFINITY);
            population[population.size() - 1]->setCrowdingDistance(INFINITY);
            float maxTEC = population[population.size() - 1]->getTEC();
            float minTEC = population[0]->getTEC();

            for (int j = 1; j < population.size() - 1; j++)
            {
                float normalizedDistance =
                    (population[j + 1]->getTEC() - population[j - 1]->getTEC()) / (maxTEC - minTEC);
                population[j]->incrementCrowdingDistance(normalizedDistance);
            }
        }
    }
}

bool crowdedCompare(Individual *s1, Individual *s2)
{

    if (s1->getDominationRank() < s2->getDominationRank())
    {
        return true;
    }
    else if (s1->getDominationRank() == s2->getDominationRank())
    {
        if (s1->getCrowdingDistance() > s2->getCrowdingDistance())
        {
            return true;
        }
        return false;
    }

    return false;
}

vector<Individual *> makeNewPop(vector<Individual *> parents, int seed, int n)
{
    vector<Individual *> children;
    Xoshiro256plus rand(seed);

    vector<int> prob{1, 1, 1, 1, 0, 0, 0, 0, 0, 0}; // 1 chance of swap 0 chance of insertion

    for (int i = 0; i < parents.size(); i++)
    {

        Individual *sol = new Individual(parents[i]);

        for (int j = 0; j < n / 4; j++)
        {
            int factory1Id = rand.next() % sol->getNumFactories();
            int factory2Id = rand.next() % sol->getNumFactories();
            Factory *factory1 = sol->getFactory(factory1Id);
            Factory *factory2 = sol->getFactory(factory2Id);
            int job1 = rand.next() % factory1->getNumJobs();
            int job2 = rand.next() % factory2->getNumJobs();

            int choice = rand.next() % prob.size();
            // choice = 1;
            if (choice == 1)
            {
                sol->swap(factory1Id, factory2Id, factory1->getJob(job1), factory2->getJob(job2));
                // factory1->speedUp();
                // factory1->speedDown();
                // factory2->speedUp();
                // factory1->speedDown();
            }
            else
            {
                if (factory1->getNumJobs() - 1 > 0)
                {
                    sol->insert(factory1Id, factory2Id, factory1->getJob(job1), job2);
                }
                else if (factory2->getNumJobs() - 1 > 0)
                {
                    sol->insert(factory2Id, factory1Id, factory2->getJob(job2), job1);
                }
                else
                    sol->swap(factory1Id, factory2Id, factory1->getJob(job1), factory2->getJob(job2));
            }
            // factory1->initializeJobsStartTimes();
            // factory2->initializeJobsStartTimes();
        }

        children.push_back(sol);
    }

    return children;
}

vector<Individual *> makeNewPopV2(vector<Individual *> parents, int seed, int n)
{
    vector<Individual *> children;
    Xoshiro256plus rand(seed);

    vector<int> prob{1, 1, 1, 1, 0, 0, 0, 0, 0, 0}; // 1 chance of swap 0 chance of insertion

    for (int i = 0; i < parents.size(); i++)
    {

        Individual *sol = new Individual(parents[i]);

        for (int j = 0; j < n / 4; j++)
        {
            int factory1Id = rand.next() % sol->getNumFactories();
            int factory2Id = rand.next() % sol->getNumFactories();
            Factory *factory1 = sol->getFactory(factory1Id);
            Factory *factory2 = sol->getFactory(factory2Id);
            int job1 = rand.next() % factory1->getNumJobs();
            int job2 = rand.next() % factory2->getNumJobs();

            int choice = rand.next() % prob.size();
            choice = 1;
            if (choice == 1)
            {
                sol->swap(factory1Id, factory2Id, factory1->getJob(job1), factory2->getJob(job2));
                factory1->speedUp();
                factory1->speedDown();
                factory2->speedUp();
                factory1->speedDown();
            }
            else
            {
                if (factory1->getNumJobs() - 1 > 0)
                {
                    sol->insert(factory1Id, factory2Id, factory1->getJob(job1), job2);
                }
                else if (factory2->getNumJobs() - 1 > 0)
                {
                    sol->insert(factory2Id, factory1Id, factory2->getJob(job2), job1);
                }
                else
                    sol->swap(factory1Id, factory2Id, factory1->getJob(job1), factory2->getJob(job2));
            }
            // factory1->initializeJobsStartTimes();
            // factory2->initializeJobsStartTimes();
        }

        children.push_back(sol);
    }

    return children;
}

vector<Individual *> makeNewPopV3(vector<Individual *> parents, int seed, int n)
{
    vector<Individual *> children;
    Xoshiro256plus rand(seed);

    vector<int> prob{1, 1, 1, 1, 0, 0, 0, 0, 0, 0}; // 1 chance of swap 0 chance of insertion

    for (int i = 0; i < parents.size(); i++)
    {

        Individual *sol = new Individual(parents[i]);

        for (int j = 0; j < n / 4; j++)
        {
            int factory1Id = rand.next() % sol->getNumFactories();
            int factory2Id = rand.next() % sol->getNumFactories();
            Factory *factory1 = sol->getFactory(factory1Id);
            Factory *factory2 = sol->getFactory(factory2Id);
            int job1 = rand.next() % factory1->getNumJobs();
            int job2 = rand.next() % factory2->getNumJobs();

            int choice = rand.next() % prob.size();
            sol->swap(factory1Id, factory2Id, factory1->getJob(job1), factory2->getJob(job2));

            if (prob[choice] == 1)
            {
                factory1->speedUp();
                factory2->speedUp();
            }
            else
            {
                factory1->speedDown();
                factory2->speedDown();
            }
        }

        children.push_back(sol);
    }

    return children;
}

void NSGAII::NSGA2NextGen(int seed)
{
    Xoshiro256plus rand(seed);
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
    for (int i = 0; inserted < n && i < fronts.size() - 1; i++)
    {
        // nextGen.reserve(nextGen.size() + fronts[i].size());
        ::assignCrowdingDistance(fronts[i]);

        if (inserted + fronts[i].size() > n)
        {
            l = i;
            break;
        }

        for (int j = 0; j < fronts[i].size(); j++)
        {
            nextGen.push_back(fronts[i][j]);
            inserted++;
        }
    }

    //::assignCrowdingDistance(fronts.back());
    /*if(nextGen.size() + fronts[l].size() == n){
        nextGen.reserve(fronts[l].size());
        nextGen.insert(nextGen.end(), fronts[l].begin(), fronts[l].end());
    }*/
    if (nextGen.size() < n)
    {
        // nextGen.reserve(n);
        sort(fronts[l].begin(), fronts[l].end(), crowdedCompare);
        for (int i = 0; nextGen.size() < n; i++)
        {
            nextGen.push_back(fronts[l][i]);
        }
    }

    this->population = nextGen;
}

void NSGAII::NSGA2NextGen_operators(int seed)
{
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
    for (int i = 0; inserted < n && i < fronts.size() - 1; i++)
    {
        // nextGen.reserve(nextGen.size() + fronts[i].size());
        ::assignCrowdingDistance(fronts[i]);

        if (inserted + fronts[i].size() > n)
        {
            l = i;
            break;
        }

        for (int j = 0; j < fronts[i].size(); j++)
        {
            nextGen.push_back(fronts[i][j]);
            inserted++;
        }
    }

    //::assignCrowdingDistance(fronts.back());
    /*if(nextGen.size() + fronts[l].size() == n){
        nextGen.reserve(fronts[l].size());
        nextGen.insert(nextGen.end(), fronts[l].begin(), fronts[l].end());
    }*/
    if (nextGen.size() < n)
    {
        // nextGen.reserve(n);
        sort(fronts[l].begin(), fronts[l].end(), crowdedCompare);
        for (int i = 0; nextGen.size() < n; i++)
        {
            nextGen.push_back(fronts[l][i]);
        }
    }

    this->population = nextGen;
}

void NSGAII::NSGA2NextGen_operators_ND(int seed)
{
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
    for (int i = 0; inserted < n && i < fronts.size() - 1; i++)
    {
        // nextGen.reserve(nextGen.size() + fronts[i].size());
        ::assignCrowdingDistance(fronts[i]);

        if (inserted + fronts[i].size() > n)
        {
            l = i;
            break;
        }

        for (int j = 0; j < fronts[i].size(); j++)
        {
            nextGen.push_back(fronts[i][j]);
            inserted++;
        }
    }

    //::assignCrowdingDistance(fronts.back());
    /*if(nextGen.size() + fronts[l].size() == n){
        nextGen.reserve(fronts[l].size());
        nextGen.insert(nextGen.end(), fronts[l].begin(), fronts[l].end());
    }*/
    if (nextGen.size() < n)
    {
        // nextGen.reserve(n);
        sort(fronts[l].begin(), fronts[l].end(), crowdedCompare);
        for (int i = 0; nextGen.size() < n; i++)
        {
            nextGen.push_back(fronts[l][i]);
        }
    }

    this->population = nextGen;
}

void normalize(vector<Individual *> individuals,
               vector<tuple<float, float, int, int>> &refPoints,
               vector<tuple<float, float, Individual *>> &normalizedIndividuals)
{
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

    for (int i = 0; i < individuals.size(); i++)
    {
        float nTFT = (individuals[i]->getTFT() - minTFT) / (maxTFT - minTFT);
        float nTEC = (individuals[i]->getTEC() - minTEC) / (maxTEC - minTEC);
        tuple<float, float, Individual *> ns = make_tuple(nTFT, nTEC, individuals[i]);
        normalizedIndividuals.push_back(ns);

        // TFTs[i] = (individuals[i]->getTFT() - minTFT) / maxTFT;
        // TECs[i] = (individuals[i]->getTEC() - minTEC) / maxTEC;
    }

    for (int i = 0; i < numPoints; i++)
    {
        float x = ((float)1 / (float)numPoints) * (float)i;
        float y = 1 - ((float)1 / (float)numPoints) * (float)i;
        tuple<float, float, int, int> ref = make_tuple(x, y, 0, 0);
        refPoints.push_back(ref);
        // refPointTECs.push_back((1 / numPoints) * i);
        // refPointTFTs.push_back((1 / numPoints) * i);
    }
}

void associate(vector<tuple<float, float, int, int>> &refPoints,
               vector<tuple<float, float, Individual *>> &normSol,
               vector<tuple<Individual *, float, int>> &assocVec)
{

    int numIndividuals = normSol.size();
    int numRefPoints = refPoints.size();

    vector<int> associationVector(numIndividuals, 0);
    for (int i = 0; i < numIndividuals; i++)
    {
        float minDistance = INFINITY;
        int refPointPos;

        for (int j = 0; j < numRefPoints; j++)
        {
            // compute distance of individual from each line
            float a = get<1>(refPoints[j]);
            float b = -get<0>(refPoints[j]);
            float x0 = get<0>(normSol[i]);
            float y0 = get<1>(normSol[i]);
            float ax0 = a * x0;
            float by0 = b * y0;
            float distance = fabsf(ax0 + by0) / sqrtf(powf(a, 2) + powf(b, 2));

            if (distance < minDistance)
            {
                minDistance = distance;
                refPointPos = j;
            }
        }

        tuple<Individual *, float, int> assoc = make_tuple(get<2>(normSol[i]), minDistance, refPointPos);
        assocVec.push_back(assoc);
        // associationVector[i] = refPointPos;

        // assign PI(s) = line w closest from s
        // assign d(s) distance of sol. s to the closest line w
    }
}

bool nicheCompare(vector<tuple<Individual *, float>> &a, vector<tuple<Individual *, float>> &b)
{
    return a.size() < b.size();
}

bool distanceCompare(tuple<Individual *, float> &a, tuple<Individual *, float> &b)
{
    return get<0>(a) < get<0>(b);
}

vector<tuple<Individual *, float>> getIntersection(vector<Individual *> &solV, vector<tuple<Individual *, float>> &niche)
{
    vector<tuple<Individual *, float>> intersection;

    for (Individual *s : solV)
    {
        for (tuple<Individual *, float> n : niche)
        {
            if (s == get<0>(n))
            {
                intersection.push_back(n);
            }
        }
    }

    return intersection;
}

int minDistanceIndex(vector<tuple<Individual *, float>> v)
{

    float minDistance = INFINITY;
    int minDistancePos = 0;
    for (tuple<Individual *, float> t : v)
    {
        if (get<1>(t) < minDistance)
        {
            minDistance = get<1>(t);
        }
    }

    return minDistancePos;
}

vector<int> getLeastNicheCountPoints(vector<tuple<float, float, int, int>> &refPoints)
{
    vector<int> leastNicheCountPoints;
    int leastNicheCount = std::numeric_limits<int>::max();
    for (tuple<float, float, int, int> p : refPoints)
    {
        if (get<2>(p) < leastNicheCount && get<3>(p) != -1)
        {
            leastNicheCount = get<2>(p);
        }
    }

    for (int i = 0; i < refPoints.size(); i++)
    {
        if (get<2>(refPoints[i]) == leastNicheCount && get<3>(refPoints[i]) != -1)
        {
            leastNicheCountPoints.push_back(i);
        }
    }
    return leastNicheCountPoints;
}

int minDistancePointIndex(vector<tuple<Individual *, float, int>> &assoc)
{
    float minDistance = INFINITY;
    int minDistancePoint;

    for (int i = 0; i < assoc.size(); i++)
    {
        tuple<Individual *, float, int> p = assoc[i];
        if (get<1>(p) < minDistance)
        {
            minDistance = get<1>(p);
            minDistancePoint = i;
        }
    }

    return minDistancePoint;
}

void niching(int K, int seed, vector<tuple<float, float, int, int>> &refPoints,
             vector<tuple<Individual *, float, int>> &assoc,
             vector<Individual *> &lastFront,
             vector<Individual *> &selected)
{

    // vector<Individual *> selected(K);
    Xoshiro256plus rand(seed);

    int k = 0;
    while (k < K)
    {
        // conjunto J = indices dos pontos com menor niche count
        vector<int> J = getLeastNicheCountPoints(refPoints);
        // escolhe elemento j aleatório em J
        int j = J[rand.next() % J.size()];

        // conjunto I = elementos de Fl que estão associados a j
        vector<tuple<Individual *, float, int>> I;
        for (tuple<Individual *, float, int> el : assoc)
        {
            if (get<2>(el) == j)
            {
                for (Individual *s : lastFront)
                {
                    if (get<0>(el) == s)
                    {
                        I.push_back(el);
                    }
                }
            }
        }

        // se I não está vazio
        if (!I.empty())
        {
            Individual *s;
            int pos;
            if (get<2>(refPoints[j]) == 0)
            {
                // seleciona elemento de I com o menor distância ao ponto de referência
                pos = minDistancePointIndex(I);
            }
            else
            {
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
        }

        else
        {
            get<3>(refPoints[j]) = -1;
            // refPoints.erase(refPoints.begin()+j);
        }
    }
}

int NSGAII::nMetric()
{
    return this->dominationFronts[0].size();
}

vector<Individual *> NSGAII::getParetoFront()
{
    return this->dominationFronts[0];
}

Individual *NSGAII::INGM(Individual *sol, int seed)
{
    Xoshiro256plus rand(seed);
    Individual *new_sol = new Individual(sol);

    // Randomly choose the objective for optimization
    int random_obj = rand.next() % 2; // 0 = TFT, 1 = TEC

    // Get the factory with the largest TFT or TEC
    float largest = 0;
    int largest_index = -1;

    if (random_obj == 0) // Optimize TFT
    {
        for (int f = 0; f < this->problem->getF(); f++)
        {
            float f_tft = new_sol->getFactory(f)->getTFT();
            if (f_tft > largest)
            {
                largest = f_tft;
                largest_index = f;
            }
        }
    }
    else // Optimize TEC
    {
        for (int f = 0; f < this->problem->getF(); f++)
        {
            float f_tec = new_sol->getFactory(f)->getTEC();
            if (f_tec > largest)
            {
                largest = f_tec;
                largest_index = f;
            }
        }
    }

    vector<Job *> jobs_to_try = new_sol->getFactory(largest_index)->getJobs();
    int largest_f_total_jobs = new_sol->getFactory(largest_index)->getNumJobs();

    while (jobs_to_try.size() > largest_f_total_jobs / 2)
    {
        // Get a random job and extract from the factory
        int random_job_index = rand.next() % jobs_to_try.size();
        Job *job = jobs_to_try[random_job_index];
        jobs_to_try.erase(jobs_to_try.begin() + random_job_index);

        // Change the origin factory
        if (random_obj == 0) // Optimize TFT
        {
            new_sol->getFactory(largest_index)->randSpeedUp(seed);
            new_sol->getFactory(largest_index)->speedUp();
        }
        else // Optimize TEC
        {
            new_sol->getFactory(largest_index)->randSpeedDown(seed);
            new_sol->getFactory(largest_index)->speedDown();
            new_sol->getFactory(largest_index)->rightShift();
        }

        // Try inserting the job to every position of every factory until the individual dominates the original one
        for (int f = 0; f < this->problem->getF(); f++)
        {
            int f_num_of_jobs = new_sol->getFactory(f)->getNumJobs();
            for (int pos = 0; pos < f_num_of_jobs; pos++)
            {
                // Insert the job to the factory
                new_sol->insert(largest_index, f, job, pos);

                // Change the factory
                if (random_obj == 0) // Optimize TFT
                {
                    new_sol->getFactory(f)->randSpeedUp(seed);
                    new_sol->getFactory(f)->speedUp();
                }
                else // Optimize TEC
                {
                    new_sol->getFactory(f)->randSpeedDown(seed);
                    new_sol->getFactory(f)->speedDown();
                    new_sol->getFactory(f)->rightShift();
                }
                if (new_sol->getTFT() < sol->getTFT() &&
                    new_sol->getTEC() < sol->getTEC()) // If new_sol dominates sol
                {
                    return new_sol;
                }
                else if (new_sol->getTFT() < sol->getTFT() ||
                         new_sol->getTEC() < sol->getTEC())
                {

                    // this->updateArchive(new_sol);
                }
                new_sol->insert(f, largest_index, job, random_job_index);
            }
        }
    }
    return nullptr;
}

Individual *NSGAII::SNGM(Individual *sol, int seed)
{
    Xoshiro256plus rand(seed);
    Individual *new_sol = new Individual(sol);

    // Randomly choose the objective for optimization
    int random_obj = rand.next() % 2; // 0 = TFT, 1 = TEC

    // Get the factory with the largest TFT or TEC
    float largest = 0;
    int largest_index = -1;

    if (random_obj == 0) // Optimize TFT
    {
        for (int f = 0; f < this->problem->getF(); f++)
        {
            float f_tft = new_sol->getFactory(f)->getTFT();
            if (f_tft > largest)
            {
                largest = f_tft;
                largest_index = f;
            }
        }
    }
    else // Optimize TEC
    {
        for (int f = 0; f < this->problem->getF(); f++)
        {
            float f_tec = new_sol->getFactory(f)->getTEC();
            if (f_tec > largest)
            {
                largest = f_tec;
                largest_index = f;
            }
        }
    }

    vector<Job *> jobs_to_try = new_sol->getFactory(largest_index)->getJobs();
    int largest_f_total_jobs = new_sol->getFactory(largest_index)->getNumJobs();
    while (jobs_to_try.size() > largest_f_total_jobs / 2)
    {
        // Get a random job
        int random_job_index = rand.next() % jobs_to_try.size();
        Job *job = jobs_to_try[random_job_index];
        jobs_to_try.erase(jobs_to_try.begin() + random_job_index);

        // Try inserting the job to every position of every factory until the individual dominates the original one
        for (int f = 0; f < this->problem->getF(); f++)
        {
            bool tag = false;
            int f_num_of_jobs = new_sol->getFactory(f)->getNumJobs();
            for (int pos = 0; pos < f_num_of_jobs; pos++)
            {
                // Swap the job to the factory
                Job *job2 = new_sol->getFactory(f)->getJobs().at(pos);
                while (job2->getId() == job->getId()) // get a different job to swap
                {
                    // if all possibilities of positions on f have been tried
                    if (pos + 1 == f_num_of_jobs)
                    {
                        tag = true;
                        break;
                    }
                    else
                    {
                        pos++;
                        job2 = new_sol->getFactory(f)->getJobs().at(pos);
                    }
                }
                // Needs to change factory
                if (tag)
                    break;

                new_sol->swap(largest_index, f, job, job2);

                // Change the factories
                if (random_obj == 0) // Optimize TFT
                {
                    new_sol->getFactory(largest_index)->randSpeedUp(seed);
                    new_sol->getFactory(largest_index)->speedUp();

                    new_sol->getFactory(f)->randSpeedUp(seed);
                    new_sol->getFactory(f)->speedUp();
                }
                else // Optimize TEC
                {
                    new_sol->getFactory(largest_index)->randSpeedDown(seed);
                    new_sol->getFactory(largest_index)->speedDown();
                    new_sol->getFactory(largest_index)->rightShift();

                    new_sol->getFactory(f)->randSpeedDown(seed);
                    new_sol->getFactory(f)->speedDown();
                    new_sol->getFactory(f)->rightShift();
                }
                if (new_sol->getTFT() < sol->getTFT() &&
                    new_sol->getTEC() < sol->getTEC()) // If new_sol dominates sol
                {
                    return new_sol;
                }
                else if (new_sol->getTFT() < sol->getTFT() ||
                         new_sol->getTEC() < sol->getTEC())
                {

                    // this->updateArchive(new_sol);
                }
                new_sol->swap(f, largest_index, job, job2);
            }
        }
    }
    return nullptr;
}

Individual *NSGAII::HNGM(Individual *sol, int seed)
{
    Xoshiro256plus rand(time(NULL));

    // Randomly choose INGM or SNGM
    int random_gen = rand.next() % 2; // 0 = INGM, 1 = SNGM

    if (random_gen == 0)
        return this->INGM(sol, seed);
    // else
    return this->SNGM(sol, seed);
}

vector<Individual *> NSGAII::makenewpop_operators(vector<Individual *> parents, int seed)
{
    Xoshiro256plus rand(seed);

    // clear the new_individuals vector
    vector<Individual *> children;
    children.clear();
    Individual *sol_ptr = nullptr;

    // Generate the same number of new individuals as parents size
    // For each individual in parents, generate a neighbour
    int i = 0;
    int cont = 0;
    while (i < parents.size())
    {
        // Randomly choose which operator will be used to generate a neighbour
        int rand_op = rand.next() % 3; // 0 = INGM, 1 = SNGM, 2 = HNGM

        if (rand_op == 0)
        {
            sol_ptr = this->INGM(parents[i], rand.next() % 30000);
        }
        else if (rand_op == 1)
        {
            sol_ptr = this->SNGM(parents[i], rand.next() % 30000);
        }
        else
        {
            sol_ptr = this->HNGM(parents[i], rand.next() % 30000);
        }
        i++;
        if (sol_ptr != nullptr)
        {
            children.push_back(sol_ptr);
        }

        // if(cont == 3*parents.size()){
        //    break;
        //}

        cont++;
    }

    // Return children
    return children;
}

Individual *NSGAII::INGM_ND(Individual *sol, int seed)
{
    Xoshiro256plus rand(seed);
    Individual *new_sol = new Individual(sol);

    // Randomly choose the objective for optimization
    int random_obj = rand.next() % 2; // 0 = TFT, 1 = TEC

    // Get the factory with the largest TFT or TEC
    float largest = 0;
    int largest_index = -1;

    if (random_obj == 0) // Optimize TFT
    {
        for (int f = 0; f < this->problem->getF(); f++)
        {
            float f_tft = new_sol->getFactory(f)->getTFT();
            if (f_tft > largest)
            {
                largest = f_tft;
                largest_index = f;
            }
        }
    }
    else // Optimize TEC
    {
        for (int f = 0; f < this->problem->getF(); f++)
        {
            float f_tec = new_sol->getFactory(f)->getTEC();
            if (f_tec > largest)
            {
                largest = f_tec;
                largest_index = f;
            }
        }
    }

    vector<Job *> jobs_to_try = new_sol->getFactory(largest_index)->getJobs();
    int largest_f_total_jobs = new_sol->getFactory(largest_index)->getNumJobs();

    while (jobs_to_try.size() > largest_f_total_jobs / 2)
    {
        // Get a random job and extract from the factory
        int random_job_index = rand.next() % jobs_to_try.size();
        Job *job = jobs_to_try[random_job_index];
        jobs_to_try.erase(jobs_to_try.begin() + random_job_index);

        // Change the origin factory
        if (random_obj == 0) // Optimize TFT
        {
            new_sol->getFactory(largest_index)->randSpeedUp(seed);
            new_sol->getFactory(largest_index)->speedUp();
        }
        else // Optimize TEC
        {
            new_sol->getFactory(largest_index)->randSpeedDown(seed);
            new_sol->getFactory(largest_index)->speedDown();
            new_sol->getFactory(largest_index)->rightShift();
        }

        // Try inserting the job to every position of every factory until the individual dominates the original one
        for (int f = 0; f < this->problem->getF(); f++)
        {
            int f_num_of_jobs = new_sol->getFactory(f)->getNumJobs();
            for (int pos = 0; pos < f_num_of_jobs; pos++)
            {
                // Insert the job to the factory
                new_sol->insert(largest_index, f, job, pos);

                // Change the factory
                if (random_obj == 0) // Optimize TFT
                {
                    new_sol->getFactory(f)->randSpeedUp(seed);
                    new_sol->getFactory(f)->speedUp();
                }
                else // Optimize TEC
                {
                    new_sol->getFactory(f)->randSpeedDown(seed);
                    new_sol->getFactory(f)->speedDown();
                    new_sol->getFactory(f)->rightShift();
                }
                if (new_sol->getTFT() < sol->getTFT() ||
                    new_sol->getTEC() < sol->getTEC()) // If new_sol is non dominated
                {
                    // this->updateArchive(new_sol);
                    return new_sol;
                }
                new_sol->insert(f, largest_index, job, random_job_index);
            }
        }
    }
    return nullptr;
}

Individual *NSGAII::SNGM_ND(Individual *sol, int seed)
{
    Xoshiro256plus rand(seed);
    Individual *new_sol = new Individual(sol);

    // Randomly choose the objective for optimization
    int random_obj = rand.next() % 2; // 0 = TFT, 1 = TEC

    // Get the factory with the largest TFT or TEC
    float largest = 0;
    int largest_index = -1;

    if (random_obj == 0) // Optimize TFT
    {
        for (int f = 0; f < this->problem->getF(); f++)
        {
            float f_tft = new_sol->getFactory(f)->getTFT();
            if (f_tft > largest)
            {
                largest = f_tft;
                largest_index = f;
            }
        }
    }
    else // Optimize TEC
    {
        for (int f = 0; f < this->problem->getF(); f++)
        {
            float f_tec = new_sol->getFactory(f)->getTEC();
            if (f_tec > largest)
            {
                largest = f_tec;
                largest_index = f;
            }
        }
    }

    vector<Job *> jobs_to_try = new_sol->getFactory(largest_index)->getJobs();
    int largest_f_total_jobs = new_sol->getFactory(largest_index)->getNumJobs();
    while (jobs_to_try.size() > largest_f_total_jobs / 2)
    {
        // Get a random job
        int random_job_index = rand.next() % jobs_to_try.size();
        Job *job = jobs_to_try[random_job_index];
        jobs_to_try.erase(jobs_to_try.begin() + random_job_index);

        // Try inserting the job to every position of every factory until the individual dominates the original one
        for (int f = 0; f < this->problem->getF(); f++)
        {
            bool tag = false;
            int f_num_of_jobs = new_sol->getFactory(f)->getNumJobs();
            for (int pos = 0; pos < f_num_of_jobs; pos++)
            {
                // Swap the job to the factory
                Job *job2 = new_sol->getFactory(f)->getJobs().at(pos);
                while (job2->getId() == job->getId()) // get a different job to swap
                {
                    // if all possibilities of positions on f have been tried
                    if (pos + 1 == f_num_of_jobs)
                    {
                        tag = true;
                        break;
                    }
                    else
                        job2 = new_sol->getFactory(f)->getJobs().at(pos++);
                }
                // Needs to change factory
                if (tag)
                    break;

                new_sol->swap(largest_index, f, job, job2);

                // Change the factories
                if (random_obj == 0) // Optimize TFT
                {
                    new_sol->getFactory(largest_index)->randSpeedUp(seed);
                    new_sol->getFactory(largest_index)->speedUp();

                    new_sol->getFactory(f)->randSpeedUp(seed);
                    new_sol->getFactory(f)->speedUp();
                }
                else // Optimize TEC
                {
                    new_sol->getFactory(largest_index)->randSpeedDown(seed);
                    new_sol->getFactory(largest_index)->speedDown();
                    new_sol->getFactory(largest_index)->rightShift();

                    new_sol->getFactory(f)->randSpeedDown(seed);
                    new_sol->getFactory(f)->speedDown();
                    new_sol->getFactory(f)->rightShift();
                }
                if (new_sol->getTFT() < sol->getTFT() ||
                    new_sol->getTEC() < sol->getTEC()) // If new_sol is non dominated by sol
                {
                    // this->updateArchive(new_sol);
                    return new_sol;
                }
                new_sol->swap(f, largest_index, job, job2);
            }
        }
    }
    return nullptr;
}

Individual *NSGAII::HNGM_ND(Individual *sol, int seed)
{
    Xoshiro256plus rand(time(NULL));

    // Randomly choose INGM or SNGM
    int random_gen = rand.next() % 2; // 0 = INGM, 1 = SNGM

    if (random_gen == 0)
        return this->INGM_ND(sol, seed);
    // else
    return this->SNGM_ND(sol, seed);
}

vector<Individual *> NSGAII::makenewpop_operators_ND(vector<Individual *> parents, int seed)
{
    Xoshiro256plus rand(seed);

    // clear the new_individuals vector
    vector<Individual *> children;
    children.clear();
    Individual *sol_ptr = nullptr;

    // Generate the same number of new individuals as parents size
    // For each individual in parents, generate a neighbour
    int i = 0;
    int cont = 0;
    while (children.size() < parents.size())
    {
        // Randomly choose which operator will be used to generate a neighbour
        int rand_op = rand.next() % 3; // 0 = INGM, 1 = SNGM, 2 = HNGM

        if (rand_op == 0)
        {
            sol_ptr = this->INGM_ND(parents[i], rand.next() % 30000);
        }
        else if (rand_op == 1)
        {
            sol_ptr = this->SNGM_ND(parents[i], rand.next() % 30000);
        }
        else
        {
            sol_ptr = this->HNGM_ND(parents[i], rand.next() % 30000);
        }
        i++;
        if (sol_ptr != nullptr)
        {
            children.push_back(sol_ptr);
            cont = 0;
        }
        if (i == parents.size())
            i = 0;

        if (cont == 10 * parents.size())
        {
            break;
        }
        cont++;
    }

    // Return children
    return children;
}