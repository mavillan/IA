using namespace std;

struct Arrival;
struct Departure;
struct Params;

bool contains(const vector<string> list, string x);


int cost(map<string, string> &Assignments, map<string, Arrival> &Arrivals, map<string, Departure> &Departures,
		map<string, int> &Platforms, map<string, vector<string> > &Prefered, Params &params);

map<string, string> neighborhood(map<string, string> &Assignments, map<string, string> &Tabu,
								map<string, Arrival> &Arrivals, map<string, Departure> &Departures, 
								map<string, int> &Platforms, map<string, vector<string> > &Prefered, 
								Params &params);

map<string, string> assigner(map<string, string> &Assignments, map<string, Arrival> &Arrivals,
							map<string, Departure> &Departures, map<string, int> &Platforms,
							map<string, vector<string> > &Prefered, Params &params);

