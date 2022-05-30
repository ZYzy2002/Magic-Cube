#include "mTimer.h"

mTimer::mTimer()
{
	storeTime = steady_clock::now();
}

long long mTimer::getTime()
{
	steady_clock::time_point nowTime = steady_clock::now();
	steady_clock::duration time = nowTime - storeTime;
	return time.count();
}
