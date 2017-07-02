#include "stdafx.h"
#include <MyUtil.h>
#include <Console.h>
#include <fcntl.h>
#include <FileTreeWalker.h>

#define BUFSIZE 0x40000

class FileCat : public FileNameHandler {
private:
  char      *m_buffer;
  FILE      *m_output;
  int        m_oldmode;
  const bool m_verbose;
  const int  m_screenWidth;
public:
  FileCat(FILE *output, bool verbose);
  ~FileCat();

  void cat(const TCHAR *name);
  void handleFileName(const TCHAR *name, DirListEntry &info);
};

FileCat::FileCat(FILE *output, bool verbose)
: m_verbose(verbose)
, m_screenWidth(Console::getWindowSize(STD_ERROR_HANDLE).X - 1)
{
  m_output  = output;
  m_oldmode = setFileMode(m_output, _O_BINARY);
  setvbuf(m_output,NULL,_IOFBF,BUFSIZE);
  m_buffer = new char[BUFSIZE];
}

FileCat::~FileCat() {
  fflush(m_output);
  setFileMode(m_output,m_oldmode);
  delete[] m_buffer;
}

void FileCat::cat(const TCHAR *fname) {
  FILE *input = FOPEN(fname, _T("rb"));
  try {
    setvbuf(input,NULL,_IOFBF,BUFSIZE);
    if(m_verbose) {
      _ftprintf(stderr, _T("%-*.*s\r"), m_screenWidth, m_screenWidth, fname);
    }
    size_t r;

    while(r = fread(m_buffer, 1, BUFSIZE, input)) {
      if(fwrite(m_buffer, 1, r, m_output) != r) {
        throwException(_T("write failed"));
      }
    }
    fclose(input);
  } catch(...) {
    fclose(input);
    throw;
  }
}

void FileCat::handleFileName(const TCHAR *name, DirListEntry &info) {
  cat(name);
}

static void usage() {
  _ftprintf(stderr, _T("Usage:cat [-vr] files\n"));
  exit(-1);
}

int main(int argc, char **argv) {
  char *cp;
  bool verbose = false;
  bool recurse = false;

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'v':
        verbose = true;
        continue;
      case 'r':
        recurse = true;
        continue;
      default :
        usage();
      }
      break;
    }
  }

  try {
    if(!*argv) {
      usage();
    }
    FileTreeWalker::traverseArgv(argv, FileCat(stdout,verbose), recurse);
  } catch(Exception e) {
    _ftprintf(stderr, _T("cat:%s\n"),e.what());
    return -1;
  }

  return 0;
}
