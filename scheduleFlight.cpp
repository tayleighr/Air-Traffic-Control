/*Author: <Taylor Roozen>
Program: <scheduleFlight.cpp>
Last Modified: <11/30/2016>

Description: This program pulls the information from 'flight.in' a file 
containing flight inputs, schedule requests, and flight adjustments. It 
sends that information to the ATC class one line at a time. It then gets 
the updated schduling information from the ATC class, and prints it to the 
output file, flight.out.

*/

#include "flight.h"
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>

using namespace std;

int main()
{
	int numFlights, numCommands, lineIdx;
	string line, newLine;
	ATC flightScheduler;
	bool streaming = true;

    ifstream infile("flight.in");
	ofstream outfile("flight.out");
    
    if(getline(infile, line))
    {
        istringstream iss(line);
        iss >> numFlights >> numCommands;
    }

	lineIdx = 1;
	while (getline(infile, line))
	{
		if (lineIdx <= numFlights)
		{
			flightScheduler.inputFlight(line, lineIdx);
		}
		else
		{
			flightScheduler.processCommands(line, lineIdx);
		}
		lineIdx++;
	}

	while (streaming)
	{
		newLine = flightScheduler.generateLine();
		if (newLine != "END")
			outfile << newLine << '\n';
		else
			streaming = false;
	}

    infile.close();
	outfile.close();

	cout << "Schedule Generated." << endl;
    return 0;
}