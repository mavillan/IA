#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>

using namespace std;

//Function to load data to vector of vector structure
//Receives filename and number of clumns of the csv file
vector< vector<string> > loadData(const char* filename, int value) {
    ifstream file(filename);
    vector< vector<string> > data;
    vector<string> row(value);
    string column, tmp;
    int cont = 0;
    if(file.is_open()) 
    {
        while (getline(file, column, ';')) 
        {
            if(cont == 0) { column.erase(0, 1); }
            row.push_back(column);
            cont++;
            if(cont == value) 
            {
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
    
	string instancePath = argv[1];

	vector< vector<string> > arrivalsData;
	arrivalsData = loadData((instancePath + "arrivals").c_str(), 11);
	cout<< 

 //    vector< vector<string> > arrDepSequencesData, arrivalsData, compCatDepData, compCatResData, departuresData, facilitiesData, gatesData, 
 //    imposedConsumptionsData, initialTrainsData, jointArrDepData, parametersData, platformsData, prefPlatData, reusesData, singleTracksData, 
 //    trackGroupsData, trainCategoriesData, yardsData;

	// arrDepSequencesData = loadData((instance + "/arrDepSequences.csv").c_str(), NUMCOLARRDEPSEQ);
 //    arrivalsData = loadData((instance + "/arrivals.csv").c_str(), NUMCOLARRIVALS);
	// compCatDepData = loadData((instance + "/compCatDep.csv").c_str(), NUMCOLCOMPCATDEP);
	// compCatResData = loadData((instance + "/compCatRes.csv").c_str(), NUMCOLCOMPCATRES);
	// departuresData = loadData((instance + "/departures.csv").c_str(), NUMCOLDEPARTURES);
	// facilitiesData = loadData((instance + "/facilities.csv").c_str(), NUMCOLFACILITIES);
	// gatesData = loadData((instance + "/gates.csv").c_str(), NUMCOLGATES);
	// imposedConsumptionsData = loadData((instance + "/imposedConsumptions.csv").c_str(), NUMCOLIMPOSEDCONS);
	// initialTrainsData = loadData((instance + "/initialTrains.csv").c_str(), NUMCOLINITIALTRAINS);
	// jointArrDepData = loadData((instance + "/jointArrDep.csv").c_str(), NUMCOLJOINT);
	// parametersData = loadData((instance + "/parameters.csv").c_str(), NUMCOLPARAMETERS);
	// platformsData = loadData((instance + "/platforms.csv").c_str(), NUMCOLPLATFORMS);
	// prefPlatData = loadData((instance + "/prefPlat.csv").c_str(), NUMCOLPREFPLAT);
	// reusesData = loadData((instance + "/reuses.csv").c_str(), NUMCOLREUSES);
	//singleTracksData = loadData((instance + "/singleTracks.csv").c_str(), NUMCOLSINGLETRACKS);
	//trackGroupsData = loadData((instance + "/trackGroups.csv").c_str(), NUMCOLTRACKGROUPS);
	//trainCategoriesData = loadData((instance + "/trainCategories.csv").c_str(), NUMCOLTRAINCATEGORIES);
	//yardsData = loadData((instance + "/yards.csv").c_str(), NUMCOLYARDS);	

	return 0;
}