#include "stdafx.h"

void Database::sysTabTableInsert(const String    &tableName    ,
                                 const String    &fileName ,
                                 ULONG    sequenceno   ,
                                 char             tabletype    ,
                                 USHORT   recsize
                                ) {

  if(recsize > MAXRECSIZE) {
    throwSqlError(SQL_RECSIZE_TOO_BIG,_T("Record-definition exceeds %d bytes"),MAXRECSIZE);
  }

  if(tableName.length() == 0 || tableName.length() > MAXTABLENAME) {
    throwSqlError(SQL_INVALID_TABLENAME,_T("Invalid tableName:<%s>"),tableName.cstr());
  }

  if(fileName.length() > MAXFILENAME) {
    throwSqlError(SQL_INVALID_FILENAME,_T("Invalid filename:<%s>"),fileName.cstr());
  }

  SysTableTableData rec;
  _tcsupr(_tcscpy( rec.m_tableName, tableName.cstr()));
  _tcsupr(_tcscpy( rec.m_fileName , fileName.cstr()));
  rec.m_sequenceNo = sequenceno;
  rec.m_tableType  = tabletype;
  rec.m_recSize    = recsize;

  DataFile datafile(   *this, SYSTEM_TABLEDATA_FNAME, DBFMODE_READWRITE);
  KeyFile  indexfile1( *this, SYSTEM_TABLEKEY1_FNAME, DBFMODE_READWRITE);
  KeyFile  indexfile2( *this, SYSTEM_TABLEKEY2_FNAME, DBFMODE_READWRITE);

  DbAddr addr = datafile.insert( &rec, sizeof(rec));

  KeyFileDefinition keydef1(indexfile1);
  KeyType key1;
  keydef1.put(key1,0,String(rec.m_tableName));
  keydef1.putRecordAddr(key1, addr);
  indexfile1.insert(key1);

  KeyType key2;
  KeyFileDefinition keydef2(indexfile2);
  keydef2.put(key2, 0, rec.m_sequenceNo);
  keydef2.putRecordAddr(key2, addr );
  indexfile2.insert(key2);
}
