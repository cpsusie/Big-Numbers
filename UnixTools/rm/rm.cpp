#include "stdafx.h"
#include <MyUtil.h>
#include <FileTreeWalker.h>

class FileRemover : public FileNameHandler {
private:
  bool m_force;
  bool m_rmdirs;
  bool m_verbose;
public:
  FileRemover(bool force, bool rmdirs, bool verbose);
  void removeFile(const TCHAR *fname);
  void handleFileName(const TCHAR *name, DirListEntry &info);
  void handleEndDir(  const TCHAR *name);
};

FileRemover::FileRemover(bool force, bool rmdirs, bool verbose) {
  m_force       = force;
  m_rmdirs      = rmdirs;
  m_verbose     = verbose;
}

void FileRemover::removeFile(const TCHAR *fname) {
  try {
    if(m_verbose) {
      _tprintf(_T("removing %s\n"),fname);
    }
    UNLINK(fname);
  } catch(Exception e) {
    if(!m_force) {
      _ftprintf(stderr, _T("Cannot remove %s:%s\n"), fname, e.what());
    } else {
      try {
        CHMOD(fname,_S_IREAD | _S_IWRITE);
        UNLINK(fname);
      } catch(Exception e) {
        _ftprintf(stderr, _T("Cannot remove %s:%s\n"), fname, e.what());
      }
    }
  }
}

void FileRemover::handleFileName(const TCHAR *name, DirListEntry &info) {
  removeFile(name);
}

void FileRemover::handleEndDir(const TCHAR *name) {
  if(_tcslen(name) == 0) {
    return;
  }
  if(!m_rmdirs) {
    return;
  }
  try {
    RMDIR(name);
  } catch(Exception e) {
    _ftprintf(stderr, _T("Cannot remove dir %s\n"), e.what());
  }
}

static void usage()  {
  fprintf(stderr,"Usage:rm [-rfdv] files\n"
                 "      -r:Recurse subdirs\n"
                 "      -f:Force remove, if file is readonly\n"
                 "      -d:Don't remove directories\n"
                 "      -v:Verbose\n"
         );
  exit(-1);
}

int main(int argc, const char **argv) {
  bool recurse   = false;
  bool force     = false;
  bool rmdirs    = true;
  bool verbose   = false;
  const char *cp;

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'r': recurse = true;  continue;
      case 'f': force   = true;  continue;
      case 'd': rmdirs  = false; continue;
      case 'v': verbose = true;  continue;
      default : usage();
      }
      break;
    }
  }

  if(*argv == NULL) {
    usage();
  }

  FileTreeWalker::traverseArgv(argv, FileRemover(force, rmdirs, verbose), recurse);

  return 0;
}
