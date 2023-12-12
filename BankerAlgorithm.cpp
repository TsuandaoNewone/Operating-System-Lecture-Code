#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
using namespace std;
int resourceSize = 3;
int processNum;
int* resourceTotal;
int** MAX;
int** ALLOCATE;
int** NEED;
int processDoneCnt;
vector<int> safeSequence;
vector<int> allocateSequence;
bool* processDone;
// 函数：数组加法
void arrayAdd(const int* arr1, const int* arr2, int* result, int size) {
	for (int i = 0; i < size; ++i) {
		result[i] = arr1[i] + arr2[i];
	}
}
// 函数：数组减法
void arraySubtract(const int* arr1, const int* arr2, int* result, int size) {
	for (int i = 0; i < size; ++i) {
		result[i] = arr1[i] - arr2[i];
	}
}

// 显示数组的内容
void display() {
	printf("Total resource: ");
	for (int j = 0; j < resourceSize; j++) {
		cout << resourceTotal[j] << " ";
	}
	cout << endl;
	for (int i = 0; i < processNum; i++) {
		printf("Process %d   MAX: ", i);
		for (int j = 0; j < resourceSize; j++) {
			cout << MAX[i][j] << " ";
		}
		printf("  ALLOCATE: ");
		for (int j = 0; j < resourceSize; j++) {
			cout << ALLOCATE[i][j] << " ";
		}
		printf("  NEED: ");
		for (int j = 0; j < resourceSize; j++) {
			cout << NEED[i][j] << " ";
		}
		processDone[i] ? printf("   finished") : printf("   unfinished");
		cout << endl;
	}
	//cout << endl;
}

bool noBigger(const int* A, const int* B, const int size) {
	for (int i = 0; i < size; i++) {
		if (A[i] > B[i])
			return false;
	}
	return true;
}

bool safetyCheck(int i) {
	safeSequence.clear();
	bool* tempProcessDone = new bool[processNum];
	int* tempResourceTotal = new int[resourceSize];
	copy(processDone, processDone + processNum, tempProcessDone);
	copy(resourceTotal, resourceTotal + resourceSize, tempResourceTotal);
	int checkCnt = 0;
	bool over = false;
	while (!over) {
		over = true;
		for (int j = 0; j < processNum; j++) {
			if (!tempProcessDone[j] && noBigger(NEED[j], tempResourceTotal, resourceSize)) {
				tempProcessDone[j] = true;
				over = false;
				arrayAdd(tempResourceTotal, ALLOCATE[j], tempResourceTotal, resourceSize);
				safeSequence.push_back(j);
				checkCnt++;
			}
		}
	}
	if (checkCnt + processDoneCnt == processNum)
		return true;
	return false;
}

int main() {
	ifstream inputFile("input.txt");
	inputFile >> processNum >> resourceSize;


	resourceTotal = new int[resourceSize];
	processDone = new bool[processNum];
	MAX = new int* [processNum];
	ALLOCATE = new int* [processNum];
	NEED = new int* [processNum];

	memset(processDone, false, sizeof(processDone));
	for (int i = 0; i < resourceSize; i++) {
		inputFile >> resourceTotal[i];
	}
	for (int i = 0; i < processNum; i++) {
		int id;
		inputFile >> id;
		MAX[i] = new int[resourceSize];
		ALLOCATE[i] = new int[resourceSize];
		NEED[i] = new int[resourceSize];
		for (int j = 0; j < resourceSize; j++) {
			inputFile >> MAX[i][j];
		}
		for (int j = 0; j < resourceSize; j++) {
			inputFile >> ALLOCATE[i][j];
		}
		for (int j = 0; j < resourceSize; j++) {
			inputFile >> NEED[i][j];
		}
	}
	display();
	cout << endl;
	bool over = false;
	while(!over) {
		over = true;
		for (int i = 0; i < processNum; i++) {
			if (processDone[i]) continue;
			printf("process %d request resources,", i);
			//undone and allocatable
			if (noBigger(NEED[i], resourceTotal, resourceSize)) {
				printf(" enough resources, try allocating\n");
				// try allocating / 
				arrayAdd(ALLOCATE[i], resourceTotal, resourceTotal, resourceSize);
				processDone[i] = true;
				processDoneCnt++;
				//allocate success
				if (safetyCheck(i)) {
					printf("safe sequence found: ");
					allocateSequence.push_back(i);
					for (auto j : allocateSequence)
						printf("%d ", int(j));
					for (auto j : safeSequence)
						printf("%d ", int(j));
					copy(MAX[i], MAX[i] + resourceSize, ALLOCATE[i]);
					memset(NEED[i], 0, sizeof(NEED[i]));
					display();
					cout << endl;
					over = false;
				}
				//no safe sequence, allocate fail, recall resources
				else {
					printf("safe sequence not found, request rejected\n");
					arraySubtract(resourceTotal, ALLOCATE[i], resourceTotal, resourceSize);
					processDone[i] = false;
					processDoneCnt--;
				}
			}
			else {
				printf("not enough resources,request failed\n");
			}
		}
	}
	if (processDoneCnt == processNum) {
		printf("All processes successfully ran\n");
	}
		return 0;
}
