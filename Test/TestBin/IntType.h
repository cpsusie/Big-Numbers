#pragma once

#include <MathUtil.h>
#include <Math/int128.h>

typedef enum {
  IT_I32
 ,IT_U32
 ,IT_I64
 ,IT_U64
 ,IT_I128
 ,IT_U128
} IntegerType;

String toString(IntegerType type);

typedef int      I32;
typedef UINT     U32;
typedef INT64    I64;
typedef UINT64   U64;
typedef _int128  I128;
typedef _uint128 U128;

class IntType {
private:
  IntegerType m_type;
  union {
    I32  m_i32;
    U32  m_u32;
    I64  m_i64;
    U64  m_u64;
    I128 m_i128;
    U128 m_u128;
    U128 m_data; // to be used for reset, compare...
  };
  void checkType(const TCHAR *method, IntegerType expected) const;
public:
  IntType() : m_type(IT_I32), m_data(0) {}
  IntType(I32  v) : m_type(IT_I32  ), m_data(0) { m_i32  = v; }
  IntType(U32  v) : m_type(IT_U32  ), m_data(0) { m_u32  = v; }
  IntType(I64  v) : m_type(IT_I64  ), m_data(0) { m_i64  = v; }
  IntType(U64  v) : m_type(IT_U64  ), m_data(0) { m_u64  = v; }
  IntType(I128 v) : m_type(IT_I128 ), m_data(0) { m_i128 = v; }
  IntType(U128 v) : m_type(IT_U128 ), m_data(0) { m_u128 = v; }
  IntType(IntegerType type, const TCHAR *s, int radix);

  I32   getI32()  const;
  U32   getU32()  const;
  I64   getI64()  const;
  U64   getU64()  const;
  I128  getI128() const;
  U128  getU128() const;
  inline IntegerType getType() const {
    return m_type;
  }
  IntType &setType(IntegerType type);

  IntType &setZero();
  IntType &setRnd();
  IntType &setOne();
  IntType &setMin();
  IntType &setMax();

  IntType &operator+=(const IntType &rhs);
  IntType &operator-=(const IntType &rhs);
  IntType &operator*=(const IntType &rhs);
  IntType &operator/=(const IntType &rhs);
  IntType &operator%=(const IntType &rhs);
  IntType &operator&=(const IntType &rhs);
  IntType &operator|=(const IntType &rhs);
  IntType &operator^=(const IntType &rhs);
  inline IntType operator+(const IntType &rhs) const {
    return IntType(*this) += rhs;
  }
  inline IntType operator-() const {
    return IntType(*this).setZero() - *this;
  }
  inline IntType operator-(const IntType &rhs) const {
    return IntType(*this) -= rhs;
  }
  inline IntType operator*(const IntType &rhs) const {
    return IntType(*this) *= rhs;
  }
  inline IntType operator/(const IntType &rhs) const {
    return IntType(*this) /= rhs;
  }
  inline IntType operator%(const IntType &rhs) const {
    return IntType(*this) %= rhs;
  }
  inline IntType operator&(const IntType &rhs) const {
    return IntType(*this) &= rhs;
  }
  inline IntType operator|(const IntType &rhs) const {
    return IntType(*this) |= rhs;
  }
  inline IntType operator^(const IntType &rhs) const {
    return IntType(*this) ^= rhs;
  }
  IntType  operator~() const;

  inline bool operator==(const IntType &rhs) const {
    return (m_type == rhs.m_type) && (m_data == rhs.m_data);
  }
  inline bool operator!=(const IntType &rhs) const {
    return !(*this == rhs);
  }

  static UINT          getBitCount(         IntegerType type);
  static inline UINT   getHexDigitCount(    IntegerType type) {
    const UINT bc = getBitCount(type);
    return bc / 4 + ((bc % 4) ? 1 : 0);
  }
  static inline UINT   getByteCount(         IntegerType type) {
    const UINT bc = getBitCount(type);
    return bc / 8 + ((bc % 8) ? 1 : 0);
  }
  static bool          isSignedType(        IntegerType type);
  static inline UINT   getSignShift(        IntegerType type) {
    assert(isSignedType(type));
    return getBitCount(type) - 1;
  }
  static int           getMaxDigits10(      IntegerType type);

  inline        UINT   getBitCount()          const { return getBitCount(         getType()); }
  inline        UINT   getHexDigitCount()     const { return getHexDigitCount(    getType()); }
  inline        UINT   getByteCount()         const { return getByteCount(        getType()); }
  inline int           getMaxDigits10()       const { return getMaxDigits10(      getType()); }
  bool                 isNegative()           const;

  String toString(int radix) const;
  String toBinString() const;
  String toHexString() const;
  String getDisplayString() const;
  inline String getTypeName() const {
    return ::toString(getType());
  }
};

IntType sqr(     const IntType &i);

std::ostream  &operator<<(std::ostream  &s, const IntType &i);
std::istream  &operator>>(std::istream  &s,       IntType &i);

std::wostream &operator<<(std::wostream &s, const IntType &i);
std::wistream &operator>>(std::wistream &s,       IntType &i);
