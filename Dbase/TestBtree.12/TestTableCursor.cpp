#include "stdafx.h"

#define DBNAME _T("TEST")
#define TABLENAME _T("testTable")

void testUserRecord() {
  if(Database::exist(DBNAME))
    Database::destroy(DBNAME);

  createDatabase(DBNAME);

  Database db(DBNAME);

  TableDefinition tableDef(TABLETYPE_USER, TABLENAME);

  TestFields fields(true,TESTFIELD_ALL);
  for(int i = 0; i < fields.getFieldCount(); i++) {
    const KeyField &keyField = fields.getFieldDef(i);
    DbFieldType type = keyField.getType();
    if(type == DBTYPE_DBADDR) {
      continue;
    }
    ColumnDefinition col;
    col.m_name        = format(_T("column%02d"),tableDef.getColumnCount()+1);
    col.m_len         = keyField.getLen();
    col.m_type        = type;
    col.m_nullAllowed = isNullAllowed(type);
    tableDef.addColumn(col);
  }

  IndexDefinition indexDef;
  indexDef.m_tableName = tableDef.getTableName();
  indexDef.m_indexName = tableDef.getTableName();
  indexDef.m_indexType = INDEXTYPE_PRIMARY;
  for(i = 0; i < tableDef.getColumnCount(); i++) {
    const ColumnDefinition &col = tableDef.getColumn(i);
    if(isNullAllowed(col.getType())) {
      continue;
    }
    indexDef.m_columns.add(IndexColumn(i,true));
  }

  db.trbegin();

  db.tableCreate(tableDef);
  db.indexCreate(indexDef);

  db.trcommit();

  tableDef = db.getTableDefinition(TABLENAME);

//  tableDef.dump();

  TableRecord record(db,tableDef.getSequenceNo());

  for(i = 0; i < 10000; i++) {
    db.trbegin();
    Tuple tuple = fields.getRandomTuple();
    if(i % 100 == 0) {
      tuple.dump();
    }
    record.insert(tuple);
    db.trcommit();
  }
}
