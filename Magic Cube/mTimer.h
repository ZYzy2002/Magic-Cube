#pragma once

#include<chrono>

using namespace std::chrono;

class mTimer
{
private:
	steady_clock::time_point storeTime;
public:
	mTimer();
	long long getTime();
};

