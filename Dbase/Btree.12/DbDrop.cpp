#include "stdafx.h"
#include <direct.h>

class DeleteUserTable : public DataFileScanner {
private:
  Database &m_db;
  bool handleData(DataFile &file, DbAddr addr, RecordType &rec);
public:
  DeleteUserTable(Database &db) : m_db(db) {}
};

bool DeleteUserTable::handleData(DataFile &dbf, DbAddr addr, RecordType &rec) {
  SysTableTableData &tablerec = (SysTableTableData&)rec;

  if(tablerec.m_tableType == TABLETYPE_USER) {
    const IndexArray &indexArray = m_db.getIndexDefinitions(tablerec.m_tableName);
    DbFile::destroy(tablerec.m_fileName);
    for(UINT i = 0; i < indexArray.size(); i++) {
      DbFile::destroy(indexArray[i].m_fileName.cstr());
    }
  }
  return true;
}

static void deleteSystemFile(const SysTableDbData &dbdef, const String &filename) {
  try {
    DbFile::destroy(DbFile::dbFileName(dbdef.m_path,filename));
  } catch(sqlca) {
  }
}

void Database::destroySystemFiles(SysTableDbData &dbdef) {
  deleteSystemFile(dbdef,SYSTEM_TABLEDATA_FNAME );
  deleteSystemFile(dbdef,SYSTEM_COLUMNDATA_FNAME);
  deleteSystemFile(dbdef,SYSTEM_INDEXDATA_FNAME );
  deleteSystemFile(dbdef,SYSTEM_USERDATA_FNAME  );
  deleteSystemFile(dbdef,SYSTEM_CODEDATA_FNAME  );
  deleteSystemFile(dbdef,SYSTEM_STATDATA_FNAME  );
  deleteSystemFile(dbdef,SYSTEM_TABLEKEY1_FNAME );
  deleteSystemFile(dbdef,SYSTEM_TABLEKEY2_FNAME );
  deleteSystemFile(dbdef,SYSTEM_COLUMNKEY_FNAME );
  deleteSystemFile(dbdef,SYSTEM_INDEXKEY1_FNAME );
  deleteSystemFile(dbdef,SYSTEM_INDEXKEY2_FNAME );
  deleteSystemFile(dbdef,SYSTEM_USERKEY_FNAME   );
  deleteSystemFile(dbdef,SYSTEM_CODEKEY_FNAME   );
  deleteSystemFile(dbdef,SYSTEM_STATKEY_FNAME   );
  deleteSystemFile(dbdef,SYSTEM_LOG_FNAME       );

  _trmdir(dbdef.m_path);
}

void Database::destroy(const String &dbName) {
  if(dbName.length() > SQLAPI_MAXDBNAME) {
    throwSqlError(SQL_DBNAME_TOO_LONG,_T("Database name <%s> too long"),dbName.cstr());
  }

  SysTableDbData dbdef;

  if(!searchDbName(dbName, dbdef)) {
    throwSqlError(SQL_DB_NOT_EXIST,_T("Database <%s> doesn't exist"),dbName.cstr());
  }

  try {
    Database db(dbName);
    db.dataFileScan(SYSTABLE,DeleteUserTable(db));
    db.close();
  } catch(sqlca) {
  }

  try {
    destroySystemFiles(dbdef);
  } catch(sqlca) {
  }

  sysTabDbDelete(dbName);
}
