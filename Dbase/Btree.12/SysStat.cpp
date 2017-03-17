#include "stdafx.h"
#include <math.h>

void Database::sysTabStatInsert(const SysTableStatData &stat) {
  SysTableStatData  rec;

  rec = stat;

  _tcsupr(rec.m_indexName);

  DataFile datafile(  *this, SYSTEM_STATDATA_FNAME, DBFMODE_READWRITE);
  KeyFile  indexfile( *this, SYSTEM_STATKEY_FNAME , DBFMODE_READWRITE);

  KeyFileDefinition keydef(indexfile);
  KeyType key;
  keydef.put(key,0,String(rec.m_indexName));
  bool found = indexfile.searchMin( RELOP_EQ, key, 1);

  if(found) {
    datafile.update(keydef.getRecordAddr(key),&rec);
  }
  else {
    DbAddr addr = datafile.insert( &rec, sizeof(rec));
    keydef.put(key, 0, String(rec.m_indexName));
    keydef.putRecordAddr(key, addr);
    indexfile.insert(key);
  }
}

void Database::sysTabStatDelete(const String &indexName) {

  DataFile datafile(  *this, SYSTEM_STATDATA_FNAME, DBFMODE_READWRITE);
  KeyFile  indexfile( *this, SYSTEM_STATKEY_FNAME , DBFMODE_READWRITE);

  KeyType           key;
  KeyFileDefinition keydef(indexfile);
  keydef.put(key,0,toUpperCase(indexName));
  bool found = indexfile.searchMin( RELOP_EQ, key, 1);

  if(found) {
    datafile.remove(keydef.getRecordAddr(key));
    indexfile.remove(key);
  }
}

bool Database::sysTabStatRead(const String &indexName, SysTableStatData &stat) const {

  DataFile datafile(  *this, SYSTEM_STATDATA_FNAME, DBFMODE_READWRITE);
  KeyFile  indexfile( *this, SYSTEM_STATKEY_FNAME , DBFMODE_READWRITE);

  KeyType key;
  KeyFileDefinition keydef(indexfile);
  keydef.put(key,0,toUpperCase(indexName));
  bool found = indexfile.searchMin( RELOP_EQ, key, 1);

  if(found) {
    datafile.readRecord(keydef.getRecordAddr(key),&stat, sizeof(SysTableStatData));
  }
  return found;
}

void Database::updateStatistics( const IndexDefinition &indexDef) {
  KeyFile  indexfile( *this, indexDef.m_fileName, DBFMODE_READONLY,false);

  KeyFileDefinition keydef(indexfile);
  KeyCursor cursor(indexfile,
                   RELOP_TRUE, NULL, 0,
                   RELOP_TRUE, NULL, 0,
                   SORT_ASCENDING
                  );
  int nfields = min(4,keydef.getKeyFieldCount() - 1); // dont use DbAddr-field
  KeyType key,last;
  double groupcount[5];   // # of different elements in each group
  double totalcount = 0;
  if(cursor.hasNext()) {
    cursor.next(key);
    totalcount++;
    for(int i = 0; i <= nfields; i++)
      groupcount[i] = 1;
    last = key; // keydef.keycpy(last,key);
    while(cursor.hasNext()) {
      cursor.next(key);
      for(int i = 1; i <= nfields; i++) {
        if(keydef.keynCmp(key,last,i) != 0) {
          for(int j = i; j <= nfields; j++)
            groupcount[j]++;
          break;
        }
      }
      last = key; // keydef.keycpy(last,key);
      totalcount++;
    }
  }

  SysTableStatData stat;
  _tcsncpy(stat.m_indexName,indexDef.m_indexName.cstr(),ARRAYSIZE(stat.m_indexName));
  stat.m_totalCount = totalcount;
  stat.m_field1     = 0;
  stat.m_field2     = 0;
  stat.m_field3     = 0;
  stat.m_field4     = 0;

  if(totalcount > 0) {
    if(nfields >= 1 && groupcount[1] > 0)
      stat.m_field1 = 1.0 / groupcount[1];
    if(nfields >= 2 && groupcount[2] > 0)
      stat.m_field2 = 1.0 / groupcount[2];
    if(nfields >= 3 && groupcount[3] > 0)
      stat.m_field3 = 1.0 / groupcount[3];
    if(nfields >= 4 && groupcount[4] > 0)
      stat.m_field4 = 1.0 / groupcount[4];
  }
  trbegin();
  sysTabStatInsert(stat);
  trcommit();
}

void Database::updateStatistics(const String &tableName) {
  const IndexArray &indexArray = getIndexDefinitions(tableName);
  for(UINT i = 0; i < indexArray.size(); i++) {
    updateStatistics(indexArray[i]);
  }
}

void Database::updateStatistics() {
  KeyFile  indexfile( *this, SYSTEM_TABLEKEY1_FNAME, DBFMODE_READONLY);
  KeyCursor cursor(indexfile,
                   RELOP_TRUE, NULL, 0,
                   RELOP_TRUE, NULL, 0,
                   SORT_ASCENDING
                  );
  KeyFileDefinition keydef(indexfile);
  KeyType key;
  while(cursor.hasNext()) {
    cursor.next(key);
    String tableName;
    keydef.get(key,0,tableName);
    updateStatistics(tableName.cstr());
  }
}


void SysTableStatData::dump(FILE *f) const {
  _ftprintf(f,_T("<%s> [%lg,%lg,%lg,%lg,%lg]\n"),
    m_indexName,m_totalCount,
    m_field1,
    m_field2,
    m_field3,
    m_field4
  );
}

double SysTableStatData::selectivity(int fieldcount) const {
  switch(fieldcount) {
  case 0 : return 1;
  case 1 : return m_field1;
  case 2 : return m_field2;
  case 3 : return m_field3;
  case 4 : return m_field4;
  default:
    if(m_field4 == 1)
      return pow(0.3333,fieldcount);
    else if(m_field4 == 0) // should not happen,
      return pow(0.01,fieldcount);
    else
      return pow(m_field4, fieldcount - 4);
  }
}
