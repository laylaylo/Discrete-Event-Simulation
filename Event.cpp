#include "Event.h"

using namespace std;


Event::Event(string type, float time, int hackerID) {
	this->type = type;
	this->time = time;
	this->hackerID = hackerID;
	this->numOfChangeLines = 0; 
	this->deskNum = 0;
}

Event::Event(const Event& event) {
	this->type = event.type;
	this->time = event.time;
	this->hackerID = event.hackerID;
	this->numOfChangeLines = event.numOfChangeLines;
	this->deskNum = event.deskNum;
}

Event& Event::operator=(const Event& event) {
	if(this == &event) {
		return *this;
	}

	this->type = event.type;
	this->time = event.time;
	this->hackerID = event.hackerID;
	this->numOfChangeLines = event.numOfChangeLines;
	this->deskNum = event.deskNum;
	return *this;
}

Event::~Event() {

}