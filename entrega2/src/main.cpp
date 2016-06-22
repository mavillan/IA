#include <iostream>
#include <fstream>
#include <cstdlib>
#include <map>
#include <vector>
#include <string>
#include <algorithm>

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
static const int NUMCOLIMPOSEDCONS = 7;
static const int NUMCOLGATES = 6;


//////////////////// HELPER FUNCTIONS ////////////////////

//time transformation from "hh:mm:ss" to "secs"
int ttf(string time, bool hasD){
	int h, m , s, secs;
	if(hasD){
		if (sscanf(time.c_str(), "%*s %d:%d:%d", &h, &m, &s) >= 2){
			secs = h*3600 + m*60 + s;
		}
	}
	else{
		if (sscanf(time.c_str(), "%d:%d:%d", &h, &m, &s) >= 2){
			secs = h*3600 + m*60 + s;
		}
	}
	return secs;
}

//inverse time transformation from "secs" to "hh:mm:ss"
string ittf(int secs, bool hasD){
	return NULL;
}

//verify if "x" is contained in "list"
bool contains(const vector<int> list, int x){
	return find(list.begin(), list.end(), x) != list.end();
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

struct InitialTrain{
	string resource;
};

struct Neighbor{
	int indexGate;
	string nResource;
	int nindexGate;
};

struct Sched{
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
    //LOADING DATA
	string instancePath = argv[1];

	vector< vector<string> > arrivalsData, departuresData, arrDepSequencesData, parametersData, 
	platformsData, prefPlatData, yardsData, trackGroupsData, singleTracksData, reusesData, 
	initialTrainsData, imposedConsumptionsData, gatesData;

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
	imposedConsumptionsData = loadData((instancePath + "/imposedConsumptions.csv").c_str(), NUMCOLIMPOSEDCONS);
	gatesData = loadData((instancePath + "/gates.csv").c_str(), NUMCOLGATES);

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

	//PARAMETERS
	map<string, float> Params;
	Params["platAsgCost"] = stof(parametersData.at(9).at(1));
	Params["dwellCost"] = stof(parametersData.at(10).at(1));
	Params["reuseCost"] = stof(parametersData.at(11).at(1));
	Params["minResTime"] = ttf(parametersData.at(12).at(1), false);
	Params["maxDwellTime"] = ttf(parametersData.at(13).at(1), false);
	Params["uncovCost"] = stof(parametersData.at(16).at(1));

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
		string key = reusesData.at(i).at(0);
		Reuses[key] = reusesData.at(i).at(1);
	}

	//INITIAL TRAINS
	map<string, InitialTrain> InitialTrains;
	for(uint i=1; i < initialTrainsData.size(); i++){
		string key = initialTrainsData.at(i).at(0);
		InitialTrain it;
		it.resource = initialTrainsData.at(i).at(2);
		InitialTrains[key] = it;
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
		ng.nResource = gatesData.at(i).at(3);
		ng.nindexGate = stoi(gatesData.at(i).at(5));
		Neighbors[key].push_back(ng);
	}

	//MAKE TRAINS DICTIONARY

	cout << Consumptions["TrackGroup1"].at(0).exitTime << endl;
	return 0;
}