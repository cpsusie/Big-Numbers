#include "stdafx.h"
#include <float.h>
#include <math.h>

//#define DEBUGMODULE

class TableCursorKey {
private:
  const KeyFileDefinition &m_keydef;

  void setUndefinedField(int i);
  void putNumberInKey(int i, double d);
  bool putField(      int i, const String    &s);
  bool putField(      int i, const varchar   &v);
  bool putField(      int i, double           d);
public:
  RelationType   m_relop;
  UINT           m_fieldCount;
  KeyType        m_key;
  TableCursorKey(const KeyFileDefinition &keydef, const Tuple *keyTuple, UINT fieldCount, RelationType relop);
  String toString() const;
  inline void dump(FILE *f = stdout) const {
    _ftprintf(f, _T("%s"), toString().cstr());
  }
};

static int rangecmp(double d, int type) {
  switch(type) {
  case DBTYPE_CHAR     :
  case DBTYPE_CHARN    :
    return (d < DBMINCHAR   ) ? -1 : (d > DBMAXCHAR   ) ? 1 : 0;
  case DBTYPE_UCHAR    :
  case DBTYPE_UCHARN   :
    return (d < DBMINUCHAR  ) ? -1 : (d > DBMAXUCHAR  ) ? 1 : 0;
  case DBTYPE_SHORT    :
  case DBTYPE_SHORTN   :
    return (d < DBMINSHORT  ) ? -1 : (d > DBMAXSHORT  ) ? 1 : 0;
  case DBTYPE_USHORT   :
  case DBTYPE_USHORTN  :
    return (d < DBMINUSHORT ) ? -1 : (d > DBMAXUSHORT ) ? 1 : 0;
  case DBTYPE_INT      :
  case DBTYPE_INTN     :
    return (d < DBMININT    ) ? -1 : (d > DBMAXINT    ) ? 1 : 0;
  case DBTYPE_UINT     :
  case DBTYPE_UINTN    :
    return (d < DBMINUINT   ) ? -1 : (d > DBMAXUINT   ) ? 1 : 0;
  case DBTYPE_LONG     :
  case DBTYPE_LONGN    :
    return (d < DBMINLONG   ) ? -1 : (d > DBMAXLONG   ) ? 1 : 0;
  case DBTYPE_ULONG    :
  case DBTYPE_ULONGN   :
    return (d < DBMINULONG  ) ? -1 : (d > DBMAXULONG  ) ? 1 : 0;
  case DBTYPE_FLOAT    :
  case DBTYPE_FLOATN   :
    return (d < DBMINFLOAT  ) ? -1 : (d > DBMAXFLOAT  ) ? 1 : 0;
  case DBTYPE_DOUBLE   :
  case DBTYPE_DOUBLEN  :
    return 0;
  default:
    throwSqlError(SQL_FATAL_ERROR,_T("Invalid type in rangecmp:%d"),type);
    return 0;
  }
}

void TableCursorKey::putNumberInKey(int i, double d) {
  switch(m_keydef.getFieldDef(i).getType()) {
  case DBTYPE_CHAR     :
  case DBTYPE_CHARN    :
    { char v = (char)d;
      m_keydef.put(m_key,i,v);
      break;
    }
  case DBTYPE_UCHAR    :
  case DBTYPE_UCHARN   :
    { UCHAR v = (UCHAR)d;
      m_keydef.put(m_key,i,v);
      break;
    }
  case DBTYPE_SHORT    :
  case DBTYPE_SHORTN   :
    { short v = (short)d;
      m_keydef.put(m_key,i,v);
      break;
    }
  case DBTYPE_USHORT   :
  case DBTYPE_USHORTN  :
    { USHORT v = (USHORT)d;
      m_keydef.put(m_key,i,v);
      break;
    }
  case DBTYPE_INT      :
  case DBTYPE_INTN     :
    { int v = (int)d;
      m_keydef.put(m_key,i,v);
      break;
    }
  case DBTYPE_UINT     :
  case DBTYPE_UINTN    :
    { UINT v = (UINT)d;
      m_keydef.put(m_key,i,v);
      break;
    }
  case DBTYPE_LONG     :
  case DBTYPE_LONGN    :
    { long v = (long)d;
      m_keydef.put(m_key,i,v);
      break;
    }
  case DBTYPE_ULONG    :
  case DBTYPE_ULONGN   :
    { ULONG v = (ULONG)d;
      m_keydef.put(m_key,i,v);
      break;
    }
  case DBTYPE_FLOAT    :
  case DBTYPE_FLOATN   :
    { float v = (float)d;
      m_keydef.put(m_key,i,v);
      break;
    }
  case DBTYPE_DOUBLE   :
  case DBTYPE_DOUBLEN  :
      m_keydef.put(m_key,i,d);
      break;
  }
}

bool isInteger(double d) {
  return d == floor(d);
}

bool validChar(double d) {
  return isInteger(d) && (rangecmp(d,DBTYPE_CHAR) == 0  );
}

bool validUchar(double d) {
  return isInteger(d) && (rangecmp(d,DBTYPE_UCHAR) == 0 );
}

bool validShort(double d) {
  return isInteger(d) && (rangecmp(d,DBTYPE_SHORT) == 0 );
}

bool validUshort(double d) {
  return isInteger(d) && (rangecmp(d,DBTYPE_USHORT) == 0);
}

bool validInt(double d) {
  return isInteger(d) && (rangecmp(d,DBTYPE_INT) == 0   );
}

bool validUint(double d) {
  return isInteger(d) && (rangecmp(d,DBTYPE_UINT) == 0  );
}

bool validLong(double d) {
  return isInteger(d) && (rangecmp(d,DBTYPE_LONG) == 0  );
}

bool validUlong(double d) {
  return isInteger(d) && (rangecmp(d,DBTYPE_ULONG) == 0 );
}

bool validFloat(double d) {
  float f = (float)d;
  return (rangecmp(d,DBTYPE_FLOAT) == 0) && (f == d);
}

static float roundDownToNearestFloat(double d);

static float roundUpToNearestFloat(double d) {
  if(d > 0) {
    return (float)(((float)d) * (1.0+FLT_EPSILON));
  } else {
    return -roundDownToNearestFloat(-d);
  }
}

static float roundDownToNearestFloat(double d) {
  if(d > 0) {
    return (float)(((float)d) * (1.0 - FLT_EPSILON));
  } else {
    return -roundUpToNearestFloat(-d);
  }
}

static double roundup(double d) {
  if(isInteger(d)) {
    return d;
  } else {
    return floor(d) + 1;
  }
}

void TableCursorKey::setUndefinedField(int i) {
  switch(m_relop) {
  case RELOP_EQ:
    m_key.init();
    m_relop      = RELOP_FALSE;
    m_fieldCount = 0;
    return;
  case RELOP_GE:
  case RELOP_GT:
  case RELOP_LE:
  case RELOP_LT:
    m_fieldCount = i;
    return;
  }
}

bool TableCursorKey::putField(int i, const String &s) {
  if(s.length() > m_keydef.getFieldDef(i).getLen()) {
    switch(m_relop) {
    case RELOP_EQ:
      m_key.init();
      m_fieldCount = 0;
      m_relop      = RELOP_FALSE;
      return true;
    case RELOP_GE:
    case RELOP_GT:
      m_keydef.put(m_key,i,left(s,m_keydef.getFieldDef(i).getLen()));
      m_relop      = RELOP_GT;
      m_fieldCount = i + 1;
      return true;
    case RELOP_LE:
    case RELOP_LT:
      m_keydef.put(m_key,i,left(s,m_keydef.getFieldDef(i).getLen()));
      m_relop      = RELOP_LT;
      m_fieldCount = i + 1;
      return true;
    }
  }
  m_keydef.put(m_key, i, s);
  return false;
}

bool TableCursorKey::putField(int i, const varchar &v) {
  if(v.len() > m_keydef.getFieldDef(i).getLen()) {
    varchar tmp(m_keydef.getFieldDef(i).getLen(),v.data());
    switch(m_relop) {
    case RELOP_EQ:
      m_key.init();
      m_fieldCount = 0;
      m_relop      = RELOP_FALSE;
      return true;
    case RELOP_GE:
    case RELOP_GT:
      m_keydef.put(m_key,i,tmp);
      m_relop      = RELOP_GT;
      m_fieldCount = i + 1;
      return true;
    case RELOP_LE:
    case RELOP_LT:
      m_keydef.put(m_key,i,tmp);
      m_relop      = RELOP_LT;
      m_fieldCount = i + 1;
      return true;
    }
  }
  m_keydef.put(m_key, i, v);
  return false;
}

bool TableCursorKey::putField(int i, double d) {
  DbFieldType keyType = m_keydef.getFieldDef(i).getType();
  int c = rangecmp(d,keyType);
  if(c == 0) {
    if(isIntegerType(keyType)) {
      if(isInteger(d))
        putNumberInKey(i,d);
      else {
        switch(m_relop) {
        case RELOP_EQ:
          m_key.init();
          m_fieldCount = 0;
          m_relop      = RELOP_FALSE;
          return true;
        case RELOP_GE:
        case RELOP_GT:
          putNumberInKey(i,roundup(d));
          m_relop      = RELOP_GE;
          m_fieldCount = i + 1;
          return true;
        case RELOP_LE:
        case RELOP_LT:
          putNumberInKey(i,floor(d));
          m_relop      = RELOP_LE;
          m_fieldCount = i + 1;
          return true;
        }
      }
    } else if(keyType == DBTYPE_FLOAT || keyType == DBTYPE_FLOATN) {
      if(validFloat(d)) {
        putNumberInKey(i,d);
      } else {
        switch(m_relop) {
        case RELOP_EQ:
          m_key.init();
          m_fieldCount = 0;
          m_relop      = RELOP_FALSE;
          return true;
        case RELOP_GE:
        case RELOP_GT:
          putNumberInKey(i,roundUpToNearestFloat(d));
          m_relop      = RELOP_GE;
          m_fieldCount = i + 1;
          return true;
        case RELOP_LE:
        case RELOP_LT:
          putNumberInKey(i,roundDownToNearestFloat(d));
          m_relop      = RELOP_LE;
          m_fieldCount = i + 1;
          return true;
        }
      }
    } else {
      putNumberInKey(i,d);
    }
  } else if(c < 0) {
    switch(m_relop) {
    case RELOP_EQ:
      m_key.init();
      m_fieldCount = 0;
      m_relop      = RELOP_FALSE;
      return true;
    case RELOP_GE:
    case RELOP_GT:
      m_fieldCount = i;
      return true;
    case RELOP_LE:
    case RELOP_LT:
      m_relop      = RELOP_LT;
      m_fieldCount = i;
      return true;
    }
  } else { // c > 0
    switch(m_relop) {
    case RELOP_EQ:
      m_key.init();
      m_fieldCount = 0;
      m_relop      = RELOP_FALSE;
      return true;
    case RELOP_GE:
    case RELOP_GT:
      m_relop      = RELOP_GT;
      m_fieldCount = i;
      return true;
    case RELOP_LE:
    case RELOP_LT:
      m_fieldCount = i;
      return true;
    }
  }
  return false;
}

TableCursorKey::TableCursorKey(const KeyFileDefinition &keydef, const Tuple *keytuple, UINT fieldCount, RelationType relop) : m_keydef(keydef) {
  m_fieldCount = fieldCount;
  m_key.init();
  m_relop = relop;
  if(relop == RELOP_FALSE || relop == RELOP_TRUE) return;
  if(relop == RELOP_NQ) {
    m_relop = RELOP_TRUE;
    m_fieldCount = 0;
    return;
  }
  if(fieldCount == 0) {
    return;
  }
  if(keytuple == NULL) {
    throwSqlError(SQL_FATAL_ERROR,_T("TableCursorKey:No Tuple specified but field = %d"),fieldCount);
  }
  const Tuple &tuple = *keytuple;
  if(fieldCount > tuple.size()) {
    throwSqlError(SQL_FATAL_ERROR,_T("TableCursorKey:invalid fieldCount (=%d) specified. Tuple.size = %zd")
                                 ,fieldCount,tuple.size());
  }
  for(UINT i = 0; i < fieldCount; i++) {
    DbFieldType tupleType = tuple[i].getType();
    DbFieldType keyType = keydef.getFieldDef(i).getType();

    if(!isCompatibleType(keyType,tupleType)) {
       throwSqlError(SQL_TYPEMISMATCH,_T("Incompatible types in keydefinition(%s) and tupledefinition(%s)")
                     ,getTypeString(keyType)
                     ,getTypeString(tupleType));
    }

    const TupleField &tupField = tuple[i];
    switch(getMainType(tupleType)) {
    case MAINTYPE_NUMERIC :
      { if(!tupField.isDefined()) {
          setUndefinedField(i);
          return;
        }
        double d;
        tupField.get(d);
        if(putField(i,d)) return;
        break;
      }

    case MAINTYPE_STRING  :
      { if(!tupField.isDefined()) {
          setUndefinedField(i);
          return;
        }
        String str;
        tupField.get(str);
        switch(getMainType(keyType)) {
        case MAINTYPE_STRING:
          if(putField(i,str)) return;
          break;
        case MAINTYPE_VARCHAR:
          { varchar vch(str.cstr());
            if(putField(i,vch)) return;
          }
        }
      }
      break;

    case MAINTYPE_VARCHAR :
      { if(!tupField.isDefined()) {
          setUndefinedField(i);
          return;
        }
        varchar vch;
        tupField.get(vch);

        switch(getMainType(keyType)) {
        case MAINTYPE_STRING:
          { String    str((TCHAR*)vch.data());
            if(putField(i,str)) return;
            break;
          }
        case MAINTYPE_VARCHAR:
          if(putField(i,vch)) return;
          break;
        }
      }
      break;

    case MAINTYPE_DATE    :
      { if(!tupField.isDefined()) {
          setUndefinedField(i);
          return;
        }
        Date d;
        tupField.get(d);
        keydef.put(m_key, i, d);
      }
      break;

    case MAINTYPE_TIME    :
      { if(!tupField.isDefined()) {
          setUndefinedField(i);
          return;
        }
        Time d;
        tupField.get(d);
        keydef.put(m_key, i, d);
      }
      break;

    case MAINTYPE_TIMESTAMP:
      { if(!tupField.isDefined()) {
          setUndefinedField(i);
          return;
        }
        Timestamp d;
        tupField.get(d);
        keydef.put(m_key, i, d);
      }
      break;

    default:
      throwSqlError(SQL_FATAL_ERROR,_T("TableCursorKey:Invalid DbMainType :%d")
                                   ,getMainType(tupleType));
      break;
    }
  }
}

String TableCursorKey::toString() const {
  String result = _T("TableCursorKey:\n");
  result += format(_T("  FieldCount:%d\n"), m_fieldCount);
  result += format(_T("  relop     :%s\n"), relOpString(m_relop));
  result += format(_T("  Key       :%s\n"), m_keydef.sprintf(m_key,m_fieldCount).cstr());
  return result;
}

TableCursor::TableCursor(Database               &db      ,
                         const TableCursorParam &param   ,
                         const Tuple            *beginKey,
                         const Tuple            *endKey) : m_db(db) {
  m_keyCursor  = NULL;
  m_sequenceNo = param.m_sequenceNo;

  const TableInfo &tableInfo = m_db.getTableInfo(m_sequenceNo);

  m_indexOnly   = param.m_indexOnly;
  int usedIndex = tableInfo.findIndexNo(param.m_indexName);
  if(usedIndex < 0)
    throwSqlError(SQL_NOINDEX,_T("Index %s doesn't exist for table %s"),param.m_indexName,tableInfo.getTableName().cstr());

  KeyFileDefinition keydef = tableInfo.getKeyFileDefinition(usedIndex);
  if(!m_indexOnly) {
    m_fieldSet  = param.m_fieldSet;
  } else {
    m_fieldSet  = tableInfo.genFieldSet(usedIndex,param.m_fieldSet);
  }

  TableCursorKey startKey(keydef,beginKey,param.m_beginFieldCount, param.m_beginRelOp);
  TableCursorKey stopKey( keydef,endKey  ,param.m_endFieldCount  , param.m_endRelOp  );

#if defined(DEBUGMODULE)
  if(beginKey) { _tprintf(_T("starttup:")); beginKey->dump(); }
  if(endKey  ) { _tprintf(_T("endKey  :")); endKey->dump();   }
  _tprintf(_T("startKey  (%d,%s):"),param.m_beginFieldCount,relOpString(param.m_beginRelOp)); startKey.dump();
  _tprintf(_T("stopKey   (%d,%s):"),param.m_endFieldCount  ,relOpString(param.m_endRelOp  )); stopKey.dump();
#endif

  if((startKey.m_relop == RELOP_FALSE) || (stopKey.m_relop == RELOP_FALSE))
    return;

  KeyFile keyFile(m_db,tableInfo.getIndex(usedIndex).m_fileName,DBFMODE_READONLY,false);
  m_keyCursor = new KeyCursor(keyFile,
                              startKey.m_relop,
                             &startKey.m_key,
                              startKey.m_fieldCount,
                              stopKey.m_relop,
                             &stopKey.m_key,
                              stopKey.m_fieldCount,
                              param.m_dir);
}

TupleField fieldFromKey(const KeyFileDefinition &keydef, const KeyType &key, USHORT keycol) {
  DbFieldType type = keydef.getFieldDef(keycol).getType();
  if(!keydef.isDefined(key,keycol)) {
    return TupleField();
  }

  TupleField f;
  keydef.get(key,keycol,f);
  return f;
}

void TableCursor::next(Tuple &t) {
  if(t.size() != m_fieldSet.size()) {
    throwSqlError(SQL_INVALID_NO_OF_COLUMNS,_T("Tablecursor::fetch:Invalid number of columns specified in Tuple (=%zd) expected %zd")
                 ,t.size(), m_fieldSet.size());
  }

  if(!hasNext()) {
    throwSqlError(SQL_NOSUCHRECORD,_T("TableCursor::next:No souch record"));
  }

  KeyType key;
  m_keyCursor->next(key);
  if(m_indexOnly) {
    for(UINT i = 0; i < t.size(); i++) {
      t[i] = fieldFromKey(getKeyDef(),key,m_fieldSet[i]);
    }
  } else {
    TableRecord rec(m_db,m_sequenceNo);
    rec.read(getKeyDef().getRecordAddr(key),m_fieldSet,t);
  }
}

TableCursor::~TableCursor() {
  delete m_keyCursor;
}

bool TableCursor::hasNext() const {
  return m_keyCursor->hasNext();
}
/*
KeyType TableCursor::getCurrentKey() const {
  if(!hasCurrent())
    throwSqlError(SQL_NOCURRENTRECORD,_T("No current key"));
  KeyType key;

  m_keyCursor->getCurrentKey(key);
  return key;
}
void TableCursor::removeCurrent() {
  TableRecord rec(m_db,m_sequenceNo);
  rec.remove(m_keydef.getRecordAddr(getCurrentKey()));
}

void TableCursor::updateCurrent(Tuple &tuple) {
  TableRecord rec(m_db,m_sequenceNo);
  rec.update(m_keydef.getRecordAddr(getCurrentKey()),tuple);
}
*/

Packer &operator<<(Packer &p, const TableCursorParam &param) {
  p << param.m_sequenceNo
    << param.m_indexName
    << param.m_indexOnly
    << (char)param.m_beginRelOp
    << param.m_beginFieldCount
    << (char)param.m_endRelOp
    << param.m_endFieldCount
    << (char)param.m_dir
    << param.m_fieldSet.size();
  for(UINT i = 0; i < param.m_fieldSet.size(); i++) {
    p << param.m_fieldSet[i];
  }
  return p;
}

Packer &operator>>(Packer &p, TableCursorParam &param) {
  UINT n;
  char beginRelOp,endRelOp,dir;
  p >> param.m_sequenceNo
    >> param.m_indexName
    >> param.m_indexOnly
    >> beginRelOp
    >> param.m_beginFieldCount
    >> endRelOp
    >> param.m_endFieldCount
    >> dir
    >> n;
  param.m_beginRelOp = (RelationType)beginRelOp;
  param.m_endRelOp   = (RelationType)endRelOp;
  param.m_dir        = (SortDirection)dir;
  param.m_fieldSet.clear();
  for(UINT i = 0; i < n; i++) {
    USHORT f;
    p >> f;
    param.m_fieldSet.add(f);
  }
  return p;
}

String TableCursorParam::toString() const {
  String result = _T("TableCursorParam:\n");
  result += format(_T("  Sequenceno     :%d\n"), m_sequenceNo                   );
  result += format(_T("  Indexname      :%s\n"), m_indexName.cstr()             );
  result += format(_T("  Indexonly      :%s\n"), boolToStr(m_indexOnly)         );
  result += format(_T("  BeginRelOp     :%s\n"), relOpString(m_beginRelOp)      );
  result += format(_T("  Beginfieldcount:%d\n"), m_beginFieldCount              );
  result += format(_T("  EndRelOp       :%s\n"), relOpString(m_endRelOp)        );
  result += format(_T("  Endfieldcount  :%d\n"), m_endFieldCount                );
  result += format(_T("  Direction      :%s\n"), (m_dir == SORT_ASCENDING) ? _T("ASC") : _T("DESC"));
  result += format(_T("  FieldSet       :%s")  , m_fieldSet.toStringBasicType().cstr());
  return result;
}
