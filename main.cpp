#include "Hacker.h"
#include "Event.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <iomanip>

using namespace std;

bool operator<(const Event& e1, const Event& e2) {
	if(abs(e1.time - e2.time) < 0.00001) {
		return e2.hackerID < e1.hackerID;
	}
	return e1.time - e2.time > 0.00001;
}

bool operator<(const Hacker& h1, const Hacker& h2) {
	if(h1.numOfValidCommits == h2.numOfValidCommits) {
		if(abs(h1.startWaiting - h2.startWaiting) < 0.0001) {
			return h2.ID < h1.ID;
		}
		return h1.startWaiting - h2.startWaiting > 0.00001;
	}
	return h1.numOfValidCommits < h2.numOfValidCommits;
}

int main(int argc, char* argv[]) {

	string infile_name = argv[1]; 
	string outfile_name = argv[2]; 

	ifstream infile;
	infile.open(infile_name);

	int numOfEvents = 0;

	int numOfHackers;
	infile >> numOfHackers;

	vector<Hacker> hackers;
	priority_queue<Event> events;

	for(int i = 1; i <= numOfHackers; i++) { // creates hackers 
		float arrivalTime;
		infile >> arrivalTime;
		Hacker hacker = Hacker(i, arrivalTime);
		hackers.push_back(hacker); 
	}

	int numOfCodeCommitEvents;
	infile >> numOfCodeCommitEvents;

	while(numOfCodeCommitEvents) { // creates commit events and pushes them into priority queue
		int hackerID, numOfChangeLines;
		float time;
		infile >> hackerID >> numOfChangeLines >> time;
		Event event = Event("commit", time, hackerID);
		event.numOfChangeLines = numOfChangeLines;
		events.push(event);
		numOfEvents++;

		numOfCodeCommitEvents--;
	}

	int numOfQueueEntranceAttempts;
	infile >> numOfQueueEntranceAttempts;

	while(numOfQueueEntranceAttempts) { // creates sticker queue entrance events and pushes them into priority queue
		int hackerID;
		float time;
		infile >> hackerID >> time;
		Event event = Event("stickerQueueEntrance", time, hackerID);
		events.push(event);
		numOfEvents++;

		numOfQueueEntranceAttempts--;
	}

	int numOfStickerDesks;
	infile >> numOfStickerDesks;
	vector<float> serviceTimeStickerDesk; 
	vector<bool> isAvailableStickerDesk(numOfStickerDesks, true);
	for(int i = 0; i < numOfStickerDesks; i++) { 
		float serviceTime;
		infile >> serviceTime;
		serviceTimeStickerDesk.push_back(serviceTime);
	} 

	int numOfHoodieDesks;
	infile >> numOfHoodieDesks;
	vector<float> serviceTimeHoodieDesk; 
	vector<bool> isAvailableHoodieDesk(numOfHoodieDesks, true);
	for(int i = 0; i < numOfHoodieDesks; i++) {
		float serviceTime;
		infile >> serviceTime;
		serviceTimeHoodieDesk.push_back(serviceTime);
	}

	queue<Hacker> stickerQueue;
	priority_queue<Hacker> hoodieQueue;
	int maxStickerQueue = 0;
	int maxHoodieQueue = 0;
	int invalidAttempt1 = 0;
	int invalidAttempt2 = 0;
	float totalTime = 0;
	float timeInDesks = 0;

	while(numOfEvents != 0) {
		Event event = events.top();

		numOfEvents--;

		if(numOfEvents == 0) {
			totalTime = events.top().time;
		}
		events.pop();

		if(event.type == "commit") {
			hackers[event.hackerID-1].commit(event.numOfChangeLines);
		}
		if(event.type == "stickerQueueEntrance") {
			if(hackers[event.hackerID-1].numOfValidCommits < 3) {
				invalidAttempt1++;
			}
			if(hackers[event.hackerID-1].gifts >= 3) {
				invalidAttempt2++;
			}
			if(hackers[event.hackerID-1].numOfValidCommits >= 3 && hackers[event.hackerID-1].gifts < 3) {
				hackers[event.hackerID-1].takeGift();
				stickerQueue.push(hackers[event.hackerID-1]); 
				hackers[event.hackerID-1].setStartWaiting(event.time);
				vector<bool>::iterator itr = find(isAvailableStickerDesk.begin(), isAvailableStickerDesk.end(), true);
				if(itr != isAvailableStickerDesk.end() && stickerQueue.size() == 1) {
					// available desk and nobody else in the queue, immediately go to desk
					int stickerDeskNum = distance(isAvailableStickerDesk.begin(), itr) + 1;
					Event deskEntrance = Event("stickerDeskEntrance",event.time ,event.hackerID);
					deskEntrance.deskNum = stickerDeskNum;
					events.push(deskEntrance);
					numOfEvents++;
				}else{
					// no availeble desk wait
					if(stickerQueue.size() > maxStickerQueue) {
						maxStickerQueue = stickerQueue.size();
					}
				}
			}
		}
		if(event.type == "stickerDeskEntrance") {
			if(!stickerQueue.empty()) {
				int hackerID = stickerQueue.front().ID;
				stickerQueue.pop();
				hackers[hackerID-1].wait(event.time, "sticker");
				isAvailableStickerDesk[event.deskNum-1] = false;
				timeInDesks += serviceTimeStickerDesk[event.deskNum-1];
				Event deskExit = Event("stickerDeskExit", event.time + serviceTimeStickerDesk[event.deskNum-1], hackerID);
				deskExit.deskNum = event.deskNum;
				events.push(deskExit);  
				numOfEvents++;
			}
		}
		if(event.type == "stickerDeskExit") {
			isAvailableStickerDesk[event.deskNum-1] = true;
			Event queueEntrance = Event("hoodieQueueEntrance", event.time, event.hackerID);
			events.push(queueEntrance);
			numOfEvents++;
			if(!stickerQueue.empty()) {
				if(stickerQueue.front().ID != 0) {
					 Event deskEntrance = Event("stickerDeskEntrance", event.time, stickerQueue.front().ID);
					 deskEntrance.deskNum = event.deskNum;
					 events.push(deskEntrance);
					 numOfEvents++;
				}
			}
		}
		if(event.type == "hoodieQueueEntrance") {
 			hoodieQueue.push(hackers[event.hackerID-1]);
 			hackers[event.hackerID-1].setStartWaiting(event.time);
			vector<bool>::iterator itr = find(isAvailableHoodieDesk.begin(), isAvailableHoodieDesk.end(), true);
			if(itr != isAvailableHoodieDesk.end() && hoodieQueue.size() == 1) {
				//available desk and nobody else in the queue, immediately go to desk
				int hoodieDeskNum = distance(isAvailableHoodieDesk.begin(), itr) + 1;
				Event deskEntrance = Event("hoodieDeskEntrance", event.time, event.hackerID);
				deskEntrance.deskNum = hoodieDeskNum;
				events.push(deskEntrance); 
				numOfEvents++;
			}else{
				//no available desk wait
	 			if(hoodieQueue.size() > maxHoodieQueue) {
 				maxHoodieQueue = hoodieQueue.size();
 				}			
			}
		 }
		 if(event.type == "hoodieDeskEntrance") {
			if(!hoodieQueue.empty()) {
				int hackerID = hoodieQueue.top().ID;
				hoodieQueue.pop();
				hackers[hackerID-1].wait(event.time, "hoodie");
				isAvailableHoodieDesk[event.deskNum-1] = false;
				timeInDesks += serviceTimeHoodieDesk[event.deskNum-1];
				Event deskExit = Event("hoodieDeskExit", event.time + serviceTimeHoodieDesk[event.deskNum-1], hackerID);
				deskExit.deskNum = event.deskNum;
				events.push(deskExit);
				numOfEvents++;
			}
		}
		if(event.type == "hoodieDeskExit") {
			isAvailableHoodieDesk[event.deskNum-1] = true;
			if(!hoodieQueue.empty()) {
				if(hoodieQueue.top().ID != 0) {
					Event deskEntrance = Event("hoodieDeskEntrance", event.time, hoodieQueue.top().ID);
					deskEntrance.deskNum = event.deskNum;
					events.push(deskEntrance);
					numOfEvents++;
				}
			}
		 }
	}

	ofstream outfile;
	outfile.open(outfile_name);

	outfile << maxStickerQueue << endl;
	outfile << maxHoodieQueue << endl;

	float numOfGifts = 0;
	float waitForSticker = 0;
	float waitForHoodie = 0;
	float maxWait = -1;
	int maxWaitID = 0;
	float minWait = -1;
	bool first = true;
	int minWaitID = -1;
	float numOfValidCodeCommit = 0;
	float numOfShortCodeCommit = 0;
	float changeLines = 0;
	for(int i = 0; i < numOfHackers; i++) {
		numOfGifts += hackers[i].gifts;
		waitForSticker += hackers[i].waitForSticker;
		waitForHoodie += hackers[i].waitForHoodie;
		if(hackers[i].waitForSticker + hackers[i].waitForHoodie - maxWait > 0.00001) {
			maxWait = hackers[i].waitForSticker + hackers[i].waitForHoodie;
			maxWaitID = hackers[i].ID;

		}
		if(hackers[i].gifts == 3) {
			if(first) {
				minWaitID = hackers[i].ID;
				minWait = hackers[i].waitForSticker + hackers[i].waitForHoodie;
				first = false;
			}else if(minWait - (hackers[i].waitForSticker + hackers[i].waitForHoodie) > 0.00001) {
				minWaitID = hackers[i].ID;
				minWait = hackers[i].waitForSticker + hackers[i].waitForHoodie;
			}
		}
		numOfValidCodeCommit += hackers[i].numOfValidCommits;
		numOfShortCodeCommit += hackers[i].numOfShortCommits;
		changeLines += hackers[i].totalChangedLine;
	}

	outfile << fixed;
	outfile << setprecision(3);
	outfile << (numOfGifts / numOfHackers) << endl;
	outfile << waitForSticker / numOfGifts << endl;
	outfile << waitForHoodie / numOfGifts << endl;
	outfile << (numOfValidCodeCommit + numOfShortCodeCommit) / numOfHackers << endl;
	outfile << changeLines / (numOfValidCodeCommit + numOfShortCodeCommit) << endl;
	outfile << (waitForSticker + waitForHoodie + timeInDesks) / numOfGifts << endl;
	outfile << invalidAttempt1 << "\n" << invalidAttempt2 << endl;
	outfile << maxWaitID << " " << maxWait << endl;
	outfile << minWaitID << " " << minWait << endl;
	outfile << totalTime;


	infile.close();
	outfile.close();

    return 0;
}