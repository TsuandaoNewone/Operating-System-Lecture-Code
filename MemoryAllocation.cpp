// Online C++ compiler to run C++ program online
#include <iostream>
#include <vector>
#include<fstream>
using namespace std;
const int MAX_ITEM = 5;
const int MAX_SIZE = 128;
int lastFit = 0;
struct segment {
    int start;
    int size;
    segment(int a, int b) { start = a; size = b; }
    segment(){}
}S;
struct work {
    int id;
    int start;
    int size;
    work(int a, int b, int c) { id = a; start = b; size = c; }
    work(){}
};

vector<segment> aTable;
vector<work> works;
//find which available area to store work
int first_fit(int workSize) {
    int i = 0;
    for (auto& a : aTable) {
        if (workSize <= a.size)
            return i;
        i++;
    }
    return -1;
}
int next_fit(int workSize) {
    int i = 0;
    int startFirstFit = -1;
    for (auto& a : aTable) {
        if (workSize <= a.size) {
            if (i <= lastFit) {
                if(startFirstFit < 0)
                    startFirstFit = i;
            }
            else {
                lastFit = i;
                return i;
            }
        }
        i++;
    }
    if (startFirstFit > 0) {
        lastFit = startFirstFit;
        return startFirstFit;
    }
    return -1;
}
int best_fit(int workSize) {
    int i = 0;
    int leastArea = 999999;
    int leastAreaId = -1;
    for (auto& a : aTable) {
        if (workSize <= a.size && a.size<leastArea) {
            leastAreaId = i;
            leastArea = a.size;
        }
        i++;
    }
    if (leastAreaId >= 0) return leastAreaId;
    return -1;
}

void insertWorkAt(int id, int size) {
    const int& aSize = aTable[id].size;
    if (size > aSize)
        return;
    else if (size == aSize) {
        aTable.erase(aTable.begin()+id);
    }
    else{
        aTable[id].start += size;
        aTable[id].size -= size;
    }        
}
void revokeWork(int id) {
    int i = 0;
    for (auto& a : works) {
        if (a.id == id) break;
        i++;
    }
    int workStart = works[i].start;
    int workSize = works[i].size;
    works.erase(works.begin() + i);
    int workEnd = workStart + workSize;
    int lastItem = -2;
    int insertPosition = -1;
    i = 0;
    bool neighborArea = false;
    //iterating through available areas
    for (auto& item : aTable) {
        int  itemEnd = item.size + item.start;
        //find the insert position
        if (workStart >= itemEnd) {
            insertPosition = i+1;
        }
        //available area in the top of the work
        if (itemEnd == workStart) {
            item.size += workSize;
            lastItem = i;
            neighborArea = true;
        }
        //available area in the bottom of the work
        if (workEnd == item.start) {
            item.start -= workSize;
            neighborArea = true;
            //both happens, delete bottom area, keep the top
            if (lastItem + 1 == i) {
                aTable[lastItem].size += item.size;
                aTable.erase(aTable.begin()+i);
                break;
            }
            item.size += workSize;
        }
        i++;
    }
    //no neighboring areas found
    if (!neighborArea) {
        if (insertPosition == -1) insertPosition = 0;
        aTable.insert(aTable.begin() + insertPosition, segment(workStart, workSize));
    }
}
void displayWork() {
    int i = 0;
    cout << "=========Work Table=========" << endl;
    for (const auto& value : works) {
        printf("Assignment%d    start%d     size%d\n", value.id, value.start, value.size);
    }
}
void displayAvailabeArea(){
    int i = 0;
    cout << "=====Available Area Table====="<<endl;
    for (const auto& value : aTable) {
        printf("第%d栏    %dK     %dK 未分配\n", ++i, value.start, value.size);
    }
    while (i < MAX_ITEM) {
        printf("第%d栏                 空表目\n", ++i);
    }
    //cout << endl;
}
void initialize() {
    works.clear();
    aTable.clear();
    works.push_back(work(0, 0, 5));
    works.push_back(work(1, 5, 5));
    works.push_back(work(3, 10, 4));
    works.push_back(work(2, 26, 6));
    aTable.push_back(segment(14, 12));
    aTable.push_back(segment(32, 96));
}
int main() {
    initialize();
    printf("Initial Available Area:\n");
    displayAvailabeArea();
   // displayWork();
    ifstream inputFile("input.txt");
    string s;
    int workId, workSize;
    for (int i = 0; i < 3; i++) {
        if (i == 0)
            printf("\nFirst Fit Algorithm Starts:");
        else if (i == 1)
            printf("\nNext Fit Algorithm Starts:");
        else if (i == 2)
            printf("\nBest Fit Algorithm Starts:");
        while (inputFile >> s) {
            //store an assginment
            if (s == "Store") {
                inputFile >> workId >> workSize;
                int insertId;
                if (i == 0)
                    insertId = first_fit(workSize);
                else if (i == 1)
                    insertId = next_fit(workSize);
                else if (i == 2)
                    insertId = best_fit(workSize);
                works.push_back(work(workId,aTable[insertId].start, workSize));
                insertWorkAt(insertId, workSize);
                printf("\nInsert Assignment%d sized %d\n", workId, workSize);
                displayAvailabeArea();
            }
            //revoke an assignment
            else {
                inputFile >> workId;
                revokeWork(workId);
                printf("\nDelete Assignment%d\n", workId);
                displayAvailabeArea();
               //displayWork();
                }
         }
        inputFile.clear();
        inputFile.seekg(0, ios::beg);
        initialize();
    }
    return 0;
}