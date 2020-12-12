#include "include/timemanager.h"
using std::string;
TimeManager::TimeManager()
{
	cTime = mTime = TimeGet();
}
time_t TimeManager::TimeGet()
{
	time_t nowTime;
	time(&nowTime);
	return nowTime;
};
void TimeManager::cTimeChange()
{
	cTime = TimeGet();
}
void TimeManager::mTimeChange()
{
	mTime = TimeGet();
}
int TimeManager::TimeFactor(time_t t, string s)
{
	struct tm * p;
	p = localtime(&t);
	int ans = -1;
	switch (s[0])
	{
		case 'y':ans = p->tm_year; break;
		case 'd':ans = p->tm_mday; break;
		case 'h':ans = p->tm_hour; break;
		case 's':ans = p->tm_sec; break;
		case 'm':if (s[1] == 'o')ans = p->tm_mon; else ans = p->tm_min; break;
	}
	delete p;
	return ans;
}