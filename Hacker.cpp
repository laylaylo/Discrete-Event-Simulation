#include "Hacker.h"

using namespace std;

Hacker::Hacker(int ID, float arrivalTime) {
	this->ID = ID;
	this->arrivalTime = arrivalTime;
	this->numOfValidCommits = 0;
	this->numOfShortCommits = 0;
	this->totalChangedLine = 0;
	this->gifts = 0;
	this->startWaiting = 0;
}

Hacker::Hacker(const Hacker& hacker) {
	this->ID = hacker.ID;
	this->arrivalTime = hacker.arrivalTime;
	this->numOfValidCommits = hacker.numOfValidCommits;
	this->numOfShortCommits = hacker.numOfShortCommits;
	this->totalChangedLine = hacker.totalChangedLine;
	this->gifts = hacker.gifts;
	this->startWaiting = hacker.startWaiting;
}

Hacker& Hacker::operator=(const Hacker& hacker) {
	if(this == &hacker) {
		return *this;
	}


	this->ID = hacker.ID;
	this->arrivalTime = hacker.arrivalTime;
	this->numOfValidCommits = hacker.numOfValidCommits;
	this->numOfShortCommits = hacker.numOfShortCommits;
	this->totalChangedLine = hacker.totalChangedLine;
	this->gifts = hacker.gifts;
	this->startWaiting = hacker.startWaiting;

	return *this;
}

void Hacker::commit(int lineChanges) { 
	if(lineChanges >= 20) {
		this->numOfValidCommits = this->numOfValidCommits + 1;
	}else{
		this->numOfShortCommits++;
	}
	this->totalChangedLine += lineChanges;
}

void Hacker::takeGift() { 
	this->gifts++;
}

void Hacker::setStartWaiting(float startTime) { // takes the time in which hacker enters queue
	this->startWaiting = startTime;
}

void Hacker::wait(float finishTime, string deskType) { // calculates waiting time in given type queue
	if(abs(this->startWaiting - finishTime) > 0.00001) {
		float totalTime = finishTime - this->startWaiting;
		if(deskType == "sticker") {
			this->waitForSticker += totalTime;
		}
		if(deskType == "hoodie") {
			this->waitForHoodie += totalTime;
		}
	}	
}

Hacker::~Hacker() {

}