#include "stdafx.h"
/*
#define DEBUG
*/

KeyField::KeyField(DbFieldType type, bool asc, USHORT len, USHORT offset) {
  m_type   = type;
  m_asc    = asc;
  m_len    = len;
  m_offset = offset;
}

KeyField::KeyField() {
  m_type   = 0;
  m_asc    = false;
  m_len    = 0;
  m_offset = 0;
}

KeyFileDefinition::KeyFileDefinition(KeyFile &keyFile) {
  *this = keyFile.getDefinition();
}

KeyFileDefinition &KeyFileDefinition::init() {
  memset(this,0,sizeof(KeyFileDefinition));
  return *this;
}

KeyFileDefinition &KeyFileDefinition::addKeyField( SortDirection sortDir, DbFieldType type, int length) {
  return addField(true, sortDir, type, length);
}

KeyFileDefinition &KeyFileDefinition::addDataField(DbFieldType type, int length) {
  return addField(false, SORT_ASCENDING, type, length); // SORT_ASCENDING could be SORT_DESCENDING, because field is not part of the ordering.
}

KeyFileDefinition &KeyFileDefinition::addAddrField(bool unique) {
  return unique ? addDataField(DBTYPE_DBADDR) : addKeyField(SORT_ASCENDING,DBTYPE_DBADDR);
}

/*
bool KeyFileDefinition ::isUnique() const {
  return m_field[m_keyFieldCount-1].getType() != DBTYPE_DBADDR;
}
*/

KeyFileDefinition &KeyFileDefinition::addField(bool keyField, SortDirection sortdir, DbFieldType type, int length) {
  if(m_totalFieldCount >= MAXKEYFIELDCOUNT) {
    throwSqlError(SQL_KEY_TOO_BIG, _T("Too many fields in key. Max=%d"),MAXKEYFIELDCOUNT);
  }

  if(!keyField && getKeyFieldCount() == 0) {
    throwSqlError(SQL_NO_KEYFIELDS, _T("Cannot add datafield to keyfiledefinition, without preceding keyfields"));
  }

  if(keyField && getDataFieldCount() != 0) {
    throwSqlError(SQL_KEYFIELD_AFTER_DATAFIELD, _T("Cannot add keyfields to keyfiledefinition, when datafields have been added (#keyfields=%d, #datafields=%d)")
                                              , m_keyFieldCount, m_totalFieldCount - m_keyFieldCount);
  }

  UINT size = 0; // Size of field in bytes
  switch(type) {
  case DBTYPE_CHAR      :
  case DBTYPE_CHARN     :
  case DBTYPE_UCHAR     :
  case DBTYPE_UCHARN    : size = sizeof(char)          ; break;
  case DBTYPE_SHORT     :
  case DBTYPE_SHORTN    :
  case DBTYPE_USHORT    :
  case DBTYPE_USHORTN   : size = sizeof(short)         ; break;
  case DBTYPE_INT       :
  case DBTYPE_INTN      :
  case DBTYPE_UINT      :
  case DBTYPE_UINTN     : size = sizeof(int)           ; break;
  case DBTYPE_LONG      :
  case DBTYPE_LONGN     :
  case DBTYPE_ULONG     :
  case DBTYPE_ULONGN    : size = sizeof(long)          ; break;
  case DBTYPE_INT64     :
  case DBTYPE_INT64N    :
  case DBTYPE_UINT64    :
  case DBTYPE_UINT64N   : size = sizeof(INT64)         ; break;
  case DBTYPE_FLOAT     :
  case DBTYPE_FLOATN    : size = sizeof(float)         ; break;
  case DBTYPE_DOUBLE    :
  case DBTYPE_DOUBLEN   : size = sizeof(double)        ; break;
  case DBTYPE_DBADDR    : size = DBADDRSIZE            ; break; // 6
  case DBTYPE_STRING    :
  case DBTYPE_STRINGN   : size = length * sizeof(TCHAR); break;
  case DBTYPE_VARCHAR   :
  case DBTYPE_VARCHARN  : size = length + 1            ; break;
  case DBTYPE_DATE      :
  case DBTYPE_DATEN     : size = sizeof(Date)          ; break;
  case DBTYPE_TIME      :
  case DBTYPE_TIMEN     : size = sizeof(Time)          ; break;
  case DBTYPE_TIMESTAMP :
  case DBTYPE_TIMESTAMPN: size = sizeof(Timestamp)     ; break;
  default:
    throwSqlError(SQL_INVALID_FIELDTYPE,_T("addField:Invalid fieldtype.%d"), type);
  }

  int len = size;
  if(type == DBTYPE_VARCHAR || type == DBTYPE_VARCHARN) {
    len--;
  }

  int newKeySize = m_keySize + size + (isNullAllowed(type)?1:0);
  if(newKeySize > MAXKEYSIZE) {
    throwSqlError(SQL_KEY_TOO_BIG, _T("Key too big. Maxsize:%d"),MAXKEYSIZE);
  }

  m_field[m_totalFieldCount++] = KeyField(type
                                         ,sortdir == SORT_ASCENDING ? true : false
                                         ,len
                                         ,m_keySize);
  if(keyField) {
    m_keyFieldCount++;
  }

  m_keySize = newKeySize;

  return *this;
}

void KeyFileDefinition::checkFieldIndex(UINT index) const {
  if(index >= m_totalFieldCount) {
    throwSqlError(SQL_INVALID_KEYCOUNT,_T("KeyFileDefinition::checkFieldIndex:Field %d doesn't exist. No. of fields=%d")
                                      ,index, m_totalFieldCount);
  }
}

void KeyFileDefinition::checkFieldCount(UINT n) const {
  if(n > m_totalFieldCount) {
    throwSqlError(SQL_INVALID_KEYCOUNT,_T("KeyFileDefinition::checkFieldCount:Invalid fieldcount (=%d). No. of fields=%d")
                                      ,n, m_totalFieldCount);
  }
}

void KeyFileDefinition::checkKeyFieldCount(UINT n) const {
  if(n > m_keyFieldCount) {
    throwSqlError(SQL_INVALID_KEYCOUNT,_T("KeyFileDefinition::checkKeyFieldCount:Invalid fieldcount (=%d). No. of keyfields=%d")
                                      ,n, m_keyFieldCount);
  }
}

void KeyFileDefinition::checkFieldIsVarChar(UINT n) const {
  const KeyField &f = m_field[n];
  if(f.getType() != DBTYPE_VARCHAR && f.getType() != DBTYPE_VARCHARN) {
    throwSqlError(SQL_INVALID_FIELDTYPE,_T("key-field::Field %d not varchar"),n);
  }
}

void KeyFileDefinition::checkFieldIsDbAddr(UINT n) const {
  if(m_field[n].getType() != DBTYPE_DBADDR) {
    throwSqlError(SQL_INVALID_FIELDTYPE,_T("key-field::Field %d not DBADDR"),n);
  }
}

USHORT KeyFileDefinition::getFieldOffset(UINT n) const {
  checkFieldIndex(n);
  return m_field[n].getOffset();
}

USHORT KeyFileDefinition::getIndicatorOffset(UINT n) const {
  checkFieldIndex(n);
  if(!isNullAllowed(m_field[n].getType())) {
    throwSqlError(SQL_INVALID_FIELDTYPE,_T("key-field %d:Null not allowed"),n);
  }
  return m_field[n].getOffset() + m_field[n].getSize();
}

void *KeyFileDefinition::getFieldAddr(const KeyType &key, UINT n) const {
  return (void*)(&key.m_data[0] + getFieldOffset(n));
}

#define GETVAR       { TupleField tmp; get(key,n,tmp); if(tmp.isDefined()) tmp.get(v); }
#define PUTVAR       { TupleField tmp(v); put(key,n,tmp);                              }
#define GETTUP(type) { type tmp; getBytes(key,n,&tmp); v = tmp;                        }
#define PUTTUP(type) { type tmp; v.get(tmp); putBytes(key,n,&tmp);                     }


void KeyFileDefinition::get(const KeyType &key, UINT n,       char      &v) const { GETVAR; }
void KeyFileDefinition::put(      KeyType &key, UINT n, const char      &v) const { PUTVAR; }
void KeyFileDefinition::get(const KeyType &key, UINT n,       UCHAR     &v) const { GETVAR; }
void KeyFileDefinition::put(      KeyType &key, UINT n, const UCHAR     &v) const { PUTVAR; }
void KeyFileDefinition::get(const KeyType &key, UINT n,       short     &v) const { GETVAR; }
void KeyFileDefinition::put(      KeyType &key, UINT n, const short     &v) const { PUTVAR; }
void KeyFileDefinition::get(const KeyType &key, UINT n,       USHORT    &v) const { GETVAR; }
void KeyFileDefinition::put(      KeyType &key, UINT n, const USHORT    &v) const { PUTVAR; }
void KeyFileDefinition::get(const KeyType &key, UINT n,       int       &v) const { GETVAR; }
void KeyFileDefinition::put(      KeyType &key, UINT n, const int       &v) const { PUTVAR; }
void KeyFileDefinition::get(const KeyType &key, UINT n,       UINT      &v) const { GETVAR; }
void KeyFileDefinition::put(      KeyType &key, UINT n, const UINT      &v) const { PUTVAR; }
void KeyFileDefinition::get(const KeyType &key, UINT n,       long      &v) const { GETVAR; }
void KeyFileDefinition::put(      KeyType &key, UINT n, const long      &v) const { PUTVAR; }
void KeyFileDefinition::get(const KeyType &key, UINT n,       ULONG     &v) const { GETVAR; }
void KeyFileDefinition::put(      KeyType &key, UINT n, const ULONG     &v) const { PUTVAR; }
void KeyFileDefinition::get(const KeyType &key, UINT n,       INT64     &v) const { GETVAR; }
void KeyFileDefinition::put(      KeyType &key, UINT n, const INT64     &v) const { PUTVAR; }
void KeyFileDefinition::get(const KeyType &key, UINT n,       UINT64    &v) const { GETVAR; }
void KeyFileDefinition::put(      KeyType &key, UINT n, const UINT64    &v) const { PUTVAR; }
void KeyFileDefinition::get(const KeyType &key, UINT n,       float     &v) const { GETVAR; }
void KeyFileDefinition::put(      KeyType &key, UINT n, const float     &v) const { PUTVAR; }
void KeyFileDefinition::get(const KeyType &key, UINT n,       double    &v) const { GETVAR; }
void KeyFileDefinition::put(      KeyType &key, UINT n, const double    &v) const { PUTVAR; }
void KeyFileDefinition::get(const KeyType &key, UINT n,       Date      &v) const { GETVAR; }
void KeyFileDefinition::put(      KeyType &key, UINT n, const Date      &v) const { PUTVAR; }
void KeyFileDefinition::get(const KeyType &key, UINT n,       Time      &v) const { GETVAR; }
void KeyFileDefinition::put(      KeyType &key, UINT n, const Time      &v) const { PUTVAR; }
void KeyFileDefinition::get(const KeyType &key, UINT n,       Timestamp &v) const { GETVAR; }
void KeyFileDefinition::put(      KeyType &key, UINT n, const Timestamp &v) const { PUTVAR; }

void KeyFileDefinition::get(const KeyType &key, UINT n,       String    &v) const {
  if(!isDefined(key,n)) {
    return;
  }
  TCHAR tmp[MAXKEYSIZE+1];
  getBytes(key,n,tmp);
  tmp[m_field[n].getMaxStringLen()] = 0;
  v = tmp;
}

void KeyFileDefinition::put(KeyType &key, UINT n, const String &v) const {
  if(v.length() > m_field[n].getMaxStringLen()) {
    throwSqlError(SQL_STRING_TOO_LONG,_T("KeyFileDefinition::put:String too long to fit in keyfield[%d]. len=%d. maxlen=%d"),n,v.length(),m_field[n].getLen());
  }
  TCHAR tmp[MAXKEYSIZE+1];
  memset(tmp,0,sizeof(tmp));
  _tcscpy(tmp,v.cstr());
  putBytes(key,n,tmp);
}

void KeyFileDefinition::get(const KeyType &key, UINT n, varchar &v) const {
  if(!isDefined(key,n)) {
    return;
  }
  checkFieldIndex(n);
  checkFieldIsVarChar(n);
  const KeyField &f = m_field[n];
  UCHAR *lenbyte = (UCHAR*)getFieldAddr(key,n);
  ULONG len = *lenbyte;
  v = varchar(len,lenbyte+1);
}

void KeyFileDefinition::put(KeyType &key, UINT n, const varchar &v) const {
  checkFieldIndex(n);
  checkFieldIsVarChar(n);
  if(v.len() > m_field[n].getLen()) {
    throwSqlError(SQL_VARCHAR_TOO_LONG,_T("KeyFileDefinition::put:Varchar too long to fit in keyfield[%d]. len=%d. maxlen=%d")
                                      ,n,v.len(),m_field[n].getLen());
  }
  UCHAR *lenbyte = (UCHAR*)getFieldAddr(key,n);
  *(lenbyte++) = (UCHAR)v.len();
  memcpy(lenbyte,v.data(),v.len());
  if(isNullAllowed(m_field[n].getType())) {
    setDefined(key,n,true);
  }
}

void KeyFileDefinition:: get(const KeyType &key, UINT n, TupleField &v) const {
  if(!isDefined(key,n)) {
    v.setUndefined();
    return;
  }
  switch(m_field[n].getType()) {
  case DBTYPE_CHAR      :
  case DBTYPE_CHARN     :GETTUP(char          ); break;
  case DBTYPE_UCHAR     :
  case DBTYPE_UCHARN    :GETTUP(UCHAR         ); break;
  case DBTYPE_SHORT     :
  case DBTYPE_SHORTN    :GETTUP(short         ); break;
  case DBTYPE_USHORT    :
  case DBTYPE_USHORTN   :GETTUP(USHORT        ); break;
  case DBTYPE_INT       :
  case DBTYPE_INTN      :GETTUP(int           ); break;
  case DBTYPE_UINT      :
  case DBTYPE_UINTN     :GETTUP(UINT          ); break;
  case DBTYPE_LONG      :
  case DBTYPE_LONGN     :GETTUP(long          ); break;
  case DBTYPE_ULONG     :
  case DBTYPE_ULONGN    :GETTUP(ULONG         ); break;
  case DBTYPE_INT64     :
  case DBTYPE_INT64N    :GETTUP(INT64         ); break;
  case DBTYPE_UINT64    :
  case DBTYPE_UINT64N   :GETTUP(UINT64        ); break;
  case DBTYPE_FLOAT     :
  case DBTYPE_FLOATN    :GETTUP(float         ); break;
  case DBTYPE_DOUBLE    :
  case DBTYPE_DOUBLEN   :GETTUP(double        ); break;
  case DBTYPE_DBADDR    :{ DbAddr addr = *(DbAddrFileFormat*)getFieldAddr(key,n); v = addr; } break;
  case DBTYPE_STRING    :
  case DBTYPE_STRINGN   :{ String  tmp; get(key,n,tmp); v = tmp; } break;
  case DBTYPE_VARCHAR   :
  case DBTYPE_VARCHARN  :{ varchar tmp; get(key,n,tmp); v = tmp; } break;
  case DBTYPE_DATE      :
  case DBTYPE_DATEN     :GETTUP(Date          ); break;
  case DBTYPE_TIME      :
  case DBTYPE_TIMEN     :GETTUP(Time          ); break;
  case DBTYPE_TIMESTAMP :
  case DBTYPE_TIMESTAMPN:GETTUP(Timestamp     ); break;
  default:
    throwSqlError(SQL_INVALID_FIELDTYPE,_T("KeyFileDefinition::get:Invalid fieldtype. Keydef[%d]=%d"), n, m_field[n].getType());
  }
}

void KeyFileDefinition:: put(KeyType &key, UINT n, const TupleField &v) const {
  if(!v.isDefined()) {
    setUndefined(key,n);
    return;
  }
  switch(m_field[n].getType()) {
  case DBTYPE_CHAR      :
  case DBTYPE_CHARN     :PUTTUP(char          ); break;
  case DBTYPE_UCHAR     :
  case DBTYPE_UCHARN    :PUTTUP(UCHAR         ); break;
  case DBTYPE_SHORT     :
  case DBTYPE_SHORTN    :PUTTUP(short         ); break;
  case DBTYPE_USHORT    :
  case DBTYPE_USHORTN   :PUTTUP(USHORT        ); break;
  case DBTYPE_INT       :
  case DBTYPE_INTN      :PUTTUP(int           ); break;
  case DBTYPE_UINT      :
  case DBTYPE_UINTN     :PUTTUP(UINT          ); break;
  case DBTYPE_LONG      :
  case DBTYPE_LONGN     :PUTTUP(long          ); break;
  case DBTYPE_ULONG     :
  case DBTYPE_ULONGN    :PUTTUP(ULONG         ); break;
  case DBTYPE_INT64     :
  case DBTYPE_INT64N    :PUTTUP(INT64         ); break;
  case DBTYPE_UINT64    :
  case DBTYPE_UINT64N   :PUTTUP(UINT64        ); break;
  case DBTYPE_FLOAT     :
  case DBTYPE_FLOATN    :PUTTUP(float         ); break;
  case DBTYPE_DOUBLE    :
  case DBTYPE_DOUBLEN   :PUTTUP(double        ); break;
  case DBTYPE_DBADDR    :{ DbAddr addr; v.get(addr); *((DbAddrFileFormat*)getFieldAddr(key,n)) = addr; } break;
  case DBTYPE_STRING    :
  case DBTYPE_STRINGN   :{ String  tmp; v.get(tmp);  put(key,n,tmp); } break;
  case DBTYPE_VARCHAR   :
  case DBTYPE_VARCHARN  :{ varchar tmp; v.get(tmp);  put(key,n,tmp); } break;
  case DBTYPE_DATE      :
  case DBTYPE_DATEN     :PUTTUP(Date          ); break;
  case DBTYPE_TIME      :
  case DBTYPE_TIMEN     :PUTTUP(Time          ); break;
  case DBTYPE_TIMESTAMP :
  case DBTYPE_TIMESTAMPN:PUTTUP(Timestamp     ); break;
  default:
    throwSqlError(SQL_INVALID_FIELDTYPE,_T("KeyFileDefinition::get:Invalid fieldtype. Keydef[%d]=%d")
                                       ,n, m_field[n].getType());
  }
}

void KeyFileDefinition::getBytes(const KeyType &key, UINT n, void *v) const {
  checkFieldIndex(n);
  const KeyField &f = m_field[n];
  memcpy(v,getFieldAddr(key,n),f.getLen());
}

void KeyFileDefinition::putBytes(KeyType &key, UINT n, const void *v) const {
  checkFieldIndex(n);
  const KeyField &f = m_field[n];
  memcpy(getFieldAddr(key,n),v,f.getLen());
  if(isNullAllowed(f.getType())) {
    setDefined(key,n,true);
  }
}

KeyType &KeyFileDefinition::initField(KeyType &key, UINT n) const {
  checkFieldIndex(n);
  memset(getFieldAddr(key,n),0,m_field[n].getSize());
  return key;
}

bool KeyFileDefinition::isDefined(const KeyType &key, UINT n) const {
  if(!isNullAllowed(m_field[n].getType())) {
    return true;
  }
  return key.m_data[getIndicatorOffset(n)] == INDICATOR_DEFINED;
}

KeyType &KeyFileDefinition::setDefined(KeyType &key, UINT n, bool defined) const {
  key.m_data[getIndicatorOffset(n)] = defined ? INDICATOR_DEFINED : INDICATOR_UNDEFINED;
  if(!defined)  {
    initField(key,n);
  }
  return key;
}

DbAddr KeyFileDefinition::getRecordAddr(const KeyType &key) const { /* get the last field of the key */
  const UINT n = m_totalFieldCount - 1;
  checkFieldIsDbAddr(n);
  DbAddr addr = *((DbAddrFileFormat*)getFieldAddr(key,n));
  return addr;
}

KeyType &KeyFileDefinition::putRecordAddr(KeyType &key, const DbAddr &addr) const { /* set the last field of the key */
  const UINT n = m_totalFieldCount - 1;
  checkFieldIsDbAddr(n);
  *((DbAddrFileFormat*)getFieldAddr(key,n)) = addr;
  return key;
}

/* return 1 if key1 > key2; -1 if key1 < key2; 0 if key1 == key2 */
/* according to keydef. NULL-value is AFTER notnull-value        */
/* return 1 if key1 > key2; -1 if key1 < key2; 0 if key1 == key2 */
/* according to keydef. NULL-value is AFTER notnull-value        */
// optimized for primitive types
int KeyFileDefinition::keynCmp(const KeyType &key1, const KeyType &key2, UINT fieldCount) const {
  bool defined1, defined2;

  checkKeyFieldCount(fieldCount);

#define CMPRESULT(x)       (field.isAscending() ? (x) : -(x))

#define CMPNULLS           { defined1 = isDefined(key1,i);            \
                             defined2 = isDefined(key2,i);            \
                             if(defined1 != defined2)                 \
                               return CMPRESULT(defined2 - defined1); \
                             if(!defined1) break;                     \
                           }

#define CMPPRIMITIVE(type) { type *p1 = (type*)getFieldAddr(key1,i);  \
                             type *p2 = (type*)getFieldAddr(key2,i);  \
                             if(*p1 > *p2) return CMPRESULT(1);       \
                             if(*p1 < *p2) return CMPRESULT(-1);      \
                           }

#define CMP(type)          { TupleField v1,v2;                        \
                             get(key1,i,v1);                          \
                             get(key2,i,v2);                          \
                             int c = compare(v1,v2);                  \
                             if(c) return CMPRESULT(c);               \
                           }

  for(UINT i = 0; i < fieldCount; i++) {
    const KeyField &field = m_field[i];
    switch(field.getType()) {
    case DBTYPE_CHARN  : 
      CMPNULLS;
    case DBTYPE_CHAR   : 
      CMPPRIMITIVE(char);
      break;

    case DBTYPE_UCHARN :
      CMPNULLS;
    case DBTYPE_UCHAR  :
      CMPPRIMITIVE(UCHAR);
      break;

    case DBTYPE_SHORTN :
      CMPNULLS;
    case DBTYPE_SHORT  :
      CMPPRIMITIVE(short);
      break;

    case DBTYPE_USHORTN:
      CMPNULLS;
    case DBTYPE_USHORT :
      CMPPRIMITIVE(USHORT);
      break;

    case DBTYPE_INTN   :
      CMPNULLS;
    case DBTYPE_INT    :
      CMPPRIMITIVE(int);
      break;

    case DBTYPE_UINTN  :
      CMPNULLS;
    case DBTYPE_UINT   :
      CMPPRIMITIVE(UINT);
      break;

    case DBTYPE_LONGN  :
      CMPNULLS;
    case DBTYPE_LONG   :
      CMPPRIMITIVE(long);
      break;

    case DBTYPE_ULONGN :
      CMPNULLS;
    case DBTYPE_ULONG  :
      CMPPRIMITIVE(ULONG);
      break;

    case DBTYPE_INT64N  :
      CMPNULLS;
    case DBTYPE_INT64   :
      CMPPRIMITIVE(INT64);
      break;

    case DBTYPE_UINT64N :
      CMPNULLS;
    case DBTYPE_UINT64  :
      CMPPRIMITIVE(UINT64);
      break;

    case DBTYPE_FLOATN :
      CMPNULLS;
    case DBTYPE_FLOAT  :
      CMPPRIMITIVE(float);
      break;

    case DBTYPE_DOUBLEN:
      CMPNULLS;
    case DBTYPE_DOUBLE :
      CMPPRIMITIVE(double);
      break;

    case DBTYPE_STRINGN:
      CMPNULLS;
    case DBTYPE_STRING :
      { int c = _tcsncmp((TCHAR*)getFieldAddr(key1,i),(TCHAR*)getFieldAddr(key2,i),field.getLen());
        if(c) return CMPRESULT(c);
        break;
      }

    case DBTYPE_VARCHARN:
      CMPNULLS;
    case DBTYPE_VARCHAR:
      CMP(varchar);
      break;

    case DBTYPE_DATEN:
      CMPNULLS;
    case DBTYPE_DATE :
      CMPPRIMITIVE(Date);
      break;

    case DBTYPE_TIMEN:
      CMPNULLS;
    case DBTYPE_TIME :
      CMPPRIMITIVE(Time);
      break;

    case DBTYPE_TIMESTAMPN:
      CMPNULLS;
    case DBTYPE_TIMESTAMP :
      CMPPRIMITIVE(Timestamp);
      break;

    case DBTYPE_DBADDR  :
#ifdef BIG_DBADDR
      { int c = memcmp(getFieldAddr(key1,i), getFieldAddr(key2,i),DBADDRSIZE); // Dont care about the numeric value of the addresses.
                                                                               // As long as memcmp gives the same result when comparing the
                                                                               // same 2 addresses, the tree will consistent.
        if(c) return CMPRESULT(c);
        break;
      }
#else
      CMPPRIMITIVE(DbAddr);
#endif
      break;

    default:
      throwSqlError(SQL_INVALID_FIELDTYPE,_T("keynCmp:Invalid fieldtype. Keydef[%d]=%d")
                                         ,i, field.getType());
      break;
    }
  }
  return 0; /* keys are equal */
}

int KeyFileDefinition::keyCmp(const KeyType &key1, const KeyType &key2) const {
  return keynCmp(key1, key2, m_keyFieldCount);
}

bool KeyFileDefinition::keynCmpRelOp(RelationType relop, const KeyType &key1, const KeyType &key2, UINT fieldCount) const {
  switch(relop) {
  case RELOP_GE   : return((bool)(keynCmp(key1, key2, fieldCount) >= 0));
  case RELOP_GT   : return((bool)(keynCmp(key1, key2, fieldCount) >  0));
  case RELOP_LE   : return((bool)(keynCmp(key1, key2, fieldCount) <= 0));
  case RELOP_LT   : return((bool)(keynCmp(key1, key2, fieldCount) <  0));
  case RELOP_EQ   : return((bool)(keynCmp(key1, key2, fieldCount) == 0));
  case RELOP_NQ   : return((bool)(keynCmp(key1, key2, fieldCount) != 0));
  case RELOP_TRUE : return true;
  case RELOP_FALSE: return false;
  default:
    throwSqlError(SQL_INVALID_RELOP,_T("keynCmpRelOp::Invalid relop:%d"),relop);
  }
  return false;
}

bool KeyFileDefinition::keyCmpRelOp(RelationType relop, const KeyType &key1, const KeyType &key2) const {
  return keynCmpRelOp(relop,key1,key2, m_keyFieldCount);
}

String KeyFileDefinition::sprintf(const KeyType &key, UINT fieldCount) const {
  checkFieldCount(fieldCount);

  String result;
  for(UINT i = 0; i < fieldCount; i++) {
    TupleField v;
    get(key,i,v);

    if(i>0) {
      result += _T(",");
    }
    result += v.toString();
  }
  return result;
}

void KeyFileDefinition::fprintf( FILE *f, const KeyType &key, UINT fieldCount ) const {
  ::_ftprintf(f,_T("%s"), sprintf(key,fieldCount).cstr());
}

KeyType &KeyFileDefinition::scanf(KeyType &key, UINT fieldCount) const {
  checkFieldCount(fieldCount);

  for(UINT i = 0; i < fieldCount; i++) {
    const KeyField &field = m_field[i];
start:
    if(field.getType() == DBTYPE_STRING || 
       field.getType() == DBTYPE_STRINGN||
       field.getType() == DBTYPE_VARCHAR||
       field.getType() == DBTYPE_VARCHARN) {
      ::_tprintf(_T("Enter field %2u (%s(%d)):"),i,getTypeString(field.getType()), field.getLen());
    } else {
      ::_tprintf(_T("Enter field %2u (%-6s):"),i,getTypeString(field.getType()));
    }

    TCHAR line[1000];
    GETS(line);
    TupleField t(line);
    if(_tcscmp(line,_T("null")) == 0) {
      if(!isNullAllowed(field.getType())) {
        ::_tprintf(_T("Null not allowed in field %d, type=%s"),i, getTypeString(field.getType()));
        goto start;
      }
      t.setUndefined();
    } else {
      try {
        t.setType(field.getType());
      } catch(sqlca ca) {
        ca.dump();
        goto start;
      }
    }
    put(key,i,t);
  }
  return key;
}

String KeyFileDefinition::toString() const {
  String result = _T("KeyFileDefinition:\n");
  result       += format(_T("  FieldCount   :%d\n"),m_totalFieldCount);
  result       += format(_T("  KeyFieldCount:%d\n"),m_keyFieldCount  );
  result       += format(_T("  Keysize      :%d\n"),m_keySize        );
  result       += _T("  KeyField  Type       Length Offset Nulls Ordering\n");
  int i;
  for(i = 0; i < m_keyFieldCount; i++) {
    const KeyField &field = getFieldDef(i);
    result += format(_T("%10d  %-11s %5d %6d %-5s %s\n")
                    ,i
                    ,getTypeString(field.getType())
                    ,field.getLen()
                    ,field.getOffset()
                    ,boolToStr(isNullAllowed(field.getType()))
                    ,field.isAscending() ? _T("asc") : _T("desc"));
  }
  if(i < m_totalFieldCount) {
    result += _T("  DataField Type       Length Offset Nulls\n");
    for(; i < m_totalFieldCount; i++) {
      const KeyField &field = getFieldDef(i);
      result += format(_T("%11d %-11s %5d %6d %-5s\n")
                      ,i
                      ,getTypeString(field.getType())
                      ,field.getLen()
                      ,field.getOffset()
                      ,boolToStr(isNullAllowed(field.getType())));
    }
  }
  return result + spaceString(51,'_') + _T("\n");
}

void KeyFileDefinition::dump(FILE *f) const {
  _ftprintf(f, _T("%s"),toString().cstr());
}

const TCHAR *relOpString(RelationType relop) {
  switch(relop) {
  case RELOP_GE   : return _T(">=");
  case RELOP_GT   : return _T(">");
  case RELOP_LE   : return _T("<=");
  case RELOP_LT   : return _T("<");
  case RELOP_EQ   : return _T("=");
  case RELOP_NQ   : return _T("<>");
  case RELOP_TRUE : return _T("true");
  case RELOP_FALSE: return _T("false");
  default         : return _T("unknown");
  }
}

const TCHAR *sortDirString(SortDirection dir) {
  switch(dir) {
  case SORT_ASCENDING : return _T("ASC");
  case SORT_DESCENDING: return _T("DESC");
  default: return _T("unknown");
  }
}
