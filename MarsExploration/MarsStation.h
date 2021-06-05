#pragma once
#include <iostream>
#include "PriQ.h"
#include "Queue.h"
#include "Event.h"
#include "HashTable.h"
#include "Mission.h"
#include "Rover.h"
#include "UI.h"
#include "Action.h"

class MarsStation
{
private:
	// AvailRov[0] -> EmergRov 
	// AvailRov[1] -> PolarRov 
	// AvailRov[2] -> MountRov
	// AvailRov[3] -> EmergRovMaint 
	// AvailRov[4] -> PolarRovMaint 
	// AvailRov[5] -> MountRovMaint 

	//StatsArr[0] -> EmergRov 
	//StatsArr[1] -> PolarRov 
	//StatsArr[2] -> MountRov
	//StatsArr[3] -> EmergMission 
	//StatsArr[4] -> PolarMission 
	//StatsArr[5] -> MountMission 
	//StatsArr[6] -> AutoPromotedMission 

	//RoversInCheckUp[0] -> EmergRov
	//RoversInCheckUp[1] -> PolarRov
	//RoversInCheckUp[2] -> MountRov


	PriQ<Rover> AvaiableRovers[6];
	Queue<Rover> RoversInCheckUp[3];
	PriQ<Mission> InExecution;
	PriQ<Mission> EmeregncyMissions;
	Queue<Rover> PolarRovers;
	Queue<Mission> PolarMissions;
	Queue<Mission> CompletedMissions;
	Queue<Event> EventList;
	Queue<int> MountainousOrder;
	HashTable<Mission,101> MountainousMissions;
	

	float NumberOfExec;
	float NumberOfWait;
	int StatsArr[7];
	int AutoPro;
	int day;

public:
	MarsStation();
	void SetAvailableRovers(int , int , int , int*, int*, int*, int,int , int, int);

	void Execute();
	void checkAndAssign();

	void MoveToExec();
	void MoveToCompMissions();
	void MoveRover(Mission* mission);
	void MoveToAvailRover();


	int GetDay();

	float GetAvgExecDays();
	float GetAvgWaitDays();
	float GetAutoPromotedPercent();
	void SetAutoPro(int);
	void AutoPromote();


};