/*Author: <Taylor Roozen>
Program: <flight.h>
Last Modified: <11/30/2016>

Description:

Description: This is the header file for a flight scheduling class, ATC
See full description in the .cpp file of the same name, along with
descriptions of the class methods.
*/


#include <cstdlib>
#include <string>
#include <vector>

using namespace std;

#ifndef AIR_TRAFFIC_CONTROL
#define AIR_TRAFFIC_CONTROL

class ATC
{

public:

    ATC();
    ~ATC();
    void inputFlight(string info, int lineNumber);
	void processCommands(string command, int lineNumber);
    string generateLine();


private:
    struct flight
    {
        int ID;
        char type;
        string source, dest;
        int hour, minute, T;
        flight* left;
        flight* right;
		flight* reference;
    };

    flight* landings;
    flight* takeoffs;
    flight* flightTimes;
    flight* flightID;

    int nT, nL, nFT, numS;
	int rT, rL, lT, lL;
    
    vector<string> schedule;


	void onDemandAssignment(flight* tree, flight* f, int minTime);
	void printTraverse(flight* tree);
	flight* insert(flight* tree, flight* f);
	void copyFlight(flight* flight1, flight* flight2);
	flight* getFlight(flight* tree, int flightID);
    flight* getArray(char typ, int numFlights);
    string getString(flight* F, int numFlights);
	void remakeSchedule(int typ);
    flight* removeFlight(flight* node, int t);
	flight* successor(flight* node);
    flight* minimum(flight* node);
	flight* maximum(flight* node);
    void deleteTree(flight * tree);
   
};
#endif