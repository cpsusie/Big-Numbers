#pragma once

#ifdef __NEVER__

#pragma warning(disable:4786)
#include <fstream.h>
#define STREAMS_H_DEFINE
#include <id3.h>
#include <id3/tag.h>

#ifdef _DEBUG
#pragma comment(lib,"c:/mytools/lib/MTDebug/id3lib.lib")
#pragma comment(lib,"c:/mytools/lib/MTDebug/zlib.lib")
#else
#pragma comment(lib,"c:/mytools/lib/MTRelease/id3lib.lib")
#pragma comment(lib,"c:/mytools/lib/MTRelease/zlib.lib")
#endif

#endif // __NEVER__