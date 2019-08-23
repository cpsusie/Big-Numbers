#pragma once

#include <MathUtil.h>
#include <Math/Double80.h>

typedef enum {
  FT_FLOAT
 ,FT_DOUBLE
 ,FT_DOUBLE80
} FloatType;

String toString(FloatType type);
String fpclassToString(int fpClass); // fpClass is returnvalue from _fpclass

inline UINT bitCountToHexDigitCount(UINT bitCount) {
  return (bitCount / 4) + ((bitCount % 4) ? 1 : 0);
}

class FloatFields {
private:
  FloatType m_type;
  bool      m_sign;
  UINT      m_expoField;
  UINT64    m_sig;
  void checkType(const TCHAR *method, FloatType expected) const;
public:
  FloatFields() : m_type(FT_FLOAT), m_sign(false), m_expoField(0), m_sig(0) {
  }
  FloatFields(FloatType type, bool sign, int expoField, UINT64 sig) 
    : m_type(type)
    , m_sign(sign)
    , m_expoField(expoField & getExpoMask(type))
    , m_sig(      sig       & getSigMask(type ))
  {
  }
  FloatFields(float    v) : m_type(FT_FLOAT), m_sign(::getSign(v) ? true : false), m_expoField(getExponent(v)), m_sig(getSignificandField(v))
  {
//    float vv = getFloat();
  }
  FloatFields(double   v) : m_type(FT_DOUBLE), m_sign(::getSign(v) ? true : false), m_expoField(getExponent(v)), m_sig(getSignificandField(v))
  {
//    double vv = getDouble();
  }
  FloatFields(const Double80 &v) : m_type(FT_DOUBLE80), m_sign(getSign(v) ? true : false), m_expoField(getExponent(v)), m_sig(getSignificand(v))
  {
//    Double80 vv = getDouble80();
  }
  float    getFloat()    const;
  double   getDouble()   const;
  Double80 getDouble80() const;
  inline FloatType getType() const {
    return m_type;
  }
  FloatFields &setType(FloatType type);

  FloatFields &setZero();
  FloatFields &setTrueMin();
  FloatFields &setMin();
  FloatFields &setEps();
  FloatFields &setRnd();
  FloatFields &setOne();
  FloatFields &setMax();
  FloatFields &setPInf();
  FloatFields &setQNaN();
  FloatFields &setSNaN();

  FloatFields &operator+=(const FloatFields &rhs);
  FloatFields &operator-=(const FloatFields &rhs);
  FloatFields &operator*=(const FloatFields &rhs);
  FloatFields &operator/=(const FloatFields &rhs);
  FloatFields &operator%=(const FloatFields &rhs);
  inline FloatFields operator+(const FloatFields &rhs) const {
    return FloatFields(*this) += rhs;
  }
  inline FloatFields operator-(const FloatFields &rhs) const {
    return FloatFields(*this) -= rhs;
  }
  inline FloatFields operator*(const FloatFields &rhs) const {
    return FloatFields(*this) *= rhs;
  }
  inline FloatFields operator/(const FloatFields &rhs) const {
    return FloatFields(*this) /= rhs;
  }
  inline FloatFields operator%(const FloatFields &rhs) const {
    return FloatFields(*this) %= rhs;
  }

  inline bool operator==(const FloatFields &rhs) const {
    return (m_type      == rhs.m_type     )
        && (m_sign      == rhs.m_sign     )
        && (m_expoField == rhs.m_expoField)
        && (m_sig       == rhs.m_sig      );
  }
  inline bool operator!=(const FloatFields &rhs) const {
    return !(*this == rhs);
  }

  static UINT          getBitCount(         FloatType type);
  static UINT          getExpoBitCount(     FloatType type);
  static inline UINT   getSigBitCount(      FloatType type) {
    return getBitCount(type) - getExpoBitCount(type) - 1; // -1 because of sign-bit
  }
  static inline int    getExpoBias(         FloatType type) {
    return (1 << (getExpoBitCount(type) - 1)) - 1;
  }
  static inline UINT   getExpoHexDigitCount(FloatType type) {
    return bitCountToHexDigitCount(getExpoBitCount(type));
  }
  static inline UINT   getSigHexDigitCount( FloatType type) {
    return bitCountToHexDigitCount(getSigBitCount(type));
  }
  static inline USHORT getExpoMask(         FloatType type) {
    return (1 << getExpoBitCount(type)) - 1;
  }
  static inline UINT64 getSigMask(          FloatType type) {
    return (type == FT_DOUBLE80) ? -1 : (1ui64 << getSigBitCount(type)) - 1;
  }
  static inline UINT   getMaxExpoValue(     FloatType type) {
    return getExpoMask(type);
  }
  static inline UINT64 getMaxSigValue(      FloatType type) {
    return getSigMask(type);
  }
  static inline int    expoFieldToExpo2(    FloatType type, UINT expoField) {
    return (int)expoField - getExpoBias(type);
  }
  static inline UINT   expo2ToExpoField(    FloatType type, int expo2) {
    return (expo2 + getExpoBias(type)) & getExpoMask(type);
  }
  static inline bool   isNormalExpoField(    FloatType type, UINT expoField) {
    const UINT mask = getExpoMask(type);
    const UINT tmp  = expoField & mask;
    return tmp && (tmp < mask);
  }

  static inline UINT   getExpoShift(        FloatType type) {
    return getSigBitCount(type);
  }
  static inline UINT   getSignShift(        FloatType type) {
    return getBitCount(type) - 1;
  }
  static int           getMaxDigits10(      FloatType type);

  inline        bool   getSignBit()           const { return m_sign;                          }
  inline        UINT   getExpoField()         const { return m_expoField;                     }
  inline        UINT64 getSig()               const { return m_sig;                           }
  inline        UINT   getBitCount()          const { return getBitCount(         getType()); }
  inline        UINT   getExpoBitCount()      const { return getExpoBitCount(     getType()); }
  inline        UINT   getSigBitCount()       const { return getSigBitCount(      getType()); }
  inline        int    getExpoBias()          const { return getExpoBias(         getType()); }
  inline        int    getExpo2()             const { return expoFieldToExpo2(    getType(), getExpoField()); }
  inline        UINT   getExpoHexDigitCount() const { return getExpoHexDigitCount(getType()); }
  inline        UINT   getSigHexDigitCount()  const { return getSigHexDigitCount( getType()); }
  inline        USHORT getExpoMask()          const { return getExpoMask(         getType()); }
  inline        UINT64 getSigMask()           const { return getSigMask(          getType()); }
  inline        UINT   getMaxExpoValue()      const { return getMaxExpoValue(     getType()); }
  inline        UINT64 getMaxSigValue()       const { return getMaxSigValue(      getType()); }
  inline        UINT   getExpoShift()         const { return getExpoShift(        getType()); }
  inline int           getMaxDigits10()       const { return getMaxDigits10(      getType()); }
  int                  getFpClass()           const;

  String toBinString() const;
  String toHexString() const;
  inline String getTypeName() const {
    return toString(getType());
  }
  inline String getFpclassName() const {
    return fpclassToString(getFpClass());
  }
};

FloatFields reciproc(const FloatFields &ff);
FloatFields sqr(     const FloatFields &ff);
FloatFields sqrt(    const FloatFields &ff);
