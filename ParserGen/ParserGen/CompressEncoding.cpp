#include "stdafx.h"
#include "CompressEncoding.h"

#if defined(_DEBUG)
static inline void checkMax15Bits(const TCHAR *method, int v, const TCHAR *varName) {
  if((v & 0xffff8000) != 0) {
    throwException(_T("%s:Value %s=(%#08x) cannot be contained in 15 bits (15,16)"), method, varName, v);
  }
}

static inline void checkCodeBits(const TCHAR *method, UINT v, const TCHAR *varName) {
  if(((v >> 15) & 3) != 0) { // Same code as ParserTables::getActMethodCode
    throwException(_T("%s:Encoded variable %s has value %#08x, with non-zero value in encoding bits"), method, varName, v);
  }
}
#define CHECKMAX15BITS(v) checkMax15Bits(__TFUNCTION__,v,_T(#v))
#define CHECKCODEBITS( v) checkCodeBits( __TFUNCTION__,v,_T(#v))

#else

static inline void checkMax15Bits(int v) {
  if((v & 0xffff8000) != 0) {
    throwException(_T("Value %#08x cannot be contained in 15 bits"), v);
  }
}

static inline void checkCodeBits(UINT v) {
  if(((v >> 15) & 3) != 0) { // Same code as ParserTables::getActMethodCode
    throwException(_T("Encoded value %#08x has non-zero value in encoding bits (15,16)"), v);
  }
}
#define CHECKMAX15BITS(v) checkMax15Bits(v)
#define CHECKCODEBITS( v) checkCodeBits( v)

#endif // _DEBUG

UINT encodeValue(CompressionMethod method, int highEnd, int lowEnd) {
  CHECKMAX15BITS(abs(highEnd));
  CHECKMAX15BITS(abs(lowEnd));
  const UINT v = (highEnd << 17) | lowEnd;
  CHECKCODEBITS(v);
  return (v | encodeCompressMethod(method));
}

#if defined(TEST_ENCODING)
String tencodeMacroValue(CompressionMethod method, int highEnd, int lowEnd, const TCHAR *func, int line, const TCHAR *hstr,const TCHAR *lstr) {
  try {
    return format(_T("0x%08x"), encodeValue(method, highEnd, lowEnd));
  } catch(Exception e) {
    throwInvalidArgumentException(func, _T("line:%d, %s=%d(%#08x), %s=%d(%#08x):%s")
                                      ,line
                                      ,hstr, highEnd, highEnd
                                      ,lstr, lowEnd , lowEnd);
    return EMPTYSTRING;
  }
}

#else
String encodeMacroValue(CompressionMethod method, int highEnd, int lowEnd) {
  return format(_T("0x%08x"), encodeValue(method, highEnd, lowEnd));
}
#endif

static const TCHAR *methodNames[] = {
  _T("BinSearch" )
 ,_T("SplitNode" )
 ,_T("Immediate" )
 ,_T("BitSet"    )
};

const TCHAR *compressMethodToString(CompressionMethod method) {
  assert(method < ARRAYSIZE(methodNames));
  return methodNames[method];
}


TableTypeByteCountMap::TableTypeByteCountMap(const Grammar &g) {
  m_termBitSetCapacity  = g.getTermBitSetCapacity();
  m_shiftStateInterval  = g.getShiftStateBitSetInterval();
  m_succStateInterval   = g.getSuccStateBitSetInterval();
  m_splitNodeCount      = 0;
}

String TableTypeByteCountMap::getTableString(ByteCountTableType type) const {
  const ByteCount *bc = get(type);
  if(bc) {
    return bc->toStringTableForm().cstr();
  } else {
    return spaceString(ByteCount::tableformWidth);
  }
}

TableTypeByteCountMap &TableTypeByteCountMap::add(ByteCountTableType type, const ByteCount &bc) {
  ByteCount *e = get(type);
  if(e) {
    *e += bc;
  } else {
    put(type, bc);
  }
  return *this;
}

TableTypeByteCountMap &TableTypeByteCountMap::operator+=(const TableTypeByteCountMap &rhs) {
  for(auto it = rhs.getIterator(); it.hasNext();) {
    auto &e = it.next();
    add(e.getKey(), e.getValue());
  }
  return *this;
}

ByteCount TableTypeByteCountMap::getSum() const {
  ByteCount sum;
  for(auto it = getIterator(); it.hasNext();) {
    sum += it.next().getValue();
  }
  return sum;
}

AllTemplateTypes::AllTemplateTypes(const Grammar &grammar)
  : m_symbolType( findIntType(0, grammar.getSymbolCount() - 1))
  , m_termType(   findIntType(0, grammar.getTermCount()   - 1))
  , m_ntIndexType(findIntType(0, grammar.getNTermCount()  - 1))
  , m_stateType(  findIntType(0, grammar.getStateCount()  - 1))
  , m_actionType( ((grammar.getStateCount() < 128) && (grammar.getProductionCount() < 128))
                  ? TYPE_CHAR
                  : TYPE_SHORT)
{
}

AllTemplateTypes::AllTemplateTypes(const AbstractParserTables &tables)
  : m_symbolType( findIntType(0, tables.getSymbolCount() - 1))
  , m_termType(   findIntType(0, tables.getTermCount()   - 1))
  , m_ntIndexType(findIntType(0, tables.getNTermCount()  - 1))
  , m_stateType(  findIntType(0, tables.getStateCount()  - 1))
  , m_actionType( ((tables.getStateCount() < 128) && (tables.getProductionCount() < 128))
                  ? TYPE_CHAR
                  : TYPE_SHORT)
{
}
