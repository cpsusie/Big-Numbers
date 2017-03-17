#include "stdafx.h"

void Database::tableDrop(const String &tableName) {
  SysTableTableData tabrec;

  if(tableName.length() == 0 ||  tableName.length() > MAXTABLENAME) {
    throwSqlError(SQL_INVALID_TABLENAME,_T("Invalid tablename:<%s>"),tableName.cstr());
  }

  const String tmpname = toUpperCase(tableName);

  DataFile datafile1( *this, SYSTEM_TABLEDATA_FNAME, DBFMODE_READWRITE);
  KeyFile  indexfile1(*this, SYSTEM_TABLEKEY1_FNAME, DBFMODE_READWRITE);

  KeyFileDefinition keydef1(indexfile1);
  KeyType tabkey;
  keydef1.put(tabkey,0,tmpname);

  bool found = indexfile1.searchMin(RELOP_EQ, tabkey, 1 );
                                          /* search on first field */
  if(!found) {
    throwSqlError(SQL_INVALID_TABLENAME,_T("Table <%s> doesn't exist"), tableName.cstr() );
  }

  DbAddr dbaddr = keydef1.getRecordAddr(tabkey);
  datafile1.readRecord(dbaddr, &tabrec, sizeof(SysTableTableData) );

  if(tabrec.m_tableType == TABLETYPE_SYSTEM) { /* may not delete systemtables */
    throwSqlError(SQL_INVALID_TABLETYPE,_T("Cannot delete systemtable <%s>"),tableName.cstr() );
  }

/* delete columns */

  DataFile datafile2(  *this, SYSTEM_COLUMNDATA_FNAME, DBFMODE_READWRITE);
  KeyFile  indexfile2( *this, SYSTEM_COLUMNKEY_FNAME , DBFMODE_READWRITE);

  KeyFileDefinition keydef2(indexfile2);
  KeyType colkey;
  keydef2.put(colkey,0,tmpname);
  KeyCursor cursor2(indexfile2,
                    RELOP_EQ, &colkey, 1,
                    RELOP_EQ, &colkey, 1,
                    SORT_ASCENDING
                   );

  while(cursor2.hasNext()) {
    cursor2.next(colkey);
//    _tprintf(_T("now deleting column-key:")); keydef2.keyprintf(colkey);
//    _tprintf(_T("\n"));
    datafile2.remove(keydef2.getRecordAddr(colkey));
    indexfile2.remove(colkey);
  }

/* delete indexes */
  DataFile datafile3(  *this, SYSTEM_INDEXDATA_FNAME, DBFMODE_READWRITE);
  KeyFile  indexfile3( *this, SYSTEM_INDEXKEY1_FNAME, DBFMODE_READWRITE);

  KeyFileDefinition keydef3(indexfile3);
  KeyType inxkey1;
  keydef3.put(inxkey1,0,tmpname);

  KeyCursor cursor3( indexfile3,
                     RELOP_EQ, &inxkey1, 1,
                     RELOP_EQ, &inxkey1, 1,
                     SORT_ASCENDING
                    );

  while(cursor3.hasNext()) {
    cursor3.next(inxkey1);
    SysTableIndexData inxdata;
    datafile3.readRecord(keydef3.getRecordAddr(inxkey1),&inxdata, sizeof(SysTableIndexData));
    indexDrop(inxdata.m_indexName, true );
  }

  indexfile1.remove(tabkey);

  datafile1.remove(dbaddr);

// remove from index2
  KeyFile indexfile4(*this, SYSTEM_TABLEKEY2_FNAME, DBFMODE_READWRITE);
  KeyFileDefinition keydef4(indexfile4);
  keydef4.put(tabkey,0,tabrec.m_sequenceNo);
  keydef4.putRecordAddr(tabkey, dbaddr);
  indexfile4.remove(tabkey);

  if(DbFile::exist(tabrec.m_fileName)) {
    DbFile::destroy(tabrec.m_fileName);
  }

  removeFromCache(tabrec.m_tableName);
}

