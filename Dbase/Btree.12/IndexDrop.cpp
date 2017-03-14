#include "stdafx.h"

void Database::indexDrop(const String &indexName, bool forceDeletePrimary) {

  DataFile datafile(  *this, SYSTEM_INDEXDATA_FNAME, DBFMODE_READWRITE);
  KeyFile  indexfile1(*this, SYSTEM_INDEXKEY1_FNAME, DBFMODE_READWRITE);
  KeyFile  indexfile2(*this, SYSTEM_INDEXKEY2_FNAME, DBFMODE_READWRITE);

  KeyFileDefinition keydef2(indexfile2);
  KeyType key2;
  keydef2.put(key2,0,toUpperCase(indexName));

  if(!indexfile2.searchMin( RELOP_EQ, key2, 1)) {
    throwSqlError(SQL_INVALID_INDEXNAME,_T("Index <%s> doesn't exist"),indexName.cstr());
  }

  SysTableIndexData inxdata;
  datafile.readRecord(keydef2.getRecordAddr(key2),&inxdata, sizeof(SysTableIndexData));

  const TableDefinition &tableDef = getTableDefinition(inxdata.m_tableName);

  if(tableDef.getTableType() != TABLETYPE_USER) {
    throwSqlError(SQL_INVALID_TABLETYPE,_T("Cannot delete systemindex <%s>"),indexName.cstr());
  }

  if(inxdata.m_indexType == INDEXTYPE_PRIMARY && !forceDeletePrimary) {
    throwSqlError(SQL_DELETE_PRIMARY_INDEX,_T("Cannot delete primary key"));
  }

  KeyFileDefinition keydef1(indexfile1);
  KeyType key1;
  keydef1.put(key1,0,String(inxdata.m_tableName));
  keydef1.put(key1,1,String(inxdata.m_indexName));

  if(!indexfile1.searchMin( RELOP_EQ, key1, 2)) {
    throwSqlError(SQL_DBCORRUPTED,_T("Database corrupted"));
  }

  datafile.remove( keydef1.getRecordAddr(key1));
  indexfile1.remove( key1 );
  indexfile2.remove( key2 );

  sysTabStatDelete(indexName);

  if(DbFile::exist(inxdata.m_fileName)) {
    DbFile::destroy( inxdata.m_fileName);
  }

  removeFromCache(inxdata.m_tableName);
}
