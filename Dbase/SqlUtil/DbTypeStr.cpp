#include "stdafx.h"
#include <HashMap.h>

struct DbTypeAttribute {
  DbFieldType  m_dbType;
  DbMainType   m_mainType;
  int          m_printWidth;
  bool         m_integerType;
  const TCHAR *m_name;
};

static DbTypeAttribute DbTypeAttributeList[] = {
  DBTYPE_UNKNOWN    , MAINTYPE_VOID      ,  4 ,  false , _T("UNKNOWN")   ,
  DBTYPE_CHAR       , MAINTYPE_NUMERIC   ,  4 ,  true  , _T("CHAR")      ,
  DBTYPE_CHARN      , MAINTYPE_NUMERIC   ,  4 ,  true  , _T("CHARN")     ,
  DBTYPE_UCHAR      , MAINTYPE_NUMERIC   ,  3 ,  true  , _T("UCHAR")     ,
  DBTYPE_UCHARN     , MAINTYPE_NUMERIC   ,  4 ,  true  , _T("UCHARN")    ,
  DBTYPE_SHORT      , MAINTYPE_NUMERIC   ,  6 ,  true  , _T("SHORT")     ,
  DBTYPE_SHORTN     , MAINTYPE_NUMERIC   ,  6 ,  true  , _T("SHORTN")    ,
  DBTYPE_USHORT     , MAINTYPE_NUMERIC   ,  5 ,  true  , _T("USHORT")    ,
  DBTYPE_USHORTN    , MAINTYPE_NUMERIC   ,  5 ,  true  , _T("USHORTN")   ,
  DBTYPE_INT        , MAINTYPE_NUMERIC   , 11 ,  true  , _T("INT")       ,
  DBTYPE_INTN       , MAINTYPE_NUMERIC   , 11 ,  true  , _T("INTN")      ,
  DBTYPE_UINT       , MAINTYPE_NUMERIC   , 10 ,  true  , _T("UINT")      ,
  DBTYPE_UINTN      , MAINTYPE_NUMERIC   , 10 ,  true  , _T("UINTN")     ,
  DBTYPE_LONG       , MAINTYPE_NUMERIC   , 11 ,  true  , _T("LONG")      ,
  DBTYPE_LONGN      , MAINTYPE_NUMERIC   , 11 ,  true  , _T("LONGN")     ,
  DBTYPE_ULONG      , MAINTYPE_NUMERIC   , 10 ,  true  , _T("ULONG")     ,
  DBTYPE_ULONGN     , MAINTYPE_NUMERIC   , 10 ,  true  , _T("ULONGN")    ,
  DBTYPE_INT64      , MAINTYPE_NUMERIC   , 21 ,  true  , _T("INT64")     ,
  DBTYPE_INT64N     , MAINTYPE_NUMERIC   , 21 ,  true  , _T("INT64N")    ,
  DBTYPE_UINT64     , MAINTYPE_NUMERIC   , 20 ,  true  , _T("UINT64")    ,
  DBTYPE_UINT64N    , MAINTYPE_NUMERIC   , 20 ,  true  , _T("UINT64N")   ,
  DBTYPE_FLOAT      , MAINTYPE_NUMERIC   , 12 ,  false , _T("FLOAT")     ,
  DBTYPE_FLOATN     , MAINTYPE_NUMERIC   , 12 ,  false , _T("FLOATN")    ,
  DBTYPE_DOUBLE     , MAINTYPE_NUMERIC   , 16 ,  false , _T("DOUBLE")    ,
  DBTYPE_DOUBLEN    , MAINTYPE_NUMERIC   , 16 ,  false , _T("DOUBLEN")   ,
  DBTYPE_STRING     , MAINTYPE_STRING    , 30 ,  false , _T("STRING")    ,
  DBTYPE_STRINGN    , MAINTYPE_STRING    , 30 ,  false , _T("STRINGN")   ,
  DBTYPE_VARCHAR    , MAINTYPE_VARCHAR   , 30 ,  false , _T("VARCHAR")   ,
  DBTYPE_VARCHARN   , MAINTYPE_VARCHAR   , 30 ,  false , _T("VARCHARN")  ,
  DBTYPE_DATE       , MAINTYPE_DATE      , 10 ,  false , _T("DATE")      ,
  DBTYPE_DATEN      , MAINTYPE_DATE      , 10 ,  false , _T("DATEN")     ,
  DBTYPE_TIME       , MAINTYPE_TIME      , 10 ,  false , _T("TIME")      ,
  DBTYPE_TIMEN      , MAINTYPE_TIME      , 10 ,  false , _T("TIMEN")     ,
  DBTYPE_TIMESTAMP  , MAINTYPE_TIMESTAMP , 19 ,  false , _T("TIMESTAMP") ,
  DBTYPE_TIMESTAMPN , MAINTYPE_TIMESTAMP , 19 ,  false , _T("TIMESTAMPN"),
  DBTYPE_DBADDR     , MAINTYPE_NUMERIC   , 10 ,  false , _T("DBADDR")
};

class DbTypeNameHashMap : public StrHashMap<int> {
public:
  DbTypeNameHashMap() : StrHashMap<int>(41) {
    for(int i = 0; i < ARRAYSIZE(DbTypeAttributeList); i++) {
      put(DbTypeAttributeList[i].m_name,i);
    }
  }
};

static DbTypeNameHashMap dbTypeNameMap;

bool isNullAllowed(DbFieldType dbType) {
  return (dbType & 1) == 0;
}

DbFieldType sqlapi_getFieldType(const TCHAR *typeString) {
  const int *type = dbTypeNameMap.get(typeString);
  return (type == NULL) ? DBTYPE_UNKNOWN : DbTypeAttributeList[*type].m_dbType;
}

DbMainType getMainType(DbFieldType dbType) {
  if(dbType >= ARRAYSIZE(DbTypeAttributeList)) {
    throwSqlError(SQL_UNKNOWN_DBTYPE,_T("Unknown dbtype:<%d>"),dbType);
  }
  return DbTypeAttributeList[dbType].m_mainType;
}

int getDbTypePrintWidth(DbFieldType dbType) {
  if(dbType >= ARRAYSIZE(DbTypeAttributeList)) {
    throwSqlError(SQL_UNKNOWN_DBTYPE,_T("Unknown dbtype:<%d>"),dbType);
  }
  return DbTypeAttributeList[dbType].m_printWidth;
}

const TCHAR *getTypeString(DbFieldType dbType) {
  if(dbType >= ARRAYSIZE(DbTypeAttributeList)) {
    throwSqlError(SQL_UNKNOWN_DBTYPE,_T("Unknown dbtype:<%d>"),dbType);
  }
  return DbTypeAttributeList[dbType].m_name;
}

bool isIntegerType(DbFieldType dbType) {
  if(dbType >= ARRAYSIZE(DbTypeAttributeList)) {
    throwSqlError(SQL_UNKNOWN_DBTYPE,_T("Unknown dbtype:<%d>"),dbType);
  }
  return DbTypeAttributeList[dbType].m_integerType;
}

bool isNumericType(DbFieldType dbType) {
  return getMainType(dbType) == MAINTYPE_NUMERIC;
}

bool isStringType(DbFieldType dbType) {
  switch(getMainType(dbType)) {
  case MAINTYPE_STRING :
  case MAINTYPE_VARCHAR:
    return true;

  default:
    return false;
  }
}

bool isDateType(DbFieldType dbType) {
  return getMainType(dbType) == MAINTYPE_DATE;
}

bool isTimeType(DbFieldType dbType) {
  return getMainType(dbType) == MAINTYPE_TIME;
}

bool isTimestampType(DbFieldType dbType) {
  return getMainType(dbType) == MAINTYPE_TIMESTAMP;
}

static const TCHAR *mainTypeName[] = {
  _T("NUMERIC")
 ,_T("STRING")
 ,_T("VARCHAR")
 ,_T("DATE")
 ,_T("TIME")
 ,_T("TIMESTAMP")
 ,_T("VOID")
};

const TCHAR *getMainTypeString(DbMainType type) {
  if(type >= ARRAYSIZE(mainTypeName)) {
    throwSqlError(SQL_UNKNOWN_DBTYPE,_T("Unknown DbMainType:<%d>"),type);
  }
  return mainTypeName[type];
}

bool isCompatibleType(DbMainType mt1, DbMainType mt2) {
  if(mt1 == mt2) {
    return true;
  }
  switch(mt1) {
  case MAINTYPE_NUMERIC:
    return mt2 == MAINTYPE_NUMERIC   || mt2 == MAINTYPE_VOID;

  case MAINTYPE_STRING:
  case MAINTYPE_VARCHAR:
    return mt2 == MAINTYPE_STRING    || mt2 == MAINTYPE_VARCHAR || mt2 == MAINTYPE_VOID;

  case MAINTYPE_DATE:
    return mt2 == MAINTYPE_DATE      || mt2 == MAINTYPE_VOID;

  case MAINTYPE_TIME:
    return mt2 == MAINTYPE_TIME      || mt2 == MAINTYPE_VOID;

  case MAINTYPE_TIMESTAMP:
    return mt2 == MAINTYPE_TIMESTAMP || mt2 == MAINTYPE_VOID;

  case MAINTYPE_VOID:
    return true;

  default:
    throwSqlError(SQL_FATAL_ERROR,_T("isCompatibleType:Unknown DbMainType (=%d) (mt2=%d)"),mt1,mt2);
    return false;
  }
}

bool isCompatibleType(DbFieldType dbType1, DbFieldType dbType2) {
  return isCompatibleType(getMainType(dbType1),getMainType(dbType2));
}
