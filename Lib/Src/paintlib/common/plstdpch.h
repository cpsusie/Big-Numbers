/*
/--------------------------------------------------------------------
|
|      $Id: plstdpch_8h-source.html,v 1.3 2004/07/28 15:15:29 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

// Contains most system-specific includes and definitions. On windows
// systems, it corresponds to stdafx.h. On other systems, the
// appropriate data types and macros are declared here.

#ifndef INCL_PLSTDPCH
#define INCL_PLSTDPCH

#include "plobject.h"

#ifdef _WINDOWS
#define WIN32_LEAN_AND_MEAN  /* Prevent including <winsock*.h> in <windows.h> */
#define VC_EXTRALEAN  // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <stdlib.h> 
#include <malloc.h>  
#else
#include <stdlib.h>
#endif

#include "pldebug.h"

#endif // #ifndef _STDPCH_H_

