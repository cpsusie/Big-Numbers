#include "stdafx.h"
#include <MyUtil.h>
#include <Stack.h>
#include <FileTreeWalker.h>

#define Mbyte (1024.0*1024.0)

class ByteCount {
public:
  double m_fileBytes;
  double m_dirTotal;
  ByteCount();
};

ByteCount::ByteCount() {
  m_fileBytes = m_dirTotal = 0;
}

class CharCounter : public FileNameHandler {
private:
  const bool    m_printSubTotals;
  const bool    m_printTotalFirst;
  const bool    m_verbose;
  Stack<ByteCount> m_dirStack;
  void printInfo(const TCHAR *path, const ByteCount &byteCount);
public:
  CharCounter(bool printSubTotals, bool printTotalFirst, bool verbose);
  void handleFileName(const TCHAR *name, DirListEntry &info);
  void handleStartDir(const TCHAR *name);
  void handleEndDir(  const TCHAR *name);
};

CharCounter::CharCounter(bool printSubTotals, bool printTotalFirst, bool verbose)
: m_printSubTotals(printSubTotals)
, m_printTotalFirst(printTotalFirst)
, m_verbose(verbose)
{
}

void CharCounter::printInfo(const TCHAR *path, const ByteCount &byteCount) {
  if(m_printTotalFirst) {
    _tprintf(_T("%8.2lf Mb %-50s %8.2lf Mb\n"), byteCount.m_dirTotal/Mbyte,path, byteCount.m_fileBytes/Mbyte);
  } else {
    _tprintf(_T("%8.2lf Mb %-50s %8.2lf Mb\n"), byteCount.m_fileBytes/Mbyte,path, byteCount.m_dirTotal/Mbyte);
  }
}

void CharCounter::handleStartDir(const TCHAR *name) {
  if(m_verbose) {
    _ftprintf(stderr,_T("Scanning %-70.70s\r"), name);
  }
  m_dirStack.push(ByteCount());
}

void CharCounter::handleEndDir(const TCHAR *name) {
  ByteCount byteCount = m_dirStack.pop();
  if(m_dirStack.isEmpty()) {
    printInfo(name, byteCount);
  } else {
    if(m_printSubTotals) {
      printInfo(name, byteCount);
    }
    m_dirStack.top().m_dirTotal += byteCount.m_dirTotal;
  }
}

void CharCounter::handleFileName(const TCHAR *name, DirListEntry &info) {
  ByteCount &count = m_dirStack.top();
  count.m_fileBytes += info.size;
  count.m_dirTotal  += info.size;
}

static void usage() {
  fprintf(stderr,"Usage:du [-stv] [dir]\n"
                 "      -s: Dont print subtotals for each subdirectory.\n"
                 "      -t: Print total, including subdirectories,  at the start of each line.\n"
                 "          Default is total, exclusive subdirectories, at the start of each line.\n"
                 "          The total will default be at the end of the line.\n"
                 "      -v: Verbose. print directorynames to stderr, while scanning.\n"
         );
  exit(-1);
}

int main(int argc, char **argv) {
  char *cp;
  bool printSubTotals  = true;
  bool printTotalFirst = false;
  bool verbose         = false;

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++;*cp; cp++) {
      switch(*cp) {
      case 's':
        printSubTotals = false;
        continue;
      case 't':
        printTotalFirst = true;
        continue;
      case 'v':
        verbose = true;
        continue;
      default :
        usage();
      }
      break;
    }
  }

  const char *dir = *argv ? *argv : ".";
  FileTreeWalker::walkFileTree(dir, CharCounter(printSubTotals, printTotalFirst, verbose));
  return 0;
}

