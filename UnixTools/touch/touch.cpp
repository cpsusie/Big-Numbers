#include "stdafx.h"
#include <MyUtil.h>
#include <FileTreeWalker.h>
#include <Date.h>
#include <sys/utime.h>

class FileToucher : public FileNameHandler {
private:
  time_t m_touchTime;
public:
  FileToucher(time_t touchTime) {
    m_touchTime = touchTime;
  }
  void touch(const TCHAR *name);
  void handleFileName(const TCHAR *name, DirListEntry &info) {
    touch(name);
  }
};

void FileToucher::touch(const TCHAR *name) {
  struct _utimbuf tb;
  tb.actime = tb.modtime = m_touchTime;
  if(_tutime(name, &tb) < 0) {
    _tperror(name);
  }
}

static void usage(const char *cp = nullptr) {
  if(cp) {
    fprintf(stderr,"Invalid argument:%s\n", cp);
  }
  _ftprintf(stderr, _T("Usage:touch [-r] [-ddd/mm/yyyy hh:mm[:ss]] files...\n"
                       "            -r: recurse subdirectories.\n"
                       "            -ddd/mm/yyyy hh:mm[:ss]]: Specify the timestamp to use as the touchtime. Default is the system time.\n")
           );
  exit(-1);
}

int main(int argc, const char **argv) {
  const char *cp;
  time_t touchTime = Timestamp::getSystemTime();
  bool recurse = false;

  try {
    for(argv++; *argv && *(cp = *argv) == '-'; argv++ ) {
      for(cp++; *cp; cp++) {
        switch(*cp) {
        case 'd':
          { Date d(cp+1);
            argv++;
            Time t(*argv);
            Timestamp ts(d,t);
            touchTime = ts.gettime_t();
          }
          break;
        case 'r':
          recurse = true;
          continue;
        default :
          usage(cp);
        }
        break;
      }
    }

    if(*argv == nullptr) {
      usage();
    }
    FileTreeWalker::traverseArgv(argv, FileToucher(touchTime), recurse);
  } catch(Exception e) {
    _ftprintf(stderr, _T("touch:%s\n"), e.what());
    exit(-1);
  }
  return 0;
}
