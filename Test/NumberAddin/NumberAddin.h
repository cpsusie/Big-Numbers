#pragma once

#define ADDIN_API __declspec(dllexport)

#ifdef _M_X64
#error NumberAddin should be compiled for x86. Not x64
#endif
#ifdef _DEBUG
#error NumberAddin should only bbe compiled in release mode. not debug
#endif

typedef enum {
  PRTYPE_X86 = 0
 ,PRTYPE_X64 = 9
} ProcessorType;

/* DebugHelper structure used from within the */
typedef struct tagDEBUGHELPER {
	DWORD dwVersion;
	BOOL (WINAPI *ReadDebuggeeMemory)( const struct tagDEBUGHELPER *pThis, DWORD dwAddr, DWORD nWant, VOID* pWhere, DWORD *nGot );
	// from here only when dwVersion >= 0x20000
	unsigned __int64 (WINAPI *GetRealAddress)( const struct tagDEBUGHELPER *pThis );
	BOOL (WINAPI *ReadDebuggeeMemoryEx)( const struct tagDEBUGHELPER *pThis, unsigned __int64 qwAddr, DWORD nWant, VOID* pWhere, DWORD *nGot );
	int (WINAPI *GetProcessorType)( const struct tagDEBUGHELPER *pThis );

  DWORD getVersion() const {
    return dwVersion;
  }
  void *getRealObject(void *pobj, size_t size) const;
  ProcessorType getProcessorType() const;
} DEBUGHELPER;


  /* Exported Functions */
  ADDIN_API HRESULT WINAPI AddIn_BigReal(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/);
  ADDIN_API HRESULT WINAPI AddIn__int128(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/);
  ADDIN_API HRESULT WINAPI AddIn__uint128(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/);
