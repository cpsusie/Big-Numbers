#pragma once

#include <MyUtil.h>

void eventLogError(  const TCHAR *format,...);
void eventLogWarning(const TCHAR *format,...);
void eventLogInfo(   const TCHAR *format,...);
void eventLogSuccess(const TCHAR *format,...);

#define EVENTLOGLIBROOT "c:/mytools2015/test/eventlogLib/"

#define EVENTLOGLIB_VERSION EVENTLOGLIBROOT _PLATFORM_ _CONFIGURATION_

#ifdef GIVE_LINK_MESSAGE
#pragma message("link with " LIB_VERSION)
#endif
#pragma comment(lib, EVENTLOGLIB_VERSION "EventLogLib.lib")
