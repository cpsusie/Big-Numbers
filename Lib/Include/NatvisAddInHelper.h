#pragma once

#define ADDIN_API __declspec(dllexport)

#if defined(_M_X64)
#error NatvisAddin should be compiled for x86. Not x64
#endif
#if defined(_DEBUG)
#error NatvisAddin should only bbe compiled in release mode. not debug
#endif

#include "MyUtil.h"
#include <comdef.h>
#include <atlconv.h>

typedef enum {
  PRTYPE_X86 = 0
 ,PRTYPE_X64 = 9
} ProcessorType;

typedef unsigned __int64 QWORD;

/* DebugHelper structure used from within the */
typedef struct tagDEBUGHELPER {
	DWORD dwVersion;
	BOOL (WINAPI *ReadDebuggeeMemory)( const struct tagDEBUGHELPER *pThis, DWORD dwAddr, DWORD nWant, VOID* pWhere, DWORD *nGot );
	// from here only when dwVersion >= 0x20000
	QWORD (WINAPI *GetRealAddress)( const struct tagDEBUGHELPER *pThis );
	BOOL (WINAPI *ReadDebuggeeMemoryEx)( const struct tagDEBUGHELPER *pThis, QWORD qwAddr, DWORD nWant, VOID* pWhere, DWORD *nGot );
	int (WINAPI *GetProcessorType)( const struct tagDEBUGHELPER *pThis );

  inline DWORD getVersion() const {
    return dwVersion;
  }
  inline ProcessorType getProcessorType() const {
    return (ProcessorType)GetProcessorType(this);
  }
  inline void *getRealObject(void *dst, size_t size) const {
    return getObjectx64(dst, GetRealAddress(this), size);
  }
  inline void *getObjectx86(void *dst, DWORD addr, int size) const {
    DWORD got;
    const BOOL hr = ReadDebuggeeMemory(this, addr, size, dst, &got);
    if(got != size) throwException(_T("undefined"));
    return dst;
  }
  inline void *getObjectx64(void *dst, QWORD addr, int size) const {
    DWORD got;
    const BOOL hr = ReadDebuggeeMemoryEx(this, addr, size, dst, &got);
    if(got != size) throwException(_T("undefined"));
    return dst;
  }

} DEBUGHELPER;
