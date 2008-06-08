#include "Windows.h"
#include "Time.h"
#include "Constants.h"

#define Time_Type DWORD

#ifndef C_CLOCK
#define C_CLOCK

#ifndef SSE_BUILD
namespace WinampOpenALOut {
	public class Clock {
#else
	class Clock {
#endif

	public:

		static void Initialise();

		static Time_Type GetTime();

		static Time_Type Seconds(unsigned int s) {
			Time_Type t = s * ONE_SECOND_IN_MS;
			return t;
		}

		static Time_Type Milliseconds(unsigned int ms) {
			Time_Type t = ms;
			return t;
		}

	protected:

		static Time_Type initialTime;

	};

#ifndef SSE_BUILD
}
#endif

#endif
