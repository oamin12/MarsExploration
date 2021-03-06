#include "UI.h"
#include "MarsStation.h"
#include "Formulation.h"
#include "Cancellation.h"
#include "Promotion.h"



UI::UI(MarsStation* Mars)
{
	pMars = Mars;
}

void UI::Read(ifstream& file, Queue<Event>& eventList)
{
	char Choice;
	cout << "Select Mode:" << endl;
	cout << "i for interactive, s for Step-By-Step, x for Silent " << endl;
	cin >> Choice;
	setMode(Choice);
	if (Choice == 'i')
		cout << "Press enter to proceed" << endl;	

	/*The type of each rover*/
	int NumOfPolarRover;
	int NumOfMountRover;
	int NumOfEmergRover;

	file >> NumOfMountRover >> NumOfPolarRover >> NumOfEmergRover;

	/*The speed of each rover*/
	int* SpeedOfPolarRover = new int[NumOfPolarRover];
	int* SpeedOfMountRover = new int[NumOfMountRover];
	int* SpeedOfEmergRover = new int[NumOfEmergRover];

	/*The number of missions needed before check up*/
	int BeforeCheckup; 

	/*The Check up duration of each rover*/
	int CheckUpDurationPolar;
	int CheckUpDurationMount;
	int CheckUpDurationEmerg;

	/*Loop for different rover speed*/
	for (int i = 0; i < NumOfMountRover; i++)
		file >> SpeedOfMountRover[i];

	for (int i = 0; i < NumOfPolarRover; i++)
		file >> SpeedOfPolarRover[i];

	for (int i = 0; i < NumOfEmergRover; i++)
		file >> SpeedOfEmergRover[i];

	file >> BeforeCheckup;
	file >> CheckUpDurationMount >> CheckUpDurationPolar >> CheckUpDurationEmerg;
	pMars->SetAvailableRovers(NumOfMountRover, NumOfPolarRover, NumOfEmergRover, SpeedOfMountRover, SpeedOfPolarRover, SpeedOfEmergRover, CheckUpDurationMount, CheckUpDurationPolar, CheckUpDurationEmerg, BeforeCheckup);
	
	/*Reading the events information*/
	int AutoPromotion;

	int NumOfEvents;

	file >> AutoPromotion;
	pMars->SetAutoPro(AutoPromotion);
	file >> NumOfEvents;
	
	char EventType;


	for (int i = 0; i < NumOfEvents; i++)
	{
		file >> EventType;
		/*The Info for formulation event*/
		if (EventType == 'F')
		{
			char MissionType;
			int ED, ID, Location, MDUR, Signif;

			file >> MissionType;
			file >> ED >> ID >> Location >> MDUR >> Signif;

			/*Checking if the number of rovers equals zero so the mission wont be formulated*/
			if (NumOfPolarRover != 0 && MissionType == 'P')
			{
				Formulation* F = new Formulation(MissionType, Location, MDUR, Signif, ID, ED);
				eventList.enqueue(F);
			}
			if((NumOfEmergRover!=0 || NumOfMountRover != 0) && MissionType=='M')
			{
				Formulation* F = new Formulation(MissionType, Location, MDUR, Signif, ID, ED);
				eventList.enqueue(F);
			}
			if (MissionType == 'E')
			{
				Formulation* F = new Formulation(MissionType, Location, MDUR, Signif, ID, ED);
				eventList.enqueue(F);
			}
		}
		/*The Info for cancellation event*/
		else if (EventType == 'X')
		{
			int ED,ID;
			file >> ED >> ID;
			Cancellation* X =new Cancellation(ID,ED);
			eventList.enqueue(X);
		}
		/*The Info for promotion event*/
		else if (EventType == 'P')
		{
			int ED, ID;
			file >> ED >> ID;
			Promotion* P = new Promotion(ID, ED);
			eventList.enqueue(P);
		}
	}
}

void UI::Write(ofstream& file, Queue<Mission> CompletedMissions, int* StatsArr)
{
	file << "CD\tID\tFD\tWD\tED" << endl;
	Queue<int> Temp;
	Node<Mission>* TempMission;

	/*Looping on the completed mission list to print their info*/
	while (!CompletedMissions.isEmpty())
	{
		CompletedMissions.dequeue(TempMission);

		file << TempMission->getData()->getCD() << "\t" << TempMission->getData()->getID() << "\t" << TempMission->getData()->getFormulationDate() << "\t" << TempMission->getData()->getTotalMissionDur() << "\t" << TempMission->getData()->getED() << endl;
	}
	/*Printing the rest of the info*/
	file << "Missions: " << StatsArr[3] + StatsArr[4] + StatsArr[5] << " [M: " << StatsArr[5] << ", P: " << StatsArr[4] << ", E: " << StatsArr[3] << "]\n";
	file << "Rovers: " << StatsArr[0] + StatsArr[1] + StatsArr[2] << " [M: " << StatsArr[2] << ", P: " << StatsArr[1] << ", E: " << StatsArr[0] << "]\n";
	file << "Avg Wait = " << pMars->GetAvgWaitDays() << ", " << "Avg Exec = " << pMars->GetAvgExecDays() << endl;
	file << "Auto-promoted: " << pMars->GetAutoPromotedPercent() << "%";
}

void UI::PrintWait(PriQ<Mission> Emergency, Queue<int> MountainousSort, Queue<Mission> Polar)
{
	int CountM = 0;
	int CountP = 0;
	int CountE = 0;
	int CountW = 0;
	int CountInEx = 0;

	Queue<int> TempM;
	Queue<Mission> TempEP;
	Node<int>* TempID;
	Node<Mission>* TempMission;
	int TempInt;

	/*Collecting the lists into a queue to print them in order*/
	while (!MountainousSort.isEmpty())
	{
		MountainousSort.dequeue(TempID);
		CountM++;
		CountW++;
		TempM.enqueue(TempID->getData());
	}

	while (!Emergency.isEmpty())
	{
		Emergency.dequeue(TempMission);
		CountE++;
		CountW++;
		TempEP.enqueue(TempMission->getData());
	}
	while (!Polar.isEmpty())
	{
		Polar.dequeue(TempMission);
		CountP++;
		CountW++;
		TempEP.enqueue(TempMission->getData());
	}


	cout << "Current Day: " << pMars->GetDay() << endl;
	cout << CountW << " Waiting Missions: ";
	/*Chechking if all the lists are empty*/
	if (TempM.isEmpty() && TempEP.isEmpty())
	{
		cout << "[ ] ( ) { } ";
		cout << endl;
		cout << "--------------------------------------" << endl;
		return;
	}
	if (!TempM.isEmpty() && TempEP.isEmpty())
	{
		cout << "[ ] ( ) ";
	}
	/*Printing each list in order*/
	while (!TempEP.isEmpty())
	{
		cout << "[ ";
		for (int i = 0; i < CountE; i++)
		{
			TempEP.dequeue(TempMission);
			cout << TempMission->getData()->getID() << ",";

		}
		cout << "\b ]" << "  ";
		cout << "( ";
		for (int i = 0; i < CountP; i++)
		{
			TempEP.dequeue(TempMission);
			cout << TempMission->getData()->getID() << ",";
		}
		cout << "\b )" << "  ";
		
	}
	if (!TempM.isEmpty())
	{
		cout << "{ ";
		for (int i = 0; i < CountM; i++)
		{
			TempM.dequeue(TempID);
			int* ids = TempID->getData();
			cout << (*ids) << ",";
		}
		cout << "\b }";
	}
	else 
	{
		cout << "{ }";
	}
	cout << endl;
	cout << "--------------------------------------" << endl;
}

void UI::PrintInExecution(PriQ<Mission> InExecution)
{
	int CountM = 0;
	int CountP = 0;
	int CountE = 0;
	int CountInEx = 0;


	Queue<Mission> Emerge;
	Queue<Mission> Mount;
	Queue<Mission> Polar;
	Node<Mission>* MissionNode;
	/*Dividing the in execution list into 3 list to print each in order*/
	while (!InExecution.isEmpty())
	{
		InExecution.dequeue(MissionNode);
		if (MissionNode->getData()->getMissionType() == 'E')
		{
			CountE++;
			CountInEx++;
			Emerge.enqueue(MissionNode->getData());
		}
		else if (MissionNode->getData()->getMissionType() == 'P')
		{
			CountP++;
			CountInEx++;
			Polar.enqueue(MissionNode->getData());
		}
		else if (MissionNode->getData()->getMissionType() == 'M')
		{
			CountM++;
			CountInEx++;
			Mount.enqueue(MissionNode->getData());
		}
	}
	cout << CountInEx << " In-Execution Missions/Rovers: ";

	/*Printing all the missions in order*/
	cout << "[  ";
	while (!Emerge.isEmpty())
	{
		Emerge.dequeue(MissionNode);
		cout << MissionNode->getData()->getID() << "/" << MissionNode->getData()->getRover()->getID() << ", ";

	}
	cout << "\b\b  ] ";

	cout << "(  ";
	while (!Polar.isEmpty())
	{
		Polar.dequeue(MissionNode);
		cout << MissionNode->getData()->getID() << "/" << MissionNode->getData()->getRover()->getID() << ", ";

	}
	cout << "\b\b  ) ";


	cout << "{  ";
	while (!Mount.isEmpty())
	{
		Mount.dequeue(MissionNode);
		cout << MissionNode->getData()->getID() << "/" << MissionNode->getData()->getRover()->getID() << ", ";
	}
	cout << "\b\b  } ";

	cout << endl;

	cout << "--------------------------------------" << endl;
}

void UI::PrintAvRovers(PriQ<Rover> AvaiableRoversE, PriQ<Rover> AvaiableRoversP, PriQ<Rover> AvaiableRoversM,char c)
{
	int CountR = 0;
	int CountM = 0;
	int CountP = 0;
	int CountE = 0;

	Queue<Rover> Rovers;
	Node<Rover>* Temp;

	/*The Getting the available rovers list into one list in order for printing*/
	while (!AvaiableRoversE.isEmpty())
	{
		AvaiableRoversE.dequeue(Temp);
		Rovers.enqueue(Temp->getData());
		CountE++;
		CountR++;
	}
	while (!AvaiableRoversP.isEmpty())
	{
		AvaiableRoversP.dequeue(Temp);
		Rovers.enqueue(Temp->getData());
		CountP++;
		CountR++;
	}
	while (!AvaiableRoversM.isEmpty())
	{
		AvaiableRoversM.dequeue(Temp);
		Rovers.enqueue(Temp->getData());
		CountM++;
		CountR++;
	}
	/*Checking whether to print the available rovers or the rovers in maintenance*/
	if (c == 'A')
	{
		cout << CountR << " Available Rovers: ";
	}
	else
	{
		cout << CountR << " Rovers in Maintenance ";
	
	}

	/*Printing the information of the rovers available/Maintenance*/
	if (Rovers.isEmpty())
	{
		cout << "[ ] ( ) { } ";
	}
	while (!Rovers.isEmpty())
	{
		cout << "[  ";
		for (int i = 0; i < CountE; i++)
		{
			Rovers.dequeue(Temp);
			cout << Temp->getData()->getID() << ",";
		}

		cout << "\b  ]" << "  ";
		cout << "(  ";
		for (int i = 0; i < CountP; i++)
		{
			Rovers.dequeue(Temp);
			cout << Temp->getData()->getID() << ",";
		}
		cout << "\b  )" << "  ";
		cout << "{  ";
		for (int i = 0; i < CountM; i++)
		{
			Rovers.dequeue(Temp);
			cout << Temp->getData()->getID() << ",";
		}
		cout << "\b  }";

	}
	cout << endl;
	cout << "--------------------------------------" << endl;
}

void UI::PrintInCheckUp(Queue<Rover> RoversInCheckUpE, Queue<Rover> RoversInCheckUpP, Queue<Rover> RoversInCheckUpM)
{
	int CountC = 0;
	int CountM = 0;
	int CountP = 0;
	int CountE = 0;
	
	Queue<Rover> CheckRovers;
	Node<Rover>* Temp;

	/*The Getting the checkup rovers list into one list in order for printing*/
	while (!RoversInCheckUpE.isEmpty())
	{
		RoversInCheckUpE.dequeue(Temp);
		CheckRovers.enqueue(Temp->getData());
		CountE++;
		CountC++;
	}
	while (!RoversInCheckUpP.isEmpty())
	{
		RoversInCheckUpP.dequeue(Temp);
		CheckRovers.enqueue(Temp->getData());
		CountP++;
		CountC++;
	}
	while (!RoversInCheckUpM.isEmpty())
	{
		RoversInCheckUpM.dequeue(Temp);
		CheckRovers.enqueue(Temp->getData());
		CountM++;
		CountC++;
	}

	/*Printing the information of the check up rovers*/
	cout << CountC << " In-Checkup Rovers: ";
	if (CheckRovers.isEmpty())
	{
		cout << "[ ] ( ) { } ";
	}
	while (!CheckRovers.isEmpty())
	{
		cout << "[  ";
		for (int i = 0; i < CountE; i++)
		{
			CheckRovers.dequeue(Temp);
			cout << Temp->getData()->getID() << ",";
		}

		cout << "\b  ]" << "  ";
		cout << "(  ";
		for (int i = 0; i < CountP; i++)
		{
			CheckRovers.dequeue(Temp);
			cout << Temp->getData()->getID() << ",";
		}
		cout << "\b  )" << "  ";
		cout << "{  ";
		for (int i = 0; i < CountM; i++)
		{
			CheckRovers.dequeue(Temp);
			cout << Temp->getData()->getID() << ",";
		}
		cout << "\b  }";

	}
	cout << endl;
	cout << "--------------------------------------" << endl;



}

void UI::PrintComplete(Queue<Mission> CompletedMissions)
{
	int CountM = 0;
	int CountP = 0;
	int CountE = 0;
	int CountComp = 0;


	Queue<Mission> Emerge;
	Queue<Mission> Mount;
	Queue<Mission> Polar;
	Node<Mission>* MissionNode;

	/*The Getting the completed mission list into one list in order for printing*/
	while (!CompletedMissions.isEmpty())
	{
		CompletedMissions.dequeue(MissionNode);
		/*Checking the current to print the completed missions in this day only*/
		if (MissionNode->getData()->getMissionType() == 'E' && MissionNode->getData()->getCD()==pMars->GetDay())
		{
			CountE++;
			CountComp++;
			Emerge.enqueue(MissionNode->getData());
		}
		else if (MissionNode->getData()->getMissionType() == 'P' && MissionNode->getData()->getCD() == pMars->GetDay())
		{
			CountP++;
			CountComp++;
			Polar.enqueue(MissionNode->getData());
		}
		else if (MissionNode->getData()->getMissionType() == 'M' && MissionNode->getData()->getCD() == pMars->GetDay())
		{
			CountM++;
			CountComp++;
			Mount.enqueue(MissionNode->getData());
		}

	}

	/*Printing the information of the completed missions*/
	cout << CountComp << " Completed Missions: ";

	cout << "[  ";
	while (!Emerge.isEmpty())
	{
		Emerge.dequeue(MissionNode);
		if(MissionNode->getData()->getCD()==pMars->GetDay())
			cout << MissionNode->getData()->getID()  << ", ";

	}
	cout << "\b\b  ] ";

	cout << "(  ";
	while (!Polar.isEmpty())
	{
		Polar.dequeue(MissionNode);
		if (MissionNode->getData()->getCD() == pMars->GetDay())
			cout << MissionNode->getData()->getID() <<", ";

	}
	cout << "\b\b  ) ";


	cout << "{  ";
	while (!Mount.isEmpty())
	{
		Mount.dequeue(MissionNode);
		if (MissionNode->getData()->getCD() == pMars->GetDay())
			cout << MissionNode->getData()->getID()  << ", ";
	}
	cout << "\b\b  } ";

	cout << endl;

	cout << "--------------------------------------" << endl;
}

void UI::Mode(PriQ<Mission> Emergency, Queue<int> MountainousSort, Queue<Mission> Polar, PriQ<Mission> InExecution,PriQ<Rover> AvaiableRoversE, PriQ<Rover> AvaiableRoversP, PriQ<Rover> AvaiableRoversM,PriQ<Rover> AvaiableRoversEM, PriQ<Rover> AvaiableRoversPM, PriQ<Rover> AvaiableRoversMM, Queue<Rover> RoversInCheckUpE, Queue<Rover> RoversInCheckUpP, Queue<Rover> RoversInCheckUpM, Queue<Mission> CompletedMissions)
{
	/*Calling each printing function to print in the console each line*/
	PrintWait(Emergency, MountainousSort, Polar);
	PrintInExecution(InExecution);
	PrintAvRovers(AvaiableRoversE, AvaiableRoversP, AvaiableRoversM,'A');
	PrintAvRovers(AvaiableRoversEM, AvaiableRoversPM, AvaiableRoversMM, 'M');
	PrintInCheckUp(RoversInCheckUpE, RoversInCheckUpP, RoversInCheckUpM);
	PrintComplete(CompletedMissions);
	cout << "======================= New Day ======================" << endl;
}

void UI::SilentMode()
{
	cout << "Silent Mode \nSimulation Starts... \nSimulation ends, Output file created" << endl;
		
}

/*Setter and getter for the mode*/
char UI::getMode()
{
	return mode;
}
void UI::setMode(char choice)
{
	mode = choice;
}