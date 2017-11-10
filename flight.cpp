/*Author: <Taylor Roozen>
Program: <flight.cpp>
Last Modified: <11/30/2016>

Description:

The ATC class takes flight information, schedule requests, and flight
adjustment commands, and uses that information to update the flight
schedule for a runway. 

Each line entered for a new flight should include the flight number,
flight type (either landing(L) or takeoff(T)) departure location, arrival 
locationand flight time. It should be entered in the following format:
<36 L SEA BOS 14:15>.

Each line entered for a schedule request should specify whether the request is 
for the most recent flight (R), or a list of flights (L). Then it should list 
the flight type. It should be entered in the following format: 
<L T>.

Flight adjustments should first list the adjustment type, then follow the
following instructions per adjustment type:

Delay(D) should be followed by the flight number, and the number of minutes
that the flight is delayed by. It should be entered in the following format:
<D 76 15>

Cancellations(C) should be followed by the flight number. It should be entered
in the following format:
<C 92>

Finished(F) should be followed by flight number. It should be entered in the 
following format:
<F 32>

All cancellations and finished flights should be entered before delayed flights
so cancelled flight times can be allocated to delayed flights.

*/

#include "flight.h"
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;

/*
The consructor ATC() allocates the root of the BST of flights,
and the string vector that stores all of the output strings
until main calls generateLine() to get lines for the output file.
*/
ATC::ATC()
{
	flightTimes = new flight;
	flightTimes = NULL;
	
	nFT = 0;
	nT = 0;
	nL = 0;

	rT = -1;
	rL = -1;
	lT = -1;
	lL = -1;

	schedule.reserve(50);
	numS = 0;

}

/*
The deconstructor returns the scheduling tree memory when main goes out of 
scope.
*/
ATC::~ATC()
{
	deleteTree(flightTimes);
	return;
}

/*
generateLine() is a public function that accepts no arguments and returns a 
string. When this function is called, it returns the results of users
shedule requests. Each time it is called it returns one line,
and it returns the scheduled lines in the order that they 
were requested by the user.
*/
string ATC::generateLine()
{
	ostringstream numRequests;
	
	numRequests << schedule.size();

	static int lineNum = -2;
	lineNum++;

	if (lineNum == -1)
		return "There were " + numRequests.str() + " requests:";
	else if (lineNum >= int(schedule.size()))
		return "END";

	return schedule[lineNum];
}

/*
inputFlight() is a public function. It accepts a string and an integer
as arguments. It takes the flight information that is entered, and
assigns creates a new flight from the information. The flight
is then entered into the flight schedule.
*/
void ATC::inputFlight(string info, int lineNumber)
{
	int id, hour, minute;
	char type, colon;
	string source, dest;

	flight* newF = new flight;

	istringstream iss(info);
	iss >> id >> type >> source >> dest >> hour >> colon >> minute;

	if (iss.fail())
	{
		cout << "Flight details on line " << lineNumber << " in wrong format, couldn't add." << endl;
		return;
	}

	newF->ID = id;
	newF->type = toupper(type);
	newF->source = source;
	newF->dest = dest;
	newF->hour = hour;
	newF->minute = minute;
	newF->T = hour * 60 + minute;

	if (newF->type == 'T')
		nT++;
	else
		nL++;

	flightTimes = insert(flightTimes, newF);
}

/*
processCommands() is a public function, it accepts a string and an integer
and does not return anything. 

If the first character of the command line
is a 'C', the flight is removed from the flightTimes BST, and the schedule
is updated to reflect the cancelled flight.

If the first character of the command line is an 'F', the 
flight is removed from the flightTimes BST, and the schedule is updated to
reflect the cancelled flight.

IF the first character enterd is an 'R', the first unused schedule request
element is filled with the most recent flight information for 
the specified type of flight.

If the first charater entered is an 'L', the first nused schedule request 
element is filled with a list of all of the flight information for all 
the flights of the specified type of flight.
*/
void ATC::processCommands(string command, int lineNumber)
{

	char initCommand, flightType, initCh;
	string stringGetter;
	int id, delay;
	flight* affectedFlight, *newF;

	initCh = toupper(command[0]);
	
	if (initCh != 'C' && initCh != 'F' && initCh != 'D' && initCh != 'R' && initCh != 'L')
	{
		cout << "Disallowed command request on line " << lineNumber << " unable to process." << endl;
		return;
	}

	switch (initCh)
	{
		case 'C':
		{
			istringstream iss(command);
			iss >> initCommand >> id;

			if (iss.fail())
			{
				cout << "Disallowed command request on line " << lineNumber << " unable to process." << endl;
				return;
			}

			affectedFlight = getFlight(flightTimes, id);
			if (affectedFlight == NULL)
			{
				cout << "Flight on line " << lineNumber << " does not exist, unable to process." << endl;
				return;
			}

			if (affectedFlight->type == 'T')
			{
				flightTimes = removeFlight(flightTimes, affectedFlight->T);
				nT--;
				remakeSchedule('T');
			}
			else
			{
				flightTimes = removeFlight(flightTimes, affectedFlight->T);
				nL--;
				remakeSchedule('L');
			}
			break;
		}
		case 'F':
		{
			istringstream iss1(command);
			iss1 >> initCommand >> id;

			if (iss1.fail())
			{
				cout << "Disallowed command request on line " << lineNumber << " unable to process." << endl;
				return;
			}


			affectedFlight = getFlight(flightTimes, id);
			if (affectedFlight == NULL)
			{
				cout << "Flight on line " << lineNumber << " does not exist, unable to process." << endl;
				return;
			}

			if (affectedFlight->type == 'T')
			{
				flightTimes = removeFlight(flightTimes, affectedFlight->T);
				nT--;
				remakeSchedule('T');
			}
			else
			{
				flightTimes = removeFlight(flightTimes, affectedFlight->T);
				nL--;
				remakeSchedule('L');
			}
			break;
		}
		case 'D':
		{
			newF = new flight;

			istringstream iss2(command);
			iss2 >> initCommand >> id >> delay;

			if (iss2.fail())
			{
				cout << "Disallowed command request on line " << lineNumber << " unable to process." << endl;
				return;
			}


			affectedFlight = getFlight(flightTimes, id);

			if (affectedFlight == NULL)
			{
				cout << "Flight on line " << lineNumber << " does not exist, unable to process." << endl;
				return;
			}

			copyFlight(affectedFlight, newF);

			flightTimes = removeFlight(flightTimes, affectedFlight->T);

			onDemandAssignment(flightTimes, newF, newF->T + delay);
		
			newF->minute = newF->T % 60;
			newF->hour = (newF->T / 60) % 24;

			flightTimes = insert(flightTimes, newF);

			remakeSchedule(newF->type); //in case a flight is delayed after request for 
	
			break;
		}
		case 'R':
		{
			istringstream iss3(command);
			iss3 >> initCommand >> flightType;
	
			if (iss3.fail())
			{
				cout << "Disallowed command request on line " << lineNumber << " unable to process." << endl;
				return;
			}

			if (flightType == 'T')
			{
				schedule.push_back(getString(getArray('T', min(1,nT)),min(1,nT)));
				rT = numS;
				numS++;
			}
			else
			{
				schedule.push_back(getString(getArray('L',min(1, nL)), min(1,nL)));
				rL = numS;
				numS++;
			}
		
			break;
		}
		case 'L':
		{
		
			istringstream iss4(command);
			iss4 >> initCommand >> flightType;

			if (iss4.fail())
			{
				cout << "Disallowed command request on line " << lineNumber << " unable to process." << endl;
				return;
			}

			if (flightType == 'T')
			{
				schedule.push_back(getString(getArray('T', nT), nT));

				lT = numS;
				numS++;
			}
			else
			{
				schedule.push_back(getString(getArray('L', nL), nL));
				lL = numS;
				numS++;
			}

			break;
		}
	}
}


//PRIVATE FUNCTIONS START HERE

/*
insert() accepts a new flight as an argument, and 
the flightTimes tree. It inserts the new flight into the
flight schedule recursively.
*/
ATC::flight* ATC::insert(flight* tree, flight* newF)
{
	if (tree == NULL)
	{
		newF->left = NULL;
		newF->right = NULL;
		return newF;
	}

	if (newF->T < tree->T)
		tree->left = insert(tree->left, newF);
	else 
		tree->right = insert(tree->right, newF);
	return tree;
}

/*
copyFlight() takes two flights as arguments, and copys the information 
from the first flight into the second flight.
*/
void ATC::copyFlight(flight* flight1, flight* flight2)
{
	flight2->ID = flight1->ID;
	flight2->type = flight1->type;
	flight2->source = flight1->source;
	flight2->dest = flight1->dest;
	flight2->hour = flight1->hour;
	flight2->minute = flight1->minute;
	flight2->T = flight1->T;
}

/*
getFlight() accepts the flightTimes BST and a flight number as arguments.
It does a tree traversal and when it finds the node with the requested
flight number, it returns that node.
*/
ATC::flight* ATC::getFlight(flight* tree, int flightID)
{
	flight* l;
	flight* r;

	if(tree == NULL||tree->ID==flightID)
		return tree;
	l = getFlight(tree->left, flightID);
	r = getFlight(tree->right, flightID);
	if (l != NULL)
		return l;
	else if (r != NULL)
		return r;
	else
		return NULL;
}

/*
onDemandAssignment() accepts the flightTimes BST a flight and the adjusted
minimum time that a delayed flight can takeoff or land based on the delay
request. The tree is searched until the first open time slot with 3 minutes 
of buffer on either side of it is found. It then sets the new takeoff/
landing time of the specified flight to the new time slot.
*/
void ATC::onDemandAssignment(flight* node, flight* f, int minTime)
{
	flight* big;

	while (minTime < node->T && node->left != NULL)
	{
		node = node->left;
	}

	if (node->left == NULL && node->T - 3 >= minTime)
	{
		f->T = minTime;
		return;
	}
	
	big = successor(node);
	
	while (big != NULL)
	{
		if (node->T + 3 < minTime)
		{
			if (big->T - 3 >= minTime)
			{
				f->T = minTime;
				return;
			}
		}
		else if (big->T - 6 >= node->T)
		{
			f->T = node->T + 3;
			return;
		}
		node = big;
		big = successor(node);
	}
	f->T = max(node->T + 3, minTime);
}

/*
getArray takes a char and an integer as arguments, and returns a flight array.
It compiles a list of flights of a given type. The size of the list is 
specified by the integer input. It enters flights from soonest time
to last time.
*/
ATC::flight* ATC::getArray(char typ, int numFlights)
{
	flight* f, *arr;
	int n = 0;

	if (numFlights == 0)
		return NULL;

	arr = new flight[numFlights];

	f = minimum(flightTimes);
	while (n < numFlights)
	{
		if (f->type == typ)
		{
			arr[n] = *f;
			n++;
		}
		f = successor(f);
	}
	return arr;
}

/*
getString() accepts an flight array (from getArray()) and an integer
which represents the number of flights to be added to a schedule line.
It returns a string, which is a line of flight details.
*/
string ATC::getString(flight* fArr, int numFlights)
{
	string returner;
	ostringstream oss;
	if (fArr == NULL)
		return "No flights exist for this request";
	for (int i = 0; i<numFlights; i++)
	{
		oss << fArr[i].ID << " " << fArr[i].type;
		oss << " " << fArr[i].source << " ";
		oss << fArr[i].dest << " " << fArr[i].hour;
		oss << ":" << setw(2) << setfill('0') << fArr[i].minute;

		if (i != numFlights - 1)
			oss << ";";
	}

	delete []fArr;

	returner = oss.str();
	oss.str(string());
	oss.clear();

	return returner;
}

/*
remakeSchedule() accepts a flight type int as an argument.
It enables command requests to be taken before flight inputs.
It removes all of the schedule strings from the vector that they're stored in
up to and including the first instance of a schdule request of the specified type.
*/
void ATC::remakeSchedule(int typ)
{
	string* temp;
	int vecSize = int(schedule.size());
	int recentType, listType, numFlights, j;

	if (typ == 'T')
	{
		recentType = rT;
		listType = lT;
		numFlights = nT;
		temp = new string[vecSize];
	}
	else
	{
		recentType = rL;
		listType = lL;
		numFlights = nL;
		temp = new string[vecSize];
	}

	if (recentType >= 0)
	{
		j = 0;
		for (int i = vecSize - 1; i > recentType; i--)
		{
			temp[j] = schedule.at(i);
			schedule.pop_back();
			j++;
		}

		schedule.pop_back();

		schedule.push_back(getString(getArray(typ, 1), 1));

		for (int i = j-1; i >= 0; i--)
		{
			schedule.push_back(temp[i]);
		}
	}

	if (listType >= 0)
	{
		j = 0;
		for (int i = vecSize - 1; i > listType; i--)
		{
			temp[j] = schedule.at(i);
			schedule.pop_back();
			j++;
		}

		schedule.pop_back();

		schedule.push_back(getString(getArray(typ, numFlights), numFlights));
	
		for (int i = j-1; i >= 0; i--)
		{
			schedule.push_back(temp[i]);
		}
	}
		delete[]temp;
}

/*
deleteTree() does a complete traversal and deletion of the BST. It's
used by the destructor.
*/
void ATC::deleteTree(flight * tree)
{
	if (tree == NULL)
		return;
	deleteTree(tree->left);
	deleteTree(tree->right);
	delete tree;
	return;
}

/*
printTraverse is used for debugging (in case you need it...you shouldn't)
*/
void ATC::printTraverse(flight* tree)
{
	if (tree == NULL)
		return;
	printTraverse(tree->left);
	cout << tree->T << endl;
	printTraverse(tree->right);
	return;
}

/*
removeFlight deletes a flight of specified flight time from the fightTimes
BST.
*/
ATC::flight* ATC::removeFlight(flight* node, int t)
{
	flight* walk;


	if (node == NULL)
		return NULL;

	if (node->T > t)
	{
		node->left = removeFlight(node->left, t);
	}
	else if (node->T < t)
	{
		node->right = removeFlight(node->right, t);
	}
	else
	{
		if (node->left == NULL && node->right == NULL)
		{
			delete node;
			return NULL;
		}
		else if (node->left == NULL)
		{
			walk = node->right;
			delete node;
			return walk;
		}
		else if (node->right == NULL)
		{
		
			walk = node->left;
			delete node;
			return walk;
		}

		walk = successor(node);
		copyFlight(walk, node);
		node->right = removeFlight(node->right, walk->T);
	}
	return node;
}

/*
successor() is a standard BST successor equation. It takes a flight as an 
argument. It finds the node with the flight time following the flight time 
of the specified flight.
*/
ATC::flight* ATC::successor(flight* node)
{
	flight* walk;
	flight* returner;

	if (node->right != NULL)
		return minimum(node->right);

	returner = NULL; 
	walk = flightTimes;

	while (walk != NULL)
	{
		if (walk->T > node->T)
		{
			returner = walk;
			walk = walk->left;
		}
		else if (walk->T < node->T)
		{
			walk = walk->right;
		}
		else
		{
			return returner;
		}
	}
	return returner;
}

/*
minimum() finds the flight with the lowest flight time
in the flightTimes BST.
*/
ATC::flight* ATC::minimum(flight * node)
{
	if (node->left == NULL)
		return node;

	return minimum(node->left);

}

/*
maximum() finds the flight with the highest flight time
in the flightTimes BST.
*/
ATC::flight* ATC::maximum(flight * node)
{
	if (node->right == NULL)
		return node;

	return maximum(node->right);
}