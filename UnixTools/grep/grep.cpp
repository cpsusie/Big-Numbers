#include "stdafx.h"
#include <MyUtil.h>
#include <Console.h>
#include <Regex.h>
#include <BMAutomate.h>
#include <NumberInterval.h>
#include <FileTreeWalker.h>

#define MAXLINESIZE 4096

class Queue {
private:
  TCHAR **m_buffer;
  int m_head, m_tail, m_size, m_capacity;
public:
  Queue(int capacity);
  ~Queue();
  void remove();
  void insert(const TCHAR *line);

  bool isFull() const {
    return m_size == m_capacity;
  }
  bool isEmpty() const {
    return m_size == 0;
  }
  int getSize() const {
    return m_size;
  }
  int getCapacity() const {
    return m_capacity;
  }
  void clear();
  const TCHAR *getHead(int fromhead) const {
    return m_buffer[(m_head+fromhead)%m_capacity];
  }
};

Queue::Queue(int capacity) {
  m_capacity  = capacity;
  m_size      = 0;
  m_head      = 0;
  m_tail      = 0;
  m_buffer    = new TCHAR*[m_capacity];
  for(int i = 0; i < m_capacity; i++) {
    m_buffer[i] = new TCHAR[MAXLINESIZE];
  }
}

Queue::~Queue() {
  for(int i = 0; i < m_capacity; i++) {
    delete[] m_buffer[i];
  }
  delete[] m_buffer;
}

void Queue::remove() {
  if(isEmpty()) {
    throwException(_T("Remove from empty queue"));
  }
  m_head = (m_head + 1) % m_capacity;
  m_size--;
}

void Queue::insert(const TCHAR *line) {
  if(isFull()) {
    throwException(_T("Insert into full queue"));
  }
  _tcscpy(m_buffer[m_tail], line);
  m_tail = (m_tail + 1) % m_capacity;
  m_size++;
}

void Queue::clear() {
  m_head = m_tail = m_size = 0;
}

static void showLine(const TCHAR *fname, const TCHAR *line) {
  if(fname) {
    _tprintf(_T("%-15s:%s\n"), fname, line);
  } else {
    _tprintf(_T("%s\n"), line);
  }
}

class Grepper : public FileNameHandler {
private:
  const bool        m_useRegularExpression;
  Regex             m_regex;
  BMAutomate        m_BMAutomate;
  Queue             m_queue;
  const IntInterval m_surroundingInterval;
  const bool        m_showSurroundingLines;
  const bool        m_printFname;
  const bool        m_fnameOnly;
  const bool        m_complement;
  bool              m_verbose;
  const int         m_screenWidth;
  void showMatch(FILE *f, const TCHAR *fname, const TCHAR *line);
  bool containsPattern(FILE *f);
  bool match(const TCHAR *s) const;

public:
  Grepper(bool useRegularExpression, const String &pattern, bool ignoreCase, IntInterval &surroundingInterval, bool printFname, bool fnameOnly, bool complement, bool verbose);
  ~Grepper();
  void grep(FILE *f, const TCHAR *fname);
  void grep(const TCHAR *fname);
  void handleFileName(const TCHAR *name, DirListEntry &info);
  void setNoVerbose() {
    m_verbose = false;
  }
};

Grepper::Grepper(bool useRegularExpression, const String &pattern, bool ignoreCase, IntInterval &surroundingInterval, bool printFname, bool fnameOnly, bool complement, bool verbose)
: m_useRegularExpression(useRegularExpression)
, m_queue(-surroundingInterval.getFrom())
, m_surroundingInterval(surroundingInterval)
, m_showSurroundingLines(m_surroundingInterval.getLength() != 0)
, m_printFname(printFname)
, m_fnameOnly(fnameOnly)
, m_complement(complement)
, m_verbose(verbose)
, m_screenWidth(Console::getWindowSize(STD_ERROR_HANDLE).X - 1)
{
  if(m_useRegularExpression) {
    m_regex.compilePattern(pattern, ignoreCase ? String::upperCaseTranslate : NULL);
  } else {
    m_BMAutomate.compilePattern(pattern, true, ignoreCase ? String::upperCaseTranslate : NULL);
  }
}

Grepper::~Grepper() {
}

bool Grepper::match(const TCHAR *s) const {
  return m_useRegularExpression ? (m_regex.search(s) >= 0) : (m_BMAutomate.search(s) >= 0);
}

void Grepper::showMatch(FILE *f, const TCHAR *fname, const TCHAR *line) {
  if(!m_showSurroundingLines) {
    showLine(fname,line);
  } else {
    for(int i = 0; i < m_queue.getSize(); i++) {
      showLine(fname,m_queue.getHead(i));
    }
    showLine(fname,line);
    if(m_surroundingInterval.getTo() > 0) {
      __int64 fpos = GETPOS(f);
      TCHAR tmpline[MAXLINESIZE];
      for(int i = 0; i < m_surroundingInterval.getTo() && FGETS(tmpline, ARRAYSIZE(tmpline), f); i++) {
        showLine(fname,tmpline);
      }
      FSEEK(f, fpos);
    }
    _tprintf(_T("--\n"));
  }
}

bool Grepper::containsPattern(FILE *f) {
  TCHAR line[MAXLINESIZE];
  while(FGETS(line, ARRAYSIZE(line), f)) {
    if(match(line)) {
      return true;
    }
  }
  return false;
}

void Grepper::grep(FILE *f, const TCHAR *fname) {
  if(m_verbose && fname) {
    _ftprintf(stderr, _T("%-*.*s\r"), m_screenWidth, m_screenWidth, fname);
  }

  if(m_fnameOnly) {
    if(containsPattern(f) != m_complement) {
      _tprintf(_T("%s\n"),fname ? fname : _T("stdin"));
      return; // no need to go further
    }
  } else {
    TCHAR line[MAXLINESIZE];
    m_queue.clear();
    if(!m_printFname) {
      fname = NULL;
    }
    while(FGETS(line, ARRAYSIZE(line), f)) {
      if(match(line) != m_complement) { // match
        showMatch(f, fname, line);
      }
      if(m_queue.getCapacity() > 0) {
        if(m_queue.isFull()) {
          m_queue.remove();
        }
        m_queue.insert(line);
      }
    }
  }
}

void Grepper::grep(const TCHAR *fname) {
  FILE *f = fopen(fname, _T("r"));
  if(f == NULL) {
    _tperror(fname);
  } else {
    grep(f, fname);
    fclose(f);
  }
}

void Grepper::handleFileName(const TCHAR *name, DirListEntry &info) {
  grep(name);
}

static void usage(bool showRegexHelp = false) {
  FILE *output     = stderr;
  bool  usingPopen = false;
  if(showRegexHelp) {
    if(!isatty(stdout)) {
      output = stdout;
    } else {
      output = _popen("more","wt");
      usingPopen = true;
    }
  }

  _ftprintf( output,
    _T("Usage:grep options pattern [files...]\n"
       "      grep options -e pattern [files...]\n"
       "  Options: -i: Case-sensitive. Default is case-insensitive\n"
       "           -c:Complement. Show lines/files that does NOT match/contain pattern\n"
       "           -H:Show only filename where pattern occurs\n"
       "           -h:Suppress filename in output\n"
       "           -r:Recurse subdirectories\n"
       "           -n[count1[,count2]]: Show surrounding lines. If only count1 is specified\n"
       "             then count1 lines before and after the matching line is shown. If both count1 and count2 is specified\n"
       "             then count1 lines before and count2 lines after the matching line is shown. Default:count1=count2=3\n"
       "           -f[file]:Read <file> to get the name of the files to be searched. Default is stdin\n"
       "           -v: Verbose-mode. Show filenames while searching\n"
       "           -help:Print detailed description of syntax of regular expression when using -e pattern\n"
      )
  );
  if(showRegexHelp) {
    _ftprintf(output, _T("\n"));
    for(const TCHAR **line = Regex::getHelpText(); *line; line++) {
      _ftprintf(output, _T("%s\n"), *line);
    }
  }
  fflush(output);
  if(usingPopen) {
    _pclose(output);
  }

  exit(-1);
}

int main(int argc, const char **argv) {
  const char *cp;
  const char *fileSetFileName = NULL;
  bool        recurse         = false;
  bool        ignoreCase      = true;
  bool        fnameOnly       = false;
  bool        printFname      = true;
  bool        verbose         = false;
  bool        complement      = false;
  IntInterval surroundingLines;

  try {

    for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
      if(strcmp(cp+1,"help") == 0) {
        usage(true);
      }
      for(cp++; *cp; cp++) {
        switch(*cp) {
        case 'e':
          goto EndOfOptions;
        case 'i':
          ignoreCase   = false;
          continue;
        case 'f':
          fileSetFileName = cp+1;
          break;
        case 'h':
          printFname   = false;
          continue;
        case 'H':
          fnameOnly    = true;
          continue;
        case 'n':
          { int count1, count2;
            if(sscanf(cp+1,"%d,%d",&count1,&count2) == 2) {
              if(count1 < 0 || count2 < 0) {
                fprintf(stderr,"Number of surrounding lines to show must be >= 0. =[%d,%d]\n",count1,count2);
                usage();
              }
              if(count1 == 0 && count2 == 0) {
                fprintf(stderr,"Number of surrounding lines to show is 0\n");
                usage();
              }
            } else if(sscanf(cp+1,"%d", &count1) == 1) {
              if(count1 < 1) {
                fprintf(stderr,"Number of surrounding lines to show must be >= 1. (=%d)\n",count1);
                usage();
              }
              count2 = count1;
            } else {
              count2 = count1 = 3;
            }
            surroundingLines.setFrom(-count1);
            surroundingLines.setTo(count2);
          }
          break;
        case 'r':
          recurse      = true;
          continue;
        case 'c':
          complement   = true;
          continue;
        case 'v':
          verbose      = true;
          continue;
        default :
          usage();
        }
        break;
      }
    }

EndOfOptions:
    if(*argv == NULL) {
      usage();
    }
    bool        regexp = false;
    const char *pattern;
    if(strcmp(*argv,"-e") == 0) {
      argv++;
      if(*argv == NULL) {
        usage();
      }
      pattern = *(argv++);
      regexp = true;
    } else {
      pattern = *(argv++);
    }

    Grepper grepper(regexp, pattern, ignoreCase, surroundingLines, printFname, fnameOnly, complement, verbose);

    if(fileSetFileName) { // read the file to get the filenames to search in
      FILE *filesetFile = *fileSetFileName ? FOPEN(fileSetFileName, "r") : stdin;
      if(isatty(filesetFile)) {
        usage();
      }
      TCHAR line[256];
      while(FGETS(line, ARRAYSIZE(line), filesetFile)) {
        grepper.grep(line);
      }
      if(filesetFile != stdin) {
        fclose(filesetFile);
      }
    } else if(*argv == NULL) {
      if(isatty(stdin)) {
        usage();
      }
      grepper.setNoVerbose();
      grepper.grep(stdin, NULL);
    } else {
      FileTreeWalker::traverseArgv(argv,grepper,recurse);
    }
  } catch(Exception e) {
    _ftprintf(stderr, _T("grep:%s\n"), e.what());
    return -1;
  }
  return 0;
}
