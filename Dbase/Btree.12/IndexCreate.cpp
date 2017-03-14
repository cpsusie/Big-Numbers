#include "stdafx.h"

class KeyGenerator : public DataFileScanner {
private:
  Database                &m_database;
  KeyFile                 &m_keyFile;
  const IndexDefinition   &m_indexDef;
  const TableInfo         &m_tableInfo;
  const KeyFileDefinition &m_keydef;
  bool handleData(DataFile &file, DbAddr addr, RecordType &rec);
public:
  KeyGenerator(Database &db, KeyFile &keyFile, const IndexDefinition &indexDef, const TableInfo &tableInfo, const KeyFileDefinition &keydef);
};

KeyGenerator::KeyGenerator(Database &db, KeyFile &keyFile, const IndexDefinition &indexDef, const TableInfo &tableInfo, const KeyFileDefinition &keydef)
: m_database(db)
, m_keyFile(keyFile)
, m_indexDef(indexDef)
, m_tableInfo(tableInfo)
, m_keydef(keydef)
{
}

bool KeyGenerator::handleData(DataFile &dbf, DbAddr addr, RecordType &rec) {
  KeyType key;

  for(UINT i = 0; i < m_indexDef.getColumnCount(); i++) {
    USHORT col = m_indexDef.getColumn(i).m_col;
    DbFieldType type = m_tableInfo.getColumn(col).getType();
    switch(type) {
    case DBTYPE_VARCHARN:
      if(!m_tableInfo.isDefined(rec,col)) {
        m_keydef.setUndefined(key,i);
        break;
      }
      // continue case
    case DBTYPE_VARCHAR :
      { DbAddr vcaddr;
        m_tableInfo.get(rec,col,vcaddr);
        if(vcaddr == DB_NULLADDR) { /* Inconsistency */
          throwSqlError(SQL_DBCORRUPTED,_T("Datafile <%s> corrupted"),dbf.getName().cstr() );
        }
        varchar vc;
        dbf.readVarChar(vcaddr,vc);
        m_keydef.put(key,i,vc);
        break;
      }
    default:
      if(!m_tableInfo.isDefined(rec,col)) {
        m_keydef.setUndefined(key,i);
      } else {
        TupleField t;
        m_tableInfo.get(rec,col,t);
        m_keydef.put(key,i,t);
      }
    }
  }

  m_keydef.putRecordAddr(key,addr);

#ifdef DEBUG_USXCREATE
  _tprintf(_T("Now inserting key:")); m_keydef.keyprintf(key); _tprintf(_T("\n"));
#endif

  m_keyFile.insert(key);
  return true;
}

void Database::indexCreate(IndexDefinition &indexDef) {

  if(indexDef.getColumnCount() < 1 || indexDef.getColumnCount() >= MAXKEYFIELDCOUNT) {
    /* must be < MAXKEYFIELDCOUNT because there has to be DbAddr at the end */
    throwSqlError(SQL_KEY_TOO_BIG,_T("Invalid columncount (=%d)"),indexDef.getColumnCount());
  }

  if(indexDef.getIndexType() != INDEXTYPE_PRIMARY &&
     indexDef.getIndexType() != INDEXTYPE_UNIQUE  &&
     indexDef.getIndexType() != INDEXTYPE_NON_UNIQUE) {
     throwSqlError(SQL_INVALID_INDEXTYPE,_T("Invalid indextype (=%d)"),indexDef.getIndexType());
  }

  if(indexDef.m_fileName.length() == 0) {
    indexDef.m_fileName = getNewFileName(_T("KEY"));
  }

  if(indexDef.m_indexName.length() == 0) {
    if(indexDef.getIndexType() == INDEXTYPE_PRIMARY) {
      indexDef.m_indexName = indexDef.m_tableName;
    } else {
      throwSqlError(SQL_INVALID_INDEXNAME,_T("Invalid indexname:<%s>"),indexDef.m_indexName.cstr());
    }
  }
    
  if(DbFile::exist(indexDef.m_fileName)) {
    throwSqlError(SQL_FILE_ALREADY_EXIST,_T("File <%s> already exist"),indexDef.m_fileName.cstr());
  }

  indexDef.m_tableName = toUpperCase(indexDef.m_tableName);
  indexDef.m_indexName = toUpperCase(indexDef.m_indexName);
  indexDef.m_fileName  = toUpperCase(indexDef.m_fileName);

  const TableDefinition &tableDef = getTableDefinition(indexDef.m_tableName);

  if(tableDef.getTableType() != TABLETYPE_USER) {
    throwSqlError(SQL_INVALID_TABLETYPE,_T("Invalid tabletype (=%c)"),tableDef.getTableType());
  }

/* check that the indexname is not already used */

  KeyFile indexfile2(*this,SYSTEM_INDEXKEY2_FNAME, DBFMODE_READWRITE);
  KeyFileDefinition keydef2(indexfile2);
  KeyType key2;
  keydef2.put(key2,0,indexDef.m_indexName );
  if(indexfile2.searchMin(RELOP_EQ, key2, 1)) {
    throwSqlError(SQL_INDEX_ALREADY_EXIST,_T("Index <%s> already exist"),indexDef.m_indexName.cstr());
  }

  if(!inTmf()) {
    throwSqlError(SQL_NO_TRANSACTION,_T("indexCreate:No active tmf"));
  }

  KeyFileDefinition keydef = tableDef.getKeyFileDefinition(indexDef);

  KeyFile::create(indexDef.m_fileName, keydef);

  try {
    KeyFile  keyFile( indexDef.m_fileName, DBFMODE_READWRITE, false);
    DataFile datafile(tableDef.getFileName(), DBFMODE_READONLY , false);
    const TableInfo &tableInfo = getTableInfo(tableDef.getTableName());

  /* TODO Lock datafile to avoid new records to be inserted */

    if(indexDef.getIndexType() != INDEXTYPE_PRIMARY) {
      dataFileScan(indexDef.m_tableName, KeyGenerator(*this, keyFile, indexDef, tableInfo, keydef));
    }

    sysTabIndexInsert(indexDef);

  /* TODO Invalidate all binded program-packages; Unlock datafile */
  } catch(sqlca) {
    DbFile::destroy(indexDef.m_fileName);
    throw;
  }
  removeFromCache(indexDef.m_tableName);
}
