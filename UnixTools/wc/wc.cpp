#include "stdafx.h"
#include <MyUtil.h>
#include <Console.h>
#include <FileTreeWalker.h>
#include <io.h>

class CountResult {
public:
  ULONG m_lineCount;
  ULONG m_wordCount;
  ULONG m_charCount;
  CountResult() {
    reset();
  }
  void reset();
  void print(const String &label) const;
  CountResult &operator+=(const CountResult &r);
};

void CountResult::reset() {
  m_lineCount = 0;
  m_wordCount = 0;
  m_charCount = 0;
}

CountResult &CountResult::operator+=(const CountResult &r) {
  m_lineCount += r.m_lineCount;
  m_wordCount += r.m_wordCount;
  m_charCount += r.m_charCount;
  return *this;
}

void CountResult::print(const String &label) const {
  _tprintf(_T("%9lu %9lu %9lu   %s\n"), m_lineCount, m_wordCount, m_charCount, label.cstr());
}

static bool wordDelimiter[256];

static void initWordDelimiter() {
  for(int ch = 0; ch < 256; ch++) {
    wordDelimiter[ch] = false;
  }
  wordDelimiter['\t'] = wordDelimiter[' '] = wordDelimiter['\n'] = true;
}

static int countWords(const TCHAR *line) {
  int count = 0;
  for(TCHAR *cp = (TCHAR*)line; *cp;) {
    while(wordDelimiter[(BYTE)*cp]) cp++;
    if(*cp) {
      count++;
      cp++;
      while(*cp && !wordDelimiter[(BYTE)*cp]) {
        cp++;
      }
    }
  }
  return count;
}

#define BUFSIZE 0x40000

class WordCounter : public FileNameHandler {
private:
  bool        m_printFileContent, m_printSubtotals, m_verbose;
  int         m_screenWidth;
  int         m_filecount, m_subFileCount;
  CountResult m_subTotal, m_total;
  String      m_currentDirName;
  void printSubtotal();
public:
  WordCounter(bool printFileContent, bool printSubtotals, bool verbose = false);
  void handleFileName(const TCHAR *name, DirListEntry &info);
  void handleStartDir(const TCHAR *name);
  void handleEndDir(const TCHAR *name);
  void wc(const TCHAR *name, FILE *f);
  void wc(const TCHAR *name);
  void printTotal();
};

WordCounter::WordCounter(bool printFileContent, bool printSubtotals, bool verbose) {
  m_printFileContent = printFileContent;
  m_printSubtotals   = printSubtotals;
  m_verbose          = verbose;
  m_screenWidth      = Console::getWindowSize(STD_ERROR_HANDLE).X-1;
  m_filecount        = m_subFileCount = 0;
}

void WordCounter::wc(const TCHAR *name, FILE *f) {
  TCHAR line[4096];

  CountResult result;

  setvbuf(f,NULL,_IOFBF,BUFSIZE);
  while(_fgetts(line, ARRAYSIZE(line),f)) {
    result.m_lineCount++;
    result.m_charCount += (int)_tcslen(line) + 1;
    result.m_wordCount += countWords(line);
  }
  if(m_printFileContent) {
    result.print(name);
  }

  m_subFileCount++;
  m_filecount++;
  m_subTotal += result;
  m_total    += result;
}

void WordCounter::handleFileName(const TCHAR *name, DirListEntry &info) {
  wc(name);
}

void WordCounter::wc(const TCHAR *name) {
  FILE *f = fopen(name,_T("r"));
  if(f == NULL) {
    _tperror(name);
    return;
  }
  wc(name,f);
  fclose(f);
}

void WordCounter::handleStartDir(const TCHAR *name) {
  printSubtotal();
  m_subTotal.reset();
  m_subFileCount   = 0;
  m_currentDirName = name;

  if(m_verbose) {
    _ftprintf(stderr, _T("%-*.*s\r"), m_screenWidth, m_screenWidth, name);
  }
}

void WordCounter::handleEndDir(const TCHAR *name) {
}

void WordCounter::printSubtotal() {
  if(m_printSubtotals && m_subFileCount > 0) {
    m_subTotal.print(format(_T("subtotal for %s"), (m_currentDirName.length() == 0)?_T("."):m_currentDirName.cstr()));
    if(m_printFileContent) {
      _tprintf(_T("\n"));
    }
  }
}

void WordCounter::printTotal() {
  if(m_filecount > 1) {
    printSubtotal();
    m_total.print(_T("total"));
  }
}

static void usage() {
  _ftprintf(stderr,_T("Usage:wc [-rtsv] files...\n"
                      "         -r: Recurse subdirectories.\n"
                      "         -t: Print only total.\n"
                      "         -s: Print only subtotals and total.\n"
                      "         -v: Verbose. Print directories to stderr, while scanning files.\n")
           );
  exit(-1);
}

int main(int argc, char **argv) {
  char *cp;
  bool recurse          = false;
  bool verbose          = false;
  bool printFileContent = true;
  bool printSubtotals   = true;

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++;*cp;cp++) {
      switch(*cp) {
      case 'r':
        recurse = true;
        continue;
      case 't':
        printFileContent = false ;
        printSubtotals   = false; 
        continue;
      case 's':
        printFileContent = false;
        printSubtotals   = true;
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

  initWordDelimiter();
  WordCounter wordCounter(printFileContent, printSubtotals, verbose && !isatty(stdout));
  if(*argv == NULL) {
    if(isatty(stdin)) {
      usage();
    }
    wordCounter.wc(_T(""),stdin);
  } else {
    FileTreeWalker::traverseArgv(argv, wordCounter, recurse);
    wordCounter.printTotal();
  }
  return 0;
}
