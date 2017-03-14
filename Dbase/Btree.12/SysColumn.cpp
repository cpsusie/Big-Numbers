#include "stdafx.h"

void Database::sysTabColumnInsert(const String         &tableName
                                 ,USHORT        colno
                                 ,const String         &columnName
                                 ,DbFieldType           dbtype
                                 ,ULONG         offset
                                 ,ULONG         len
                                 ,char                  nulls
                                 ,const String         &defaultValue
                                 ) {

  if(columnName.length() == 0 ||  columnName.length() > MAXCOLUMNNAME) {
    throwSqlError(SQL_INVALID_COLUMNNAME,_T("Invalid columnname:<%s>. Valid length=[1..%d]"), columnName.cstr(),MAXCOLUMNNAME);
  }

  if(tableName.length() == 0 || tableName.length() > MAXTABLENAME) {
    throwSqlError(SQL_INVALID_TABLENAME,_T("Invalid tablename:<%s>. Valid length=[1..%d]"), tableName.cstr(), MAXTABLENAME);
  }

  if(defaultValue.length() > MAXDEFAULTVALUE) {
    throwSqlError(SQL_INVALID_DEFAULT,_T("Invalid defaultvalue:<%s>. Max length=%d"), defaultValue.cstr(), MAXDEFAULTVALUE);
  }

/*
  _tprintf(_T("Inserting %-13s,%2d,%-15s,%-10s,%3d,%3d,%c into columns\n"),
          tableName,colno,columnName,getTypeString[dbtype],offset,len,nulls);
*/

  SysTableColumnData rec;
  _tcsupr(_tcscpy(rec.m_tableName, tableName.cstr()));
  rec.m_colNo  = colno;
  _tcsupr(_tcscpy(rec.m_columnName,columnName.cstr()));
  _tcscpy(rec.m_dbType, getTypeString(dbtype));
  rec.m_offset = offset;
  rec.m_len    = len;
  rec.m_nulls  = nulls;
  _tcscpy(rec.m_defaultValue,defaultValue.cstr());

  DataFile datafile(  *this, SYSTEM_COLUMNDATA_FNAME, DBFMODE_READWRITE);
  KeyFile  indexfile( *this, SYSTEM_COLUMNKEY_FNAME,  DBFMODE_READWRITE);

  DbAddr addr = datafile.insert( &rec, sizeof(rec));

  KeyFileDefinition keydef(indexfile);
  KeyType key;

  keydef.put(key, 0, String(rec.m_tableName));
  keydef.put(key, 1, rec.m_colNo);
  keydef.putRecordAddr(key,addr);

  indexfile.insert(key);
}
