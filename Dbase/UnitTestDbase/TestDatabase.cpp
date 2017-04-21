#include "stdafx.h"
#include "DbFileTester.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestDatabase {

#include <UnitTestTraits.h>

  TEST_CLASS(TestDatabase) {
    public:

    void createDestroy(const String &dbname) {
      OUTPUT(_T("test create/destroy database"));

      if(Database::exist(dbname))
        Database::destroy(dbname);

      createDatabase(dbname);
      Database::destroy(dbname);
    }

    void createUserTable(const String &dbname) {
      OUTPUT(_T("test create usertable"));

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
    
    TEST_METHOD(testCreateDestroy) {
      createDestroy(_T("test"));
    }

    TEST_METHOD(testCreateUserTable) {
      createUserTable(_T("test"));
    }
  };
}
