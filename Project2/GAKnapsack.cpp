#include <utility>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <functional>
#include <time.h>
#include <algorithm>
#include <map>
#include <thread>
#include <cstdio>
#include <ctime>
#include <future>
#include <chrono> 

static const int num_threads = 50;
std::default_random_engine generator;


int GenerateRandNumber(double &num, double maxVal)
{
	std::uniform_real_distribution<double> distribution(0.0, maxVal);
	num = distribution(generator);
	return 0;
}

int GenerateRandNumber(int &num, int maxVal, int min = 0)
{
	std::uniform_int_distribution<int> distribution(min, maxVal);
	num = distribution(generator);
	return 0;
}









//randomly generate genome and return the total value and total weight of the genome
//willGenerate is a vector where each index is either 0 if that indexes correpsonding pair is not in the genome
//or 1 if it is not
std::pair<int,int> createGenome(std::vector<int> &willGenerate,  std::vector<int> &weights, std::vector<int> &values, const int &maxValue)
{
	int trackWeight = 0;
	int valueReturn = 0;
	int include = 0;
	for(int j = 0; j < weights.size(); ++j)
	{
		 GenerateRandNumber(include, 1);
		 willGenerate.push_back(include);
		 if(include) //if random number is one then this pair will be included in the population
		 {
			 trackWeight+=weights[j]; //keep track of total weight in knapsack
			 valueReturn+=values[j]; //keep track of total values in knapsack
		 }
	}
	return std::pair<int, int>((trackWeight > maxValue) ? -1*valueReturn : valueReturn, trackWeight); //if weight is greater than max value then make the value negative
}

//recalculate fitness
int recalcFitness(int maxWeight, std::pair<int, int> &fitness, int &weight, int &val, int flipValue) 
{
	//add to genome so increase weight and value
	if(flipValue)
	{
		fitness.first= std::abs(fitness.first);
		fitness.first += val;
		fitness.second += weight;
		fitness.first = (fitness.second > maxWeight) ? fitness.first*-1 : fitness.first;
	}
	else //remove so do opposite
	{
		fitness.first= std::abs(fitness.first);
		fitness.first -= val;
		fitness.second -= weight;
		fitness.first = (fitness.second > maxWeight) ? fitness.first*-1 : fitness.first;
	}
	//std::cout<<fitness.first<<" "<<maxWeight<<std::endl;
	return 0;
}



//Genetic algorithm to determine optimal objects in knapsack
//Runs for less than two minutes
std::string runGeneticAlgo(std::vector<int> wieghts, std::vector<int> values, int maxWeight)
{
	//std::cout<<maxWeight;
	std::clock_t start;
	start = std::clock();  
    double duration;   
	int pop = 0;
	GenerateRandNumber(pop, 1000, 650); //Generate random population number between 650-1000 
	generator.seed(time(NULL));
	int populationSize = values.size();
	std::multimap<std::pair<int, int>, std::vector<int>> population;
	int k = 0;
	int kk = pop;
	for(; k < kk; ++k)
	{
		std::vector<int> a;
	    std::pair<int, int> x = createGenome(a, wieghts, values, maxWeight);
		population.insert(std::pair<std::pair<int, int>, std::vector<int>>(x, a));  
	}

	int oneParent = 0;
	int twoParent = 0;
	int randCrossoverSpot= 0;
	int mutateValue = 0;
	int mutateSpot = 0;
	int iterations = 0; 
	while(true)
	{
		    iterations+=1;
			GenerateRandNumber(oneParent, kk-1); //select first genome for mating
			GenerateRandNumber(twoParent, kk-1); //select second genome for mating
			GenerateRandNumber(randCrossoverSpot, populationSize-1); //randomSpot for crossover
			GenerateRandNumber(mutateValue, 12); //decide rather to randomly mutate if 1 mutate offspring 1
			GenerateRandNumber(mutateSpot, populationSize-1); //randomSpot for crossover
			//if 2 mutate offspring 2, otherwise  dont mutate
			std::vector<int> offspring1, offspring2;
			int count = 0;
			std::pair<int, int> value1, value2; 
		
			std::multimap<std::pair<int, int>, std::vector<int>>::iterator it=population.begin();
			std::advance(it, oneParent);
			offspring1 = it->second;
			value1 = it->first;
			it=population.begin();
			std::advance(it, twoParent);
			offspring2 = it->second;	
			value2= it->first;
			//breed takes place here
			int temp1 = offspring1[randCrossoverSpot];
			int temp2 = offspring2[randCrossoverSpot];
			offspring1[randCrossoverSpot] = temp2;
      		offspring2[randCrossoverSpot] = temp1;
		 
			
			if(temp1 != offspring1[randCrossoverSpot])
			{
				recalcFitness(maxWeight, value1, wieghts[randCrossoverSpot], values[randCrossoverSpot], offspring1[randCrossoverSpot]);
			}
			if(temp2 != offspring2[randCrossoverSpot])
			{
				recalcFitness(maxWeight, value2, wieghts[randCrossoverSpot], values[randCrossoverSpot], offspring2[randCrossoverSpot]);
			}
	
			//mutation takes place here
			if(mutateValue == 1)
			{
				offspring1[mutateSpot] =  (offspring1[mutateSpot]) ? 0 : 1; 
				recalcFitness(maxWeight, value1, wieghts[mutateSpot], values[mutateSpot], offspring1[mutateSpot]);
			}
			if(mutateValue == 2)
			{
				offspring2[mutateSpot] = (offspring2[mutateSpot]) ? 0 : 1; 
				recalcFitness(maxWeight, value2, wieghts[mutateSpot], values[mutateSpot], offspring2[mutateSpot]);
			}
			
			population.insert(std::pair<std::pair<int, int>, std::vector<int>>(value1, offspring1));  
			population.insert(std::pair<std::pair<int, int>, std::vector<int>>(value2, offspring2));
			//kill off members of the population with the lowest fitness
			population.erase(population.begin());
			population.erase(population.begin());
			it = population.begin();
			std::advance(it, kk-1);
		    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
			if(duration > 120 || population.begin()->first == it->first)
			{
				    std::vector<int> finalOk = population.begin()->second;
					int checW, checV;
					checW = checV = 0;
					for(int i = 0; i < finalOk.size(); ++i)
					{
						if(finalOk[i])
						{
							checV += values[i];
							checW += wieghts[i];
						}
						//std::cout<<values[i]<<" "<<finalOk[i]<<std::endl;
					}
					std::string out = "";
					for (k = 0; k < it->second.size(); ++k)
					{
						if(it->second[k])
						out+= std::to_string(k) + ",";
					}
				 //   std::cout<<(it->first.first)<<" "<<(it->first.second)<<" "<<out<<" "<<pop<<std::endl;
					std::string sp = " ";
					std::string o = std::to_string(it->first.first);
					o+= sp + std::to_string(it->first.second);
					o+= sp + out + sp;
					o+= std::to_string(pop) + sp + std::to_string(iterations) +sp;
					o+=std::to_string(checV) + sp + std::to_string(checW);
					return o;
			}
	}
}

/*Read input file with pairs of the form (WEIGHT . VALUE).
ex.
 (23 . 27)
 (47 . 34)
 (22 . 9)
*/
 int getWeightValuePairs(std::string filePath, std::vector<int> &wieghts, std::vector<int> &values)
 {
	std::ifstream in(filePath);
	std::string str;
	while(in>>str)
	{
		if(str[0] == '(')
		{
			wieghts.push_back(std::stoi(str.substr(1, 2)));
		}
		else if(str[0] != '.')
		{
			values.push_back(std::stoi(str.substr(0, 2)));
		}
	}
	return 0;
 }


 //requires command line arg data file path and the second arg max weight value
int main(int argc, char* argv[])
{
	std::string stringWeight = argv[2];
    const int maxWeight = std::stoi(stringWeight);
	//std::string stringIterations = argv[4];
    //const int iterations = std::stoi(stringWeight);
	//std::cout<<maxWeight;
	std::vector<int> wieghts;
	std::vector<int> values;
	getWeightValuePairs(argv[1], wieghts, values);
	
	while(true)
	{
	  std::clock_t start;
	  start = std::clock(); 
	  int duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
	  std::vector<std::future<std::string>> futures;
	  for (int i = 0; i < num_threads; ++i) 
      {
              futures.push_back(std::async( runGeneticAlgo, wieghts, values, maxWeight));
      }
	  for(int i = 0; i < futures.size(); ++i)
		 futures[i].wait();
		 std::fstream fs;
		 std::string out = argv[3];
         fs.open (out, std::fstream::out | std::fstream::app);
		 //fs<<"value, weight, indexes of included objects seperated by commas, total population, total iterations, value check [should match 1st value], value weight [should match 2nd value]\n";
         std::cout<<"new round"<<std::endl;
		 for(auto &e : futures) 
		 {
			fs<<e.get()<<"\n";
	     }
		 fs.close();
	  }
	
	return 0;
}