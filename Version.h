#define PI_VER2 "v0.8.[b7]"

#ifdef __alpha
#define PI_VER PI_VER2 " (AXP)"
#else

#ifndef SSE_BUILD
#define PI_VER PI_VER2 " (x86)"
#else
#define PI_VER PI_VER2 " (SSE2) (x86)"
#endif

#endif

#define TITLE "Wumpus OpenAL Output " PI_VER