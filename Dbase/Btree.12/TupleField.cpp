#include "stdafx.h"
#include <math.h>
#include <float.h>
#include <limits.h>
#include <comdef.h>
#include <atlconv.h>

#define MAX_I64 0x7fffffffffffffffi64
#define sign(x) (((x) < 0) ? -1 : ((x) > 0) ? 1 : 0)

static double getDouble(UINT64 x) {
  if(x <= _I64_MAX) {
    return (double)((INT64)x);
  } else {
    double d = (double)((INT64)(x & MAX_I64));
    d += MAX_I64;
    d++;
    return d;
  }
}


void TupleField::allocate(DbFieldType type) {
  m_type = type;
  switch(type) {
  case DBTYPE_CSTRING  :
  case DBTYPE_CSTRINGN :
  case DBTYPE_WSTRING  :
  case DBTYPE_WSTRINGN :
    m_string    = new String();
    break;
  case DBTYPE_VARCHAR :
  case DBTYPE_VARCHARN:
    m_varchar   = new varchar();
    break;
  case DBTYPE_DATE    :
  case DBTYPE_DATEN   :
    m_date      = new Date();
    break;
  case DBTYPE_TIME    :
  case DBTYPE_TIMEN   :
    m_time      = new Time();
    break;
  case DBTYPE_TIMESTAMP  :
  case DBTYPE_TIMESTAMPN :
    m_timestamp = new Timestamp();
    break;
  }
}

void TupleField::cleanup() {
  switch(m_type) {
  case DBTYPE_CSTRING    :
  case DBTYPE_CSTRINGN   :
  case DBTYPE_WSTRING    :
  case DBTYPE_WSTRINGN   :
    delete m_string;
    m_string    = NULL;
    break;
  case DBTYPE_VARCHAR    :
  case DBTYPE_VARCHARN   :
    delete m_varchar;
    m_varchar   = NULL;
    break;
  case DBTYPE_DATE       :
  case DBTYPE_DATEN      :
    delete m_date;
    m_date      = NULL;
    break;
  case DBTYPE_TIME       :
  case DBTYPE_TIMEN      :
    delete m_time;
    m_time      = NULL;
    break;
  case DBTYPE_TIMESTAMP  :
  case DBTYPE_TIMESTAMPN :
    delete m_timestamp;
    m_timestamp = NULL;
    break;
  }
}

TupleField::TupleField() {
  allocate(DBTYPE_CHAR);
  m_defined = false;
}

TupleField::TupleField(char v) {
  allocate(DBTYPE_CHAR);
  m_defined = true;
  m_char    = v;
}

TupleField::TupleField(UCHAR v) {
  allocate(DBTYPE_UCHAR);
  m_defined = true;
  m_char    = v;
}

TupleField::TupleField(short v) {
  allocate(DBTYPE_SHORT);
  m_defined = true;
  m_short   = v;
}

TupleField::TupleField(USHORT v) {
  allocate(DBTYPE_USHORT);
  m_defined = true;
  m_short   = v;
}

TupleField::TupleField(int v) {
  allocate(DBTYPE_INT);
  m_defined = true;
  m_int     = v;
}

TupleField::TupleField(UINT v) {
  allocate(DBTYPE_UINT);
  m_defined = true;
  m_int     = v;
}

TupleField::TupleField(long v) {
  allocate(DBTYPE_LONG);
  m_defined = true;
  m_int     = v;
}

TupleField::TupleField(ULONG v) {
  allocate(DBTYPE_ULONG);
  m_defined = true;
  m_int     = v;
}

TupleField::TupleField(INT64 v) {
  allocate(DBTYPE_INT64);
  m_defined = true;
  m_int64   = v;
}

TupleField::TupleField(UINT64 v) {
  allocate(DBTYPE_UINT64);
  m_defined = true;
  m_int64   = v;
}

TupleField::TupleField(float v) {
  allocate(DBTYPE_FLOAT);
  m_defined = true;
  m_float   = v;
}

TupleField::TupleField(double v) {
  allocate(DBTYPE_DOUBLE);
  m_defined = true;
  m_double  = v;
}

TupleField::TupleField(const char *v) {
  allocate(DBTYPE_TSTRING);
  m_defined = true;
  *m_string = v;
}

TupleField::TupleField(const wchar_t *v) {
  allocate(DBTYPE_TSTRING);
  m_defined = true;
  *m_string = v;
}

TupleField::TupleField(const String &v) {
  allocate(DBTYPE_TSTRING);
  m_defined = true;
  *m_string = v;
}

TupleField::TupleField(const varchar &v) {
  allocate(DBTYPE_VARCHAR);
  m_defined  = true;
  *m_varchar = v;
}

TupleField::TupleField(const Date &v) {
  allocate(DBTYPE_DATE);
  m_defined = true;
  *m_date   = v;
}

TupleField::TupleField(const Time &v) {
  allocate(DBTYPE_TIME);
  m_defined = true;
  *m_time   = v;
}

TupleField::TupleField(const Timestamp &v) {
  allocate(DBTYPE_TIMESTAMP);
  m_defined    = true;
  *m_timestamp = v;
}

TupleField &TupleField::setUndefined() {
  m_defined = false;
  return *this;
}

TupleField::TupleField(const TupleField &rhs) {
  if(!rhs.isDefined()) {
    m_defined = false;
    m_type = DBTYPE_CHAR;
    return;
  }
  m_type    = rhs.m_type;
  m_defined = true;
  switch(m_type) {
  case DBTYPE_CHAR      :
  case DBTYPE_CHARN     :
  case DBTYPE_UCHAR     :
  case DBTYPE_UCHARN    :
    m_char      = rhs.m_char;
    break;
  case DBTYPE_SHORT     :
  case DBTYPE_SHORTN    :
  case DBTYPE_USHORT    :
  case DBTYPE_USHORTN   :
    m_short     = rhs.m_short;
    break;
  case DBTYPE_INT       :
  case DBTYPE_INTN      :
  case DBTYPE_UINT      :
  case DBTYPE_UINTN     :
    m_int       = rhs.m_int;
    break;
  case DBTYPE_LONG      :
  case DBTYPE_LONGN     :
  case DBTYPE_ULONG     :
  case DBTYPE_ULONGN    :
    m_long      = rhs.m_long;
    break;
  case DBTYPE_INT64     :
  case DBTYPE_INT64N    :
  case DBTYPE_UINT64    :
  case DBTYPE_UINT64N   :
    m_int64     = rhs.m_int64;
    break;
  case DBTYPE_FLOAT     :
  case DBTYPE_FLOATN    :
    m_float     = rhs.m_float;
    break;
  case DBTYPE_DOUBLE    :
  case DBTYPE_DOUBLEN   :
    m_double    = rhs.m_double;
    break;
  case DBTYPE_CSTRING   :
  case DBTYPE_CSTRINGN  :
  case DBTYPE_WSTRING   :
  case DBTYPE_WSTRINGN  :
    m_string    = new String(*rhs.m_string);
    break;
  case DBTYPE_VARCHAR   :
  case DBTYPE_VARCHARN  :
    m_varchar   = new varchar(*rhs.m_varchar);
    break;
  case DBTYPE_DATE      :
  case DBTYPE_DATEN     :
    m_date      = new Date(*rhs.m_date);
    break;
  case DBTYPE_TIME      :
  case DBTYPE_TIMEN     :
    m_time      = new Time(*rhs.m_time);
    break;
  case DBTYPE_TIMESTAMP :
  case DBTYPE_TIMESTAMPN:
    m_timestamp = new Timestamp(*rhs.m_timestamp);
    break;
  default:
    m_string = NULL;
    throwSqlError(SQL_INVALID_FIELDTYPE,_T("Invalid fieldtype. TupleField.type=%d"),m_type);
  }
}

TupleField::~TupleField() {
  cleanup();
}

TupleField &TupleField::operator=(const TupleField &rhs) {
  if(&rhs == this) {
    return *this;
  }
  if(!rhs.isDefined()) {
    return setUndefined();
  }
  if(rhs.m_type != m_type) {
    cleanup();
    allocate(rhs.m_type);
  }
  m_defined = true;
  switch(m_type) {
  case DBTYPE_CHAR      :
  case DBTYPE_CHARN     :
  case DBTYPE_UCHAR     :
  case DBTYPE_UCHARN    :
    m_char     = rhs.m_char;
    break;
  case DBTYPE_SHORT     :
  case DBTYPE_SHORTN    :
  case DBTYPE_USHORT    :
  case DBTYPE_USHORTN   :
    m_short    = rhs.m_short;
    break;
  case DBTYPE_INT       :
  case DBTYPE_INTN      :
  case DBTYPE_UINT      :
  case DBTYPE_UINTN     :
    m_int      = rhs.m_int;
    break;
  case DBTYPE_LONG      :
  case DBTYPE_LONGN     :
  case DBTYPE_ULONG     :
  case DBTYPE_ULONGN    :
    m_long     = rhs.m_long;
    break;
  case DBTYPE_INT64     :
  case DBTYPE_INT64N    :
  case DBTYPE_UINT64    :
  case DBTYPE_UINT64N   :
    m_int64    = rhs.m_int64;
    break;
  case DBTYPE_FLOAT     :
  case DBTYPE_FLOATN    :
    m_float    = rhs.m_float;
    break;
  case DBTYPE_DOUBLE    :
  case DBTYPE_DOUBLEN   :
    m_double   = rhs.m_double;
    break;
  case DBTYPE_CSTRING   :
  case DBTYPE_CSTRINGN  :
  case DBTYPE_WSTRING   :
  case DBTYPE_WSTRINGN  :
    *m_string  = *rhs.m_string;
    break;
  case DBTYPE_VARCHAR   :
  case DBTYPE_VARCHARN  :
    *m_varchar = *rhs.m_varchar;
    break;
  case DBTYPE_DATE      :
  case DBTYPE_DATEN     :
    *m_date    = *rhs.m_date;
    break;
  case DBTYPE_TIME      :
  case DBTYPE_TIMEN     :
    *m_time    = *rhs.m_time;
    break;
  case DBTYPE_TIMESTAMP :
  case DBTYPE_TIMESTAMPN:
    *m_timestamp = *rhs.m_timestamp;
    break;
  default:
    throwSqlError(SQL_INVALID_FIELDTYPE,_T("Invalid fieldtype. TupleField.type=%d"),m_type);
  }
  return *this;
}

void checkNumberRange(double v, double min, double max, bool integer) {
  if(v < min || v > max) {
    throwSqlError(SQL_RANGEERROR,_T("Value %lf exceeds valid range [%lg,%lg]"),v,min,max);
  }
  if(integer && (v != floor(v))) {
    throwSqlError(SQL_RANGEERROR,_T("Value not integer"));
  }
}

void TupleField::get(char &v) const {
  if(!isDefined()) {
    return;
  }
  double d;
  get(d);
  checkNumberRange(d, DBMINCHAR,    DBMAXCHAR, true);
  v = (char)d;
}

void TupleField::get(UCHAR &v) const {
  if(!isDefined()) {
    return;
  }
  double d;
  get(d);
  checkNumberRange(d, DBMINUCHAR,   DBMAXUCHAR, true);
  v = (UCHAR)d;
}

void TupleField::get(short &v) const {
  if(!isDefined()) {
    return;
  }
  double d;
  get(d);
  checkNumberRange(d, DBMINSHORT,   DBMAXSHORT, true);
  v = (short)d;
}

void TupleField::get(USHORT &v) const {
  if(!isDefined()) {
    return;
  }
  double d;
  get(d);
  checkNumberRange(d, DBMINUSHORT,  DBMAXUSHORT, true);
  v = (USHORT)d;
}

void TupleField::get(int &v) const {
  if(!isDefined()) {
    return;
  }
  double d;
  get(d);
  checkNumberRange(d, DBMININT,     DBMAXINT, true);
  v = (int)d;
}

void TupleField::get(UINT &v) const {
  if(!isDefined()) {
    return;
  }
  double d;
  get(d);
  checkNumberRange(d, DBMINUINT,    DBMAXUINT, true);
  v = (UINT)d;
}

void TupleField::get(long &v) const {
  if(!isDefined()) {
    return;
  }
  double d;
  get(d);
  checkNumberRange(d, DBMINLONG,    DBMAXLONG, true);
  v = (long)d;
}

void TupleField::get(ULONG &v) const {
  if(!isDefined()) {
    return;
  }
  double d;
  get(d);
  checkNumberRange(d, DBMINULONG,   DBMAXULONG, true);
  v = (ULONG)d;
}

#pragma warning(disable : 4305)

void TupleField::get(INT64 &v) const {
  if(!isDefined()) {
    return;
  }
  switch(m_type) {
  case DBTYPE_CHAR      :
  case DBTYPE_CHARN     :
  case DBTYPE_UCHAR     :
  case DBTYPE_UCHARN    :
  case DBTYPE_SHORT     :
  case DBTYPE_SHORTN    :
  case DBTYPE_USHORT    :
  case DBTYPE_USHORTN   :
  case DBTYPE_INT       :
  case DBTYPE_INTN      :
  case DBTYPE_UINT      :
  case DBTYPE_UINTN     :
  case DBTYPE_LONG      :
  case DBTYPE_LONGN     :
  case DBTYPE_ULONG     :
  case DBTYPE_ULONGN    :
  case DBTYPE_FLOAT     :
  case DBTYPE_FLOATN    :
  case DBTYPE_DOUBLE    :
  case DBTYPE_DOUBLEN   :
    double d;
    get(d);
    checkNumberRange(d, DBMININT64,    DBMAXINT64, true);
    if(d > 0) {
      v = ((INT64)d) & MAX_I64;
    } else {
      v = (INT64)d;
    }
    break;
  case DBTYPE_INT64  :
  case DBTYPE_INT64N :
    v = m_int64;
    break;

  case DBTYPE_UINT64 :
  case DBTYPE_UINT64N:
    if(((UINT64)m_int64) > DBMAXINT64) {
      throwSqlError(SQL_RANGEERROR,_T("Value %I64u exceeds valid range [%I64d,%I64d]"),m_int64,DBMININT64,DBMAXINT64);
    }
    v = m_int64;
    break;
  default:
    throwSqlError(SQL_FATAL_ERROR, _T("Type mismatch in TupleField::get(INT64) type:%s"),getTypeString(m_type));
  }
}

void TupleField::get(UINT64 &v) const {
  if(!isDefined()) {
    return;
  }
  switch(m_type) {
  case DBTYPE_CHAR      :
  case DBTYPE_CHARN     :
  case DBTYPE_UCHAR     :
  case DBTYPE_UCHARN    :
  case DBTYPE_SHORT     :
  case DBTYPE_SHORTN    :
  case DBTYPE_USHORT    :
  case DBTYPE_USHORTN   :
  case DBTYPE_INT       :
  case DBTYPE_INTN      :
  case DBTYPE_UINT      :
  case DBTYPE_UINTN     :
  case DBTYPE_LONG      :
  case DBTYPE_LONGN     :
  case DBTYPE_ULONG     :
  case DBTYPE_ULONGN    :
  case DBTYPE_FLOAT  :
  case DBTYPE_FLOATN :
  case DBTYPE_DOUBLE :
  case DBTYPE_DOUBLEN:
    double d;
    get(d);
    checkNumberRange(d, DBMINUINT64,    DBMAXUINT64, true);
    v = (UINT64)d;
    break;
  case DBTYPE_INT64  :
  case DBTYPE_INT64N :
    if(m_int64 < DBMINUINT64) {
      throwSqlError(SQL_RANGEERROR,_T("Value %I64d exceeds valid range [%I64u,%I64u]"),m_int64,DBMINUINT64,DBMAXUINT64);
    }
    v = (UINT64)m_int64;
    break;

  case DBTYPE_UINT64 :
  case DBTYPE_UINT64N:
    v = (UINT64)m_int64;
    break;
  default:
    throwSqlError(SQL_FATAL_ERROR, _T("Type mismatch in TupleField::get(UINT64) type:%s"),getTypeString(m_type));
  }
}

void TupleField::get(float &v) const {
  if(!isDefined()) {
    return;
  }
  double d;
  get(d);
  checkNumberRange(d, DBMINFLOAT,   DBMAXFLOAT,false);
  v = (float)d;
}

void TupleField::get(double &v) const {
  if(!isDefined()) {
    return;
  }
  switch(m_type) {
  case DBTYPE_CHAR      :
  case DBTYPE_CHARN     :
    v = m_char;
    break;
  case DBTYPE_UCHAR     :
  case DBTYPE_UCHARN    :
    v = (UCHAR)m_char;
    break;
  case DBTYPE_SHORT     :
  case DBTYPE_SHORTN    :
    v = m_short;
    break;
  case DBTYPE_USHORT    :
  case DBTYPE_USHORTN   :
    v = (USHORT)m_short;
    break;
  case DBTYPE_INT       :
  case DBTYPE_INTN      :
    v = m_int;
    break;
  case DBTYPE_UINT      :
  case DBTYPE_UINTN     :
    v = (UINT)m_int;
    break;
  case DBTYPE_LONG      :
  case DBTYPE_LONGN     :
    v = m_long;
    break;
  case DBTYPE_ULONG     :
  case DBTYPE_ULONGN    :
    v = (ULONG)m_long;
    break;
  case DBTYPE_INT64     :
  case DBTYPE_INT64N    :
    if((m_int64 >= (((INT64)1)<<53)) || (m_int64 < -(((INT64)1)<<53))) {
      throwSqlError(SQL_LOOSEPRECISION,_T("Cannot get INT64 (%I64d) as a double. Lose of precision."), m_int64);
    }
    v = (double)m_int64;
    break;

  case DBTYPE_UINT64    :
  case DBTYPE_UINT64N   :
    if(((UINT64)m_int64 >= (((UINT64)1)<<53))) {
      throwSqlError(SQL_LOOSEPRECISION,_T("Cannot get UINT64 (%I64u) as a double. Lose of precision."), m_int64);
    }
    v = getDouble((UINT64)m_int64);
    break;

  case DBTYPE_FLOAT     :
  case DBTYPE_FLOATN    :
    v = m_float;
    break;
  case DBTYPE_DOUBLE    :
  case DBTYPE_DOUBLEN   :
    v = m_double;
    break;
  default:
    throwSqlError(SQL_FATAL_ERROR, _T("Type mismatch in TupleField::get(double). Type:%s"),getTypeString(m_type));
  }
}

void TupleField::get(String &v) const {
  if(!isDefined()) {
    return;
  }
  switch(m_type) {
  case DBTYPE_CSTRING   :
  case DBTYPE_CSTRINGN  :
  case DBTYPE_WSTRING   :
  case DBTYPE_WSTRINGN  :
    v = *m_string;
    break;
  case DBTYPE_VARCHAR   :
  case DBTYPE_VARCHARN  :
    v = (TCHAR*)m_varchar->data();
    break;
  default:
    throwSqlError(SQL_FATAL_ERROR,_T("Type mismatch in TupleField::get(String). Type:%s"),getTypeString(m_type));
  }
}

void TupleField::get(varchar &v) const {
  if(!isDefined()) {
    return;
  }
  USES_CONVERSION;
  switch(m_type) {
  case DBTYPE_CSTRING   :
  case DBTYPE_CSTRINGN  :
    { v = T2A((TCHAR*)(m_string->cstr()));  }
    break;
  case DBTYPE_WSTRING   :
  case DBTYPE_WSTRINGN  :
    { v = T2W((TCHAR*)(m_string->cstr()));  }
    break;
  case DBTYPE_VARCHAR   :
  case DBTYPE_VARCHARN  :
    v = *m_varchar;
    break;
  default:
    throwSqlError(SQL_FATAL_ERROR,_T("Type mismatch in TupleField::get(varchar). Type:%s"),getTypeString(m_type));
  }
}

void TupleField::get(Date &v) const {
  if(!isDefined()) {
    return;
  }
  if(m_type != DBTYPE_DATE && m_type != DBTYPE_DATEN) {
    throwSqlError(SQL_FATAL_ERROR,_T("Type mismatch in TupleField::get(Date). Type:%s"),getTypeString(m_type));
  }
  v = *m_date;
}

void TupleField::get(Time &v) const {
  if(!isDefined()) {
    return;
  }
  if(m_type != DBTYPE_TIME && m_type != DBTYPE_TIMEN) {
    throwSqlError(SQL_FATAL_ERROR,_T("Type mismatch in TupleField::get(Time). Type:%s"),getTypeString(m_type));
  }
  v = *m_time;
}

void TupleField::get(Timestamp &v) const {
  if(!isDefined()) {
    return;
  }
  if(m_type != DBTYPE_TIMESTAMP && m_type != DBTYPE_TIMESTAMPN) {
    throwSqlError(SQL_FATAL_ERROR,_T("Type mismatch in TupleField::get(Timestamp). Type:%s"),getTypeString(m_type));
  }
  v = *m_timestamp;
}

TupleField operator+(const TupleField &tf1, const TupleField &tf2) {
  if(!tf1.isDefined() || !tf2.isDefined()) {
    return TupleField();
  }
  if(tf1.getType() == DBTYPE_DATE || tf1.getType() == DBTYPE_DATEN) {
    Date   d1;
    double d2;
    tf1.get(d1);
    tf2.get(d2);
    TupleField result;
    try {
      result = d1 + (int)d2;
    } catch(Exception e) {
      throwSqlError(SQL_DOMAIN_ERROR,_T("%s"),e.what());
    }
    return result;
  }
  if(tf2.getType() == DBTYPE_DATE || tf2.getType() == DBTYPE_DATEN) {
    double d1;
    Date   d2;
    tf1.get(d1);
    tf2.get(d2);
    TupleField result;
    try {
      result = d2 + (int)d1;
    } catch(Exception e) {
      throwSqlError(SQL_DOMAIN_ERROR,_T("%s"),e.what());
    }
    return result;
  }

  double d1, d2;
  tf1.get(d1);
  tf2.get(d2);
  TupleField result;
  result = d1 + d2;
  return result;
}

TupleField operator-(const TupleField &tf1, const TupleField &tf2) {
  if(!tf1.isDefined() || !tf2.isDefined()) {
    return TupleField();
  }
  if(tf1.getType() == DBTYPE_DATE || tf1.getType() == DBTYPE_DATEN) {
    Date d1;
    tf1.get(d1);
    if(tf2.getType() == DBTYPE_DATE || tf2.getType() == DBTYPE_DATEN) {
      Date d2;
      tf2.get(d2);
      TupleField result;
      result = d1 - d2;
      return result;
    } else {
      double d2;
      tf2.get(d2);
      TupleField result;
      try {
        result = d1 - (int)d2;
      } catch(Exception e) {
        throwSqlError(SQL_DOMAIN_ERROR,_T("%s"),e.what());
      }
      return result;
    }
  }

  double d1, d2;
  tf1.get(d1);
  tf2.get(d2);
  TupleField result;
  result = d1 - d2;
  return result;
}

TupleField operator-(const TupleField &v) {
  if(!v.isDefined()) {
    return v;
  }

  double d;
  v.get(d);
  TupleField result;
  result = -d;
  return result;
}

TupleField operator*(const TupleField &tf1, const TupleField &tf2) {
  if(!tf1.isDefined() || !tf2.isDefined()) {
    return TupleField();
  }
  double d1,d2;
  tf1.get(d1);
  tf2.get(d2);
  TupleField result;
  result = d1 * d2;
  return result;
}

TupleField operator/(const TupleField &tf1, const TupleField &tf2) {
  if(!tf1.isDefined() || !tf2.isDefined()) {
    return TupleField();
  }
  double d1,d2;
  tf1.get(d1);
  tf2.get(d2);
  TupleField result;
  if(d2 == 0) {
    return result; // undefined. Not error. throwSqlError(SQL_DIVISION_BY_ZERO,_T("Division by zero"));
  }
  result = d1 / d2;
  return result;
}

TupleField operator%(const TupleField &tf1, const TupleField &tf2) {
  if(!tf1.isDefined() || !tf2.isDefined()) {
    return TupleField();
  }
  double d1,d2;
  tf1.get(d1);
  tf2.get(d2);
  TupleField result;
  if(d2 == 0) {
    return result; // undefined. Not error. throwSqlError(SQL_MODULUS_BY_ZERO,_T("Modulus by zero"));
  }
  result = fmod(d1,d2);
  return result;
}

double sqlPow(double x, double y) { // handles negative x with y=integer
  if(y == 0) {
    return 1;
  } else if(x >= 0) {
    return pow(x,y);
  } else if(y == floor(y)) {
    int d = (int)y;
    if(d % 2 == 0) {
      return pow(-x,y);
    } else {
      return -pow(-x,y);
    }
  }
  throwSqlError(SQL_DOMAIN_ERROR,_T("Negative argument to exponentiation not allowed"));
  return 0;
}

TupleField sqlPow(const TupleField &tf1, const TupleField &tf2) {
  if(!tf1.isDefined() || !tf2.isDefined()) {
    return TupleField();
  }
  double d1,d2;
  tf1.get(d1);
  tf2.get(d2);
  TupleField result;
  result = sqlPow(d1,d2);
  return result;
}

TupleField operator&&(const TupleField &tf1, const TupleField &tf2) {
  bool defined1 = tf1.isDefined();
  bool defined2 = tf2.isDefined();
  int d1,d2;
  tf1.get(d1);
  tf2.get(d2);

  if(defined1 && defined2) {
    return d1 && d2;
  }
  if(!defined1 && defined2) {
    return (d2 == 1) ? TupleField() : TupleField(0);
  }
  if(defined1 && !defined2) {
    return (d1 == 1) ? TupleField() : TupleField(0);
  }
  return TupleField();
}

TupleField operator||(const TupleField &tf1, const TupleField &tf2) {
  bool defined1 = tf1.isDefined();
  bool defined2 = tf2.isDefined();
  int d1,d2;
  tf1.get(d1);
  tf2.get(d2);

  if(defined1 && defined2) {
    return d1 || d2;
  }
  if(!defined1 && defined2) {
    return (d2 == 1) ? TupleField(1) : TupleField();
  }
  if(defined1 && !defined2) {
    return (d1 == 1) ? TupleField(1) : TupleField();
  }
  return TupleField();
}

TupleField operator!(const TupleField &v) {
  if(!v.isDefined()) {
    return TupleField();
  }
  int d;
  v.get(d);
  return !d;
}

TupleField operator>(const TupleField &tf1, const TupleField &tf2) {
  switch(compare(tf1,tf2)) {
  case  1: return 1;
  case  0:
  case -1: return 0;
  }
  return TupleField();
}

TupleField operator>=(const TupleField &tf1, const TupleField &tf2) {
  switch(compare(tf1,tf2)) {
  case  1:
  case  0: return 1;
  case -1: return 0;
  }
  return TupleField();
}

TupleField operator<(const TupleField &tf1, const TupleField &tf2) {
  switch(compare(tf1,tf2)) {
  case  1:
  case  0: return 0;
  case -1: return 1;
  }
  return TupleField();
}

TupleField operator<=(const TupleField &tf1, const TupleField &tf2) {
  switch(compare(tf1,tf2)) {
  case  1: return 0;
  case  0:
  case -1: return 1;
  }
  return TupleField();
}

TupleField operator==(const TupleField &tf1, const TupleField &tf2) {
  switch(compare(tf1,tf2)) {
  case  1: return 0;
  case  0: return 1;
  case -1: return 0;
  }
  return TupleField();
}

TupleField operator!=(const TupleField &tf1, const TupleField &tf2) {
  switch(compare(tf1,tf2)) {
  case  1: return 1;
  case  0: return 0;
  case -1: return 1;
  }
  return TupleField();
}

TupleField concat(const TupleField &tf1, const TupleField &tf2) {
  if(!tf1.isDefined() || !tf2.isDefined()) {
    return TupleField();
  }
  String s1,s2;
  tf1.get(s1);
  tf2.get(s2);
  return TupleField(s1 + s2);
}

// assume both tf1 and tf2 is defined and getMainType(tf1.getType()) == getMainType(tf2.getType()) == MAINTYPE_NUMERIC
static int compareNumeric(const TupleField &tf1, const TupleField &tf2) {
  switch(tf1.getType()) {
  case DBTYPE_INT64  :
  case DBTYPE_INT64N :
    { INT64 v1;
      tf1.get(v1);
      switch(tf2.getType()) {
      case DBTYPE_INT64  :
      case DBTYPE_INT64N :
        { INT64 v2;
          tf2.get(v2);
          return v1 < v2 ? -1 : v1 > v2 ? 1 : 0;
        }
        break;

      case DBTYPE_UINT64 :
      case DBTYPE_UINT64N:
        { UINT64 uv2;
          tf2.get(uv2);
          if(v1 < 0) {
            return -1;
          } else {
            UINT64 uv1 = (UINT64)v1;
            return uv1 > uv2 ? -1 : uv1 < uv2 ? 1 : 0;
          }
        }
        break;
      default            :
        { double v2;
          tf2.get(v2);
          double c = v1 - v2;
          return sign(c);
        }
      }
    }
    break;

  case DBTYPE_UINT64 :
  case DBTYPE_UINT64N:
    { UINT64 uv1;
      tf1.get(uv1);
      switch(tf2.getType()) {
      case DBTYPE_INT64  :
      case DBTYPE_INT64N :
        { INT64 v2;
          tf2.get(v2);
          if(v2 < 0) {
            return 1;
          } else {
            UINT64 uv2 = (UINT64)v2;
            return uv1 > uv2 ? -1 : uv1 < uv2 ? 1 : 0;
          }
        }
        break;

      case DBTYPE_UINT64 :
      case DBTYPE_UINT64N:
        { UINT64 uv2;
          tf2.get(uv2);
          return uv1 > uv2 ? -1 : uv1 < uv2 ? 1 : 0;
        }
        break;
      default            :
        { double v2;
          tf2.get(v2);
          return getDouble(uv1) > v2 ? -1 : getDouble(uv1) < v2 ? 1 : 0;
        }
      }
    }
    break;

  default:
    switch(tf2.getType()) {
    case DBTYPE_INT64  :
    case DBTYPE_INT64N :
    case DBTYPE_UINT64 :
    case DBTYPE_UINT64N:
      return -compareNumeric(tf2,tf1);

    default:
      { double d1,d2;
        tf1.get(d1);
        tf2.get(d2);
        double c = d1 - d2;
        return sign(c);
      }
    }
  }
  throwException(_T("compareNumeric dropped to the end. tf1.type=%d. tf2.type=%d"),tf1.getType(), tf2.getType());
}

int compare(const TupleField &tf1, const TupleField &tf2) {
  if(!tf1.isDefined() || !tf2.isDefined()) {
    return -2;
  }
  switch(getMainType(tf1.getType())) {
  case MAINTYPE_NUMERIC:
    return compareNumeric(tf1,tf2);

  case MAINTYPE_STRING :
    { String s1,s2;
      tf1.get(s1);
      tf2.get(s2);
      int c = _tcscmp(s1.cstr(),s2.cstr());
      return sign(c);
    }

  case MAINTYPE_VARCHAR:
    { varchar vch1,vch2;
      tf1.get(vch1);
      tf2.get(vch2);
      int c = compare(vch1,vch2);
      return sign(c);
    }

  case MAINTYPE_DATE   :
    { Date d1,d2;
      tf1.get(d1);
      tf2.get(d2);
      int c = d1 - d2;
      return sign(c);
    }

  case MAINTYPE_TIME   :
    { Time t1,t2;
      tf1.get(t1);
      tf2.get(t2);
      return timeCmp(t1,t2);
    }

  case MAINTYPE_TIMESTAMP:
    { Timestamp t1,t2;
      tf1.get(t1);
      tf2.get(t2);
      return timestampCmp(t1,t2);
    }

  default:
    throwSqlError(SQL_FATAL_ERROR,_T("compare:Unknown type of TupleField:%d"),tf1.getType());
    return 0;
  }
}

//-----------------------------Cast functions ------------------------------------

static double stringToNumber(const String &v) {
  double d;
  if(_stscanf(v.cstr(),_T("%le"),&d) != 1) {
    throwSqlError(SQL_INVALIDCAST,_T("Invalid cast. String <%s> not numeric"), v.cstr());
  }
  return d;
}

static String numberToString(double v) {
  return format(_T("%lg"),v);
}

//-----------------------------------------------------------------

static Date stringToDate(const String &v) {
  try {
    return Date(v);
  } catch(Exception) {
    throwSqlError(SQL_INVALIDCAST,_T("Invalid cast. String <%s> not a valid date"), v.cstr());
    return Date();
  }
}

static Time stringToTime(const String &v) {
  try {
    return Time(v);
  } catch(Exception) {
    throwSqlError(SQL_INVALIDCAST,_T("Invalid cast. String <%s> not a valid time"), v.cstr());
    return Time();
  }
}

static Timestamp stringToTimestamp(const String &v) {
  try {
    return Timestamp(v);
  } catch(Exception) {
    throwSqlError(SQL_INVALIDCAST,_T("Invalid cast. varchar <%s> not a valid timestamp"), v.cstr());
    return Timestamp();
  }
}

//-----------------------------------------------------------------
static double dateToNumber(const Date &v) {
  int day,month,year;
  v.getDMY(day,month,year);
  return year * 10000 + month * 100 + day;
}


static Date numberToDate(double v) {
  try {
    long lu = (ULONG)v;
    return Date(lu % 100, (lu / 100) % 100, lu / 10000);
  } catch(Exception) {
    throwSqlError(SQL_INVALIDCAST,_T("Invalid cast. Number %lg not a valid date"), v);
    return Date();
  }
}
//-----------------------------------------------------------------

static Time numberToTime(double v) {
  try {
    long lu = (ULONG)v;
    return Time(lu / 10000000,(lu / 100000) % 100, (lu/1000) % 100, lu % 1000);
  } catch(Exception) {
    throwSqlError(SQL_INVALIDCAST,_T("Invalid cast. Number %lg not a valid time"), v);
    return Time();
  }
}

static double timeToNumber(const Time &v) {
  int hour,minute,second,milliSecond;
  v.getHMS(hour,minute,second,milliSecond);
  return ((hour * 100 + minute) * 100 + second) * 1000 + milliSecond;
}

//-----------------------------------------------------------------

static Timestamp dateToTimestamp(const Date &v) {
  int day,month,year;
  v.getDMY(day,month,year);
  return Timestamp(day,month,year,0,0,0);
}

static double timestampToNumber(const Timestamp &v) {
  return dateToNumber(v.getDate()) * 1000000000 + timeToNumber(v.getTime());
}

static Timestamp numberToTimestamp(double v) {
  return Timestamp(numberToDate(v/1000000000),numberToTime(fmod(v,1000000000)));
}

//-----------------------------------------------------------------

void TupleField::setValue(double v) {
  switch(m_type) {
  case DBTYPE_CHAR      :
  case DBTYPE_CHARN     :
    *this = (char)v;
    break;

  case DBTYPE_UCHAR     :
  case DBTYPE_UCHARN    :
    *this = (UCHAR)v;
    break;

  case DBTYPE_SHORT     :
  case DBTYPE_SHORTN    :
    *this = (short)v;
    break;

  case DBTYPE_USHORT    :
  case DBTYPE_USHORTN   :
    *this = (USHORT)v;
    break;

  case DBTYPE_INT       :
  case DBTYPE_INTN      :
    *this = (int)v;
    break;

  case DBTYPE_UINT      :
  case DBTYPE_UINTN     :
    *this = (UINT)v;
    break;

  case DBTYPE_LONG      :
  case DBTYPE_LONGN     :
    *this = (long)v;
    break;

  case DBTYPE_ULONG     :
  case DBTYPE_ULONGN    :
    *this = (ULONG)v;
    break;

  case DBTYPE_INT64     :
  case DBTYPE_INT64N    :
    *this = (INT64)v;
    break;

  case DBTYPE_UINT64    :
  case DBTYPE_UINT64N   :
    *this = (UINT64)v;
    break;

  case DBTYPE_FLOAT     :
  case DBTYPE_FLOATN    :
    *this = (float)v;
    break;

  case DBTYPE_DOUBLE    :
  case DBTYPE_DOUBLEN   :
    *this = v;
    break;

  case DBTYPE_CSTRING   :
  case DBTYPE_CSTRINGN  :
  case DBTYPE_WSTRING   :
  case DBTYPE_WSTRINGN  :
    *this = numberToString(v);
    break;

  case DBTYPE_VARCHAR   :
  case DBTYPE_VARCHARN  :
    *this = varchar(numberToString(v).cstr());
    break;

  case DBTYPE_DATE      :
  case DBTYPE_DATEN     :
    *this = numberToDate(v);
    break;

  case DBTYPE_TIME      :
  case DBTYPE_TIMEN     :
    *this = numberToTime(v);
    break;

  case DBTYPE_TIMESTAMP :
  case DBTYPE_TIMESTAMPN:
    *this = numberToTimestamp(v);
    break;

  default:
    throwSqlError(SQL_INVALIDCAST,_T("Invalid cast. Cannot cast number to %s"),getTypeString(m_type));
  }
}

void TupleField::setValue(const String &v) {
  if(isNumericType(m_type)) {
    setValue(stringToNumber(v));
  } else {
    switch(m_type) {
    case DBTYPE_CSTRING   :
    case DBTYPE_CSTRINGN  :
    case DBTYPE_WSTRING   :
    case DBTYPE_WSTRINGN  :
      *this = v;
      break;

    case DBTYPE_VARCHAR   :
    case DBTYPE_VARCHARN  :
      *this = varchar(v.cstr());
      break;

    case DBTYPE_DATE      :
    case DBTYPE_DATEN     :
      *this = stringToDate(v);
      break;

    case DBTYPE_TIME      :
    case DBTYPE_TIMEN     :
      *this = stringToTime(v);
      break;

    case DBTYPE_TIMESTAMP :
    case DBTYPE_TIMESTAMPN:
      *this = stringToTimestamp(v);
      break;

    default:
      throwSqlError(SQL_INVALIDCAST,_T("Invalid cast. Cannot cast string to %s"),getTypeString(m_type));
    }
  }
}

void TupleField::setValue(const Date &v) {
  if(isNumericType(m_type)) {
    setValue(dateToNumber(v));
  } else {
    switch(m_type) {
    case DBTYPE_CSTRING   :
    case DBTYPE_CSTRINGN  :
    case DBTYPE_WSTRING   :
    case DBTYPE_WSTRINGN  :
      *this = v.toString();
      break;

    case DBTYPE_VARCHAR   :
    case DBTYPE_VARCHARN  :
      *this = varchar(v.toString().cstr());
      break;

    case DBTYPE_DATE      :
    case DBTYPE_DATEN     :
      *this = v;
      break;

    case DBTYPE_TIMESTAMP :
    case DBTYPE_TIMESTAMPN:
      *this = dateToTimestamp(v);
      break;

    default:
      throwSqlError(SQL_INVALIDCAST,_T("Invalid cast. Cannot cast date to %s"),getTypeString(m_type));
    }
  }
}

void TupleField::setValue(const Time &v) {
  if(isNumericType(m_type)) {
    setValue(timeToNumber(v));
  } else {
    switch(m_type) {
    case DBTYPE_CSTRING   :
    case DBTYPE_CSTRINGN  :
    case DBTYPE_WSTRING   :
    case DBTYPE_WSTRINGN  :
      *this = v.toString();
      break;

    case DBTYPE_VARCHAR   :
    case DBTYPE_VARCHARN  :
      *this = varchar(v.toString().cstr());
      break;

    case DBTYPE_TIME      :
    case DBTYPE_TIMEN     :
      *this = v;
      break;

    case DBTYPE_TIMESTAMP :
    case DBTYPE_TIMESTAMPN:
      *this = Timestamp(Date(),v);
      break;

    default:
      throwSqlError(SQL_INVALIDCAST,_T("Invalid cast. Cannot cast time to %s"),getTypeString(m_type));
    }
  }
}

void TupleField::setValue(const Timestamp &v) {
  if(isNumericType(m_type)) {
    setValue(timestampToNumber(v));
  } else {
    switch(m_type) {
    case DBTYPE_CSTRING   :
    case DBTYPE_CSTRINGN  :
    case DBTYPE_WSTRING   :
    case DBTYPE_WSTRINGN  :
      *this = v.toString(ddMMyyyyhhmmssSSS);
      break;

    case DBTYPE_VARCHAR   :
    case DBTYPE_VARCHARN  :
      *this = varchar(v.toString(ddMMyyyyhhmmssSSS));
      break;

    case DBTYPE_TIMESTAMP :
    case DBTYPE_TIMESTAMPN:
      *this = v;
      break;

    case DBTYPE_DATE      :
    case DBTYPE_DATEN     :
      *this = v.getDate();
      break;

    case DBTYPE_TIME      :
    case DBTYPE_TIMEN     :
      *this = v.getTime();
      break;

    default:
      throwSqlError(SQL_INVALIDCAST,_T("Invalid cast. Cannot cast timestamp to %s"),getTypeString(m_type));
    }
  }
}

void TupleField::setValue(const varchar &v) {
  if(isNumericType(m_type)) {
    setValue(stringToNumber(String((TCHAR*)v.data())));
  } else {
    switch(m_type) {
    case DBTYPE_CSTRING   :
    case DBTYPE_CSTRINGN  :
    case DBTYPE_WSTRING   :
    case DBTYPE_WSTRINGN  :
      *this = String((TCHAR*)v.data());
      break;

    case DBTYPE_VARCHAR   :
    case DBTYPE_VARCHARN  :
      *this = v;
      break;

    case DBTYPE_DATE      :
    case DBTYPE_DATEN     :
      *this = stringToDate(String((TCHAR*)v.data()));
      break;

    case DBTYPE_TIME      :
    case DBTYPE_TIMEN     :
      *this = stringToTime(String((TCHAR*)v.data()));
      break;

    case DBTYPE_TIMESTAMP :
    case DBTYPE_TIMESTAMPN:
      *this = stringToTimestamp(String((TCHAR*)v.data()));
      break;

    default:
      throwSqlError(SQL_INVALIDCAST,_T("Invalid cast. Cannot cast varchar to %s")
                                   ,getTypeString(m_type));
    }
  }
}

void TupleField::setType(DbFieldType newType) {
  if(newType == m_type || !isDefined()) {
    return;
  }

  TupleField newv;
  newv.allocate(newType);

  if(isNumericType(m_type)) {
    double d;
    get(d);
    newv.setValue(d);
  } else {
    switch(m_type) {
    case DBTYPE_CSTRING   :
    case DBTYPE_CSTRINGN  :
    case DBTYPE_WSTRING   :
    case DBTYPE_WSTRINGN  :
      { String str;
        get(str);
        newv.setValue(str);
      }
      break;
    case DBTYPE_VARCHAR   :
    case DBTYPE_VARCHARN  :
      { varchar vch;
        get(vch);
        newv.setValue(vch);
      }
      break;
    case DBTYPE_DATE      :
    case DBTYPE_DATEN     :
      { Date d;
        get(d);
        newv.setValue(d);
      }
      break;
    case DBTYPE_TIME      :
    case DBTYPE_TIMEN     :
      { Time d;
        get(d);
        newv.setValue(d);
      }
      break;
    case DBTYPE_TIMESTAMP :
    case DBTYPE_TIMESTAMPN:
      { Timestamp d;
        get(d);
        newv.setValue(d);
      }
      break;
    }
  }
  *this = newv;
}

void TupleField::get(SqlApiVarList &hv) const {
  if(!isDefined()) {
    if(!isNullAllowed(hv.getType())) {
      throwSqlError(SQL_NOINDICATOR, _T("Unable to return nullvalue in hostvar because no indicator is specified"));
    }
    *hv.sqlind = INDICATOR_UNDEFINED;
    return;
  }
  USES_CONVERSION;
  switch(hv.getType()) {
  case DBTYPE_CHAR      :
  case DBTYPE_CHARN     :
    get(*(char*)hv.sqldata);
    break;
  case DBTYPE_UCHAR     :
  case DBTYPE_UCHARN    :
    get(*(UCHAR*)hv.sqldata);
    break;
  case DBTYPE_SHORT     :
  case DBTYPE_SHORTN    :
    get(*(short*)hv.sqldata);
    break;
  case DBTYPE_USHORT    :
  case DBTYPE_USHORTN   :
    get(*(USHORT*)hv.sqldata);
    break;
  case DBTYPE_INT       :
  case DBTYPE_INTN      :
    get(*(int*)hv.sqldata);
    break;
  case DBTYPE_UINT      :
  case DBTYPE_UINTN     :
    get(*(UINT*)hv.sqldata);
    break;
  case DBTYPE_LONG      :
  case DBTYPE_LONGN     :
    get(*(long*)hv.sqldata);
    break;
  case DBTYPE_ULONG     :
  case DBTYPE_ULONGN    :
    get(*(ULONG*)hv.sqldata);
    break;
  case DBTYPE_FLOAT     :
  case DBTYPE_FLOATN    :
    get(*(float*)hv.sqldata);
    break;
  case DBTYPE_DOUBLE    :
  case DBTYPE_DOUBLEN   :
    get(*(double*)hv.sqldata);
    break;
  case DBTYPE_CSTRING   :
  case DBTYPE_CSTRINGN  :
    { String s;
      get(s);
      const ULONG nbytes = (ULONG)s.length()*sizeof(char);
      if(nbytes > hv.sqllen) {
        throwSqlError(SQL_STRING_TOO_LONG,_T("String too long to fit in hostvar"));
      }
      const char    *cstr = T2A(s.cstr());
      memcpy(hv.sqldata, cstr, nbytes);
      hv.sqllen = nbytes;
    }
    break;
  case DBTYPE_WSTRING   :
  case DBTYPE_WSTRINGN  :
    { String s;
      get(s);
      const ULONG nbytes = (ULONG)s.length()*sizeof(wchar_t);
      if(nbytes > hv.sqllen) {
        throwSqlError(SQL_STRING_TOO_LONG,_T("String too long to fit in hostvar"));
      }
      const wchar_t *wstr = T2W(s.cstr());
      memcpy(hv.sqldata, wstr, nbytes);
      hv.sqllen = nbytes;
    }
    break;
  case DBTYPE_VARCHAR   :
  case DBTYPE_VARCHARN  :
    { varchar vch;
      get(vch);
      if(vch.len() > hv.sqllen) {
        throwSqlError(SQL_VARCHAR_TOO_LONG,_T("varchar too long to fit in hostvar"));
      }
      memcpy(hv.sqldata,vch.data(),vch.len());
      hv.sqllen = vch.len();
    }
    break;
  case DBTYPE_DATE      :
  case DBTYPE_DATEN     :
    get(*(Date*)hv.sqldata);
    break;
  case DBTYPE_TIME      :
  case DBTYPE_TIMEN     :
    get(*(Time*)hv.sqldata);
    break;
  case DBTYPE_TIMESTAMP :
  case DBTYPE_TIMESTAMPN:
    get(*(Timestamp*)hv.sqldata);
    break;
  default:
    throwSqlError(SQL_FATAL_ERROR,_T("Unknown type in gethostvar() type=%d"),hv.getType());
  }
}

TupleField &TupleField::operator=(const SqlApiVarList &hv) {
  if(isNullAllowed(hv.getType()) && *hv.sqlind == INDICATOR_UNDEFINED) {
    return setUndefined();
  }
  switch(hv.getType()) {
  case DBTYPE_CHAR      :
  case DBTYPE_CHARN     :
    *this = *(char*)hv.sqldata;
    break;
  case DBTYPE_UCHAR     :
  case DBTYPE_UCHARN    :
    *this = *(UCHAR*)hv.sqldata;
    break;
  case DBTYPE_SHORT     :
  case DBTYPE_SHORTN    :
    *this = *(short*)hv.sqldata;
    break;
  case DBTYPE_USHORT    :
  case DBTYPE_USHORTN   :
    *this = *(USHORT*)hv.sqldata;
    break;
  case DBTYPE_INT       :
  case DBTYPE_INTN      :
    *this = *(int*)hv.sqldata;
    break;
  case DBTYPE_UINT      :
  case DBTYPE_UINTN     :
    *this = *(UINT*)hv.sqldata;
    break;
  case DBTYPE_LONG      :
  case DBTYPE_LONGN     :
    *this = *(long*)hv.sqldata;
    break;
  case DBTYPE_ULONG     :
  case DBTYPE_ULONGN    :
    *this = *(ULONG*)hv.sqldata;
    break;
  case DBTYPE_FLOAT     :
  case DBTYPE_FLOATN    :
    *this = *(float*)hv.sqldata;
    break;
  case DBTYPE_DOUBLE    :
  case DBTYPE_DOUBLEN   :
    *this = *(double*)hv.sqldata;
    break;
  case DBTYPE_CSTRING   :
  case DBTYPE_CSTRINGN  :
    *this = (char*)hv.sqldata;
    break;
  case DBTYPE_WSTRING   :
  case DBTYPE_WSTRINGN  :
    *this = (wchar_t*)hv.sqldata;
    break;
  case DBTYPE_VARCHAR   :
  case DBTYPE_VARCHARN  :
    { varchar vch(hv.sqllen, hv.sqldata);
      *this = vch;
      break;
    }
  case DBTYPE_DATE      :
  case DBTYPE_DATEN     :
    *this = *(Date*)hv.sqldata;
    break;
  case DBTYPE_TIME      :
  case DBTYPE_TIMEN     :
    *this = *(Time*)hv.sqldata;
    break;
  case DBTYPE_TIMESTAMP :
  case DBTYPE_TIMESTAMPN:
    *this = *(Timestamp*)hv.sqldata;
    break;
  default:
    throwSqlError(SQL_FATAL_ERROR,_T("Unknown type in gethostvar() type=%d"),hv.getType());
  }
  return *this;
}

#define TOSTRING(type,form) { type v; get(v); return format(form,v); }

String TupleField::toString() const {
  if(!isDefined()) {
    return _T("null");
  }
  switch(m_type) {
  case DBTYPE_CHAR      :
  case DBTYPE_CHARN     :
    TOSTRING(char,_T("%d"));
  case DBTYPE_UCHAR     :
  case DBTYPE_UCHARN    :
    TOSTRING(UCHAR,_T("%d"));
  case DBTYPE_SHORT     :
  case DBTYPE_SHORTN    :
    TOSTRING(short,_T("%hd"));
  case DBTYPE_USHORT    :
  case DBTYPE_USHORTN   :
    TOSTRING(USHORT,_T("%hu"));
  case DBTYPE_INT       :
  case DBTYPE_INTN      :
    TOSTRING(int,_T("%d"));
  case DBTYPE_UINT      :
  case DBTYPE_UINTN     :
    TOSTRING(UINT,_T("%u"));
  case DBTYPE_LONG      :
  case DBTYPE_LONGN     :
    TOSTRING(long,_T("%ld"));
  case DBTYPE_ULONG     :
  case DBTYPE_ULONGN    :
    TOSTRING(ULONG,_T("%lu"));
  case DBTYPE_INT64     :
  case DBTYPE_INT64N    :
    TOSTRING(INT64,_T("%I64d"));
  case DBTYPE_UINT64    :
  case DBTYPE_UINT64N   :
    TOSTRING(UINT64,_T("%I64u"));
  case DBTYPE_FLOAT     :
  case DBTYPE_FLOATN    :
    TOSTRING(float,_T("%.6g"));
  case DBTYPE_DOUBLE    :
  case DBTYPE_DOUBLEN   :
    TOSTRING(double,_T("%.10lg"));
  case DBTYPE_CSTRING   :
  case DBTYPE_CSTRINGN  :
  case DBTYPE_WSTRING   :
  case DBTYPE_WSTRINGN  :
    { String v;
      get(v);
      return String(_T("\"")) + v + _T("\"");
    }
  case DBTYPE_VARCHAR   :
  case DBTYPE_VARCHARN  :
    { varchar v;
      get(v);
      return String(_T("\"")) + String((TCHAR*)v.data()) + _T("\"");
    }
  case DBTYPE_DATE      :
  case DBTYPE_DATEN     :
    { Date v;
      get(v);
      return v.toString();
    }
  case DBTYPE_TIME      :
  case DBTYPE_TIMEN     :
    { Time v;
      get(v);
      return v.toString(_T("hh:mm:ss:SSS"));
    }
  case DBTYPE_TIMESTAMP :
  case DBTYPE_TIMESTAMPN:
    { Timestamp v;
      get(v);
      return v.toString(_T("dd.MM.yyyy hh:mm:ss:SSS"));
    }
  default:
    throwSqlError(SQL_FATAL_ERROR,_T("Unknown type in TupleField::dump() type=%d"),m_type);
    return "unknown";
  }
}

void TupleField::dump(FILE *f) const {
  _ftprintf(f,_T("%s"),toString().cstr());
}
