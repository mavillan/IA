using namespace std;

struct Departure;
struct Params;

int fitness(map<string, string> &Matches, map<string, int> &Trains, map<string, Departure> &Departures,
			map<string, string> &Reuses, Params &params);

map<string, string> neighborhood(map<string, string> &Matches, map<string, string> &Tabu, map<string, int> &Trains,
								map<string, Departure> &Departures, map<string, string> &Reuses, Params &params);

map<string, string> matcher(map<string, string> &Matches, map<string, int> &Trains, 
							map<string, Departure> &Departures, map<string, string> &Reuses, 
							Params &params);

