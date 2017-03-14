#include "stdafx.h"

static ULONG typeFixedSize(DbFieldType type) {
  switch(type) {
  case DBTYPE_CHAR       :
  case DBTYPE_CHARN      :
  case DBTYPE_UCHAR      :
  case DBTYPE_UCHARN     : return sizeof(char     );
  case DBTYPE_SHORT      :
  case DBTYPE_SHORTN     :
  case DBTYPE_USHORT     :
  case DBTYPE_USHORTN    : return sizeof(short    );
  case DBTYPE_INT        :
  case DBTYPE_INTN       :
  case DBTYPE_UINT       :
  case DBTYPE_UINTN      : return sizeof(int      );
  case DBTYPE_LONG       :
  case DBTYPE_LONGN      :
  case DBTYPE_ULONG      :
  case DBTYPE_ULONGN     : return sizeof(long     );
  case DBTYPE_INT64      :
  case DBTYPE_INT64N     :
  case DBTYPE_UINT64     :
  case DBTYPE_UINT64N    : return sizeof(INT64  );
  case DBTYPE_FLOAT      :
  case DBTYPE_FLOATN     : return sizeof(float    );
  case DBTYPE_DOUBLE     :
  case DBTYPE_DOUBLEN    : return sizeof(double   );
  case DBTYPE_VARCHAR    :
  case DBTYPE_VARCHARN   : return DBADDRSIZE;
  case DBTYPE_DATE       :
  case DBTYPE_DATEN      : return sizeof(Date     );
  case DBTYPE_TIME       :
  case DBTYPE_TIMEN      : return sizeof(Time     );
  case DBTYPE_TIMESTAMP  :
  case DBTYPE_TIMESTAMPN : return sizeof(Timestamp);
  default:
    throwSqlError(SQL_INVALID_FIELDTYPE,_T("typeFixedSize:Invalid type (=%d)"),type);
    return 0;
  }
}

ULONG ColumnInfo::size() const {
  switch(getType()) {
  case DBTYPE_STRING :
  case DBTYPE_STRINGN:
    return m_len;
  default:
    return typeFixedSize(getType());
  }
};

int TableDefinition::findColumnIndex(const String &columnName) const {
  for(size_t i = 0; i < m_columns.size(); i++) {
    if(columnName.equalsIgnoreCase(m_columns[i].m_name)) {
      return (int)i;
    }
  }
  return -1;
}

ColumnDefinition::ColumnDefinition(const SysTableColumnData &colData) {
  m_name         = colData.m_columnName;
  m_type         = sqlapi_getFieldType(colData.m_dbType);
  m_offset       = colData.m_offset;
  m_len          = colData.m_len;
  m_nullAllowed  = colData.m_nulls == 'Y';
  m_defaultValue = colData.m_defaultValue;
}

ColumnDefinition::ColumnDefinition(DbFieldType type, const String &name, ULONG len, const String defaultValue) {
  m_type         = type;
  m_name         = name;
  m_len          = len;
  m_nullAllowed  = isNullAllowed(type);
  m_defaultValue = defaultValue;
}

TableDefinition::TableDefinition(TableType type, const String &tableName, const String &fileName) {
  init(type,tableName,fileName);
}

TableDefinition::TableDefinition(const SysTableTableData &tableData) {
  init((TableType)tableData.m_tableType,tableData.m_tableName,tableData.m_fileName);
  m_recSize    = tableData.m_recSize;
  m_sequenceNo = tableData.m_sequenceNo;
}

void TableDefinition::init(TableType type, const String &tableName, const String &fileName) {
  m_tableType  = type;
  m_tableName  = tableName;
  m_fileName   = fileName;
  m_sequenceNo = 0;
  m_recSize    = 0;
  m_columns.clear();
}

void TableDefinition::addColumn(const ColumnDefinition &colDef) {
  ColumnDefinition col(colDef);
  UINT fieldSize = adjustOffset(col);
  if(m_recSize + fieldSize > MAXRECSIZE) {
    throwSqlError(SQL_RECSIZE_TOO_BIG,_T("Record-definition of <%s> exceeds %d bytes. size=%d bytes"),m_tableName.cstr(), MAXRECSIZE, m_recSize);
  }
  m_recSize += fieldSize;
  m_columns.add(col);
}

void TableDefinition::addColumn(const SysTableColumnData &colData) {
  m_columns.add(ColumnDefinition(colData));
  // NB! no adjustment of recsize of offset.
}

UINT TableDefinition::adjustOffset(ColumnDefinition &colDef) {
  colDef.m_offset = m_recSize;
  ULONG fieldSize = colDef.size();
  switch(colDef.getType()) {
  case DBTYPE_VARCHAR :
  case DBTYPE_VARCHARN:
    break;
  default:
    colDef.m_len = fieldSize;
    break;
  }
  if(isNullAllowed(colDef.getType())) {
    fieldSize++; /* make space for the indicator */
  }
  return fieldSize;
}

KeyFileDefinition TableDefinition::getKeyFileDefinition(const IndexDefinition &indexDef) const {
  KeyFileDefinition keydef;
  for(UINT i = 0; i < indexDef.getColumnCount();i++) {
    const IndexColumn &indexColumn = indexDef.getColumn(i);
    const ColumnDefinition &col    = m_columns[indexColumn.m_col];
    SortDirection sortdir = indexColumn.m_asc ? SORT_ASCENDING : SORT_DESCENDING;
    keydef.addKeyField(sortdir,col.getType(),col.m_len);
  }
  return keydef.addAddrField(indexDef.isUnique());
}

TableInfo::TableInfo(const TableDefinition &tableDef, const IndexArray &indexArray) {
  init(tableDef);
  m_indexArray = indexArray;
}

void TableInfo::init(const TableDefinition &tableDef) {
  m_sequenceNo   = tableDef.getSequenceNo();
  m_fileName     = tableDef.getFileName();
  m_recSize      = tableDef.getRecordSize();
  for(UINT i = 0; i < tableDef.getColumnCount(); i++) {
    const ColumnDefinition &colDef = tableDef.getColumn(i);
    ColumnInfo col;
    col.m_type   = colDef.getType();
    col.m_offset = colDef.m_offset;
    col.m_len    = colDef.m_len;
    m_columns.add(col);
  }
  m_allFields.clear();
  for(UINT i = 0; i < getColumnCount(); i++) {
    m_allFields.add(i);
  }
}

#define GETVAR       { TupleField tmp; get(rec,n,tmp); if(tmp.isDefined()) tmp.get(v); }
#define PUTVAR       { TupleField tmp(v); put(rec,n,tmp);                              }
#define GETTUP(type) { type tmp; getBytes(rec,n,&tmp); v = tmp;                        }
#define PUTTUP(type) { type tmp; v.get(tmp); putBytes(rec,n,&tmp);                     }

void TableInfo::get(const RecordType &rec, UINT n,                char    &v) const { GETVAR; }
void TableInfo::put(      RecordType &rec, UINT n, const          char    &v) const { PUTVAR; }
void TableInfo::get(const RecordType &rec, UINT n,       UCHAR    &v) const { GETVAR; }
void TableInfo::put(      RecordType &rec, UINT n, const UCHAR    &v) const { PUTVAR; }
void TableInfo::get(const RecordType &rec, UINT n,                short   &v) const { GETVAR; }
void TableInfo::put(      RecordType &rec, UINT n, const          short   &v) const { PUTVAR; }
void TableInfo::get(const RecordType &rec, UINT n,       USHORT   &v) const { GETVAR; }
void TableInfo::put(      RecordType &rec, UINT n, const USHORT   &v) const { PUTVAR; }
void TableInfo::get(const RecordType &rec, UINT n,                int     &v) const { GETVAR; }
void TableInfo::put(      RecordType &rec, UINT n, const          int     &v) const { PUTVAR; }
void TableInfo::get(const RecordType &rec, UINT n,       UINT     &v) const { GETVAR; }
void TableInfo::put(      RecordType &rec, UINT n, const UINT     &v) const { PUTVAR; }
void TableInfo::get(const RecordType &rec, UINT n,                long    &v) const { GETVAR; }
void TableInfo::put(      RecordType &rec, UINT n, const          long    &v) const { PUTVAR; }
void TableInfo::get(const RecordType &rec, UINT n,       ULONG    &v) const { GETVAR; }
void TableInfo::put(      RecordType &rec, UINT n, const ULONG    &v) const { PUTVAR; }
void TableInfo::get(const RecordType &rec, UINT n,                INT64 &v) const { GETVAR; }
void TableInfo::put(      RecordType &rec, UINT n, const          INT64 &v) const { PUTVAR; }
void TableInfo::get(const RecordType &rec, UINT n,       UINT64 &v) const { GETVAR; }
void TableInfo::put(      RecordType &rec, UINT n, const UINT64 &v) const { PUTVAR; }
void TableInfo::get(const RecordType &rec, UINT n,       float            &v) const { GETVAR; }
void TableInfo::put(      RecordType &rec, UINT n, const float            &v) const { PUTVAR; }
void TableInfo::get(const RecordType &rec, UINT n,       double           &v) const { GETVAR; }
void TableInfo::put(      RecordType &rec, UINT n, const double           &v) const { PUTVAR; }
void TableInfo::get(const RecordType &rec, UINT n,       Date             &v) const { GETVAR; }
void TableInfo::put(      RecordType &rec, UINT n, const Date             &v) const { PUTVAR; }
void TableInfo::get(const RecordType &rec, UINT n,       Time             &v) const { GETVAR; }
void TableInfo::put(      RecordType &rec, UINT n, const Time             &v) const { PUTVAR; }
void TableInfo::get(const RecordType &rec, UINT n,       Timestamp        &v) const { GETVAR; }
void TableInfo::put(      RecordType &rec, UINT n, const Timestamp        &v) const { PUTVAR; }

void TableInfo::get(const RecordType &rec, UINT n, String &v) const {
  if(!isDefined(rec,n)) {
    return;
  }
  char tmp[MAXRECSIZE+1];
  getBytes(rec,n,tmp);
  tmp[m_columns[n].m_len] = 0;
  v = tmp;
}

void TableInfo::put(RecordType &rec, UINT n, const String &v) const {
  if(v.length() > m_columns[n].m_len) {
    throwSqlError(SQL_STRING_TOO_LONG,_T("TableInfo::put:String too long to fit in keyfield[%d]. len=%d. maxlen=%d"),n,v.length(),m_columns[n].m_len);
  }
  TCHAR tmp[MAXRECSIZE+1];
  memset(tmp,0,sizeof(tmp));
  _tcscpy(tmp,v.cstr());
  putBytes(rec,n,tmp);
}

void TableInfo::get(const RecordType &rec, UINT n, TupleField &v) const {
  if(!isDefined(rec,n)) {
    v.setUndefined();
    return;
  }
  switch(m_columns[n].getType()) {
  case DBTYPE_CHAR      :
  case DBTYPE_CHARN     :GETTUP(char     ); break;
  case DBTYPE_UCHAR     :
  case DBTYPE_UCHARN    :GETTUP(UCHAR    ); break;
  case DBTYPE_SHORT     :
  case DBTYPE_SHORTN    :GETTUP(short    ); break;
  case DBTYPE_USHORT    :
  case DBTYPE_USHORTN   :GETTUP(USHORT   ); break;
  case DBTYPE_INT       :
  case DBTYPE_INTN      :GETTUP(int      ); break;
  case DBTYPE_UINT      :
  case DBTYPE_UINTN     :GETTUP(UINT     ); break;
  case DBTYPE_LONG      :
  case DBTYPE_LONGN     :GETTUP(long     ); break;
  case DBTYPE_ULONG     :
  case DBTYPE_ULONGN    :GETTUP(ULONG    ); break;
  case DBTYPE_INT64     :
  case DBTYPE_INT64N    :GETTUP(INT64    ); break;
  case DBTYPE_UINT64    :
  case DBTYPE_UINT64N   :GETTUP(UINT64   ); break;
  case DBTYPE_FLOAT     :
  case DBTYPE_FLOATN    :GETTUP(float    ); break;
  case DBTYPE_DOUBLE    :
  case DBTYPE_DOUBLEN   :GETTUP(double   ); break;
  case DBTYPE_STRING    :
  case DBTYPE_STRINGN   :{ String  tmp; get(rec,n,tmp); v = tmp; } break;
  case DBTYPE_VARCHAR   :
  case DBTYPE_VARCHARN  :{ DbAddrFileFormat addrFF; getBytes(rec,n,&addrFF); DbAddr addr = addrFF; v = addr; } break;
  case DBTYPE_DATE      :
  case DBTYPE_DATEN     :GETTUP(Date     ); break;
  case DBTYPE_TIME      :
  case DBTYPE_TIMEN     :GETTUP(Time     ); break;
  case DBTYPE_TIMESTAMP :
  case DBTYPE_TIMESTAMPN:GETTUP(Timestamp); break;
  default:
    throwSqlError(SQL_INVALID_FIELDTYPE,_T("TableInfo::get:Invalid fieldtype. m_columns[%d]=%d"), n, m_columns[n].getType());
  }
}

void TableInfo::put(RecordType &rec, UINT n, const TupleField &v) const {
  if(!v.isDefined()) {
    setUndefined(rec,n);
    return;
  }
  switch(m_columns[n].getType()) {
  case DBTYPE_CHAR      :
  case DBTYPE_CHARN     :PUTTUP(char     ); break;
  case DBTYPE_UCHAR     :
  case DBTYPE_UCHARN    :PUTTUP(UCHAR    ); break;
  case DBTYPE_SHORT     :
  case DBTYPE_SHORTN    :PUTTUP(short    ); break;
  case DBTYPE_USHORT    :
  case DBTYPE_USHORTN   :PUTTUP(USHORT   ); break;
  case DBTYPE_INT       :
  case DBTYPE_INTN      :PUTTUP(int      ); break;
  case DBTYPE_UINT      :
  case DBTYPE_UINTN     :PUTTUP(UINT     ); break;
  case DBTYPE_LONG      :
  case DBTYPE_LONGN     :PUTTUP(long     ); break;
  case DBTYPE_ULONG     :
  case DBTYPE_ULONGN    :PUTTUP(ULONG    ); break;
  case DBTYPE_INT64     :
  case DBTYPE_INT64N    :PUTTUP(INT64    ); break;
  case DBTYPE_UINT64    :
  case DBTYPE_UINT64N   :PUTTUP(UINT64   ); break;
  case DBTYPE_FLOAT     :
  case DBTYPE_FLOATN    :PUTTUP(float    ); break;
  case DBTYPE_DOUBLE    :
  case DBTYPE_DOUBLEN   :PUTTUP(double   ); break;
  case DBTYPE_STRING    :
  case DBTYPE_STRINGN   :{ String tmp; v.get(tmp); put(rec,n,tmp); } break;
  case DBTYPE_VARCHAR   :
  case DBTYPE_VARCHARN  :{ DbAddr addr; v.get(addr); DbAddrFileFormat addrFF; addrFF = addr; putBytes(rec,n,&addrFF); } break;
  case DBTYPE_DATE      :
  case DBTYPE_DATEN     :PUTTUP(Date     ); break;
  case DBTYPE_TIME      :
  case DBTYPE_TIMEN     :PUTTUP(Time     ); break;
  case DBTYPE_TIMESTAMP :
  case DBTYPE_TIMESTAMPN:PUTTUP(Timestamp); break;
  default:
    throwSqlError(SQL_INVALID_FIELDTYPE,_T("TableInfo::put:Invalid fieldtype. m_columns[%d]=%d"), n, m_columns[n].getType());
  }
}

void TableInfo::checkFieldIndex(UINT index) const {
  if(index >= m_columns.size()) {
    throwSqlError(SQL_INVALID_KEYCOUNT,_T("Field %d doesn't exist in table %s. No. of fields:%d")
                 , index, m_fileName.cstr(),m_columns.size());
  }
}

void TableInfo::checkFieldCount(UINT fieldCount) const {
  if(fieldCount > m_columns.size()) {
    throwSqlError(SQL_INVALID_KEYCOUNT,_T("Invalid fieldcount (=%d). No. of fields:%d")
                 , fieldCount, m_columns.size());
  }
}

void *TableInfo::getFieldAddr(const RecordType &rec, UINT n) const {
  checkFieldIndex(n);
  return (char*)rec.m_data + m_columns[n].m_offset;
}

void TableInfo::getBytes(const RecordType &rec, UINT n, void *v) const {
  memcpy(v, getFieldAddr(rec,n), m_columns[n].size());
}

void TableInfo::putBytes(RecordType &rec, UINT n, const void *v) const {
  const ColumnInfo &column = m_columns[n];
  memcpy(getFieldAddr(rec,n), v, column.size());
  if(isNullAllowed(column.getType())) {
    setDefined(rec,n,true);
  }
}

void TableInfo::initField(RecordType &rec, UINT n) const {
  memset(getFieldAddr(rec,n),0,m_columns[n].size());
}

bool TableInfo::recordsEqual(const RecordType &rec1, const RecordType &rec2) const {
  return memcmp(rec1.m_data,rec2.m_data,m_recSize) == 0;
}

int TableInfo::findIndexNo(const String &indexName) const {
  for(size_t i = 0; i < m_indexArray.size(); i++) {
    if(m_indexArray[i].m_indexName == indexName) {
      return (int)i;
    }
  }
  return -1;
}

int TableInfo::findPrimaryIndex() const {
  for(size_t i = 0; i < m_indexArray.size(); i++) {
    if(m_indexArray[i].getIndexType() == INDEXTYPE_PRIMARY) {
      return (int)i;
    }
  }
  return -1;
}

ULONG TableInfo::getIndicatorOffset(UINT n) const {
  checkFieldIndex(n);
  if(!isNullAllowed(m_columns[n].getType())) {
    throwSqlError(SQL_INVALID_FIELDTYPE,_T("Field %d:Null not allowed"),n);
  }
  return m_columns[n].m_offset + m_columns[n].size();
}

void TableInfo::setDefined(RecordType &rec, UINT n, bool defined) const {
  rec.m_data[getIndicatorOffset(n)] = defined ? INDICATOR_DEFINED : INDICATOR_UNDEFINED;
  if(!defined) {
    initField(rec,n);
  }
}

void TableInfo::setUndefined(RecordType &rec, UINT n) const {
  setDefined(rec,n,false);
}

bool TableInfo::isDefined(const RecordType &rec, UINT n) const {
  if(!isNullAllowed(m_columns[n].getType())) {
    return true;
  }
  return rec.m_data[getIndicatorOffset(n)] == INDICATOR_DEFINED;
}

KeyFileDefinition TableInfo::getKeyFileDefinition(UINT index) const {
  const IndexDefinition &indexDef = m_indexArray[index];

  KeyFileDefinition keydef;
  for(UINT j = 0; j < indexDef.getColumnCount(); j++) {
    const IndexColumn &indexColumn = indexDef.getColumn(j);
    const ColumnInfo &colInfo = m_columns[indexColumn.m_col];
    SortDirection sortdir = indexColumn.m_asc ? SORT_ASCENDING : SORT_DESCENDING;
    keydef.addKeyField(sortdir,colInfo.getType(),colInfo.m_len);
  }
  return keydef.addAddrField(indexDef.isUnique());
}

FieldSet TableInfo::genFieldSet(UINT index, const FieldSet &fields) const {
  const IndexDefinition &indexDef = m_indexArray[index];
  FieldSet keyFieldSet;
  for(size_t i = 0; i < fields.size(); i++) {
    int j = indexDef.getFieldIndex(fields[i]);
    if(j < 0) {
      throwSqlError(SQL_FATAL_ERROR,_T("Column %d is not member of index %s in table %s\n")
                                   ,fields[i],indexDef.m_indexName.cstr(),m_fileName.cstr());
    }
    keyFieldSet.add(j);
  }
  return keyFieldSet;
}

String ColumnInfo::toString() const {
  return format(_T("%-10s %4d   %4d"), getTypeString(getType()), m_offset, m_len);
}

String ColumnDefinition::toString() const {
  String result = format(_T("%-13s %-8s %8s   %4d   %4d")
                       , m_name.cstr()
                       , getTypeString(getType())
                       , m_nullAllowed ? _T("NULL") : _T("NOT NULL")
                       , m_offset
                       , m_len
                       
    );
  if(m_defaultValue.length() > 0) {
    result += _T(" <") + m_defaultValue + _T(">");
  }
  return result;
}

String TableInfo::toString() const {
  String result = String(_T("TableInfo:\n"))
                + format(_T("  SequenceNo :%d\n"),m_sequenceNo     )
                + format(_T("  Filename   :%s\n"),m_fileName.cstr())
                + format(_T("  Recordsize :%d\n"),m_recSize        )
                + format(_T("  Columncount:%d\n"),m_columns.size() );
  result += _T("  Idx. Type     Offset Length\n");
  for(size_t i = 0; i < m_columns.size(); i++) {
    result += format(_T("  %3u  %s\n"),(UINT)i,m_columns[i].toString().cstr());
  }
  for(size_t i = 0; i < m_indexArray.size(); i++) {
    result += _T("  ") + m_indexArray[i].toString();
  }
  return result;
}

String TableDefinition::toString() const {
  String result = String(_T("TableDefinition:\n"))
                + format(_T("  Tablename  :<%s>\n"),m_tableName.cstr())
                + format(_T("  Sequenceno :%d\n")  ,m_sequenceNo      )
                + format(_T("  Filename   :<%s>\n"),m_fileName.cstr() )
                + format(_T("  Type       :<%c>\n"),m_tableType       )
                + format(_T("  Recordsize :%d\n")  ,m_recSize         )
                + format(_T("  Columncount:%d\n")  ,m_columns.size()  );
  result += _T("  Idx. Name          Type     Null     Offset Length Default\n");
  for(size_t i = 0; i < m_columns.size(); i++) {
    result += format(_T("  %3u  %s\n"),(UINT)i,m_columns[i].toString().cstr());
  }
  return result;
}

void TableInfo::dump(FILE *f) const {
  _ftprintf(f,_T("%s"),toString().cstr());
}

void TableDefinition::dump(FILE *f) const {
  _ftprintf(f,_T("%s"),toString().cstr());
}
