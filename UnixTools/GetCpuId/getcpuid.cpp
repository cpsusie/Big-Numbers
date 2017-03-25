#include "stdafx.h"
#include <CPUInfo.h>

static void printfSystemInfo() {
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  _tprintf(_T("#Processors:%d\n"), info.dwNumberOfProcessors);
}

static void usage() {
  _ftprintf(stderr,_T("Usage:getcpuid [-l] [-w]\n"));
  exit(-1);
}

int main(int argc, char **argv) {
  char *cp;
  bool longdesc    = false;
  bool windowsCall = false;

  try {
//  printf("sizeof(FeatureInfo):%d\n", sizeof(FeatureInfo));
    for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
      for(cp++; *cp; cp++) {
        switch(*cp) {
        case 'l': longdesc    = true; continue;
        case 'w': windowsCall = true; continue;
        default : usage();
        }
        break;
      }
    }

    if(windowsCall) {
      printfSystemInfo();
    } else {
      if(CPUInfo::CPUSupportsId()) {
        CPUInfo c;
        _tprintf(_T("Vendor           :%s\n")   , c.m_vendor.cstr());
        _tprintf(_T("BrandString      :%s\n")   , c.m_processorBrandString.cstr());
        _tprintf(_T("Serialnumber     :%I64u\n"), c.m_processorSerialNumber);
        _tprintf(_T("No. of processors:%d\n")   , c.m_CPUBasics.numProcessors);
        _tprintf(_T("Processortype    :%s\n")   , c.m_processorType.cstr());
        _tprintf(_T("Family           :%d\n")   , c.m_versionInfo.family);
        _tprintf(_T("Model            :%d\n")   , c.m_versionInfo.model);
        _tprintf(_T("Stepping Id      :%d\n")   , c.m_versionInfo.steppingId);
        _tprintf(_T("FeatureInfo      :%s\n")   , c.m_featureInfo.toString(longdesc).cstr());
      }
    }
  } catch (Exception e) {
    _ftprintf(stderr, _T("Exception:%s\n"), e.what());
    return -1;
  }
  return 0;
}
