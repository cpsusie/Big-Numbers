#include "stdafx.h"

void Database::sysTabCodeInsert(SysTableCodeKey &key, void *rec, UINT size) {
  KeyType  keyt;
  DbAddr   addr;

  DataFile datafile(  *this, SYSTEM_CODEDATA_FNAME, DBFMODE_READWRITE);
  KeyFile  indexfile( *this, SYSTEM_CODEKEY_FNAME , DBFMODE_READWRITE);

  addr = datafile.insert(rec, size);

  KeyFileDefinition keydef(indexfile);
  keydef.put(keyt,0,String(key.m_filename));
  keydef.put(keyt,1,key.m_nr);
  keydef.putRecordAddr(keyt,addr);

  indexfile.insert(keyt);
}

void Database::sysTabCodeDelete(const SysTableCodeKey &key) {
  KeyType  keyt;

  DataFile datafile(  *this, SYSTEM_CODEDATA_FNAME, DBFMODE_READWRITE);
  KeyFile  indexfile( *this, SYSTEM_CODEKEY_FNAME , DBFMODE_READWRITE);

  KeyFileDefinition keydef(indexfile);
  keydef.put(keyt,0,String(key.m_filename));
  keydef.put(keyt,1,key.m_nr);

  bool found = indexfile.searchMin(RELOP_EQ, keyt, 2 );
                                          /* search on first field */
  if(!found) {
    throwSqlError(SQL_NOT_FOUND,_T("Code <%s,%d> not found"),key.m_filename,key.m_nr);
  }

  DbAddr dbaddr = keydef.getRecordAddr(keyt);

  indexfile.remove(keyt);
  datafile.remove(dbaddr);
}

void Database::sysTabCodeDelete(const String &filename) {

  KeyType key;

  DataFile datafile(  *this, SYSTEM_CODEDATA_FNAME, DBFMODE_READWRITE);
  KeyFile  indexfile( *this, SYSTEM_CODEKEY_FNAME , DBFMODE_READWRITE);

// _tprintf(_T("Now deleting module <%s>\n"),filename);

  KeyFileDefinition keydef(indexfile);
  keydef.put(key, 0, filename);
  int count = 0;
  KeyCursor cursor( indexfile, RELOP_EQ, &key, 1, RELOP_EQ, &key,1,SORT_ASCENDING);
  while(cursor.hasNext()) {
    cursor.next(key);
//    _tprintf(_T("deleting key:")); keydef.keyprintf(key); _tprintf(_T("\n"));
    datafile.remove(keydef.getRecordAddr(key));
    indexfile.remove(key);
    count++;
  }
  if(count == 0) {
    throwSqlError(SQL_NOT_FOUND,_T("Programfile <%s> not found"),filename.cstr());
  }
}

