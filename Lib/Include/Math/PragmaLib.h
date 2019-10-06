#pragma once

#include <Config.h>

#ifdef LONGDOUBLE
#pragma comment(lib,TM_LIB_VERSION "LDMath.lib")
#else
#pragma comment(lib,TM_LIB_VERSION "Math.lib")
#endif

