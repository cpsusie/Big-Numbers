#include "stdafx.h"

void sqlCreateIndex(Database &db, CreateIndexData &data) {
  const TableDefinition &tableDef = db.getTableDefinition(data.m_tableName);
  IndexDefinition        indexDef;
  
  indexDef.m_tableName = tableDef.getTableName();
  indexDef.m_fileName  = EMPTYSTRING;
  indexDef.m_indexName = data.m_indexName;
  indexDef.m_indexType = data.m_unique ? INDEXTYPE_UNIQUE : INDEXTYPE_NON_UNIQUE;
  for(UINT i = 0; i < data.m_columns.size(); i++) { // find hver column i data
    const IndexColumnName &col = data.m_columns[i];
    int j = tableDef.findColumnIndex(col.m_colName);
    if(j < 0)
      throwSqlError(SQL_INVALID_COLUMNNAME,_T("Column %s not defined"),col.m_colName.cstr());
    else {
      indexDef.m_columns.add(IndexColumn(j,col.m_asc));
    }
  }
  db.indexCreate(indexDef);
}

String CreateIndexData::toString() const {
  String result = _T("CreateIndexData:\n");
  result += format(_T("  Indexname  :<%s>\n"), m_indexName.cstr());
  result += format(_T("  Tablename  :<%s>\n"), m_tableName.cstr());
  result += format(_T("  Unique     :%s\n"  ), boolToStr(m_unique));
  result += format(_T("  ColumnCount:%zd\n" ), m_columns.size());
  for(size_t i = 0; i < m_columns.size(); i++) {
    const IndexColumnName &col = m_columns[i];
    result += format(_T("    ColName[%zd]:<%s> <%c>\n"), i,col.m_colName.cstr(),col.m_asc ? 'A':'D');
  }
  return result;
}

Packer &operator<<(Packer &p, const IndexColumnName &col) {
  p << col.m_colName << col.m_asc;
  return p;
}

Packer &operator>>(Packer &p, IndexColumnName &col) {
  p >> col.m_colName >> col.m_asc;
  return p;
}

Packer &operator<<(Packer &p, const CreateIndexData &inx) {
  UINT columnCount = (UINT)inx.m_columns.size();
  p << inx.m_tableName
    << inx.m_indexName
    << inx.m_unique
    << columnCount;
  for(UINT i = 0; i < columnCount; i++)
    p << inx.m_columns[i];
  return p;
}

Packer &operator>>(Packer &p, CreateIndexData &inx) {
  UINT columnCount;
  p >> inx.m_tableName
    >> inx.m_indexName
    >> inx.m_unique
    >> columnCount;
  for(UINT i = 0; i < columnCount; i++) {
    IndexColumnName col;
    p >> col;
    inx.m_columns.add(col);
  }
  return p;
}
