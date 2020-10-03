#include "stdafx.h"

static bool openAndDumpKeyFile(  const Database &db, const String &filename, FILE *f) {
  KeyFile keyFile(db,filename, DBFMODE_READONLY);
  _ftprintf(f,_T("Dumping %s\n"), filename.cstr());
  keyFile.dump(f, DUMP_ALL | DUMP_NEWLINE);
  return true;
}

static bool openAndDumpDataFile( Database &db, const String &tableName, FILE *f) {
  _ftprintf(f,_T("Dumping %s\n"), tableName.cstr());
  db.dataFileDump( tableName, f );
  return true;
}

class TableDumper : public DataFileScanner {
private:
  Database &m_db;
  FILE     *m_output;
public:
  TableDumper(Database &db, FILE *output);
  bool handleData(DataFile &file, DbAddr addr, RecordType &rec);
};

TableDumper::TableDumper(Database &db, FILE *output) : m_db(db) {
  m_output = output;
}

bool TableDumper::handleData(DataFile &file, DbAddr addr, RecordType &rec) {
  TableDefinition *ustp = (TableDefinition *)&rec;
  if(ustp->getTableType() == TABLETYPE_USER) {
    openAndDumpDataFile(m_db, ustp->getTableName(),m_output);
  }
  return true;
}

void Database::dump(FILE *f, bool dumpall) {
  if(!openAndDumpDataFile(*this,SYSTABLE               ,f)) return;
  if(!openAndDumpKeyFile( *this,SYSTEM_TABLEKEY1_FNAME ,f)) return;
  if(!openAndDumpDataFile(*this,SYSCOLUMN              ,f)) return;
  if(!openAndDumpKeyFile( *this,SYSTEM_COLUMNKEY_FNAME ,f)) return;
  if(!openAndDumpDataFile(*this,SYSINDEX               ,f)) return;
  if(!openAndDumpKeyFile( *this,SYSTEM_INDEXKEY1_FNAME ,f)) return;
  if(!openAndDumpKeyFile( *this,SYSTEM_INDEXKEY2_FNAME ,f)) return;
  if(!openAndDumpDataFile(*this,SYSUSER                ,f)) return;
  if(!openAndDumpKeyFile( *this,SYSTEM_USERKEY_FNAME   ,f)) return;

  for(Iterator<Entry<String,TableDefinition> > it1 = m_tableDefTableNameCache.getIterator(); it1.hasNext();) {
    Entry<String,TableDefinition> &entry = it1.next();
    _ftprintf(f,_T("tableDefTableNameCache:<%s>\n"),entry.getKey().cstr());
    entry.getValue().dump(f);
  }
  for(Iterator<Entry<long,TableDefinition*> > it2 = m_tableDefSequenceCache.getIterator(); it2.hasNext();) {
    Entry<long,TableDefinition*> &entry = it2.next();
    _ftprintf(f,_T("tableDefSequenceCache:<%d>\n"),entry.getKey());
    entry.getValue()->dump(f);
  }

  for(Iterator<Entry<String,IndexArray> > it3 = m_indexTableNameCache.getIterator(); it3.hasNext(); ) {
    Entry<String,IndexArray> &entry = it3.next();
    _ftprintf(f,_T("indexTableNameCache:<%s>\n"),entry.getKey().cstr());
    const IndexArray &indexArray = entry.getValue();
    for(size_t i = 0; i < indexArray.size(); i++) {
      indexArray[i].dump(f);
    }
  }
  for(Iterator<Entry<String,IndexDefinition*> > it4 = m_indexDefIndexNameCache.getIterator(); it4.hasNext(); ) {
    Entry<String,IndexDefinition*> &entry = it4.next();
    _ftprintf(f,_T("indexDefIndexNameCache:<%s>\n"),entry.getKey().cstr());
    entry.getValue()->dump(f);
  }

  if(dumpall) {
    dataFileScan(SYSTABLE, TableDumper(*this,f));
  }
}
