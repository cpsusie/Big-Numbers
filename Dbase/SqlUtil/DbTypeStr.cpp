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
  DBTYPE_UNKNOWN    , MAINTYPE_VOID      ,  4 ,  false , _T("UNKNOWN")   ,  //  0
  DBTYPE_DBADDR     , MAINTYPE_NUMERIC   , 10 ,  false , _T("DBADDR")    ,  //  1
  DBTYPE_UNKNOWN    , MAINTYPE_VOID      ,  4 ,  false , _T("UNKNOWN")   ,  //  2
  DBTYPE_CHAR       , MAINTYPE_NUMERIC   ,  4 ,  true  , _T("CHAR")      ,  //  3
  DBTYPE_CHARN      , MAINTYPE_NUMERIC   ,  4 ,  true  , _T("CHARN")     ,  //  4
  DBTYPE_UCHAR      , MAINTYPE_NUMERIC   ,  3 ,  true  , _T("UCHAR")     ,  //  5
  DBTYPE_UCHARN     , MAINTYPE_NUMERIC   ,  4 ,  true  , _T("UCHARN")    ,  //  6
  DBTYPE_SHORT      , MAINTYPE_NUMERIC   ,  6 ,  true  , _T("SHORT")     ,  //  7
  DBTYPE_SHORTN     , MAINTYPE_NUMERIC   ,  6 ,  true  , _T("SHORTN")    ,  //  8
  DBTYPE_USHORT     , MAINTYPE_NUMERIC   ,  5 ,  true  , _T("USHORT")    ,  //  9
  DBTYPE_USHORTN    , MAINTYPE_NUMERIC   ,  5 ,  true  , _T("USHORTN")   ,  // 10
  DBTYPE_INT        , MAINTYPE_NUMERIC   , 11 ,  true  , _T("INT")       ,  // 11
  DBTYPE_INTN       , MAINTYPE_NUMERIC   , 11 ,  true  , _T("INTN")      ,  // 12
  DBTYPE_UINT       , MAINTYPE_NUMERIC   , 10 ,  true  , _T("UINT")      ,  // 13
  DBTYPE_UINTN      , MAINTYPE_NUMERIC   , 10 ,  true  , _T("UINTN")     ,  // 14
  DBTYPE_LONG       , MAINTYPE_NUMERIC   , 11 ,  true  , _T("LONG")      ,  // 15
  DBTYPE_LONGN      , MAINTYPE_NUMERIC   , 11 ,  true  , _T("LONGN")     ,  // 16
  DBTYPE_ULONG      , MAINTYPE_NUMERIC   , 10 ,  true  , _T("ULONG")     ,  // 17
  DBTYPE_ULONGN     , MAINTYPE_NUMERIC   , 10 ,  true  , _T("ULONGN")    ,  // 18
  DBTYPE_INT64      , MAINTYPE_NUMERIC   , 21 ,  true  , _T("INT64")     ,  // 19
  DBTYPE_INT64N     , MAINTYPE_NUMERIC   , 21 ,  true  , _T("INT64N")    ,  // 20
  DBTYPE_UINT64     , MAINTYPE_NUMERIC   , 20 ,  true  , _T("UINT64")    ,  // 21
  DBTYPE_UINT64N    , MAINTYPE_NUMERIC   , 20 ,  true  , _T("UINT64N")   ,  // 22
  DBTYPE_FLOAT      , MAINTYPE_NUMERIC   , 12 ,  false , _T("FLOAT")     ,  // 23
  DBTYPE_FLOATN     , MAINTYPE_NUMERIC   , 12 ,  false , _T("FLOATN")    ,  // 24
  DBTYPE_DOUBLE     , MAINTYPE_NUMERIC   , 16 ,  false , _T("DOUBLE")    ,  // 25
  DBTYPE_DOUBLEN    , MAINTYPE_NUMERIC   , 16 ,  false , _T("DOUBLEN")   ,  // 26
  DBTYPE_CSTRING    , MAINTYPE_STRING    , 30 ,  false , _T("CSTRING")   ,  // 27
  DBTYPE_CSTRINGN   , MAINTYPE_STRING    , 30 ,  false , _T("CSTRINGN")  ,  // 28
  DBTYPE_WSTRING    , MAINTYPE_STRING    , 30 ,  false , _T("WSTRING")   ,  // 29
  DBTYPE_WSTRINGN   , MAINTYPE_STRING    , 30 ,  false , _T("WSTRINGN")  ,  // 30
  DBTYPE_VARCHAR    , MAINTYPE_VARCHAR   , 30 ,  false , _T("VARCHAR")   ,  // 31
  DBTYPE_VARCHARN   , MAINTYPE_VARCHAR   , 30 ,  false , _T("VARCHARN")  ,  // 32
  DBTYPE_DATE       , MAINTYPE_DATE      , 10 ,  false , _T("DATE")      ,  // 33
  DBTYPE_DATEN      , MAINTYPE_DATE      , 10 ,  false , _T("DATEN")     ,  // 34
  DBTYPE_TIME       , MAINTYPE_TIME      , 10 ,  false , _T("TIME")      ,  // 35
  DBTYPE_TIMEN      , MAINTYPE_TIME      , 10 ,  false , _T("TIMEN")     ,  // 36
  DBTYPE_TIMESTAMP  , MAINTYPE_TIMESTAMP , 19 ,  false , _T("TIMESTAMP") ,  // 37
  DBTYPE_TIMESTAMPN , MAINTYPE_TIMESTAMP , 19 ,  false , _T("TIMESTAMPN"),  // 38
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
  return (type == nullptr) ? DBTYPE_UNKNOWN : DbTypeAttributeList[*type].m_dbType;
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
