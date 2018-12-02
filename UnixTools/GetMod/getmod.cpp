#include "stdafx.h"

class ModulePrinter {
private:
  const bool     m_uniqueName;
  const bool     m_showDlls;
  const bool     m_showCmdLine;
  const bool     m_sortByPath;
  StringIHashSet m_moduleNameSet;
public:
  ModulePrinter(bool uniqName, bool showDlls, bool showCmdLine, bool sortByPath);
  Array<ProcessInfo> getProcessInfo();
  void printProcesses();
  void printProcessInfo(const ProcessInfo &prInfo);
};

ModulePrinter::ModulePrinter(bool uniqueName, bool showDlls, bool showCmdLine, bool sortByPath)
: m_uniqueName( uniqueName )
, m_showDlls(   showDlls   )
, m_showCmdLine(showCmdLine)
, m_sortByPath( sortByPath )
{
}

static int ulongCmp(const DWORD &l1, const DWORD &l2) {
  return l1 > l2 ? 1 : l1 < l2 ? -1 : 0;
}

static int processInfoCmpProcessId(const ProcessInfo &p1, const ProcessInfo &p2) {
  return ulongCmp(p1.getProcessId(), p2.getProcessId());
}

static int processInfoCmpProcessPath(const ProcessInfo &p1, const ProcessInfo &p2) {
  return p1.getExePath().equalsIgnoreCase(p2.getExePath()) ? 0 : 1;
}

Array<ProcessInfo> ModulePrinter::getProcessInfo() {
  const CompactArray<DWORD> processIds = getProcessIds();
  Array<ProcessInfo> result;
  enableTokenPrivilege(SE_SECURITY_NAME, true);
  enableTokenPrivilege(SE_DEBUG_NAME   , true);
  for(size_t i = 0; i < processIds.size(); i++) {
    result.add(ProcessInfo(processIds[i]));
  }
  return result;
}

void ModulePrinter::printProcesses() {
  Array<ProcessInfo> processArray = getProcessInfo();
  if(m_sortByPath) {
    processArray.sort(processInfoCmpProcessPath);
  } else  {
    processArray.sort(processInfoCmpProcessId);
  }
  printf("%-8s %8s %-50s \n","pr.Id", "Size","Path");
  for(size_t i = 0; i < processArray.size(); i++) {
    printProcessInfo(processArray[i]);
  }
}

void ModulePrinter::printProcessInfo(const ProcessInfo &prInfo) {
  const Array<ModuleInfo> &moduleArray = prInfo.getModuleArray();
  for(size_t i = 0; i < moduleArray.size(); i++) {
    const ModuleInfo &module = moduleArray[i];
    if(m_uniqueName && m_moduleNameSet.contains(module.getPath())) {
      continue;
    }
    m_moduleNameSet.add(module.getPath());
    if(i == 0) { // this is the exe-file
      _tprintf(_T("%-8d %8d %s"), prInfo.getProcessId(), module.getSize(), module.getPath().cstr());
      if(m_showCmdLine) {
        _tprintf(_T(" %s"), prInfo.getCommandLine().cstr());
      }
      _tprintf(_T("\n"));
      if(!m_showDlls) {
        return;
      }
    } else {
      _tprintf(_T("%-8s %8d %s\n"), _T("----"), module.getSize(), module.getPath().cstr());
    }
  }
}

static void usage() {
  fprintf(stderr,"getmod [-ue]\n"
                 "      -u: Print only modules once.\n"
                 "      -d: Print dlls's loaded for exe-file\n"
                 "      -c: Print process commandline\n"
                 "      -p: Sort processes by Path\n"
         );
  exit(-1);
}

int main(int argc, char **argv) {
  bool uniqueName  = false;
  bool showDlls    = false;
  bool showCmdLine = false;
  bool sortByPath  = false;

  char *cp;
  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'u': uniqueName  = true; continue;
      case 'd': showDlls    = true; continue;
      case 'c': showCmdLine = true; continue;
      case 'p': sortByPath  = true; continue;
      default : usage();
      }
    }
    break;
  }

  try {
    ModulePrinter(uniqueName, showDlls, showCmdLine, sortByPath).printProcesses();
  } catch(Exception e) {
    _ftprintf(stderr, _T("%s\n"), e.what());
  }
  return 0;
}
