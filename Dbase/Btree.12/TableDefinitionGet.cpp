#include "stdafx.h"

Packer &operator<<(Packer &p, const ColumnDefinition &col) {
  char type = col.m_type;
  p << col.m_name
    << type
    << col.m_offset
    << col.m_len
    << col.m_nullAllowed
    << col.m_defaultValue;
  return p;
}

Packer &operator>>(Packer &p, ColumnDefinition &col) {
  char type;
  p >> col.m_name
    >> type
    >> col.m_offset
    >> col.m_len
    >> col.m_nullAllowed
    >> col.m_defaultValue;
  col.m_type = (DbFieldType)type;
  return p;
}

Packer &operator<<(Packer &p, const TableDefinition &tableDef) {
  UINT n    = tableDef.getColumnCount();
  char         type = tableDef.getTableType();

  p << tableDef.m_tableName
    << tableDef.m_fileName
    << type
    << tableDef.m_sequenceNo
    << tableDef.m_recSize
    << n;

  for(UINT i = 0; i < n; i++) {
    p << tableDef.getColumn(i);
  }
  return p;
}

Packer &operator>>(Packer &p, TableDefinition &tableDef) {
  UINT n;
  char type;
  tableDef.init(TABLETYPE_USER,EMPTYSTRING);
  p >> tableDef.m_tableName
    >> tableDef.m_fileName
    >> type
    >> tableDef.m_sequenceNo
    >> tableDef.m_recSize
    >> n;
  tableDef.m_tableType = (TableType)type;
  for(UINT i = 0; i < n; i++) {
    ColumnDefinition col;
    p >> col;
    tableDef.m_columns.add(col);
  }
  return p;
}

void Database::removeFromCache(const String &tableName) {
  String      tmpName    = toUpperCase(tableName);
  IndexArray *indexArray = m_indexTableNameCache.get(tmpName);
  if(indexArray) {
    for(size_t i = 0; i < indexArray->size(); i++) {
      IndexDefinition &indexDef = (*indexArray)[i];
      m_indexDefIndexNameCache.remove(indexDef.m_indexName);
    }
  }
  m_indexTableNameCache.remove(tmpName);
  TableDefinition *tableDefp = m_tableDefTableNameCache.get(tmpName);
  if(tableDefp != nullptr) {
    m_tableDefSequenceCache.remove(tableDefp->getSequenceNo());
    m_tableDefTableNameCache.remove(tmpName);
  }
  TableInfo *tableInfop = m_tableInfoTableNameCache.get(tmpName);
  if(tableInfop != nullptr) {
    m_tableInfoSequenceCache.remove(tableInfop->getSequenceNo());
    m_tableInfoTableNameCache.remove(tmpName);
  }
}

const TableDefinition &Database::getTableDefinition(const String &tableName) const {
  if(tableName.length() > MAXTABLENAME) {
    throwSqlError(SQL_INVALID_TABLENAME,_T("Table <%s> is too long"), tableName.cstr() );
  }

  String tmpName = toUpperCase(tableName);
  TableDefinition *tableDefp = m_tableDefTableNameCache.get(tmpName);
  if(tableDefp == nullptr) {
    TableDefinition tableDef = readTableDefinition(tmpName);
    m_tableDefTableNameCache.put(tmpName,tableDef);
    tableDefp = m_tableDefTableNameCache.get(tmpName);
    m_tableDefSequenceCache.put(tableDef.getSequenceNo(),tableDefp);
  }
  return *tableDefp;
}

const TableDefinition &Database::getTableDefinition(ULONG sequenceNo) const {
  TableDefinition **tableDefpp = m_tableDefSequenceCache.get(sequenceNo);
  if(tableDefpp == nullptr) {
    TableDefinition tableDef = readTableDefinition(sequenceNo);
    m_tableDefTableNameCache.put(tableDef.getTableName(),tableDef);
    TableDefinition *tableDefp = m_tableDefTableNameCache.get(tableDef.getTableName());
    m_tableDefSequenceCache.put(tableDefp->getSequenceNo(),tableDefp);
    tableDefpp = m_tableDefSequenceCache.get(sequenceNo);
  }
  return **tableDefpp;
}

const TableInfo &Database::getTableInfo(const String &tableName) const {
  if(tableName.length() > MAXTABLENAME) {
    throwSqlError(SQL_INVALID_TABLENAME,_T("Table <%s> is too long"), tableName.cstr() );
  }

  String tmpName = toUpperCase(tableName);
  TableInfo *tableInfop = m_tableInfoTableNameCache.get(tmpName);
  if(tableInfop == nullptr) {
    const TableDefinition &tableDef   = getTableDefinition(tableName);
    const IndexArray      &indexArray = getIndexDefinitions(tableDef.getTableName());
    TableInfo tableInfo(tableDef,indexArray);
    m_tableInfoTableNameCache.put(tmpName,tableInfo);
    tableInfop = m_tableInfoTableNameCache.get(tmpName);
    m_tableInfoSequenceCache.put(tableInfop->getSequenceNo(),tableInfop);
  }
  return *tableInfop;
}

const TableInfo &Database::getTableInfo(ULONG sequenceNo) const {
  TableInfo **tableInfopp = m_tableInfoSequenceCache.get(sequenceNo);
  if(tableInfopp != nullptr) {
    return **tableInfopp;
  }
  const TableDefinition &tableDef = getTableDefinition(sequenceNo);
  return getTableInfo(tableDef.getTableName());
}


TableDefinition Database::readTableDefinition(ULONG sequenceNo) const {
  KeyFile  indexfile(*this, SYSTEM_TABLEKEY2_FNAME , DBFMODE_READONLY);
  DataFile datafile( *this, SYSTEM_TABLEDATA_FNAME , DBFMODE_READONLY);

  KeyFileDefinition keydef(indexfile);
  KeyType tabkey;
  keydef.put(tabkey,0, sequenceNo);

  if(!indexfile.searchMin( RELOP_EQ, tabkey, 1)) {
    throwSqlError(SQL_INVALID_TABLENAME,_T("Table <%d> doesn't exist"), sequenceNo);
  }

  SysTableTableData tabdata;
  datafile.readRecord(keydef.getRecordAddr(tabkey), &tabdata, sizeof(SysTableTableData));

  return readColumnDefinitions(tabdata);
}

TableDefinition Database::readTableDefinition(const String &tableName) const {
  KeyFile  indexfile(*this, SYSTEM_TABLEKEY1_FNAME , DBFMODE_READONLY);
  DataFile datafile( *this, SYSTEM_TABLEDATA_FNAME , DBFMODE_READONLY);

  KeyFileDefinition keydef(indexfile);
  KeyType tabkey;
  keydef.put(tabkey,0,toUpperCase(tableName));

  if(!indexfile.searchMin( RELOP_EQ, tabkey, 1)) {
    throwSqlError(SQL_INVALID_TABLENAME,_T("Table <%s> doesn't exist"), tableName.cstr() );
  }

  SysTableTableData tabdata;
  datafile.readRecord(keydef.getRecordAddr(tabkey), &tabdata, sizeof(SysTableTableData));

  return readColumnDefinitions(tabdata);
}


TableDefinition Database::readColumnDefinitions(const SysTableTableData &tabData) const {
  TableDefinition result(tabData);

  KeyFile indexfile(*this, SYSTEM_COLUMNKEY_FNAME  , DBFMODE_READONLY);
  DataFile datafile(*this, SYSTEM_COLUMNDATA_FNAME , DBFMODE_READONLY);

  KeyType colkey;
  KeyFileDefinition keydef2(indexfile);
  keydef2.put(colkey,0,result.getTableName());

  KeyCursor cursor(indexfile,
                   RELOP_EQ, &colkey, 1,
                   RELOP_EQ, &colkey, 1,
                   SORT_ASCENDING);

  while(cursor.hasNext()) {
    cursor.next(colkey);
    SysTableColumnData colData;
    datafile.readRecord(keydef2.getRecordAddr(colkey),&colData, sizeof(SysTableColumnData));
    result.addColumn(colData);
  }
  return result;
}
