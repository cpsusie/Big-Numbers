#include "stdafx.h"

void Database::readIndexDefinitions(const String &tableName, IndexArray &indexArray) const {
  const String tmpName = toUpperCase(tableName);
  indexArray.clear();

  const TableDefinition &tableDef = getTableDefinition(tmpName); /* just to check the tmpName */
  DataFile dataFile( *this, SYSTEM_INDEXDATA_FNAME, DBFMODE_READONLY);
  KeyFile  indexFile(*this, SYSTEM_INDEXKEY1_FNAME, DBFMODE_READONLY);

  KeyFileDefinition keydef(indexFile);
  KeyType key;
  keydef.put(key,0,tmpName);

  KeyCursor cursor(indexFile,
                   RELOP_EQ, &key, 1,
                   RELOP_EQ, &key, 1,
                   SORT_ASCENDING);

  while(cursor.hasNext()) {
    cursor.next(key);
    SysTableIndexData index;
    dataFile.readRecord(keydef.getRecordAddr(key),&index, sizeof(SysTableIndexData));
    indexArray.add(index);
  }
}

const IndexArray &Database::getIndexDefinitions(const String &tableName) const {
  if(tableName.length() > MAXTABLENAME) {
    throwSqlError(SQL_INVALID_TABLENAME,_T("Tablename <%s> is too long"), tableName.cstr() );
  }

  const String tmpName = toUpperCase(tableName);
  IndexArray *indexArrayp = m_indexTableNameCache.get(tmpName);
  if(indexArrayp == NULL) {
    IndexArray indexArray;
    readIndexDefinitions(tmpName,indexArray);
    m_indexTableNameCache.put(tmpName,indexArray);
    indexArrayp = m_indexTableNameCache.get(tmpName);
    for(size_t i = 0; i < indexArrayp->size(); i++) {
      IndexDefinition *indexDefp = &(*indexArrayp)[i];
      m_indexDefIndexNameCache.put(indexDefp->m_indexName,indexDefp);
    }
    indexArrayp = m_indexTableNameCache.get(tmpName);
  }
  return *indexArrayp;
}

const IndexDefinition &Database::getIndexDefinition(const String &indexName) const {
  if(indexName.length() > MAXINDEXNAME) {
    throwSqlError(SQL_INVALID_INDEXNAME,_T("Indexname <%s> is too long"), indexName.cstr() );
  }
  
  const String tmpName = toUpperCase(indexName);
  IndexDefinition **indexDefpp = m_indexDefIndexNameCache.get(tmpName);
  if(indexDefpp == NULL) {
    DataFile dataFile( *this,SYSTEM_INDEXDATA_FNAME, DBFMODE_READONLY);
    KeyFile  indexFile(*this,SYSTEM_INDEXKEY2_FNAME, DBFMODE_READONLY);

    KeyFileDefinition keydef(indexFile);
    KeyType key;
    keydef.put(key,0,tmpName);

    if(!indexFile.searchMin(RELOP_EQ, key, 1)) {
      throwSqlError(SQL_INVALID_INDEXNAME, _T("Index <%s> doesn't exist"),indexName.cstr());
    }

    SysTableIndexData indexData;
    dataFile.readRecord(keydef.getRecordAddr(key),&indexData, sizeof(indexData));
    getIndexDefinitions(indexData.m_tableName);
    indexDefpp = m_indexDefIndexNameCache.get(tmpName);
  }
  if(indexDefpp == NULL) {
    throwSqlError(SQL_INVALID_INDEXNAME, _T("Index <%s> doesn't exist"),indexName.cstr());
  }

  return **indexDefpp;
}

bool IndexDefinition::columnIsMember(UINT colindex) const {
  for(size_t i = 0; i < m_columns.size(); i++) {
    if(m_columns[i].m_col == colindex) {
      return true;
    }
  }
  return false;
}

int IndexDefinition::getFieldIndex(UINT colindex) const {
  for(size_t i = 0; i < m_columns.size(); i++) {
    if(m_columns[i].m_col == colindex) {
      return (int)i;
    }
  }
  return -1;
}

Packer &operator<<(Packer &p, const IndexColumn &indexColumn) {
  p << indexColumn.m_col
    << indexColumn.m_asc;
  return p;
}

Packer &operator>>(Packer &p, IndexColumn &indexColumn) {
  p >> indexColumn.m_col
    >> indexColumn.m_asc;
  return p;
}

Packer &operator<<(Packer &p, const IndexDefinition &indexDef) {
  UINT colCount  = (UINT)indexDef.m_columns.size();
  char indexType = indexDef.m_indexType;
  p << indexDef.m_tableName
    << indexDef.m_indexName
    << indexDef.m_fileName
    << indexType
    << colCount;

  for(UINT i = 0; i < colCount; i++) {
    p << indexDef.m_columns[i];
  }
  return p;
}

Packer &operator>>(Packer &p, IndexDefinition &indexDef) {
  UINT colCount;
  char indexType;
  p >> indexDef.m_tableName
    >> indexDef.m_indexName
    >> indexDef.m_fileName
    >> indexType
    >> colCount;
  indexDef.m_indexType = (IndexType)indexType;

  for(UINT i = 0; i < colCount; i++) {
    IndexColumn col(0, true);
    p >> col;
    indexDef.m_columns.add(col);
  }
  return p;
}
