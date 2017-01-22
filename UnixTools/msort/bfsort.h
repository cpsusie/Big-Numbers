#pragma once

#include "HelpFile.h"

class BalancedFileSort {
private:
#define NO_FILES 4

  HelpFile     m_helpFile[NO_FILES],*m_currentHelpFile;
  int          m_currentOutput;
  long         m_blockCount;
  FILE        *m_finalOutput;
  bool         m_verbose;
  Comparator<String>  *m_stringComparator;
  AbstractComparator  &m_comparator;
  bool leq(const TCHAR *s1, const TCHAR *s2) { return m_comparator.cmp(s1,s2) <= 0; }

  void shiftOutput();
  void destroyHelpFiles();
  void setCurrentOutput(int newValue);
  void sortAndFlush(StringArray &lines, FILE *output = NULL);
  void distributeLines(int i, FILE *f);
  void makeRuns(int input);
  void finalMerge(int input);
  void verbose(const TCHAR *format,...);
public:  
  void sort(FILE *inputFile, FILE *outputFile);
  ~BalancedFileSort();
  BalancedFileSort(AbstractComparator &comparator, bool verbose = false);
};
