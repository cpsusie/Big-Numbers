#include "stdafx.h"

static String datansprintf(DataFile &dbf, RecordType &rec, const TableInfo &tableInfo, UINT fieldCount) {
  String result;

  tableInfo.checkFieldCount(fieldCount,__TFUNCTION__);

  for(UINT i = 0; i < fieldCount; i++) {
    String tmp;
    switch(tableInfo.getColumn(i).getType()) {
    case DBTYPE_VARCHARN:
      if(!tableInfo.isDefined(rec,i)) {
        tmp = _T("null");
        break;
      }
      // continue case
    case DBTYPE_VARCHAR:
      { DbAddr addr;
        tableInfo.get(rec,i,addr);
        if(addr == DB_NULLADDR) { /* Inconsistency */
          throwSqlError(SQL_DBCORRUPTED,_T("Datafile <%s> corrupted"),tableInfo.getFileName().cstr());
        } else {
          varchar vc;
          dbf.readVarChar(addr,vc);
          TupleField v = vc;
          tmp = v.toString();
        }
      }
      break;

    default:
      { TupleField v;
        tableInfo.get(rec,i,v);
        tmp = v.toString();
      }
      break;
    }
    if(i > 0) {
      result += _T(",");
    }
    result += tmp;
  }
  return result;
}

class RecordDumper : public DataFileScanner {
private:
  const TableInfo &m_tableInfo;
  FILE            *m_output;
public:
  RecordDumper(const TableInfo &tableInfo, FILE *output);
  bool handleData(DataFile &file, DbAddr addr, RecordType &rec);
};

RecordDumper::RecordDumper(const TableInfo &tableInfo, FILE *output) : m_tableInfo(tableInfo) {
  m_output = output;
}

bool RecordDumper::handleData(DataFile &file, DbAddr addr, RecordType &rec) {
  _ftprintf(m_output,_T("%s\n"),datansprintf(file, rec, m_tableInfo, m_tableInfo.getColumnCount()).cstr());
  return true;
}

void Database::dataFileDump(const String &tableName, FILE *f) {
  const TableInfo &tableInfo = getTableInfo(tableName);
  dataFileScan(tableName, RecordDumper(tableInfo,f));
}

void Database::KeyFiledump(const String &indexName, FILE *f) {
  const IndexDefinition &indexDef = getIndexDefinition(indexName);
  KeyFile keyFile( indexDef.m_fileName.cstr(), DBFMODE_READONLY, NULL);
  KeyFileDefinition keydef(keyFile);
  KeyCursor cursor(keyFile, RELOP_TRUE, NULL, 0, RELOP_TRUE, NULL, 0, SORT_ASCENDING);
  while(cursor.hasNext()) {
    KeyType key;
    cursor.next(key);
    keydef.fprintf(f,key);
    _ftprintf(f,_T("\n"));
  }
}
