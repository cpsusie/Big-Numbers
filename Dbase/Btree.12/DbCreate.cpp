#include "stdafx.h"
#include <stddef.h>

DbNames::DbNames() {
  _tcscpy(TABLEDATA_FNAME       , _T("table.dat"        )     );
  _tcscpy(TABLEKEY1_FNAME       , _T("table1.key"       )     );
  _tcscpy(TABLEKEY2_FNAME       , _T("table2.key"       )     );
  _tcscpy(COLUMNDATA_FNAME      , _T("column.dat"       )     );
  _tcscpy(COLUMNKEY_FNAME       , _T("column.key"       )     );
  _tcscpy(INDEXDATA_FNAME       , _T("index.dat"        )     );
  _tcscpy(INDEXKEY1_FNAME       , _T("index1.key"       )     );
  _tcscpy(INDEXKEY2_FNAME       , _T("index2.key"       )     );
  _tcscpy(USERDATA_FNAME        , _T("user.dat"         )     );
  _tcscpy(USERKEY_FNAME         , _T("user.key"         )     );
  _tcscpy(CODEDATA_FNAME        , _T("code.dat"         )     );
  _tcscpy(CODEKEY_FNAME         , _T("code.key"         )     );
  _tcscpy(STATDATA_FNAME        , _T("stat.dat"         )     );
  _tcscpy(STATKEY_FNAME         , _T("stat.key"         )     );
  _tcscpy(LOG_FNAME             , _T("tmf.log"          )     );
  _tcscpy(DBDATA_FNAME          , _T("c:\\dbdef\\dbdef.dat")  );
  _tcscpy(DBKEY_FNAME           , _T("c:\\dbdef\\dbdef.key")  );
  _tcscpy(DBLOG_FNAME           , _T("c:\\dbdef\\dbdef.log")  );

  _tcscpy(SYSTABLE_LOGICALNAME  , _T("SYSTABLE"         )     );
  _tcscpy(SYSCOLUMN_LOGICALNAME , _T("SYSCOLUMN"        )     );
  _tcscpy(SYSINDEX_LOGICALNAME  , _T("SYSINDEX"         )     );
  _tcscpy(SYSUSER_LOGICALNAME   , _T("SYSUSER"          )     );
  _tcscpy(SYSCODE_LOGICALNAME   , _T("SYSCODE"          )     );
  _tcscpy(SYSSTAT_LOGICALNAME   , _T("SYSSTAT"          )     );
};

DbNames systemFileNames;

static void createSystemDataFile(const SysTableDbData &dbdef, const String &name) {
  String fullName = DbFile::dbFileName(dbdef.m_path,name);
  if(DbFile::exist(fullName)) {
    DbFile::destroy(fullName);
  }
  DataFile::create(fullName);
}

static void createSystemKeyFile(const SysTableDbData &dbdef, const String &name, const KeyFileDefinition &keydef) {
  String fullName = DbFile::dbFileName(dbdef.m_path,name);
  if(DbFile::exist(fullName)) {
    DbFile::destroy(fullName);
  }
  KeyFile::create(fullName, keydef);
}

#define SIZEOF(s,m) sizeof(((s*)0)->m)
#define OFFSETSIZE(s,m) offsetof(s,m),SIZEOF(s,m)

void Database::create(SysTableDbData &dbdef) {
  KeyFileDefinition keydef;
  bool              insertDone = false;

  if(searchDbName(dbdef.m_dbName, dbdef)) {
    throwSqlError(SQL_DB_ALREADY_EXIST,_T("Database <%s> already exists"),dbdef.m_dbName);
  }

  try {
    sysTabDbInsert(dbdef);
    insertDone = true;

    String logFileName = DbFile::dbFileName(dbdef.m_path, SYSTEM_LOG_FNAME);

    if(DbFile::exist(logFileName)) {
      DbFile::destroy(logFileName);
    }

    DbFile::create(logFileName);

    Database db(dbdef.m_dbName);

    db.trbegin();

    createSystemDataFile(dbdef,SYSTEM_TABLEDATA_FNAME  );
    createSystemDataFile(dbdef,SYSTEM_COLUMNDATA_FNAME );
    createSystemDataFile(dbdef,SYSTEM_INDEXDATA_FNAME  );
    createSystemDataFile(dbdef,SYSTEM_USERDATA_FNAME   );

    createSystemDataFile(dbdef,SYSTEM_CODEDATA_FNAME   );
    createSystemDataFile(dbdef,SYSTEM_STATDATA_FNAME   );

  // creating keydefinition for systable TABLE
    keydef.init();
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_STRING,SIZEOF(SysTableTableData,m_tableName));   // tableName
    keydef.addAddrField(true);
    createSystemKeyFile(dbdef, SYSTEM_TABLEKEY1_FNAME, keydef);

    keydef.init();
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_ULONG );                                         // sequenceNo
    keydef.addAddrField(true);
    createSystemKeyFile(dbdef, SYSTEM_TABLEKEY2_FNAME, keydef);

  // creating keydefinition for systable COLUMN
    keydef.init();
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_STRING,SIZEOF(SysTableColumnData,m_tableName));  // tableName
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_USHORT);                                         // colno
    keydef.addAddrField(true);
    createSystemKeyFile(dbdef, SYSTEM_COLUMNKEY_FNAME, keydef);

  // creating keydefinition for systable INDEX1
    keydef.init();
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_STRING,SIZEOF(SysTableIndexData,m_tableName));   // tableName
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_STRING,SIZEOF(SysTableIndexData,m_indexName));   // indexName
    keydef.addAddrField(true);
    createSystemKeyFile(dbdef, SYSTEM_INDEXKEY1_FNAME, keydef);

  // creating keydefinition for systable INDEX2
    keydef.init();
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_STRING,SIZEOF(SysTableIndexData,m_indexName));   // indexName
    keydef.addAddrField(true);
    createSystemKeyFile(dbdef, SYSTEM_INDEXKEY2_FNAME, keydef);

  // creating keydefinition for systable USER
    keydef.init();
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_STRING,SIZEOF(SysTableUserData,m_username));     // userName
    keydef.addAddrField(true);
    createSystemKeyFile(dbdef, SYSTEM_USERKEY_FNAME, keydef);

  // creating keydefinition for systable CODE
    keydef.init();
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_STRING,SIZEOF(SysTableCodeKey,m_filename));      // filename
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_INT   );                                         // nr
    keydef.addAddrField(true);
    createSystemKeyFile(dbdef, SYSTEM_CODEKEY_FNAME, keydef);

  // creating keydefinition for systable STAT
    keydef.init();
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_STRING,SIZEOF(SysTableStatData,m_indexName));    // indexName
    keydef.addAddrField(true);
    createSystemKeyFile(dbdef, SYSTEM_STATKEY_FNAME, keydef);

    db.trcommit();

    db.trbegin();

    ULONG sequenceNo = 0;
    String fullName = DbFile::dbFileName(dbdef.m_path,SYSTEM_TABLEDATA_FNAME);
    db.sysTabTableInsert( SYSTABLE    ,fullName, sequenceNo++, TABLETYPE_SYSTEM, sizeof(SysTableTableData)  );

    fullName = DbFile::dbFileName(dbdef.m_path,SYSTEM_COLUMNDATA_FNAME );
    db.sysTabTableInsert( SYSCOLUMN   ,fullName, sequenceNo++, TABLETYPE_SYSTEM, sizeof(SysTableColumnData) );

    fullName = DbFile::dbFileName(dbdef.m_path,SYSTEM_CODEDATA_FNAME   );
    db.sysTabTableInsert( SYSCODE     ,fullName, sequenceNo++, TABLETYPE_SYSTEM, MAXRECSIZE                 );

    fullName = DbFile::dbFileName(dbdef.m_path,SYSTEM_INDEXDATA_FNAME  );
    db.sysTabTableInsert( SYSINDEX    ,fullName, sequenceNo++, TABLETYPE_SYSTEM, sizeof(SysTableIndexData)  );

    fullName = DbFile::dbFileName(dbdef.m_path,SYSTEM_USERDATA_FNAME   );
    db.sysTabTableInsert( SYSUSER     ,fullName, sequenceNo++, TABLETYPE_SYSTEM, sizeof(SysTableUserData)   );

    fullName = DbFile::dbFileName(dbdef.m_path,SYSTEM_STATDATA_FNAME   );
    db.sysTabTableInsert( SYSSTAT     ,fullName, sequenceNo++, TABLETYPE_SYSTEM, sizeof(SysTableStatData)   );

    db.sysTabColumnInsert( SYSTABLE , 0,"TABLENAME"   ,DBTYPE_STRING, OFFSETSIZE(SysTableTableData , m_tableName    ),'N',"");
    db.sysTabColumnInsert( SYSTABLE , 1,"FILENAME"    ,DBTYPE_STRING, OFFSETSIZE(SysTableTableData , m_fileName     ),'N',"");
    db.sysTabColumnInsert( SYSTABLE , 2,"SEQUENCENO"  ,DBTYPE_ULONG , OFFSETSIZE(SysTableTableData , m_sequenceNo   ),'N',"");
    db.sysTabColumnInsert( SYSTABLE , 3,"TABLETYPE"   ,DBTYPE_STRING, OFFSETSIZE(SysTableTableData , m_tableType    ),'N',"");
    db.sysTabColumnInsert( SYSTABLE , 4,"RECSIZE"     ,DBTYPE_USHORT, OFFSETSIZE(SysTableTableData , m_recSize      ),'N',"");

    db.sysTabColumnInsert( SYSCOLUMN, 0,"TABLENAME"   ,DBTYPE_STRING, OFFSETSIZE(SysTableColumnData, m_tableName    ),'N',"");
    db.sysTabColumnInsert( SYSCOLUMN, 1,"COLNO"       ,DBTYPE_USHORT, OFFSETSIZE(SysTableColumnData, m_colNo        ),'N',"");
    db.sysTabColumnInsert( SYSCOLUMN, 2,"COLUMNNAME"  ,DBTYPE_STRING, OFFSETSIZE(SysTableColumnData, m_columnName   ),'N',"");
    db.sysTabColumnInsert( SYSCOLUMN, 3,"DBTYPE"      ,DBTYPE_STRING, OFFSETSIZE(SysTableColumnData, m_dbType       ),'N',"");
    db.sysTabColumnInsert( SYSCOLUMN, 4,"OFFSET"      ,DBTYPE_ULONG , OFFSETSIZE(SysTableColumnData, m_offset       ),'N',"");
    db.sysTabColumnInsert( SYSCOLUMN, 5,"LEN"         ,DBTYPE_ULONG , OFFSETSIZE(SysTableColumnData, m_len          ),'N',"");
    db.sysTabColumnInsert( SYSCOLUMN, 6,"NULLALLOWED" ,DBTYPE_STRING, OFFSETSIZE(SysTableColumnData, m_nulls        ),'N',"");
    db.sysTabColumnInsert( SYSCOLUMN, 7,"DEFAULTVALUE",DBTYPE_STRING, OFFSETSIZE(SysTableColumnData, m_defaultValue ),'N',"");

    db.sysTabColumnInsert( SYSINDEX , 0,"TABLENAME"   ,DBTYPE_STRING, OFFSETSIZE(SysTableIndexData , m_tableName    ),'N',"");
    db.sysTabColumnInsert( SYSINDEX , 1,"INDEXNAME"   ,DBTYPE_STRING, OFFSETSIZE(SysTableIndexData , m_indexName    ),'N',"");
    db.sysTabColumnInsert( SYSINDEX , 2,"FILENAME"    ,DBTYPE_STRING, OFFSETSIZE(SysTableIndexData , m_fileName     ),'N',"");
    db.sysTabColumnInsert( SYSINDEX , 3,"INDEXTYPE"   ,DBTYPE_STRING, OFFSETSIZE(SysTableIndexData , m_indexType    ),'N',"");
    db.sysTabColumnInsert( SYSINDEX , 4,"COLCOUNT"    ,DBTYPE_USHORT, OFFSETSIZE(SysTableIndexData , m_colCount     ),'N',"");
    db.sysTabColumnInsert( SYSINDEX , 5,"COLUMNS"     ,DBTYPE_STRING, OFFSETSIZE(SysTableIndexData , m_columns      ),'N',"");
    db.sysTabColumnInsert( SYSINDEX , 6,"SORTDIR"     ,DBTYPE_STRING, OFFSETSIZE(SysTableIndexData , m_ascending    ),'N',"");

    db.sysTabColumnInsert( SYSUSER  , 0,"USERNAME"    ,DBTYPE_STRING, OFFSETSIZE(SysTableUserData  , m_username     ),'N',"");
    db.sysTabColumnInsert( SYSUSER  , 1,"PASSWORD"    ,DBTYPE_STRING, OFFSETSIZE(SysTableUserData  , m_password     ),'N',"");

    db.sysTabColumnInsert( SYSSTAT  , 0,"INDEXNAME"   ,DBTYPE_STRING, OFFSETSIZE(SysTableStatData  , m_indexName    ),'N',"");
    db.sysTabColumnInsert( SYSSTAT  , 1,"TOTALCOUNT"  ,DBTYPE_DOUBLE, OFFSETSIZE(SysTableStatData  , m_totalCount   ),'N',"");
    db.sysTabColumnInsert( SYSSTAT  , 2,"FIELD1"      ,DBTYPE_DOUBLE, OFFSETSIZE(SysTableStatData  , m_field1       ),'N',"");
    db.sysTabColumnInsert( SYSSTAT  , 3,"FIELD2"      ,DBTYPE_DOUBLE, OFFSETSIZE(SysTableStatData  , m_field2       ),'N',"");
    db.sysTabColumnInsert( SYSSTAT  , 4,"FIELD3"      ,DBTYPE_DOUBLE, OFFSETSIZE(SysTableStatData  , m_field3       ),'N',"");
    db.sysTabColumnInsert( SYSSTAT  , 5,"FIELD4"      ,DBTYPE_DOUBLE, OFFSETSIZE(SysTableStatData  , m_field4       ),'N',"");

    IndexDefinition indexRec;
  // creating index for systable SYSTABLE KEY1
    indexRec.m_tableName    = SYSTABLE;
    indexRec.m_indexName    = "TABLEIX1";
    indexRec.m_fileName     = DbFile::dbFileName(dbdef.m_path,SYSTEM_TABLEKEY1_FNAME);
    indexRec.m_indexType    = INDEXTYPE_PRIMARY;
    indexRec.m_columns.clear();
    indexRec.m_columns.add(IndexColumn(0,true));  // tableName
    db.sysTabIndexInsert(indexRec);

  // creating index for systable SYSTABLE KEY2
    indexRec.m_tableName    = SYSTABLE;
    indexRec.m_indexName    = "TABLEIX2";
    indexRec.m_fileName     = DbFile::dbFileName(dbdef.m_path,SYSTEM_TABLEKEY2_FNAME);
    indexRec.m_indexType    = INDEXTYPE_UNIQUE;
    indexRec.m_columns.clear();
    indexRec.m_columns.add(IndexColumn(2,true));  // sequenceNo
    db.sysTabIndexInsert(indexRec);

  // creating index for systable SYSCOLUMN
    indexRec.m_tableName    = SYSCOLUMN;
    indexRec.m_indexName    = "COLUMNIX";
    indexRec.m_fileName     = DbFile::dbFileName(dbdef.m_path,SYSTEM_COLUMNKEY_FNAME);
    indexRec.m_indexType    = INDEXTYPE_PRIMARY;
    indexRec.m_columns.clear();
    indexRec.m_columns.add(IndexColumn(0,true));  // tableName
    indexRec.m_columns.add(IndexColumn(1,true));  // colno
    db.sysTabIndexInsert(indexRec);

  // creating index for systable SYSINDEX KEY1
    indexRec.m_tableName    = SYSINDEX;
    indexRec.m_indexName    = "INDEXIX1";
    indexRec.m_fileName     = DbFile::dbFileName(dbdef.m_path,SYSTEM_INDEXKEY1_FNAME);
    indexRec.m_indexType    = INDEXTYPE_PRIMARY;
    indexRec.m_columns.clear();
    indexRec.m_columns.add(IndexColumn(0,true));  // tableName
    indexRec.m_columns.add(IndexColumn(1,true));  // indexName
    db.sysTabIndexInsert(indexRec);

  // creating index for systable SYSINDEX KEY2
    indexRec.m_tableName    = SYSINDEX;
    indexRec.m_indexName    = "INDEXIX2";
    indexRec.m_fileName     = DbFile::dbFileName(dbdef.m_path,SYSTEM_INDEXKEY2_FNAME);
    indexRec.m_indexType    = INDEXTYPE_UNIQUE;
    indexRec.m_columns.clear();
    indexRec.m_columns.add(IndexColumn(1,true));  // indexName
    db.sysTabIndexInsert(indexRec);

  // creating index for systable SYSUSER
    indexRec.m_tableName    = SYSUSER ;
    indexRec.m_indexName    = "USERIX";
    indexRec.m_fileName     = DbFile::dbFileName(dbdef.m_path,SYSTEM_USERKEY_FNAME  );
    indexRec.m_indexType    = INDEXTYPE_PRIMARY;
    indexRec.m_columns.clear();
    indexRec.m_columns.add(IndexColumn(0,true));  // userName
    db.sysTabIndexInsert(indexRec);

  // creating index for systable SYSCODE
    indexRec.m_tableName    = SYSCODE;
    indexRec.m_indexName    = "CODEIX";
    indexRec.m_fileName     = DbFile::dbFileName(dbdef.m_path,SYSTEM_CODEKEY_FNAME  );
    indexRec.m_indexType    = INDEXTYPE_PRIMARY;
    indexRec.m_columns.clear();
    indexRec.m_columns.add(IndexColumn(0,true));  // filename
    indexRec.m_columns.add(IndexColumn(1,true));  // nr
    db.sysTabIndexInsert(indexRec);

  // creating index for systable SYSSTAT
    indexRec.m_tableName    =  SYSSTAT;
    indexRec.m_indexName    = "STATIX";
    indexRec.m_fileName     = DbFile::dbFileName(dbdef.m_path,SYSTEM_STATKEY_FNAME  );
    indexRec.m_indexType    = INDEXTYPE_PRIMARY;
    indexRec.m_columns.clear();
    indexRec.m_columns.add(IndexColumn(0,true));  // indexName
    db.sysTabIndexInsert(indexRec);

    db.sysTabUserInsert( "ADM","ADM"); // create user ADM
    db.trcommit();
  } catch(sqlca ca) {
    if(insertDone) {
      sysTabDbDelete(dbdef.m_dbName);
    }
    try {
      destroySystemFiles(dbdef);
    } catch(...) {
      // ignore
    }

    throw ca;
  }
}
