#include "stdafx.h"

class ModulePrinter {
private:
  bool          m_uniqueName;
  bool          m_exeOnly;
  bool          m_sortByPath;
  StringHashSet m_moduleNameSet;
public:
  ModulePrinter(bool uniqName, bool exeOnly, bool sortByPath);
  Array<ProcessInfo> getProcessInfo();
  void printProcesses();
  void printProcessInfo(const ProcessInfo &prInfo);
};

ModulePrinter::ModulePrinter(bool uniqueName, bool exeOnly, bool sortByPath) {
  m_uniqueName = uniqueName;
  m_exeOnly    = exeOnly;
  m_sortByPath = sortByPath;
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
  CompactArray<DWORD> processIds = getProcessIds();
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
    if(i == 0) {
      _tprintf(_T("%-8d %8d %s\n"), prInfo.getProcessId(), module.getSize(), module.getPath().cstr());
      if(m_exeOnly) {
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
                 "      -e: Print only Exe-files\n"
                 "      -p: Sort processes by Path\n"
         );
  exit(-1);
}

int main(int argc, char **argv) {
  bool uniqueName = false;
  bool exeOnly    = false;
  bool sortByPath = false;

  char *cp;
  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'u': uniqueName = true; continue;
      case 'e': exeOnly    = true; continue;
      case 'p': sortByPath = true; continue;
      default : usage();
      }
    }
    break;
  }

  try {
    ModulePrinter(uniqueName,exeOnly, sortByPath).printProcesses();
  } catch(Exception e) {
    _ftprintf(stderr, _T("%s\n"), e.what());
  }
  return 0;
}
