#include <string>
#include <iostream>

using namespace std;

#ifndef HACKER_H
#define HACKER_H

class Hacker {

public:
	int ID;
	float arrivalTime;
	int numOfValidCommits;
	int numOfShortCommits;
	int totalChangedLine;
	int gifts;
	float startWaiting;
	float waitForSticker;
	float waitForHoodie;

	Hacker(int ID, float arrivalTime);
	Hacker(const Hacker& hacker);
	Hacker& operator=(const Hacker& hacker);
	void commit(int lineChanges);
	void takeGift();
	void setStartWaiting(float startTime);
	void wait(float finishTime, string deskType);

	~Hacker();

};

#endif
