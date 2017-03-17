#include "stdafx.h"

void createDatabase(const String &dbname) {
  SysTableDbData dbdef;

  _tcscpy(dbdef.m_dbName,dbname.cstr());
  _tcscpy(dbdef.m_path,_T("test"));
  for(int i = 0; i < 256; i++)
    dbdef.m_colseq[i] = i;
  Database::create(dbdef);
}

static void testCreateDestroy(const String &dbname) {
  _tprintf(_T("test create/destroy database\n"));

  if(Database::exist(dbname))
    Database::destroy(dbname);

  createDatabase(dbname);
  Database::destroy(dbname);
}

static void testCreateUserTable(const String &dbname) {
  _tprintf(_T("test create usertable\n"));

  if(Database::exist(dbname))
    Database::destroy(dbname);

  createDatabase(dbname);
  Database db(dbname);

  TableDefinition tableDef(TABLETYPE_USER,_T("testTable"));

  tableDef.addColumn(ColumnDefinition(DBTYPE_INT,_T("testcolumn")));

  IndexDefinition indexDef;
  indexDef.m_tableName = tableDef.getTableName();
  indexDef.m_indexType = INDEXTYPE_PRIMARY;
  indexDef.m_columns.add(IndexColumn(0,true));

  db.trbegin();
  db.tableCreate(tableDef);
  db.indexCreate(indexDef);
  db.trcommit();
  db.close();

  Database::destroy(dbname);
}

void testDatabase() {
  testCreateDestroy(_T("test"));
  testCreateUserTable(_T("test"));
}
