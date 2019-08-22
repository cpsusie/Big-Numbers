#include "stdafx.h"
#include <FileTreeWalker.h>
#include "ResourceFile.h"

//#define DUMPSYMBOLS

class ResourceChecker : public FileNameHandler {
private:
  bool m_ok;
  bool m_verbose;
  UINT m_fileCheckCount;
public:
  ResourceChecker(bool verbose) : m_verbose(verbose), m_fileCheckCount(0) {
    m_ok = true;
  }
  void checkResources(const String &fileName);
  void handleFileName(const TCHAR *name, DirListEntry &info);
  inline bool isOk() const {
    return m_ok;
  }
  inline UINT getFilesChecked() const {
    return m_fileCheckCount;
  }
};

void ResourceChecker::checkResources(const String &fileName) {
#ifdef DUMPSYMBOLS
  FILE *log = NULL;
#endif

  try {

#ifdef DUMPSYMBOLS
    log = MKFOPEN(_T("c:\\temp\\checkMnemonics.txt"), _T("w"));
#endif
    if(m_verbose) {
      _ftprintf(stderr, _T("Analyzing %s                                                      \r"), fileName.cstr());
    }
    m_fileCheckCount++;
    ResourceFile rf(fileName);
    if(!rf.isOk()) {
      rf.listErrors();
      m_ok = false;
    }
    if(!rf.isEmpty()) {

#ifdef DUMPSYMBOLS
      _ftprintf(log, _T("%s\n"), rf.toString().cstr());
#endif
      rf.analyze();
      if(!rf.isOk()) {
        rf.listErrors();
        m_ok = false;
      }
    }

#ifdef DUMPSYMBOLS
    fclose(log);
#endif

  } catch(Exception e) {
#ifdef DUMPSYMBOLS
    if(log) {
      fclose(log);
    }
#endif
    _tprintf(_T("Exception:%s\n"), e.what());
    m_ok = false;
  }
}

void ResourceChecker::handleFileName(const TCHAR *name, DirListEntry &info) {
  checkResources(name);
}

static void usage() {
  _ftprintf( stderr, _T("Usage:CheckMnemonics [-rv] files\n"));
  exit(-1);
}

int main(int argc, const char **argv) {
  bool   recurse = false;
  bool   verbose = false;
  const char *cp;
// ResourceLex::findBestHashMapSize();
// return 0;

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'r':
        recurse = true;
        continue;
      case 'v':
        verbose = true;
        continue;
      default:
        usage();
      }
    }
  }

  try {
    if(!*argv) {
      _ftprintf(stderr, _T("No argument\n"));
      return -1;
    } else {
      ResourceChecker checker(verbose);
      FileTreeWalker::traverseArgv(argv, checker, recurse);
      if(checker.getFilesChecked() == 0) {
        _ftprintf(stderr, _T("No files checked\n"));
        return -1;
      } else if(checker.isOk()) {
        _ftprintf(stderr, _T("No errors found\n"));
        return 0;
      } else {
        return -1;
      }
    }
  } catch (Exception e) {
    _ftprintf(stderr, _T("Exception:%s\n"), e.what());
    return -1;
  }
}
