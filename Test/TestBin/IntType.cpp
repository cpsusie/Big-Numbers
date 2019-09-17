#include "StdAfx.h"
#include "IntType.h"
#include <Random.h>

using namespace std;

void IntType::checkType(const TCHAR *method, IntegerType expected) const {
  if(getType() != expected) {
    throwException(_T("%s:Wrong IntegerType:%s, expected:%s"), method, ::toString(getType()).cstr(), ::toString(expected).cstr());
  }
}

#define DEFAULT_WRONGTYPE(type)  default: throwInvalidArgumentException(__TFUNCTION__, _T("%s=%d"), _T(#type), type)

I32    IntType::getI32()   const {
  checkType(__TFUNCTION__, IT_I32);
  return m_i32;
}
U32    IntType::getU32()  const {
  checkType(__TFUNCTION__, IT_U32);
  return m_u32;
}
I64    IntType::getI64()   const {
  checkType(__TFUNCTION__, IT_I64);
  return m_i64;
}
U64    IntType::getU64()  const {
  checkType(__TFUNCTION__, IT_U64);
  return m_u64;
}
I128   IntType::getI128()  const {
  checkType(__TFUNCTION__, IT_I128);
  return m_i128;
}
U128  IntType::getU128() const {
  checkType(__TFUNCTION__, IT_U128);
  return m_u128;
}

IntType &IntType::setType(IntegerType type) {
  const IntegerType oldType = getType();
  if(type != oldType) {
    switch(type) {
    case IT_I32   :
      switch(oldType) {
      case IT_I32  : *this = (I32 )getI32();  break;
      case IT_U32  : *this = (I32 )getU32();  break;
      case IT_I64  : *this = (I32 )getI64();  break;
      case IT_U64  : *this = (I32 )getU64();  break;
      case IT_I128 : *this = (I32 )getI128(); break;
      case IT_U128 : *this = (I32 )getU128(); break;
      DEFAULT_WRONGTYPE(oldType);
      }
      break;
    case IT_U32  :
      switch(oldType) {
      case IT_I32  : *this = (U32 )getI32();  break;
      case IT_U32  : *this = (U32 )getU32();  break;
      case IT_I64  : *this = (U32 )getI64();  break;
      case IT_U64  : *this = (U32 )getU64();  break;
      case IT_I128 : *this = (U32 )getI128(); break;
      case IT_U128 : *this = (U32 )getU128(); break;
      DEFAULT_WRONGTYPE(oldType);
      }
      break;
    case IT_I64   :
      switch(oldType) {
      case IT_I32  : *this = (I64 )getI32();  break;
      case IT_U32  : *this = (I64 )getU32();  break;
      case IT_I64  : *this = (I64 )getI64();  break;
      case IT_U64  : *this = (I64 )getU64();  break;
      case IT_I128 : *this = (I64 )getI128(); break;
      case IT_U128 : *this = (I64 )getU128(); break;
      DEFAULT_WRONGTYPE(oldType);
      }
      break;
    case IT_U64  :
      switch(oldType) {
      case IT_I32  : *this = (U64 )getI32();  break;
      case IT_U32  : *this = (U64 )getU32();  break;
      case IT_I64  : *this = (U64 )getI64();  break;
      case IT_U64  : *this = (U64 )getU64();  break;
      case IT_I128 : *this = (U64 )getI128(); break;
      case IT_U128 : *this = (U64 )getU128(); break;
      DEFAULT_WRONGTYPE(oldType);
      }
      break;
    case IT_I128  :
      switch(oldType) {
      case IT_I32  : *this = (I128)getI32();  break;
      case IT_U32  : *this = (I128)getU32();  break;
      case IT_I64  : *this = (I128)getI64();  break;
      case IT_U64  : *this = (I128)getU64();  break;
      case IT_I128 : *this = (I128)getI128(); break;
      case IT_U128 :
        { const _int128 newv = m_u128;
          *this = IntType(newv);
        }
        break;
      DEFAULT_WRONGTYPE(oldType);
      }
      break;
    case IT_U128  :
      switch(oldType) {
      case IT_I32  : *this = (U128)getI32();  break;
      case IT_U32  : *this = (U128)getU32();  break;
      case IT_I64  : *this = (U128)getI64();  break;
      case IT_U64  : *this = (U128)getU64();  break;
      case IT_I128 : *this = (U128)getI128(); break;
      case IT_U128 : *this = (U128)getU128(); break;
      DEFAULT_WRONGTYPE(oldType);
      }
      break;
    DEFAULT_WRONGTYPE(type);
    }
  }
  return *this;
}

IntType &IntType::setZero() {
  switch(getType()) {
  case IT_I32  : m_i32  = 0; break;
  case IT_U32  : m_u32  = 0; break;
  case IT_I64  : m_i64  = 0; break;
  case IT_U64  : m_u64  = 0; break;
  case IT_I128 : m_i128 = 0; break;
  case IT_U128 : m_u128 = 0; break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

#undef min
IntType &IntType::setMin() {
  switch(getType()) {
  case IT_I32  : m_i32  = numeric_limits<I32 >::min(); break;
  case IT_U32  : m_u32  = numeric_limits<U32 >::min(); break;
  case IT_I64  : m_i64  = numeric_limits<I64 >::min(); break;
  case IT_U64  : m_u64  = numeric_limits<U64 >::min(); break;
  case IT_I128 : m_i128 = numeric_limits<I128>::min(); break;
  case IT_U128 : m_u128 = numeric_limits<U128>::min(); break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

#undef max
IntType &IntType::setMax() {
  switch(getType()) {
  case IT_I32  : m_i32  = numeric_limits<I32 >::max(); break;
  case IT_U32  : m_u32  = numeric_limits<U32 >::max(); break;
  case IT_I64  : m_i64  = numeric_limits<I64 >::max(); break;
  case IT_U64  : m_u64  = numeric_limits<U64 >::max(); break;
  case IT_I128 : m_i128 = numeric_limits<I128>::max(); break;
  case IT_U128 : m_u128 = numeric_limits<U128>::max(); break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

IntType &IntType::setRnd() {
  switch(getType()) {
  case IT_I32  : m_i32  = randInt();    break;
  case IT_U32  : m_u32  = randInt();    break;
  case IT_I64  : m_i64  = randInt64();  break;
  case IT_U64  : m_u64  = randInt64();  break;
  case IT_I128 : m_i128 = randInt128(); break;
  case IT_U128 : m_u128 = randInt128(); break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

IntType &IntType::setOne() {
  switch(getType()) {
  case IT_I32  : m_i32  = 1; break;
  case IT_U32  : m_u32  = 1; break;
  case IT_I64  : m_i64  = 1; break;
  case IT_U64  : m_u64  = 1; break;
  case IT_I128 : m_i128 = 1; break;
  case IT_U128 : m_u128 = 1; break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

IntType &IntType::operator+=(const IntType &rhs) {
  checkType(__TFUNCTION__, rhs.getType());
  switch(getType()) {
  case IT_I32  : *this = (I32)( getI32()  + rhs.getI32());  break;
  case IT_U32  : *this = (U32)( getU32()  + rhs.getU32());  break;
  case IT_I64  : *this = (I64)( getI64()  + rhs.getI64());  break;
  case IT_U64  : *this = (U64)( getU64()  + rhs.getU64());  break;
  case IT_I128 : *this = (I128)(getI128() + rhs.getI128()); break;
  case IT_U128 : *this = (U128)(getU128() + rhs.getU128()); break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}
IntType &IntType::operator-=(const IntType &rhs) {
  checkType(__TFUNCTION__, rhs.getType());
  switch(getType()) {
  case IT_I32  : *this = (I32)( getI32()  - rhs.getI32());  break;
  case IT_U32  : *this = (U32)( getU32()  - rhs.getU32());  break;
  case IT_I64  : *this = (I64)( getI64()  - rhs.getI64());  break;
  case IT_U64  : *this = (U64)( getU64()  - rhs.getU64());  break;
  case IT_I128 : *this = (I128)(getI128() - rhs.getI128()); break;
  case IT_U128 : *this = (U128)(getU128() - rhs.getU128()); break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

IntType &IntType::operator*=(const IntType &rhs) {
  checkType(__TFUNCTION__, rhs.getType());
  switch(getType()) {
  case IT_I32  : *this = (I32)( getI32()  * rhs.getI32());  break;
  case IT_U32  : *this = (U32)( getU32()  * rhs.getU32());  break;
  case IT_I64  : *this = (I64)( getI64()  * rhs.getI64());  break;
  case IT_U64  : *this = (U64)( getU64()  * rhs.getU64());  break;
  case IT_I128 : *this = (I128)(getI128() * rhs.getI128()); break;
  case IT_U128 : *this = (U128)(getU128() * rhs.getU128()); break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

IntType &IntType::operator/=(const IntType &rhs) {
  checkType(__TFUNCTION__, rhs.getType());
  switch(getType()) {
  case IT_I32  : *this = (I32)( getI32()  / rhs.getI32());  break;
  case IT_U32  : *this = (U32)( getU32()  / rhs.getU32());  break;
  case IT_I64  : *this = (I64)( getI64()  / rhs.getI64());  break;
  case IT_U64  : *this = (U64)( getU64()  / rhs.getU64());  break;
  case IT_I128 : *this = (I128)(getI128() / rhs.getI128()); break;
  case IT_U128 : *this = (U128)(getU128() / rhs.getU128()); break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

IntType &IntType::operator%=(const IntType &rhs) {
  checkType(__TFUNCTION__, rhs.getType());
  switch(getType()) {
  case IT_I32  : *this = (I32)( getI32()  % rhs.getI32());  break;
  case IT_U32  : *this = (U32)( getU32()  % rhs.getU32());  break;
  case IT_I64  : *this = (I64)( getI64()  % rhs.getI64());  break;
  case IT_U64  : *this = (U64)( getU64()  % rhs.getU64());  break;
  case IT_I128 : *this = (I128)(getI128() % rhs.getI128()); break;
  case IT_U128 : *this = (U128)(getU128() % rhs.getU128()); break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

IntType &IntType::operator&=(const IntType &rhs) {
  checkType(__TFUNCTION__, rhs.getType());
  switch(getType()) {
  case IT_I32  : *this = (I32)( getI32()  & rhs.getI32());  break;
  case IT_U32  : *this = (U32)( getU32()  & rhs.getU32());  break;
  case IT_I64  : *this = (I64)( getI64()  & rhs.getI64());  break;
  case IT_U64  : *this = (U64)( getU64()  & rhs.getU64());  break;
  case IT_I128 : *this = (I128)(getI128() & rhs.getI128()); break;
  case IT_U128 : *this = (U128)(getU128() & rhs.getU128()); break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}
IntType &IntType::operator|=(const IntType &rhs) {
  checkType(__TFUNCTION__, rhs.getType());
  switch(getType()) {
  case IT_I32  : *this = (I32)( getI32()  | rhs.getI32());  break;
  case IT_U32  : *this = (U32)( getU32()  | rhs.getU32());  break;
  case IT_I64  : *this = (I64)( getI64()  | rhs.getI64());  break;
  case IT_U64  : *this = (U64)( getU64()  | rhs.getU64());  break;
  case IT_I128 : *this = (I128)(getI128() | rhs.getI128()); break;
  case IT_U128 : *this = (U128)(getU128() | rhs.getU128()); break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}
IntType &IntType::operator^=(const IntType &rhs) {
  checkType(__TFUNCTION__, rhs.getType());
  switch(getType()) {
  case IT_I32  : *this = (I32)( getI32()  ^ rhs.getI32());  break;
  case IT_U32  : *this = (U32)( getU32()  ^ rhs.getU32());  break;
  case IT_I64  : *this = (I64)( getI64()  ^ rhs.getI64());  break;
  case IT_U64  : *this = (U64)( getU64()  ^ rhs.getU64());  break;
  case IT_I128 : *this = (I128)(getI128() ^ rhs.getI128()); break;
  case IT_U128 : *this = (U128)(getU128() ^ rhs.getU128()); break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}
IntType IntType::operator~() const {
  switch(getType()) {
  case IT_I32  : return IntType((I32) ~getI32() ); 
  case IT_U32  : return IntType((U32) ~getU32() );
  case IT_I64  : return IntType((I64) ~getI64() );
  case IT_U64  : return IntType((U64) ~getU64() );
  case IT_I128 : return IntType((I128)~getI128());
  case IT_U128 : return IntType((U128)~getU128());
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

UINT IntType::getBitCount(IntegerType type) { // static
  switch(type) {
  case IT_I32  :
  case IT_U32  : return 32;
  case IT_I64  :
  case IT_U64  : return 64;
  case IT_I128 :
  case IT_U128 : return 128;
  DEFAULT_WRONGTYPE(type);
  }
  return 0;
}

bool IntType::isSignedType(IntegerType type) { // static
  switch(type) {
  case IT_I32 :
  case IT_I64 :
  case IT_I128: return true;
  case IT_U32 :
  case IT_U64 :
  case IT_U128: return false;
  DEFAULT_WRONGTYPE(type);
  }
  return false;
}

int IntType::getMaxDigits10(IntegerType type) {
  switch(type) {
  case IT_I32  : return numeric_limits<I32 >::max_digits10;
  case IT_U32  : return numeric_limits<U32 >::max_digits10;
  case IT_I64  : return numeric_limits<I64 >::max_digits10;
  case IT_U64  : return numeric_limits<U64 >::max_digits10;
  case IT_I128 : return numeric_limits<I128>::max_digits10;
  case IT_U128 : return numeric_limits<U128>::max_digits10;
  DEFAULT_WRONGTYPE(type);
  }
  return 0;
}

bool IntType::isNegative() const {
  switch(getType()) {
  case IT_I32  : return m_i32  < 0;
  case IT_I64  : return m_i64  < 0;
  case IT_I128 : return m_i128 < 0;
  case IT_U32  : 
  case IT_U64  : 
  case IT_U128 : return false;
  DEFAULT_WRONGTYPE(getType());
  }
  return false;
}

IntType::IntType(IntegerType type, const TCHAR *s, int radix) : m_type(type), m_data(0) {
  if(radix != 10) {
    switch(type) {
    case IT_I32  : m_i32  = _tcstoul(   s, NULL, radix); break;
    case IT_U32  : m_u32  = _tcstoul(   s, NULL, radix); break;
    case IT_I64  : m_i64  = _tcstoui64( s, NULL, radix); break;
    case IT_U64  : m_u64  = _tcstoui64( s, NULL, radix); break;
    case IT_I128 : m_i128 = _tcstoui128(s, NULL, radix); break;
    case IT_U128 : m_u128 = _tcstoui128(s, NULL, radix); break;
    DEFAULT_WRONGTYPE(type);
    }
  } else {
    switch(type) {
    case IT_I32  : m_i32  = _tcstol(    s, NULL, radix); break;
    case IT_U32  : m_u32  = _tcstoul(   s, NULL, radix); break;
    case IT_I64  : m_i64  = _tcstoi64(  s, NULL, radix); break;
    case IT_U64  : m_u64  = _tcstoui64( s, NULL, radix); break;
    case IT_I128 : m_i128 = _tcstoi128( s, NULL, radix); break;
    case IT_U128 : m_u128 = _tcstoui128(s, NULL, radix); break;
    DEFAULT_WRONGTYPE(type);
    }
  }
}

String IntType::toString(int radix) const {
  char str[200];
  switch(getType()) {
  case IT_I32  : _itoa(    m_i32 , str, radix); break;
  case IT_U32  : _ultoa(   m_u32 , str, radix); break;
  case IT_I64  : _i64toa(  m_i64 , str, radix); break;
  case IT_U64  : _ui64toa( m_u64 , str, radix); break;
  case IT_I128 : _i128toa( m_i128, str, radix); break;
  case IT_U128 : _ui128toa(m_u128, str, radix); break;
  DEFAULT_WRONGTYPE(getType());
  }
  return str;
}

String IntType::toBinString() const {
  return dataToBinString(&m_data, getBitCount());
}

String IntType::toHexString() const {
  String s = toString(16);
  int w = getHexDigitCount();
  const intptr_t missing = w - s.length();
  if(missing > 0) {
    s.insert(0, missing, '0');
  }
  return s;
}

String IntType::getDisplayString() const {
  return format(_T("Hex:%-48s  Value:%s"), dataToHexString(&m_data, getByteCount()).cstr(), toString(10).cstr());
}

#define caseStr(v) case v: return _T(#v)

String toString(IntegerType type) {
  switch(type) {
  caseStr(IT_I32 );
  caseStr(IT_U32 );
  caseStr(IT_I64 );
  caseStr(IT_U64 );
  caseStr(IT_I128);
  caseStr(IT_U128);
  default:
    return format(_T("Unknown floating point type:%#X"), type);
  }
}

IntType sqr(const IntType &i) {
  return i * i;
}

ostream &operator<<(ostream &s, const IntType &i) {
  switch(i.getType()) {
  case IT_I32  : return s << i.getI32();
  case IT_U32  : return s << i.getU32();
  case IT_I64  : return s << i.getI64();
  case IT_U64  : return s << i.getU64();
  case IT_I128 : return s << i.getI128();
  case IT_U128 : return s << i.getU128();
  DEFAULT_WRONGTYPE(i.getType());
  }
  return s;
}

wostream &operator<<(wostream &s, const IntType &i) {
  switch(i.getType()) {
  case IT_I32  : return s << i.getI32();
  case IT_U32  : return s << i.getU32();
  case IT_I64  : return s << i.getI64();
  case IT_U64  : return s << i.getU64();
  case IT_I128 : return s << i.getI128();
  case IT_U128 : return s << i.getU128();
  DEFAULT_WRONGTYPE(i.getType());
  }
  return s;
}

istream &operator>>(istream &s, IntType &i) {
  switch(i.getType()) {
  case IT_I32  :
    { I32  v;
      s >> v;
      i =  v;
      return s;
    }
  case IT_U32  :
    { U32  v;
      s >> v;
      i =  v;
      return s;
    }
  case IT_I64  :
    { I64  v;
      s >> v;
      i =  v;
      return s;
    }
  case IT_U64  :
    { U64  v;
      s >> v;
      i =  v;
      return s;
    }
  case IT_I128 :
    { I128 v;
      s >> v;
      i =  v;
      return s;
    }
  case IT_U128 :
    { U128 v;
      s >> v;
      i =  v;
      return s;
    }
  DEFAULT_WRONGTYPE(i.getType());
  }
  return s;
}

wistream &operator>>(wistream &s, IntType &i) {
  switch(i.getType()) {
  case IT_I32  :
    { I32  v;
      s >> v;
      i = v;
      return s;
    }
  case IT_U32  :
    { U32  v;
      s >> v;
      i = v;
      return s;
    }
  case IT_I64  :
    { I64  v;
      s >> v;
      i = v;
      return s;
    }
  case IT_U64  :
    { U64  v;
      s >> v;
      i = v;
      return s;
    }
  case IT_I128 :
    { I128 v;
      s >> v;
      i = v;
      return s;
    }
  case IT_U128 :
    { U128 v;
      s >> v;
      i = v;
      return s;
    }
  DEFAULT_WRONGTYPE(i.getType());
  }
  return s;
}
