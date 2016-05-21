#pragma once

#ifdef _DEBUG

#ifdef LONGDOUBLE
#pragma comment(lib,"c:/mytools2015/lib/MTDebug32/LDMath.lib")
#else
#pragma comment(lib,"c:/mytools2015/lib/MTDebug32/Math.lib")
#endif

#else

#ifdef LONGDOUBLE
#pragma comment(lib,"c:/mytools2015/lib/MTRelease32/LDMath.lib")
#else
#pragma comment(lib,"c:/mytools2015/lib/MTRelease32/Math.lib")
#endif

#endif