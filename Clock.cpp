#include "Clock.h"

#ifndef SSE_BUILD
namespace WinampOpenALOut {
#endif

	Time_Type Clock::initialTime = 0;

	void Clock::Initialise() {
		initialTime = GetTickCount();
	}

	Time_Type Clock::GetTime() {
		Time_Type time = GetTickCount() - initialTime;
		return time;
	}
#ifndef SSE_BUILD
}
#endif