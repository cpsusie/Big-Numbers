#pragma once

#include <Config.h>

#if defined(LONGDOUBLE)
#pragma comment(lib,TM_LIB_VERSION "LDExpression.lib")
#else
#pragma comment(lib,TM_LIB_VERSION "Expression.lib")
#endif

