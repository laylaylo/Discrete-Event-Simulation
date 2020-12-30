#include <string>

using namespace std;

#ifndef EVENT_H
#define EVENT_H

class Event {

public:
	float time;
	string type;
	int hackerID;
	int numOfChangeLines;
	int deskNum;

	Event(string type, float time, int hackerID);
	Event(const Event& event);
	Event& operator=(const Event& event);
	~Event();
};


#endif

