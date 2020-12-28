#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H
#include <ctime>
#include <string>
using std::string;
class TimeManager
{
public:
    TimeManager();
    time_t TimeGet();
    void cTimeChange();
    void mTimeChange();
    int TimeFactor(time_t t,string s);
private:
    time_t cTime, mTime;
};

#endif // TIMEMANAGER_H
