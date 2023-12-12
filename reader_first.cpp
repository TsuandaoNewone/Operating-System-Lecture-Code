#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <fstream>
#include <vector>
#include <condition_variable>
#include <algorithm>
#define maxThreads 100
using namespace std;
void reader(int id, char userType, double startTime, double endureTiem);
void writer(int id, char userType, double startTime, double endureTiem);
condition_variable cv;
mutex mtx;
bool write = false;
bool ready = false;
int readerCnt = 0;
struct USER {
	int id;
	double startTime;
	USER(int id_, double startTime_) : id(id_), startTime(startTime_) {}
};
vector<USER> writers;
bool compare(USER a, USER b) {
	return a.startTime > b.startTime;
}
thread threads[maxThreads];
int main()
{
	ifstream inputFile("input.txt");
	if (!inputFile.is_open()) {
		cerr << "Failed to open the file.\n";
		return 1;
	}
	int cnt = 0;
	int id;
	char userType;
	double startTime, endureTime;
	while (inputFile >> id >> userType >> startTime >> endureTime) {
		printf("%d %c %f %f\n", id, userType, startTime, endureTime);
		if (userType == 'R') {
			threads[cnt++] = thread(reader, id, userType, startTime, endureTime);
		}
		else {
			threads[cnt++] = thread(writer, id, userType, startTime, endureTime);
			writers.push_back(USER(id, startTime));
		}
	}
	sort(writers.begin(), writers.end(), compare);
	{
		lock_guard<std::mutex> lock(mtx);
		// threads all created, wake them up
		ready = true;
		cv.notify_all();
	}
	//check before join a thread
	for (auto& a : threads) {
		if (a.joinable())
			a.join();
	}
	return 0;
}
void reader(int id, char userType, double startTime, double endureTime) {
	{
		unique_lock<mutex> lk(mtx);
		cv.wait(lk, [] {return ready; });
	}
	this_thread::sleep_for(chrono::milliseconds((long long)(startTime * 1000)));
	printf("reader %d request to read\n", id);
	//stuck until no one's writing
	while (true) {
		//reader-first requirements:
		if (!write || readerCnt > 0) {
			lock_guard<mutex> lk(mtx);
			readerCnt++;
			printf("reader %d is reading\n", id);
			break;
		}
	}
	this_thread::sleep_for(chrono::milliseconds((long long)(endureTime * 1000)));
	{
		lock_guard<mutex> lk(mtx);
		readerCnt--;
		printf("reader %d finished reading\n", id);
	}
}
void writer(int id, char userType, double startTime, double endureTiem) {
	{
		unique_lock<mutex> lk(mtx);
		cv.wait(lk, [] {return ready; });
	}
	this_thread::sleep_for(chrono::milliseconds((long long)(startTime * 1000)));
	printf("writer %d request to write\n", id);
	while (true) {
		lock_guard<mutex> lk(mtx);
		if (readerCnt == 0 && writers.back().id == id) {
			write = true;
			printf("writer %d is writing\n", id);
			this_thread::sleep_for(chrono::milliseconds((long long)(startTime * 1000)));
			write = false;
			printf("writer %d finished writing\n", id);
			if (!writers.empty())
				writers.pop_back();
			break;
		}
	}
}