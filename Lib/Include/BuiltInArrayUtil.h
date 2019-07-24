#pragma once

#ifdef ARRAYSIZE
#undef ARRAYSIZE
#endif

#define ARRAYSIZE(a)        (sizeof(a)/sizeof((a)[0]))
#define LASTVALUE(a)        (a[ARRAYSIZE(a)-1])
