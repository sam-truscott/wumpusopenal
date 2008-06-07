#include "Clock.h"

#ifndef SSE_BUILD
namespace WinampOpenALOut {
#endif

	Time_Type Clock::initialTime = 0;

	void Clock::Initialise() {
		//initialTime = 0;
		//initialTime = GetTime();
		initialTime = GetTickCount();
	}

	Time_Type Clock::GetTime() {
		Time_Type time = GetTickCount() - initialTime;
		return time;
	}
#ifndef SSE_BUILD
}
#endif