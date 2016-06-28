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

//Needed structs
struct Arrival{
	string idTrain;
	int arrTime;
	string arrSeq;
	int idealDwell;
	int  maxDwell;
};

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


//verify if string "x" is contained in "list"
bool contains(const vector<string> list, string x){
	return find(list.begin(), list.end(), x) != list.end();
}

//Computes the cost due to non satisfied preferences and excess of capacity on platforms
int cost(map<string, string> &Assignments, map<string, Arrival> &Arrivals, map<string, Departure> &Departures,
		map<string, int> &Platforms, map<string, vector<string> > &Prefered, Params &params){
	//Cost due to non satisfied preferences on platforms
	int cost1 = 0;
	//Cost due to non satisfied capacity limits of platforms
	//int cost2 = 0;
	//Associated costs
	int platAsgCost = params.platAsgCost;
	//int platExcCost = 5*platAsgCost;

	string aod, plat;

	for(auto const &a: Assignments){
		aod = a.first;
		plat = a.second;
		//if assigned platform is not in prefered
		if(!contains(Prefered[aod], plat)){
			cost1 += platAsgCost;
		}
	}

	return cost1;
}


map<string, string> neighborhood(map<string, string> &Assignments, map<string, string> &Tabu,
								map<string, Arrival> &Arrivals, map<string, Departure> &Departures, 
								map<string, int> &Platforms, map<string, vector<string> > &Prefered, 
								Params &params){
	//Neighbor assignment
	map<string, string> Neighbor;
	//Temporal variable were generated neighbor matches are stored
	map<string, string> tmpNeighbor;

	int costNeighbor = 100000000;
	int costTmpNeighbor;

	//Random generation settings
	random_device rd;
	mt19937 rng(rd());
	uniform_int_distribution<int> uniform(1, Platforms.size());

	string aod, plat;
	string aodTabu, platTabu;

	for(auto const &a : Assignments){
		aod = a.first;
		plat = a.second;

		//New Neighbor to be generated
		tmpNeighbor = Assignments;

		//Generating a random platform
		string nplat = "Platform"+to_string(uniform(rng));

		//If Tabu, then continue		
		if(Tabu[aod]==nplat) continue;
		//INSERT MOVEMENT
		tmpNeighbor[aod] = nplat;

		//Computing the cost of neighbor assignment
		costTmpNeighbor = cost(tmpNeighbor, Arrivals, Departures, Platforms, Prefered, params);

		//Update the best neighbor til now
		if(costTmpNeighbor < costNeighbor){
			Neighbor = tmpNeighbor;
			costNeighbor = costTmpNeighbor;
			aodTabu = aod;
			platTabu = plat;
		}
	}
	//Add the TABU assignments
	Tabu[aodTabu] = platTabu;
 	return Neighbor;
}


map<string, string> assigner(map<string, string> &Assignments, map<string, Arrival> &Arrivals,
							map<string, Departure> &Departures, map<string, int> &Platforms,
							map<string, vector<string> > &Prefered, Params &params){
	//Copy of initial solution to best solution and current solution
	map<string, string> currentSol = Assignments;
	map<string, string> bestSol = Assignments;	

	//Cost of solutions
	int costCurrentSol = cost(currentSol, Arrivals, Departures, Platforms, Prefered, params);
	int costBestSol = cost(bestSol, Arrivals, Departures, Platforms, Prefered, params);

	//TABU ASSIGNMENTS
	map<string, string> Tabu;

	int nIter = params.nIter2;
	uint tabuLength = params.tabuLength2;

	for(int i=1; i <= nIter; i++){
		currentSol = neighborhood(currentSol, Tabu, Arrivals, Departures, Platforms, Prefered, params);
		costCurrentSol = cost(currentSol, Arrivals, Departures, Platforms, Prefered, params);

		//Update the new best solution
		if(costCurrentSol < costBestSol){
			bestSol = currentSol;
			costBestSol = costCurrentSol;
		}

		//Verify the length of the Tabu map
		while(Tabu.size() > tabuLength){
			Tabu.erase(Tabu.begin());
		}
	}
	return bestSol;
}