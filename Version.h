#define PI_VER2 "v0.9.1"

#ifdef _DEBUG
#define PI_VER2 PI_VER2 "[DEBUG]"
#endif

#ifdef __alpha
#define PI_VER PI_VER2 " (AXP)"
#else
#define PI_VER PI_VER2 " (x86)"
#endif

#define TITLE "Wumpus OpenAL Output " PI_VER