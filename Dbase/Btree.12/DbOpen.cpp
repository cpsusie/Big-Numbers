#include "stdafx.h"

Database::Database(const String &dbName) {
  SysTableDbData  dbdef;

  m_logFile = NULL;

  if(dbName.length() > SQLAPI_MAXDBNAME) {
    throwSqlError(SQL_DBNAME_TOO_LONG,_T("Database name <%s> too long"),dbName.cstr());
  }

  if(!searchDbName( dbName, dbdef)) {
    throwSqlError(SQL_DB_NOT_EXIST,_T("Database <%s> doesn't exist"),dbName.cstr());
  }
    
  m_dbName = dbdef.m_dbName;
  m_path   = dbdef.m_path;
  for(int i = 0; i < 256; i++) {
    m_colseq[i] = dbdef.m_colseq[i];
  }

  String fullname = DbFile::dbFileName(dbdef.m_path, SYSTEM_LOG_FNAME);
  m_logFile = new LogFile(fullname);

  if(m_logFile->getCount() > 0) { /* recover Database */
    _ftprintf(stderr, _T("Recovering Database!\n"));
    trabort();
  }
}

void Database::close() {
  if(m_logFile) {
    if(m_logFile->inTmf()) {
      try {
        trabort();
      } catch(sqlca) {
        delete m_logFile;
        m_logFile = NULL;
        throw;
      }
    }
    delete m_logFile;
    m_logFile = NULL;
  }
}

Database::~Database() {
  close();
}

void Database::trbegin() {
  if(m_logFile->inTmf()) {
    throwSqlError(SQL_NESTED_TRANSACTION,_T("trbegin:tmf already opened"));
  }
  m_logFile->begin();
}

void Database::trcommit() {
  if(!m_logFile->inTmf()) {
    throwSqlError(SQL_NO_TRANSACTION,_T("trcommit:No active tmf"));
  }
  m_logFile->commit();
}

void Database::trabort() {
  if(!m_logFile->inTmf()) {
    throwSqlError(SQL_NO_TRANSACTION,_T("trabort:No active tmf"));
  }
  m_logFile->abort();
}

String Database::getNewFileName(const String &extension) const {
  for(int i = 0; i < 10000; i++) {
    String fileName = format(_T("%s\\UT%06d.%s"),m_path.cstr(),i,extension.cstr());
    if(!DbFile::exist(fileName.cstr())) {
      return fileName;
    }
  }
  return _T("");
}

UCHAR Database::getMinColSeq() const {
  int currentIndex = 0;
  UCHAR currentMin = m_colseq[currentIndex];
  for(int i = 1; i < 256; i++) {
    if(m_colseq[i] < currentMin) {
      currentIndex = i;
      currentMin = m_colseq[i];
    }
  }
  return currentIndex;
}

UCHAR Database::getMaxColSeq() const {
  int currentIndex = 0;
  UCHAR currentMax = m_colseq[currentIndex];
  for(int i = 1; i < 256; i++) {
    if(m_colseq[i] > currentMax) {
      currentIndex = i;
      currentMax = m_colseq[i];
    }
  }
  return currentIndex;
}
