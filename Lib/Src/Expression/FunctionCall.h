#pragma once

#include <Math/MathLib.h>

namespace Expr {

typedef void (*BuiltInFunction)();
typedef Real (*BuiltInFunctionRef1)(const Real &x);
typedef Real (*BuiltInFunctionRef2)(const Real &x, const Real &y);
typedef Real (*BuiltInFunction1)(Real x);
typedef Real (*BuiltInFunction2)(Real x, Real y);

class FunctionCall {
  static String &removeColons(String &name) {
    return name.replace(_T(':'),EMPTYSTRING);
  }
  static String makeSignatureString(const String &name, const TCHAR *param) {
    String tmp(name);
    return format(_T("%s%s"),removeColons(tmp).cstr(),param);
  }
public:
  const BuiltInFunction m_fp;
  const String          m_signature;
  inline FunctionCall() : m_fp(nullptr), m_signature(EMPTYSTRING) {
  }
  inline FunctionCall(const BuiltInFunction fp, const String &name, const TCHAR *paramStr)
    : m_fp(fp)
    , m_signature(makeSignatureString(name,paramStr))
  {
  }
  inline FunctionCall(BuiltInFunction1 fp, const String &name)
    : m_fp((BuiltInFunction)fp)
    , m_signature(makeSignatureString(name,_T("(real x)")))
  {
  }
  inline FunctionCall(BuiltInFunction2 fp, const String &name)
    : m_fp((BuiltInFunction)fp)
    , m_signature(makeSignatureString(name,_T("(real x, real y)")))
  {
  }
  inline FunctionCall(BuiltInFunctionRef1 fp, const String &name)
    : m_fp((BuiltInFunction)fp)
    , m_signature(makeSignatureString(name,_T("(real &x)")))
  {
  }
  inline FunctionCall(BuiltInFunctionRef2 fp, const String &name)
    : m_fp((BuiltInFunction)fp)
    , m_signature(makeSignatureString(name,_T("(real &x, real &y)")))
  {
  }
  String toString() const;
};

class ValueAddressCalculation {
  const CompactRealArray           &m_valueTable;         // Reference to ParserTree::m_valueTable
  // Offset in bytes, of esi/rsi from m_valueTable[0], when code is executing. 0 <= m_esiOffset < 128
  void                             *m_esi;
  char                              m_esiOffset;
  void setValueCount(size_t valueCount);
public:
  ValueAddressCalculation(const CompactRealArray &valueTable) : m_valueTable(valueTable) {
    setValueCount(valueTable.size());
  }
  inline UINT getValueCount() const {
    return (UINT)m_valueTable.size();
  }
  inline int getESIOffset(UINT valueIndex) const {
    if(valueIndex >= getValueCount()) {
      throwInvalidArgumentException(__TFUNCTION__, _T("valueIndex=%u. #values=%u"), valueIndex, getValueCount());
    }
    return (int)valueIndex * sizeof(Real) - m_esiOffset;
  }
  inline char getESITableOffset() const {
    return m_esiOffset;
  }
  void *getESIValue() const {
    return m_esi;
  }
  inline UINT esiOffsetToIndex(int offset) const {
    return (offset + m_esiOffset) / sizeof(Real);
  }
  inline size_t realRefToIndex(const Real *p) const {
    return p - m_valueTable.getBuffer();
  }
};

}; // namespace Expr
