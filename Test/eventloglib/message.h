#pragma once

#include <MyUtil.h>

void eventLogError(  _In_z_ _Printf_format_string_ TCHAR const * const format,...);
void eventLogWarning(_In_z_ _Printf_format_string_ TCHAR const * const format,...);
void eventLogInfo(   _In_z_ _Printf_format_string_ TCHAR const * const format,...);
void eventLogSuccess(_In_z_ _Printf_format_string_ TCHAR const * const format,...);

#define EVENTLOGLIBROOT "c:/mytools2015/test/eventlogLib/"

#define EVENTLOGLIB_VERSION EVENTLOGLIBROOT _PLATFORM_ _CONFIGURATION_

#ifdef GIVE_LINK_MESSAGE
#pragma message("link with " LIB_VERSION)
#endif
#pragma comment(lib, EVENTLOGLIB_VERSION "EventLogLib.lib")
