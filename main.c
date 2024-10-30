#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 8

typedef enum{
	MainLane, DiversionLane, LeftMainLane, RightMainLane, LeftDiversionLane, RightDiversionLane, PedestrianMainLane, PedestrianDiversionLane
}LaneType;

typedef struct{
	LaneType laneName;
	int timer;
}Lane;

typedef struct{
	Lane laneList[MAX];
	int size;
}PriorityQueue;

Lane createLane(LaneType L, int timer);
void enqueueLane(PriorityQueue *p, Lane x);
Lane dequeueLane(PriorityQueue *p);
void displayLane(Lane x);
int calculateTime(PriorityQueue p);
LaneType convertString(char *laneString);

void getDataFromFile_act1(char *filePath, PriorityQueue *p);
void getDataFromFile_act2(char *filePath, PriorityQueue *p);

void saveFile_act1(char *filePath, PriorityQueue p);
void saveFile_act2(char *filePath, PriorityQueue p);


int main(){
	PriorityQueue lanes = {.size = 0};
	
//	getDataFromFile_act1("traffic.dat", &lanes);
	getDataFromFile_act2("traffic_result.dat", &lanes);

	int i;
	for(i = 0; i < lanes.size; i++){
		displayLane(lanes.laneList[i]);
		printf("\n");
	}
	
	printf("\nTime until Main Pedestrian crosses: %d seconds", calculateTime(lanes));
	
//	saveFile_act1("traffic.dat", lanes);
	

	return 0;
}

Lane createLane(LaneType L, int timer){
	Lane x;
	
	x.laneName = L;
	x.timer = timer;
	
	return x;
}

void enqueueLane(PriorityQueue *p, Lane x){
	if(p->size == MAX){
		exit;
	}
	
	p->laneList[p->size] = x; //insert at end of the heap
	p->size++;
	
	int child = p->size - 1;
	int parent = (child-1)/2;
	
	while(child > 0 && p->laneList[child].laneName < p->laneList[parent].laneName){ //compare the inserted data with its parent
		Lane temp = p->laneList[child];
		p->laneList[child] = p->laneList[parent];
		p->laneList[parent] = temp;
		child = parent;
		parent = (child-1)/2;
	}
}

Lane dequeueLane(PriorityQueue *p){
	Lane del;
	
	del = p->laneList[0];
	p->laneList[0] = p->laneList[--p->size];
	
	int parent = 0;
	while (1) {
        int leftChild = 2 * parent + 1;
        int rightChild = 2 * parent + 2;
        int minIndex = parent;

        if (leftChild < p->size && p->laneList[leftChild].laneName < p->laneList[minIndex].laneName) {
            minIndex = leftChild;
        }
        if (rightChild < p->size && p->laneList[rightChild].laneName < p->laneList[minIndex].laneName) {
            minIndex = rightChild;
        }
        if (minIndex == parent) {
            break;
        }

        Lane temp = p->laneList[parent];
        p->laneList[parent] = p->laneList[minIndex];
        p->laneList[minIndex] = temp;

        parent = minIndex;
    }
	
	
	return del;
}

void saveFile_act1(char *filePath, PriorityQueue p){
	FILE *fp;
	fp = fopen(filePath, "wb");
	
	if(fp == NULL){
		exit;
	}
	
//	fwrite(&p, sizeof(p), 1, fp); // save the whole priorityQueue structure instead
	fwrite(p.laneList, sizeof(Lane), p.size, fp); // save each of Lanes data type instead
	
	fclose(fp);
}

void saveFile_act2(char *filePath, PriorityQueue p){
	// to work
}

void getDataFromFile_act1(char *filePath, PriorityQueue *p){
	FILE *fp;
	fp = fopen(filePath, "rb");
	
	if(fp != NULL){
		Lane temp;
		while(fread(&temp, sizeof(Lane), 1, fp) == 1 && p->size < MAX){
			enqueueLane(p, temp);
		}
	}
	
	fclose(fp);
}

void getDataFromFile_act2(char *filePath, PriorityQueue *p){
	FILE *fp;
	fp = fopen(filePath, "r");
	
	if(fp != NULL){
		int scenarioChosen = 0;
		while(!(scenarioChosen > 0 && scenarioChosen < 10)){
			printf("Select a Scenario (1-9): ");
			scanf("%d", &scenarioChosen);
		}
		
		int scenarioCurrent;
		char line[256];
		for(scenarioCurrent = 1; fgets(line, sizeof(line), fp) && scenarioChosen != scenarioCurrent; scenarioCurrent++){}
		
		char *token = strtok(line, ",");
		while (token && p->size < MAX) {
			Lane lane;
			lane.laneName = convertString(token);
			token = strtok(NULL, ",");
			if (token) {
				lane.timer = atoi(token);
				enqueueLane(p, lane);
				token = strtok(NULL, ",");
			}
		}
	}
	
	fclose(fp);
}

void displayLane(Lane x){
	char name[30];
	switch(x.laneName){
		case 0:
			strcpy(name, "Main Lane");
			break;
		case 1:
			strcpy(name, "Diversion Lane");
			break;
		case 2:
			strcpy(name, "Left From Main Lane");
			break;
		case 3:
			strcpy(name, "Right From Main Lane");
			break;
		case 4:
			strcpy(name, "Left From Diversion Lane");
			break;
		case 5:
			strcpy(name, "Right From Diversion Lane");
			break;
		case 6:
			strcpy(name, "Main Pedestrian Lane");
			break;
		case 7:
			strcpy(name, "Diversion Pedestrian Lane");
			break;	
		default:
			strcpy(name, "Unknown");
			break;
	}
	printf("%s - %d seconds", name, x.timer);
}

int calculateTime(PriorityQueue p){
	int totalTime = 0;
	while(p.size > 0 && p.laneList[0].laneName != PedestrianMainLane){
		Lane get = dequeueLane(&p);
		totalTime += get.timer;
	}
	return totalTime;
}

LaneType convertString(char *laneString){
	if (strcmp(laneString, "MainLane") == 0) return MainLane;
	if (strcmp(laneString, "DiversionLane") == 0) return DiversionLane;
	if (strcmp(laneString, "LeftMainLane") == 0) return LeftMainLane;
	if (strcmp(laneString, "RightMainLane") == 0) return RightMainLane;
	if (strcmp(laneString, "LeftDiversionLane") == 0) return LeftDiversionLane;
	if (strcmp(laneString, "RightDiversionLane") == 0) return RightDiversionLane;
	if (strcmp(laneString, "PedestrianMainLane") == 0) return PedestrianMainLane;
	if (strcmp(laneString, "PedestrianDiversionLane") == 0) return PedestrianDiversionLane;
	return -1;
} 

