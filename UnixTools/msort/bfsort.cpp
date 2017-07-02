#include "stdafx.h"
#include "bfsort.h"

class StringComparator : public Comparator<String> {
private:
  AbstractComparator &m_comparator;
public:
  int compare(const String &e1, const String &e2) {
    return m_comparator.cmp(e1.cstr(),e2.cstr());
  }
  StringComparator(AbstractComparator &comparator) : m_comparator(comparator) {
  }
  AbstractComparator *clone() const {
    return new StringComparator(m_comparator);
  }
};

void BalancedFileSort::verbose(const TCHAR *format,...) {
  if(m_verbose) {
    va_list argptr;
    va_start(argptr, format);
#include <tchar.h>
    _vftprintf(stderr, format, argptr);
    va_end(argptr);
    fprintf(stderr,"\n");
  }
}

BalancedFileSort::BalancedFileSort(AbstractComparator &comparator, bool verbose) : m_comparator(comparator) {
  m_verbose = verbose;
  m_stringComparator = new StringComparator(comparator);
}

BalancedFileSort::~BalancedFileSort() {
  destroyHelpFiles();
  delete m_stringComparator;
}

void BalancedFileSort::shiftOutput() {
  setCurrentOutput(m_currentOutput + ((m_currentOutput % 2) ? -1 : 1));
}

void BalancedFileSort::setCurrentOutput(int newValue) {
  m_currentOutput = newValue;
  m_currentHelpFile = &m_helpFile[m_currentOutput];
}

void BalancedFileSort::destroyHelpFiles() {
  verbose(_T("destroying tmpfile"));

  for(int i = 0; i < NO_FILES; i++)
    m_helpFile[i].destroy();
}

void BalancedFileSort::sortAndFlush(StringArray &lines, FILE *output) {
  verbose(_T("sorting block %ld (%d lines)"), m_blockCount+1,lines.size());

  lines.sort(*m_stringComparator);

  if(output != NULL) {

    verbose(_T("writing result"));

    for(size_t i = 0; i < lines.size(); i++) {
      _fputts(lines[i].cstr(),output);
    }
  } else {
    if(!m_currentHelpFile->isOpen()) {
      m_currentHelpFile->open(WRITEMODE);
    }

    verbose(_T("writing block to %s"),m_currentHelpFile->getName());

    for(size_t i = 0; i < lines.size();i++) {
      m_currentHelpFile->writeLine(lines[i].cstr());
    }
    m_blockCount++;
  }

  verbose(_T("block written"));
}

void BalancedFileSort::distributeLines(int i, FILE *f) {
  Line buf;
  StringArray lines;
//  int nbytes = 0;

  m_blockCount = 0;
  setCurrentOutput(i);
  while(_fgetts(buf, ARRAYSIZE(buf), f) != NULL) {
    lines.add(buf);
//    nbytes += lines.last().len() + 13;
    if(lines.size() == 2000000) {
      sortAndFlush(lines);
      shiftOutput();
      lines.clear();
//      verbose("nbytes:%d",nbytes);
//      nbytes = 0;
    }
  }
  if(m_blockCount == 0) {
    sortAndFlush(lines,m_finalOutput);
  } else {
    if(lines.size() > 0) {
      sortAndFlush(lines);
    }
  }
}

void BalancedFileSort::makeRuns(int input) {
  Line buf0,buf1,buf2;

#define swap(cp1,cp2) { TCHAR *tmp=cp1;cp1=cp2; cp2=tmp; }
#define readin(p) l##p = in##p.readLine(l##p)
#define MOVE(p) {                            \
  m_currentHelpFile->writeLine(l##p);        \
  swap(l##p,l2);                             \
  readin(p);                                 \
  Continue##p = l##p ? leq(l2,l##p) : false; \
  }

  setCurrentOutput((input+2) % NO_FILES);
  HelpFile &in0  = m_helpFile[input];
  HelpFile &in1  = m_helpFile[input+1];
  HelpFile &out0 = m_helpFile[m_currentOutput];
  HelpFile &out1 = m_helpFile[m_currentOutput+1];
  TCHAR *l0 = buf0;
  TCHAR *l1 = buf1;
  TCHAR *l2 = buf2;

  in0.open(READMODE);
  in1.open(READMODE);
  out0.open(WRITEMODE);
  out1.open(WRITEMODE);
  readin(0);
  readin(1);
  for(m_blockCount = 0; l0 || l1; m_blockCount++) {

    verbose(_T("merge (%s,%s) -> %s"), in0.getName(),in1.getName(),m_currentHelpFile->getName());

    bool Continue0 = (l0 != NULL);
    bool Continue1 = (l1 != NULL);
    while(Continue0 || Continue1) {
      if(!Continue1)      MOVE(0)
      else if(!Continue0) MOVE(1)
      else if(leq(l0,l1)) MOVE(0)
      else                MOVE(1);
    }
    shiftOutput();
  }
}

void BalancedFileSort::finalMerge(int input) {
  Line buf0,buf1;
  HelpFile &in0 = m_helpFile[input];
  HelpFile &in1 = m_helpFile[input+1];
  TCHAR *l0 = buf0;
  TCHAR *l1 = buf1;

#define FMOVE(p) { _fputts(l##p,m_finalOutput); readin(p); }

  verbose(_T("merge (%s,%s) -> result"), in0.getName(),in1.getName());

  in0.open(READMODE);
  in1.open(READMODE);
  readin(0);
  readin(1);
  while(l0 || l1) {
    if(!l1)             FMOVE(0)
    else if(!l0)        FMOVE(1)
    else if(leq(l0,l1)) FMOVE(0)
    else                FMOVE(1);
  }
}

void BalancedFileSort::sort(FILE *inputfile, FILE *outputfile) {
  int t = 0;

  m_finalOutput = outputfile;

  distributeLines(t,inputfile);

  verbose(_T("distributeLines complete. %5ld blocks"), m_blockCount);

  while(m_blockCount > 2) {
    verbose(_T("%5ld blocks"), m_blockCount);
    makeRuns(t);
    t = 2 - t;
  }

  if(m_blockCount >= 1) {
    finalMerge(t);
  }

  destroyHelpFiles();

  verbose(_T("all done"));
}

