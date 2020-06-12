#pragma once

#include <Config.h>

#if defined(GIVE_LINK_MESSAGE)
#pragma message("link with " TM_LIB_VERSION)
#endif
#pragma comment(lib, TM_LIB_VERSION "MFCUtil.lib")
