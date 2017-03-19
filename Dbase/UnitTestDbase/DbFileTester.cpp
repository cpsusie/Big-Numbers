#include "stdafx.h"
#include "DbFileTester.h"

#define LOGFILENAME  _T("test.log")

DbFileTester::DbFileTester(bool useTransaction) {
  if(useTransaction) {
    DbFile::create(LOGFILENAME);
    m_logFile = new LogFile(LOGFILENAME);
  } else {
    m_logFile = NULL;
  }
}

DbFileTester::~DbFileTester() {
  if(m_logFile != NULL) {
    delete m_logFile;
    DbFile::destroy(LOGFILENAME);
  }
}

void DbFileTester::beginTrans() {
  verify(m_logFile != NULL);
  m_logFile->begin();
}

void DbFileTester::commitTrans() {
  verify(m_logFile != NULL);
  m_logFile->commit();
}

void DbFileTester::abortTrans() {
  verify(m_logFile != NULL);
  m_logFile->abort();
}
