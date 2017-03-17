#include "stdafx.h"

ULONG Database::getMaxTableSequenceNo() const {
  KeyFile        index(*this, SYSTEM_TABLEKEY2_FNAME, DBFMODE_READONLY); // find max sequenceno
  KeyCursor cursor(index,
                   RELOP_TRUE, NULL, 0,
                   RELOP_TRUE, NULL, 0,
                   SORT_DESCENDING);
  KeyType key;
  KeyFileDefinition keydef(index);
  if(!cursor.hasNext()) {
    throwSqlError(SQL_DBCORRUPTED,_T("Cannot find max tablesequenceno. Database corrupted"));
  }
  cursor.next(key);
  ULONG sequenceno;
  keydef.get(key,0,sequenceno);
  return sequenceno;
}

void Database::tableCreate(const TableDefinition &tableDef) {

  if(tableDef.getRecordSize() > MAXRECSIZE || tableDef.getRecordSize() == 0) {
    throwSqlError(SQL_INVALID_RECSIZE,_T("Invalid recordsize (=%d)"),tableDef.getRecordSize());
  }

  String fileName = tableDef.getFileName();

  if(fileName.length() == 0) {
    fileName = getNewFileName("DAT");
  }

  for(UINT i = 0; i < tableDef.getColumnCount(); i++) {
    const ColumnDefinition &col1 = tableDef.getColumn(i);
    for(UINT j = 0; j < i; j++ ) {
      const ColumnDefinition &col2 = tableDef.getColumn(j);
      if(col1.m_name.equalsIgnoreCase(col2.m_name))
        throwSqlError(SQL_DUPLICATE_FIELDNAME,_T("Duplicate fieldname:<%s>"),col1.m_name.cstr());
    }
  }

  KeyFile           indexFile(*this, SYSTEM_TABLEKEY1_FNAME , DBFMODE_READONLY);
  KeyFileDefinition keydef(indexFile);
  KeyType           tabkey;
  const String      tmpname = toUpperCase(tableDef.getTableName());
  keydef.put(tabkey,0,tmpname);

  if(indexFile.searchMin(RELOP_EQ, tabkey, 1)) { /* search on first field */
    throwSqlError(SQL_TABLE_ALREADY_EXIST,_T("Table <%s> already exist"), tmpname.cstr());
  }

  if(DbFile::exist(fileName)) {
    throwSqlError(SQL_FILE_ALREADY_EXIST,_T("File <%s> already exist"),fileName.cstr());
  }

  ULONG sequenceNo = getMaxTableSequenceNo() + 1;

  sysTabTableInsert(tmpname,
                    fileName,
                    sequenceNo,
                    TABLETYPE_USER,
                    tableDef.getRecordSize()
                    );

  for(UINT i = 0; i < tableDef.getColumnCount(); i++) {
    const ColumnDefinition &tabcol = tableDef.getColumn(i);

    sysTabColumnInsert(tmpname,
                       i,
                       tabcol.m_name,
                       tabcol.getType(),
                       tabcol.m_offset,
                       tabcol.m_len,
                       (char)(tabcol.m_nullAllowed ? 'Y' : 'N'),
                       tabcol.m_defaultValue
                       );
  }

  DataFile::create(fileName);
}

void Database::tableCreateLike(const String &tableName,
                               const String &likename ,
                               const String &fileName
                              ) {
  if(tableName.length() == 0 ||  tableName.length() > MAXTABLENAME) {
    throwSqlError(SQL_INVALID_TABLENAME,_T("Invalid tablename:<%s>"),tableName.cstr());
  }

  const TableDefinition &tableDef   = getTableDefinition(likename);
  const IndexArray      &indexArray = getIndexDefinitions(likename);

  int primusx = -1;
  for(size_t i = 0; i < indexArray.size(); i++) { /* find the definition of the primary key */
    if(indexArray[i].getIndexType() == INDEXTYPE_PRIMARY) {
      primusx = (int)i;
      break;
    }
  }

  TableDefinition newTableDef(TABLETYPE_USER,tableName,fileName);
  for(UINT i = 0; i < tableDef.getColumnCount(); i++) {
    const ColumnDefinition &col = tableDef.getColumn(i);
    newTableDef.addColumn(col);
  }

  tableCreate(newTableDef);

  if(primusx >= 0) {
    IndexDefinition indexDef = indexArray[primusx];
    indexDef.m_tableName = tableName;
    indexDef.m_indexName = EMPTYSTRING;
    indexDef.m_fileName  = EMPTYSTRING;
    indexCreate(indexDef);
  }
}
