#include "stdafx.h"
#include "bfsort.h"

#define VERBOSE(...) if(m_verbose) verbose(__VA_ARGS__)

void verbose(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr, format);
  _vftprintf(stderr, format, argptr);
  va_end(argptr);
}

void BalancedFileSort::destroyHelpFiles() {
  VERBOSE(_T("Destroying tmpfile\n"));
  for(int i = 0; i < NO_FILES; i++) {
    m_helpFile[i].destroy();
  }
}

void BalancedFileSort::sortAndFlush(StringArray &lines, FILE *output) {
  const size_t n = lines.size();
  VERBOSE(_T("Sorting block %ld (%s lines)\n"), m_blockCount+1,format1000(n).cstr());

  lines.sort(m_comparator);

  if(output != nullptr) {
    VERBOSE(_T("Writing result\n"));
    for(size_t i = 0; i < n; i++) {
      writeLine(output, lines[i]);
    }
  } else {
    if(!m_currentHelpFile->isOpen()) {
      m_currentHelpFile->open(WRITEMODE);
    }
    VERBOSE(_T("Writing block to %s..."),m_currentHelpFile->getName());

    for(size_t i = 0; i < n;i++) {
      m_currentHelpFile->writeLine(lines[i]);
    }
    m_blockCount++;
  }
  VERBOSE(_T("block written\n"));
}

void BalancedFileSort::distributeLines(int i, FILE *f) {
  String line;
  StringArray lines;
//  int nbytes = 0;

  m_blockCount = 0;
  setCurrentOutput(i);
  while(readLine(f, line)) {
    lines.add(line);
//    nbytes += lines.last().len() + 13;
    if(lines.size() == 2000000) {
      sortAndFlush(lines);
      shiftOutput();
      lines.clear();
//      VERBOSE("nbytes:%d",nbytes);
//      nbytes = 0;
    }
  }
  if(m_blockCount == 0) {
    sortAndFlush(lines, m_finalOutput);
  } else if(lines.size() > 0) {
    sortAndFlush(lines);
  }
}
#define DEFINELINE(i) String buf##i, *l##i = &buf##i
#define READIN(p) l##p = in##p.readLine(*l##p)
#define MOVE(p) {                            \
  m_currentHelpFile->writeLine(*l##p);       \
  std::swap(l##p,l2);                        \
  READIN(p);                                 \
  Continue##p = l##p ? leq(l2,l##p) : false; \
}
#define FINALMOVE(p) {                       \
  writeLine(m_finalOutput,*l##p);            \
  READIN(p);                                 \
}

void BalancedFileSort::makeRuns(int input) {
  DEFINELINE(0);
  DEFINELINE(1);
  DEFINELINE(2);

  setCurrentOutput(input^2);
  HelpFile &in0  = m_helpFile[input];
  HelpFile &in1  = m_helpFile[input+1];
  HelpFile &out0 = m_helpFile[m_currentOutput];
  HelpFile &out1 = m_helpFile[m_currentOutput+1];

  in0.open(READMODE);
  in1.open(READMODE);
  out0.open(WRITEMODE);
  out1.open(WRITEMODE);
  READIN(0);
  READIN(1);
  for(m_blockCount = 0; l0 || l1; m_blockCount++) {
    VERBOSE(_T("Merge (%s,%s) -> %s..."), in0.getName(),in1.getName(),m_currentHelpFile->getName());
    const UINT64 blockStartLine = m_currentHelpFile->getLineCount();
    bool Continue0 = (l0 != nullptr);
    bool Continue1 = (l1 != nullptr);
    while(Continue0 || Continue1) {
      if(!Continue1)      MOVE(0)
      else if(!Continue0) MOVE(1)
      else if(leq(l0,l1)) MOVE(0)
      else                MOVE(1);
    }
    VERBOSE(_T("(%s lines)\n"), format1000(m_currentHelpFile->getLineCount()-blockStartLine).cstr());
    shiftOutput();
  }
}

void BalancedFileSort::finalMerge(int input) {
  DEFINELINE(0);
  DEFINELINE(1);
  HelpFile &in0 = m_helpFile[input];
  HelpFile &in1 = m_helpFile[input+1];

  VERBOSE(_T("Merge (%s,%s) -> result\n"), in0.getName(),in1.getName());

  in0.open(READMODE);
  in1.open(READMODE);
  READIN(0);
  READIN(1);
  while(l0 || l1) {
    if(!l1)             FINALMOVE(0)
    else if(!l0)        FINALMOVE(1)
    else if(leq(l0,l1)) FINALMOVE(0)
    else                FINALMOVE(1);
  }
}

void BalancedFileSort::sort(FILE *inputfile, FILE *outputfile) {
  int t = 0;

  m_finalOutput = outputfile;
  distributeLines(t, inputfile);

  VERBOSE(_T("DistributeLines complete. %5ld blocks\n"), m_blockCount);
  while(m_blockCount > 2) {
    VERBOSE(_T("%5ld blocks\n"), m_blockCount);
    makeRuns(t);
    t ^= 2;
  }
  if(m_blockCount >= 1) {
    finalMerge(t);
  }
  destroyHelpFiles();
  VERBOSE(_T("All done\n"));
}
