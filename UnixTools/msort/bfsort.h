#pragma once

#include <MyUtil.h>
#include <StringArray.h>
#include "HelpFile.h"

typedef Comparator<String> StringComparator;

class BalancedFileSort {
private:
#define NO_FILES 4

  HelpFile          m_helpFile[NO_FILES],*m_currentHelpFile;
  int               m_currentOutput;
  long              m_blockCount;
  FILE             *m_finalOutput;
  bool              m_verbose;
  StringComparator &m_comparator;
  inline void setCurrentOutput(int newValue) {
    m_currentHelpFile = &m_helpFile[m_currentOutput=newValue];
  }
  inline void shiftOutput() {
    setCurrentOutput(m_currentOutput^1);
  }
  inline bool leq(const String *s1, const String *s2) {
    return m_comparator.compare(*s1,*s2) <= 0;
  }
  void destroyHelpFiles();
  void sortAndFlush(StringArray &lines, FILE *output = NULL);
  void distributeLines(int i, FILE *f);
  void makeRuns(int input);
  void finalMerge(int input);
public:
  BalancedFileSort(StringComparator &comparator, bool verbose = false)
    : m_comparator(comparator)
  {
    m_verbose = verbose;
  }
  ~BalancedFileSort() {
    destroyHelpFiles();
  }
  void sort(FILE *inputFile, FILE *outputFile);
  inline bool isVerbose() const {
    return m_verbose;
  }
};

void verbose(_In_z_ _Printf_format_string_ TCHAR const * const format,...);
