#include <iostream>
#include <fstream>
#include <cstdlib>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <iomanip>

using namespace std;


struct Departure{
	int depTime;
	string depSeq;
	int idealDwell;
	int maxDwell;
};

struct Params{
	//Minimum time if trains changes of direction
	int revTime;
	//Cost associated with non-satisfied preferred platform assignment
	int platAsgCost;
	//Cost of every second of difference between ideal and actual time in platform
	float dwellCost;
	//Cost of preferred reuse not satisfied in the solution
	int reuseCost;
	//Minimum duration of use of resource
	int minResTime;
	//Maximum duration of use of a platform in absence of an arrival or departure
	int maxDwellTime;
	//Cost associated with any uncovered departure or arrival or unuser train
	int uncovCost;
	//Number of iterations of Tabu Search to perform on Matching stage
	int nIter1;
	//Length of Tabu map on Matching stage
	int tabuLength1;
	//Number of iterations of Tabu Search to perform on Assigning stage
	int nIter2;
	//Length of Tabu map on Assigning stage
	int tabuLength2;
	//Length of trains
	string trLength;
};


//Computes the fitness of the match
int fitness(map<string, string> &Matches, map<string, int> &Trains, map<string, Departure> &Departures,
			map<string, string> &Reuses, Params &params){
	//Fitness due to uncovered departures
	int fit1 = 0;
	//Fitness due to non satisfied reuse
	int fit2 = 0;

	string dep, tr;

	for(auto const &m : Matches){
		dep = m.first;
		tr = m.second;

		//If arrival time is after the departure time it corresponds to a uncovered departure
		if(Trains[tr] >= Departures[dep].depTime){
			fit1 += params.uncovCost;
		}
		//This train has no reuse
		if(Reuses.find(tr)==Reuses.end()) continue;
		else{
			//If reuse non satisfied
			if(Reuses[tr] != dep){
				fit2 += params.reuseCost;
			}
		}
	}
	return fit1+fit2;
}																																	


map<string, string> neighborhood(map<string, string> &Matches, map<string, string> &Tabu, map<string, int> &Trains,
								map<string, Departure> &Departures, map<string, string> &Reuses, Params &params){
	//Neighbor Match
	map<string, string> Neighbor;
	//Temporal variable were generated neighbor matches are stored
	map<string, string> tmpNeighbor;

	int fitNeighbor = 100000000;
	int fitTmpNeighbor;

	//Random generation settings
	random_device rd;
	mt19937 rng(rd());
	uniform_int_distribution<int> uniform(1, Trains.size());

	string dep, tr;
	string depTabu1, trTabu1;
	string depTabu2, trTabu2;
	//type = 1 -> SWAP
	//type = 2 -> INSERT
	int type = 0;
	int tmpType = 0;

	for(auto const &m : Matches){
		dep = m.first;
		tr = m.second;

		//New Neighbor to be generated
		tmpNeighbor = Matches;

		//Generating a random train
		string ntr = "Train"+to_string(uniform(rng));

		//If new train already present in the solution -> SWAP
		bool present = false;
		string ndep; 
		for(auto const &m : Matches){
			if(ntr == m.second){
				ndep = m.first;
				present = true;
				break;
			}
		}
		//verify if tabu movement
		bool isTabu1 = false;
		bool isTabu2 = false;
		if(Tabu.find(dep)!=Tabu.end()){
			isTabu1 = (Tabu[dep] == ntr);
		}
		if(Tabu.find(ndep)!=Tabu.end()){
			isTabu2 = (Tabu[ndep] == tr);
		}

		//If present and not tabu
		if(present && !(isTabu1 || isTabu2)){
			tmpNeighbor[dep] = ntr;
			tmpNeighbor[ndep] = tr;
			tmpType = 1;
			
			//Adding Tabu matches to Tabu map
			//Tabu[dep] = tr;
			//Tabu[ndep] = ntr;
		}

		//Otherwise -> INSERT
		else if(!isTabu1){
			tmpNeighbor[dep] = ntr;
			tmpType = 2;
			
			//Adding Tabu matches to Tabu map
			//Tabu[dep] = tr;
		}

		//Computing the fitness for neighbor match
		fitTmpNeighbor = fitness(tmpNeighbor, Trains, Departures, Reuses, params);

		//Update the best neighbor til now
		if(fitTmpNeighbor < fitNeighbor){
			Neighbor = tmpNeighbor;
			fitNeighbor = fitTmpNeighbor;
			type = tmpType;
			//Store the possibly Tabu match(es)
			if(type==1){
				depTabu1 = dep;
				trTabu1 = tr;
				depTabu2 = ndep;
				trTabu2 = ntr;
			}
			else if(type==2){
				depTabu1 = dep;
				trTabu1 = tr;
			}
		}
	}
	//Add the corresponding Tabu assignments
	if(type==1){
		Tabu[depTabu1] = trTabu1;
		Tabu[depTabu2] = trTabu2;
	}
	else if(type==2){
		Tabu[depTabu1] = trTabu1;
	}
	return Neighbor;
}

map<string, string> matcher(map<string, string> &Matches, map<string, int> &Trains, 
							map<string, Departure> &Departures, map<string, string> &Reuses, 
							Params &params){
	//Copy of initial solution to best solution and current solution
	map<string, string> currentSol = Matches;
	map<string, string> bestSol = Matches;

	//Fitness of solutions
	int fitCurrentSol = fitness(currentSol, Trains, Departures, Reuses, params);
	int fitBestSol = fitness(bestSol, Trains, Departures, Reuses, params);

	//TABU MATCHES
	map<string, string> Tabu;

	int nIter = params.nIter1;
	uint tabuLength = params.tabuLength1;

	for(int i=1; i <= nIter; i++){
		currentSol = neighborhood(currentSol, Tabu, Trains, Departures, Reuses, params);
		fitCurrentSol = fitness(currentSol, Trains, Departures, Reuses, params);

		//Update the new best solution
		if(fitCurrentSol < fitBestSol){
			bestSol = currentSol;
			fitBestSol = fitCurrentSol;
		}

		//Verify the length of the Tabu map
		while(Tabu.size() > tabuLength){
			Tabu.erase(Tabu.begin());
		}
	}
	return bestSol;
}