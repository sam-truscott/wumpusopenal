#ifdef _DEBUG
#define PI_VER2 "v0.9.0 b3 [DEBUG]"
#else
#define PI_VER2 "v0.9.0 b3"
#endif

#ifdef __alpha
#define PI_VER PI_VER2 " (AXP)"
#else
#define PI_VER PI_VER2 " (x86)"
#endif

#define TITLE "Wumpus OpenAL Output " PI_VER