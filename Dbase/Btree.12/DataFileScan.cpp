#include "stdafx.h"

void Database::dataFileScan( const String &tableName, DataFileScanner &scanner) {

  const TableInfo &tableInfo = getTableInfo(tableName);

  DataFile dataFile(*this,tableInfo.getFileName(), DBFMODE_READONLY, nullptr);
  KeyFile index(tableInfo.getIndex(0).m_fileName, DBFMODE_READONLY, nullptr);
  KeyFileDefinition keydef(index);
  KeyCursor cursor(index,
                   RELOP_TRUE, nullptr, 0,
                   RELOP_TRUE, nullptr, 0,
                   SORT_ASCENDING);

  KeyType key;
  while(cursor.hasNext()) {
    cursor.next(key);
    DbAddr addr = keydef.getRecordAddr(key);
    RecordType record;
    dataFile.readRecord(addr, record.m_data, sizeof(record.m_data));
    if(!scanner.handleData(dataFile,addr, record)) {
      break;
    }
  }
}
