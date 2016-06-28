#include <iostream>
#include <fstream>
#include <cstdlib>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <iomanip>

#include "matcher.h"
#include "assigner.h"

using namespace std;

static const int NUMCOLARRIVALS = 11;
static const int NUMCOLDEPARTURES = 8;
static const int NUMCOLARRDEPSEQ = 3;
static const int NUMCOLPARAMETERS = 2;
static const int NUMCOLPLATFORMS = 2;
static const int NUMCOLPREFPLAT = 2;
static const int NUMCOLYARDS = 2;
static const int NUMCOLTRACKGROUPS = 3;
static const int NUMCOLSINGLETRACKS = 3;
static const int NUMCOLREUSES = 2;
static const int NUMCOLINITIALTRAINS = 5;
static const int NUMCOLTRAINCATEGORIES = 7;
static const int NUMCOLIMPOSEDCONS = 7;
static const int NUMCOLGATES = 6;


//////////////////// HELPER FUNCTIONS ////////////////////

//time transformation from "hh:mm:ss" to "secs"
int ttf(string time, bool hasD){
	int h, m , s, secs;
	if(hasD){
		if (sscanf(time.c_str(), "%*s %d:%d:%d", &h, &m, &s) >= 2){
			secs = h*3600 + m*60 + s;
			return secs;
		}
	}
	else{
		if (sscanf(time.c_str(), "%d:%d:%d", &h, &m, &s) >= 2){
			secs = h*3600 + m*60 + s;
			return secs;
		}
	}
	return -1;
}

//inverse time transformation from "secs" to "hh:mm:ss"
string ittf(int secs, bool hasD){
	return NULL;
}

//verify if "x" is contained in "list"
bool contains(const vector<int> list, int x){
	return find(list.begin(), list.end(), x) != list.end();
}


//Writes "Matches" results on "filename" file
void Logger(map<string, string> &Matches, int minObjFunc, string filename){
	string dep, tr;
	ofstream file;
	file.open(filename,fstream::out);
	for(auto const &m : Matches){
		dep = m.first;
		tr = m.second;
		file << setw(8) << left << dep << tr << endl;
	}
	file << endl;
	file << "Minimum Objective Func: " << minObjFunc << endl;
	file.close();
}

//////////////////// ENF OF HELPER FUNCTIONS ////////////////////


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

struct SingleTrack{
	int lenght;
	int capa;
};

struct TrackGroup{
	int trTime;
	int hwTime;
};

struct Consumption{
	int type;
	int beg;
	int end;
	string originGate;
	string destinationGate;
	int entranceTime;
	int exitTime;	
};

struct Neighbor{
	int indexGate;
	string nresource;
	int nindexGate;
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



//Function to load data to vector of vector structure
//Receives filename and number of columns of the csv file
vector< vector<string> > loadData(const char* filename, int value) {
    ifstream file(filename);
    vector< vector<string> > data;
    vector<string> row(value);
    string column, tmp;
    int cont = 0;
    if(file.is_open()){
        while (getline(file, column, ';')){
            if(cont == 0) { column.erase(0, 1); }
            row.push_back(column);
            cont++;
            if(cont == value){
                data.push_back(row);
                row.clear();
                cont = 0;
            }
        }
        file.close();
    }
    return data;
}


int main(int argc, char *argv[]) 
{
    //PARSING ARGUMENTS
	string instancePath = "../instances/"+(string)argv[1]+"/";
	int nIter1, tabuLength1;
	int nIter2, tabuLength2;
	nIter1 = atoi(argv[2]);
	tabuLength1 = atoi(argv[3]);
	nIter2 = atoi(argv[4]);
	tabuLength2 = atoi(argv[5]);

	
	//LOADING DATA
	vector< vector<string> > arrivalsData, departuresData, arrDepSequencesData, parametersData, 
	platformsData, prefPlatData, yardsData, trackGroupsData, singleTracksData, reusesData, 
	initialTrainsData, trainCategoriesData, imposedConsumptionsData, gatesData;

	arrivalsData = loadData((instancePath + "arrivals.csv").c_str(), NUMCOLARRIVALS);
	departuresData = loadData((instancePath + "departures.csv").c_str(), NUMCOLDEPARTURES);
	arrDepSequencesData = loadData((instancePath + "arrDepSequences.csv").c_str(), NUMCOLARRDEPSEQ);
	parametersData = loadData((instancePath + "parameters.csv").c_str(), NUMCOLPARAMETERS);
	platformsData = loadData((instancePath + "/platforms.csv").c_str(), NUMCOLPLATFORMS);
	prefPlatData = loadData((instancePath + "/prefPlat.csv").c_str(), NUMCOLPREFPLAT);
	yardsData = loadData((instancePath + "/yards.csv").c_str(), NUMCOLYARDS);
	trackGroupsData = loadData((instancePath + "/trackGroups.csv").c_str(), NUMCOLTRACKGROUPS);
	singleTracksData = loadData((instancePath + "/singleTracks.csv").c_str(), NUMCOLSINGLETRACKS);
	reusesData = loadData((instancePath + "/reuses.csv").c_str(), NUMCOLREUSES);
	initialTrainsData = loadData((instancePath + "/initialTrains.csv").c_str(), NUMCOLINITIALTRAINS);
	trainCategoriesData = loadData((instancePath + "/trainCategories.csv").c_str(), NUMCOLTRAINCATEGORIES);
	imposedConsumptionsData = loadData((instancePath + "/imposedConsumptions.csv").c_str(), NUMCOLIMPOSEDCONS);
	gatesData = loadData((instancePath + "/gates.csv").c_str(), NUMCOLGATES);

	//PARAMETERS
	Params params;
	//Minimum time if trains changes of direction
	params.revTime = ttf(parametersData.at(3).at(1), false);
	//Cost associated with non-satisfied preferred platform assignment
	params.platAsgCost = stoi(parametersData.at(9).at(1));
	//Cost of every second of difference between ideal and actual time in platform
	params.dwellCost = stof(parametersData.at(10).at(1));
	//Cost of preferred reuse not satisfied in the solution
	params.reuseCost = stoi(parametersData.at(11).at(1));
	//Minimum duration of use of resource
	params.minResTime = ttf(parametersData.at(12).at(1), false);
	//Maximum duration of use of a platform in absence of an arrival or departure
	params.maxDwellTime = ttf(parametersData.at(13).at(1), false);
	//Cost associated with any uncovered departure or arrival or unuser train
	params.uncovCost = stoi(parametersData.at(16).at(1));
	//Length of trains
	params.trLength = trainCategoriesData.at(1).at(1);
	//Number of iterations of Tabu Search to perform on Matching stage
	params.nIter1 = nIter1;
	//Length of Tabu map on Matching stage
	params.tabuLength1 = tabuLength1;
	//Number of iterations of Tabu Search to perform on Assigning stage
	params.nIter2 = nIter2;
	//Length of Tabu map on Assigning stage
	params.tabuLength2 = tabuLength2;



	//ARRIVALS
	map<string, Arrival> Arrivals;
	for(uint i=1; i < arrivalsData.size(); i++){
		string key = arrivalsData.at(i).at(0);
		Arrival arr;
		arr.idTrain = arrivalsData.at(i).at(1);
		arr.arrTime = ttf(arrivalsData.at(i).at(2), true);
		arr.arrSeq = arrivalsData.at(i).at(3);
		arr.idealDwell = ttf(arrivalsData.at(i).at(7), false);
		arr.maxDwell = ttf(arrivalsData.at(i).at(8), false);
		Arrivals[key] = arr;
	}

	//DEPARTURES
	map<string, Departure> Departures;
	for(uint i=1; i < departuresData.size(); i++){
		string key = departuresData.at(i).at(0);
		Departure dep;
		dep.depTime = ttf(departuresData.at(i).at(1), true);
		dep.depSeq = departuresData.at(i).at(2);
		dep.idealDwell = ttf(departuresData.at(i).at(4), false);
		dep.maxDwell = ttf(departuresData.at(i).at(5), false);
		Departures[key] = dep;
	}

	//ARRIVAL AND DEPARTURE SEQUENCES
	map<string, vector<string> > Sequences;
	for(uint i=1; i < arrDepSequencesData.size(); i+=4){
		string key = arrDepSequencesData.at(i).at(0);
		vector<string> trackGroups(4);
		trackGroups.at(0) = arrDepSequencesData.at(i).at(1);
		trackGroups.at(1) = arrDepSequencesData.at(i+1).at(1);
		trackGroups.at(2) = arrDepSequencesData.at(i+2).at(1);
		trackGroups.at(3) = arrDepSequencesData.at(i+3).at(1);
		Sequences[key] = trackGroups;
	}

	//PLATFORMS
	map<string, int> Platforms;
	for(uint i=1; i < platformsData.size(); i++){
		string key = platformsData.at(i).at(0);
		Platforms[key] = stoi(platformsData.at(i).at(1));
	}

	//PREFERED PLATFORM
	map<string, vector<string> > Prefered;
	for(uint i=1; i < prefPlatData.size(); i++){
		string key = prefPlatData.at(i).at(0);
		if(Prefered.count(key)==0){
			vector<string> platforms;
			Prefered[key] = platforms;
		} 
		Prefered[key].push_back(prefPlatData.at(i).at(1));
	}

	//YARDS
	map<string, int> Yards;
	for(uint i=1; i < yardsData.size(); i++){
		string key = yardsData.at(i).at(0);
		Yards[key] = stoi(yardsData.at(i).at(1));
	}

	//TRACK GROUPS
	map<string, TrackGroup> TrackGroups;
	for(uint i=1; i < trackGroupsData.size(); i++){
		string key = trackGroupsData.at(i).at(0);
		TrackGroup tg;
		tg.trTime = ttf(trackGroupsData.at(i).at(1), false);
		tg.hwTime = ttf(trackGroupsData.at(i).at(2), false);
		TrackGroups[key] = tg;
	}

	//SINGLE TRACKS
	map<string, SingleTrack> SingleTracks;
	for(uint i=1; i < singleTracksData.size(); i++){
		string key = singleTracksData.at(i).at(0);
		SingleTrack st;
		st.lenght = stoi(singleTracksData.at(i).at(1));
		st.capa = stoi(singleTracksData.at(i).at(2));
		SingleTracks[key] = st;
	}

	//REUSES
	map<string, string> Reuses;
	for(uint i=1; i < reusesData.size(); i++){
		string key = Arrivals[reusesData.at(i).at(0)].idTrain;
		Reuses[key] = reusesData.at(i).at(1);
	}

	//INITIAL TRAINS
	map<string, string> InitialTrains;
	for(uint i=1; i < initialTrainsData.size(); i++){
		string key = initialTrainsData.at(i).at(0);
		InitialTrains[key] = initialTrainsData.at(i).at(2);
	}

	//IMPOSED CONSUMPTIONS
	map<string, vector<Consumption> > Consumptions;
	for(uint i=1; i < imposedConsumptionsData.size(); i++){
		string key = imposedConsumptionsData.at(i).at(0);
		if(Consumptions.count(key)==0){
			vector<Consumption> cons;
			Consumptions[key] = cons;
		} 
		Consumption cp;
		if(key.substr(0,5)!="Track"){
			cp.type = 1;
			cp.beg = ttf(imposedConsumptionsData.at(i).at(1), true);
			cp.end = ttf(imposedConsumptionsData.at(i).at(2), true);
		}
		else{
			cp.type = 2;
			cp.originGate = imposedConsumptionsData.at(i).at(4);
			cp.destinationGate = imposedConsumptionsData.at(i).at(5);
			cp.entranceTime = ttf(imposedConsumptionsData.at(i).at(6), true);
			cp.exitTime = cp.entranceTime + TrackGroups[key].trTime;
		}
		Consumptions[key].push_back(cp);
	}

	//GATES
	map<string, vector<Neighbor> > Neighbors;
	for(uint i=1; i < gatesData.size(); i+=2){
		string key = gatesData.at(i).at(0);
		if(Neighbors.count(key)==0){
			vector<Neighbor> neigh;
			Neighbors[key] = neigh;
		}
		Neighbor ng;
		ng.indexGate = stoi(gatesData.at(i).at(2));
		ng.nresource = gatesData.at(i).at(3);
		if(ng.nresource.length()==0){
			//Enf Of System
			ng.nresource = "EOS";
			ng.nindexGate = 0;
		}
		else{
			ng.nindexGate = stoi(gatesData.at(i).at(5));
		}
		Neighbors[key].push_back(ng);
	}

	//TRAINS DICTIONARY
	map<string, int> Trains;
	for(auto const &arr : Arrivals){
		string key = arr.second.idTrain;
		Trains[key] = arr.second.arrTime;
	}

	for(auto const &tr: InitialTrains){
		string key = tr.first;
		Trains[key] = 0;
	}

	//GENERATING THE INITIAL SOLUTION FOR MATCHINGS
	map<string, string> Matches;
	for(uint i = 1; i <= Departures.size(); i++){
		Matches["Dep"+to_string(i)] = "Train"+to_string(i);
	}

	cout << "[Stage 1] Matching" << endl;
	cout << "Initial Objective Func: ";
	cout << fitness(Matches, Trains, Departures, Reuses, params) << endl;

	//Perform the match stage
	Matches = matcher(Matches, Trains, Departures, Reuses, params);
	cout << "Minimum Objective Func: ";
	int minObjFunc = fitness(Matches, Trains, Departures, Reuses, params);
	cout << minObjFunc << endl;


	//Filtering unfeasible matches and assigning it as "UNMATCHED"
	for(auto const &m : Matches){
		string dep = m.first;
		string tr = m.second;
		if(Trains[tr] >= Departures[dep].depTime){
			Matches[dep] = "UNMATCHED";
		}
	}

	//Write the results of Matches to log file
	string outPath = "./logs/matches"+(string)argv[1];
	Logger(Matches, minObjFunc, outPath);


	//GENERATING THE INITIAL SOLUTION FOR ASSIGNMENTS
	int numPlat = (int)Platforms.size();
	int platInd = -1;
	map<string, string> Assignments;
	for(auto const &m: Matches){
		string dep = m.first;
		string tr = m.second;
		if(tr=="UNMATCHED") continue;
		int ind;
		string arr;
		if(sscanf(tr.c_str(), "%*[^0123456789]%d", &ind)==1); 
		arr = "Arr"+to_string(ind);
		//assignments
		platInd = (platInd+1)%numPlat;
		Assignments[dep] = "Platform"+to_string(platInd+1);
		platInd = (platInd+1)%numPlat;
		Assignments[arr] = "Platform"+to_string(platInd+1);
	}

	cout << endl << "[Stage 2] Assignments" << endl;
	cout << "Initial Objective Func: ";
	cout << cost(Assignments, Arrivals, Departures, Platforms, Prefered, params) << endl;

	//Perform the assignment stage
	Assignments = assigner(Assignments, Arrivals, Departures, Platforms, Prefered, params);
	cout << "Minimum Objective Func: ";
	minObjFunc = cost(Assignments, Arrivals, Departures, Platforms, Prefered, params);
	cout << minObjFunc << endl;

	//Write the results of Assignments to log file
	outPath = "./logs/assignments"+(string)argv[1];
	Logger(Assignments, minObjFunc, outPath);
	return 0;
}