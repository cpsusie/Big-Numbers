#include "stdafx.h"

bool Database::searchDbName(const String &dbName, SysTableDbData &dbdef) {
  if(dbName.length() > SQLAPI_MAXDBNAME) {
    throwSqlError(SQL_DBNAME_TOO_LONG,_T("Database name <%s> too long"),dbName.cstr());
  }

  if(!DbFile::exist(SYSTEM_DBDATA_FNAME)) {
    return false;
  }

  DataFile dataFile(  SYSTEM_DBDATA_FNAME, DBFMODE_READONLY, NULL);
  KeyFile  indexFile( SYSTEM_DBKEY_FNAME,  DBFMODE_READONLY, NULL);
  KeyType  key;

  KeyFileDefinition keydef(indexFile);
  keydef.put(key, 0, toUpperCase(dbName));

  if(!indexFile.searchMin(RELOP_EQ, key, 1)) {
    return false;
  }

  dataFile.readRecord(keydef.getRecordAddr(key), &dbdef, sizeof(SysTableDbData));
  return true;
}

bool Database::exist(const String &dbName) {
  SysTableDbData dummy;
  return searchDbName(dbName,dummy);
}

void Database::sysTabDbInsert(SysTableDbData &dbdef) {
  if(!DbFile::exist(SYSTEM_DBDATA_FNAME)) {
    createDbDb();
  }

  LogFile logFile(SYSTEM_DBLOG_FNAME);

  DataFile dataFile( SYSTEM_DBDATA_FNAME, DBFMODE_READWRITE, &logFile);
  KeyFile  indexFile(SYSTEM_DBKEY_FNAME,  DBFMODE_READWRITE, &logFile);

  logFile.begin();
  _tcsupr(dbdef.m_dbName);
  try {
    DbAddr addr = dataFile.insert(&dbdef, sizeof(SysTableDbData));

    KeyFileDefinition keydef(indexFile);
    KeyType key;
    keydef.put(key, 0, String(dbdef.m_dbName));
    keydef.putRecordAddr(key, addr);
    indexFile.insert(key);
  } catch(sqlca) {
    logFile.abort();
    throw;
  }
  logFile.commit();
}

void Database::sysTabDbDelete(const String &dbName) {
  LogFile logFile(SYSTEM_DBLOG_FNAME);
  KeyType key;

  DataFile dataFile( SYSTEM_DBDATA_FNAME, DBFMODE_READWRITE, &logFile);
  KeyFile  indexFile(SYSTEM_DBKEY_FNAME,  DBFMODE_READWRITE, &logFile);

  KeyFileDefinition keydef(indexFile);
  keydef.put(key, 0, toUpperCase(dbName));

  if(!indexFile.searchMin(RELOP_EQ, key, 1)) {
    throwSqlError(SQL_DB_NOT_EXIST,_T("Database <%s> doesn't exist"),dbName.cstr());
  }

  logFile.begin();
  try {
    dataFile.remove(keydef.getRecordAddr(key));
    indexFile.remove(key);
  } catch(sqlca) {
    logFile.abort();
    throw;
  }
  logFile.commit();
}

void Database::createDbDb() {
#pragma warning(disable:4101)
  SysTableDbData dbdef; /* is used in sizeof ! */

  DataFile::create(SYSTEM_DBDATA_FNAME);
  KeyFileDefinition keydef;
  keydef.addKeyField(SORT_ASCENDING,DBTYPE_TSTRING, ARRAYSIZE(dbdef.m_dbName));
  keydef.addAddrField(true);
  KeyFile::create( SYSTEM_DBKEY_FNAME, keydef);
  DbFile::create(SYSTEM_DBLOG_FNAME);
}
