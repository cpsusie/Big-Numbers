#include "stdafx.h"
#include <MyUtil.h>
#include <io.h>
#include <signal.h>
#include "bfsort.h"

typedef int (*CompareFunction)(const String &s1, const String &s2);

class PartialComparator : public FunctionComparator<String> {
private:
  bool m_desc;
public:
  int compare(const String &s1, const String &s2) {
    return m_desc ? -m_usersuppliedcmp(&s1, &s2) : m_usersuppliedcmp(&s1, &s2);
  }
  PartialComparator(::CompareFunction cmp, bool desc = false)
    : FunctionComparator<String>(cmp), m_desc(desc)
  {
  }
  void setDescending() {
    m_desc = true;
  }
  AbstractComparator *clone() const {
    return new PartialComparator((::CompareFunction)m_usersuppliedcmp, m_desc);
  }
};

class CompoundComparator : public StringComparator {
public:
  Array<PartialComparator> m_comparelist;
  int compare(const String &e1, const String &e2);
  void setDescending() {
    m_comparelist.last().setDescending();
  }
};

int CompoundComparator::compare(const String &s1, const String &s2) {
  for(size_t i = 0; i < m_comparelist.size(); i++) {
    int result;
    if((result = m_comparelist[i].compare(s1,s2)) != 0) {
      return result;
    }
  }
  return 0;
}

static int numericCompare(const String &s1, const String &s2) {
  const double d1 = _ttof(s1.cstr());
  const double d2 = _ttof(s2.cstr());
  return (d2 > d1) ? -1 : (d2 < d1) ? 1 : 0;
}

static BalancedFileSort *bfsp = NULL;

static void interruptHandler(int s) {
  if(bfsp && bfsp->isVerbose()) {
    _ftprintf(stderr, _T("\ninterrupted by user\n"));
  }
  SAFEDELETE(bfsp);
  exit(-1);
}

static void usage() {
  _ftprintf(stderr, _T("Usage:msort [-ddrive] [-v] [-inr] [inputfile]\n"));
  exit(-1);
}

int main(int argc, char **argv) {
  char *cp;
//  CompoundComparator cc;
  bool              verbose = false;
  PartialComparator defaultComparator(stringHashCmp);
  PartialComparator numericComparator(numericCompare);
  PartialComparator ignoreCaseComparator(stringiHashCmp);
  PartialComparator *currentComparator = &defaultComparator;

  for(argv++; *argv && ((*(cp = *argv) == '-')); argv++) {
    for(cp++;*cp;cp++) {
      switch(*cp) {
      case 'i':
        currentComparator = &ignoreCaseComparator;
        continue;
      case 'n':
        currentComparator = &numericComparator;
        continue;
      case 'r':
        currentComparator->setDescending();
        continue;
      case 'v':
        verbose = true;
        continue;
      case 'd':
        if(cp[1] == 0) usage();
        HelpFile::setDrive(cp[1]);
        break;
      default:
        usage();
      }
      break;
    }
  }
  try {
    const double start = getProcessTime();
    FILE *inputFile = *argv ? FOPEN(*argv,_T("r")) : stdin;
    if(isatty(inputFile)) {
      usage();
    }
    setvbuf(inputFile,NULL,_IOFBF,BUF_SIZE);
    setvbuf(stdout,NULL,_IOFBF,BUF_SIZE);
    bfsp = new BalancedFileSort(*currentComparator,verbose); TRACE_NEW(bfsp);
    signal(SIGINT,interruptHandler);
    bfsp->sort(inputFile,stdout);
    SAFEDELETE(bfsp);

    if(verbose) {
      _ftprintf(stderr, _T("processtime:%6.2lf sec.\n"), (getProcessTime()-start) / 1000000);
    }
  } catch(Exception e) {
    _ftprintf(stderr, _T("msort:%s\n"), e.what());
    return -1;
  }
  return 0;
}
