#include "stdafx.h"
#include <MyUtil.h>
#include <io.h>
#include <signal.h>
#include "bfsort.h"

typedef int (*CompareFunction)(const TCHAR *s1, const TCHAR *s2);

class PartialComparator : public AbstractComparator {
private:
  CompareFunction m_cmp;
  bool            m_desc;
public:
  int cmp(const void *e1, const void *e2);
  PartialComparator(CompareFunction cmp, bool desc = false) { m_cmp = cmp, m_desc = desc; }
  void setDescending() { m_desc = true; }
  AbstractComparator *clone() const { return new PartialComparator(m_cmp,m_desc); }
};

int PartialComparator::cmp(const void *e1, const void *e2) {
  const int res = m_cmp((const TCHAR*)e1,(const TCHAR*)e2);
  return m_desc ? -res : res;
}

class CompoundComparator : public AbstractComparator {
public:
  Array<PartialComparator> m_comparelist;
  int cmp(const void *e1, const void *e2);
  void setDescending() { m_comparelist.last().setDescending(); }
};

int CompoundComparator::cmp(const void *s1, const void *s2) {
  for(size_t i = 0; i < m_comparelist.size(); i++) {
    int result;
    if((result = m_comparelist[i].cmp(s1,s2)) != 0) {
      return result;
    }
  }
  return 0;
}

static int numericCompare(const TCHAR *s1, const TCHAR *s2) {
  double d1 = _ttof(s1);
  double d2 = _ttof(s2);
  return (d2 > d1) ? -1 : (d2 < d1) ? 1 : 0;
}

static BalancedFileSort *bfsp = NULL;
static bool verbose = false;

static void interruptHandler(int s) {
  if(verbose) {
    _ftprintf(stderr, _T("\ninterrupted by user\n"));
  }
  delete bfsp;
  bfsp = NULL;
  exit(-1);
}

static void usage() {
  _ftprintf(stderr, _T("Usage:msort [-ddrive] [-v] [-inr] [inputfile]\n"));
  exit(-1);
}

int main(int argc, char **argv) {
  char *cp;
//  CompoundComparator cc;
  PartialComparator defaultComparator(_tcscmp); 
  PartialComparator numericComparator(numericCompare);
  PartialComparator ignoreCaseComparator(streicmp);
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
    double start = getProcessTime();
    FILE *inputFile = *argv ? FOPEN(*argv,_T("r")) : stdin;
    if(isatty(inputFile)) {
      usage();
    }
    setvbuf(inputFile,NULL,_IOFBF,BUF_SIZE);
    setvbuf(stdout,NULL,_IOFBF,BUF_SIZE);
    bfsp = new BalancedFileSort(*currentComparator,verbose);
    signal(SIGINT,interruptHandler);
    bfsp->sort(inputFile,stdout);
    delete bfsp;

    if(verbose) {
      _ftprintf(stderr, _T("processtime:%6.2lf sec.\n"), (getProcessTime()-start) / 1000000);
    }
  } catch(Exception e) {
    _ftprintf(stderr, _T("msort:%s\n"), e.what());
    return -1;
  }
  return 0;
}
