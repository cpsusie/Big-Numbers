#include "stdafx.h"
#include <Tokenizer.h>

SysTableIndexData::SysTableIndexData(const IndexDefinition &indexDef) {
  if(indexDef.m_tableName.length() > MAXTABLENAME) {
    throwSqlError(SQL_INVALID_TABLENAME,_T("Tablename <%s> too long"),indexDef.m_tableName.cstr());
  }
  if(indexDef.m_indexName.length() > MAXINDEXNAME) {
    throwSqlError(SQL_INVALID_INDEXNAME,_T("Indexname <%s> too long"),indexDef.m_indexName.cstr());
  }
  if(indexDef.m_fileName.length() > MAXFILENAME) {
    throwSqlError(SQL_INVALID_FILENAME,_T("Filename <%s> too long"), indexDef.m_fileName.cstr());
  }

  _tcsupr(_tcsncpy(m_tableName,indexDef.m_tableName.cstr(),ARRAYSIZE(m_tableName)));
  _tcsupr(_tcsncpy(m_indexName,indexDef.m_indexName.cstr(),ARRAYSIZE(m_indexName)));
  _tcsupr(_tcsncpy(m_fileName ,indexDef.m_fileName.cstr() ,ARRAYSIZE(m_fileName)));
  m_indexType = indexDef.getIndexType();
  m_colCount  = indexDef.getColumnCount();
  String colstr;
  UINT i;
  for(i = 0; i < indexDef.getColumnCount(); i++) {
    const IndexColumn &column = indexDef.getColumn(i);
    int col = column.m_col;
    if(col > 999 || col < 0) {
      throwSqlError(SQL_FATAL_ERROR,_T("Cannot save colno=%d in SysTableIndexData"),col);
    }
    colstr += format(_T("%d "), col);
    m_ascending[i] = column.m_asc ? 'A' : 'D';
  }
  m_ascending[i] = '\0';
  if(colstr.length() > ARRAYSIZE(m_columns)) {
    throwSqlError(SQL_FATAL_ERROR,_T("Cannot save IndexDefinition. Too many columns"));
  }
  _tcscpy(m_columns,colstr.cstr());
}

IndexDefinition::IndexDefinition(const SysTableIndexData &indexData) {
  m_tableName = toUpperCase(indexData.m_tableName);
  m_indexName = toUpperCase(indexData.m_indexName);
  m_fileName  = toUpperCase(indexData.m_fileName);
  m_indexType = (IndexType)indexData.m_indexType;

  int i = 0;
  for(Tokenizer tok(indexData.m_columns,_T(" ")); i < indexData.m_colCount && tok.hasNext(); ) {
    m_columns.add(IndexColumn(tok.getInt(),indexData.m_ascending[i++] == 'A'));
  }

  if(i != indexData.m_colCount) {
    throwSqlError(SQL_FATAL_ERROR,_T("Inconsistency in SysTableIndexData. colcount=%d columns=%s"),indexData.m_colCount,indexData.m_columns);
  }
}

void Database::sysTabIndexInsert(const IndexDefinition &indexDef) {
  DataFile datafile(   *this, SYSTEM_INDEXDATA_FNAME, DBFMODE_READWRITE);
  KeyFile  indexfile1( *this, SYSTEM_INDEXKEY1_FNAME, DBFMODE_READWRITE);
  KeyFile  indexfile2( *this, SYSTEM_INDEXKEY2_FNAME, DBFMODE_READWRITE);
  KeyFileDefinition keydef1(indexfile1);
  KeyFileDefinition keydef2(indexfile2);

  SysTableIndexData usxfilelayout(indexDef);

  DbAddr addr = datafile.insert(&usxfilelayout, sizeof(usxfilelayout));

  KeyType key1;
  keydef1.put(key1,0,String(usxfilelayout.m_tableName));
  keydef1.put(key1,1,String(indexDef.m_indexName));
  keydef1.putRecordAddr(key1, addr);
  indexfile1.insert(key1);

  KeyType key2;
  keydef2.put(key2,0,String(usxfilelayout.m_indexName));
  keydef2.putRecordAddr(key2,addr);
  indexfile2.insert(key2);
}
