#include "stdafx.h"
#include <Console.h>
#include <ProcessTools.h>

static int dwordcmp(const DWORD &p1, const DWORD &p2) {
  return p1 - p2;
}

double getMemoryUsage() {
  try {
    enableTokenPrivilege(SE_DEBUG_NAME,true);
  } catch(Exception e) {
    _tprintf(_T("%s\n"),e.what());
  }

  double total = 0;
  CompactArray<DWORD> processIds = getProcessIds();
  processIds.sort(dwordcmp);
  for(size_t i = 0; i < processIds.size(); i++) {
    try {
      PROCESS_MEMORY_COUNTERS pm = getProcessMemoryUsage(processIds[i]);
      total += pm.WorkingSetSize;
    } catch(Exception e) {
//    _tprintf("%d:%s\n",ProcessIDs[i],e.what());
    }
  }
  return total;
}

#define KByte 1024.0
#define MByte (KByte*KByte)

static double MegaBytes(double bytes) {
  return bytes / MByte;
}

static double KiloBytes(double bytes) {
  return bytes / KByte;
}

typedef enum {
  KBYTE
 ,MBYTE
} ByteFormat;

class MemUsagePrinter {
private:
  ByteFormat m_format;
  String formatMemoryAmount(double bytes);
public:
  void show();
  MemUsagePrinter(ByteFormat format = MBYTE) {
    m_format = format;
  }
};

static String add1000Points(const String &s) {
  String result(s);
  intptr_t comma = result.find(_T('.'));
  if(comma < 0) {
    comma = result.length()-1;
  } else {
    result[comma] = _T(',');
  }
  for(comma -= 3; comma > 0 && _istdigit(result[comma]); comma -= 3) {
    result.insert(comma,_T('.'));
  }
  return result;
}

String MemUsagePrinter::formatMemoryAmount(double bytes) {
  switch(m_format) {
  case KBYTE:
    return format(_T("%15s"), add1000Points(format(_T("%.2lf"), KiloBytes(bytes))).cstr()) + " Kb";
  case MBYTE:
    return format(_T("%15s"), add1000Points(format(_T("%.2lf"), MegaBytes(bytes))).cstr()) + " Mb";
  }
  return _T("");
}

void MemUsagePrinter::show() {
  double processUsage = getMemoryUsage();

  PERFORMANCE_INFORMATION pi;
  GetPerformanceInfo(&pi,sizeof(pi));
  
  _tprintf(_T("ProcessUsage  :%s\n"), formatMemoryAmount(processUsage).cstr());
  _tprintf(_T("KernelPaged   :%s\n"), formatMemoryAmount((double)pi.KernelPaged*pi.PageSize).cstr());
  _tprintf(_T("KernelNonpaged:%s\n"), formatMemoryAmount((double)pi.KernelNonpaged*pi.PageSize).cstr());
  _tprintf(_T("KernelTotal   :%s\n"), formatMemoryAmount((double)pi.KernelTotal*pi.PageSize).cstr());
  _tprintf(_T("SystemCache   :%s\n"), formatMemoryAmount((double)pi.SystemCache*pi.PageSize).cstr());
  _tprintf(_T("Available     :%s\n"), formatMemoryAmount((double)pi.PhysicalAvailable*pi.PageSize).cstr());
  _tprintf(_T("_______________________\n"));
  const double sum = processUsage + (double)pi.KernelTotal*pi.PageSize + (double)pi.PhysicalAvailable*pi.PageSize;
  _tprintf(_T("Sum           :%s\n"), formatMemoryAmount(sum).cstr());

  _tprintf(_T("RAM (total)   :%s\n"), formatMemoryAmount((double)pi.PhysicalTotal*pi.PageSize).cstr());
  _tprintf(_T("Difference    :%s\n"), formatMemoryAmount((double)pi.PhysicalTotal*pi.PageSize - sum).cstr());
}

static void usage() {
  _ftprintf(stderr,_T("findmemusage: invalid argument.\n"));
  _ftprintf(stderr,_T("Usage:findmemusage [-mk] [-sseconds]\n"));
  exit(-1);
}

int main(int argc, char **argv) {
  char      *cp;
  bool       monitoring = false;
  ByteFormat format     = MBYTE;
  int        seconds    = 1;

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'm':
        monitoring = true; 
        continue;
      case 'k':
        format = KBYTE;
        continue;
      case 's':
        if(sscanf(cp+1, "%d", &seconds) != 1) {
          usage();
        }
        if(seconds < 1) {
          seconds = 1;
        }
        monitoring = true;
        break;
      default :
        usage();
      }
      break;
    }
  }

  if(monitoring) {
    Console::clear();
    for(;;) {
      Console::setCursorPos(0,0);
      MemUsagePrinter(format).show();
      Sleep(seconds*1000);
    }
  } else {
    MemUsagePrinter(format).show();
  }
  return 0;
}
