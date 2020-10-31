#pragma once

#include <TreeMap.h>
#include <LRParser.h>
#include "TemplateWriter.h"

class ByteCount {
private:
  UINT m_countx86;
  UINT m_countx64;

  ByteCount getAlignedSize() const {
    const int restx86 = m_countx86%4, restx64 = m_countx64%8;
    return ByteCount(restx86 ? (m_countx86 + (4-restx86)) : m_countx86
                    ,restx64 ? (m_countx64 + (8-restx64)) : m_countx64);
  }
public:
  inline ByteCount() : m_countx86(0), m_countx64(0) {
  }
  inline ByteCount(UINT countx86, UINT countx64) : m_countx86(countx86), m_countx64(countx64) {
  }
  inline void clear() {
    m_countx86 = m_countx64 = 0;
  }
  inline UINT getByteCount(Platform platform) const {
    return (platform == PLATFORM_X86) ? m_countx86 : m_countx64;
  }
  inline bool isEmpty() const {
    return (m_countx86 == 0) && (m_countx64 == 0);
  }
  inline ByteCount operator+(const ByteCount &rhs) const {
    return ByteCount(m_countx86+rhs.m_countx86, m_countx64+rhs.m_countx64);
  }
  inline ByteCount &operator+=(const ByteCount &rhs) {
    m_countx86 += rhs.m_countx86;
    m_countx64 += rhs.m_countx64;
    return *this;
  }

  friend inline ByteCount operator*(UINT n, const ByteCount &c) {
    return ByteCount(n*c.m_countx86,n*c.m_countx64);
  }
  friend inline ByteCount operator*(const ByteCount &c, UINT n) {
    return ByteCount(n*c.m_countx86,n*c.m_countx64);
  }

  static inline ByteCount wordAlignedSize(const ByteCount &c, UINT n) { // static
    return (n *c).getAlignedSize();
  }

  static inline ByteCount wordAlignedSize(UINT size) {
    return wordAlignedSize(ByteCount(size,size),1);
  }
  static const ByteCount s_pointerSize;

  inline String toString() const {
    return format(_T("%s(x86)/%s(x64) bytes")
                 ,format1000(m_countx86).cstr()
                 ,format1000(m_countx64).cstr());
  }
};

typedef enum {
  TYPE_CHAR
 ,TYPE_UCHAR
 ,TYPE_SHORT
 ,TYPE_USHORT
 ,TYPE_INT
 ,TYPE_UINT
} IntegerType;

IntegerType  findUintType(     UINT        maxValue);
const TCHAR *getTypeName(      IntegerType type    );
UINT         getTypeSize(      IntegerType type    );
ByteArray    bitSetToByteArray(const BitSet &set);
void         newLine(MarginFile &output, String &comment = String(_T("")), int minColumn=0);

typedef CompactShortArray  RawActionArray;
typedef CompactUshortArray SuccesorArray;

class GrammarCoder {
private:
  const String     m_templateName;
  const Grammar   &m_grammar;
  const String     m_implOutputDir;
  const String     m_headerOutputDir;
  const CodeFlags  m_flags;
  const String     m_sourceName;
  const String     m_grammarName;
  const String     m_parserClassName;
  const String     m_tablesClassName;
  const String     m_docFileName;
  const String     m_nameSpace;
  ByteCount        m_tablesByteCount;
public:
  GrammarCoder(const String &templateName
             , Grammar      &grammar
             , const String &outputDirImpl
             , const String &outputDirHeaders
             , const String &nameSpace
             , CodeFlags     flags);
  ~GrammarCoder();
  void generateParser();
  void generateDocFile();
  void generateDocFile(MarginFile &output);

  const Grammar &getGrammar() {
    return m_grammar;
  }

  const String &getSourceName() const {
    return m_sourceName;
  }

  const String &getParserClassName() const {
    return m_parserClassName;
  }

  const String &getTablesClassName() const {
    return m_tablesClassName;
  }

  const CodeFlags &getFlags() const {
    return m_flags;
  }

  void setByteCount(const ByteCount &count) {
    m_tablesByteCount = count;
  }

  const ByteCount &getByteCount() const {
    return m_tablesByteCount;
  }
};

class GrammarTables : public ParserTables {
private:
  UINT                         m_terminalCount;
  CompactUshortArray           m_productionLength, m_left;
  StringArray                  m_symbolName;
  Array<CompactIntArray>       m_rightSide;
  Array<ActionArray>           m_stateActions;
  Array<ActionArray>           m_stateSucc;
  const String                 m_parserClassName;
  const String                 m_tablesClassName;
  mutable BitSet               m_compressibleStateSet;
  mutable ByteCount            m_countTableBytes;
  mutable IntegerType          m_terminalType, m_NTIndexType, m_symbolType, m_actionType, m_stateType;

  void                findTemplateTypes() const;

  UINT                getMaxInputCount() const;
  void                initCompressibleStateSet();
  bool                calcIsCompressibleState(UINT state) const;
  inline bool         isCompressibleState(    UINT state) const {
    return m_compressibleStateSet.contains(state);
  }
  inline bool         isOneItemState(UINT state) const {
    return m_stateActions[state].size() == 1;
  }

  void      printCpp( MarginFile &output, bool useTableCompression) const;
  void      printJava(MarginFile &output, bool useTableCompression) const;
  ByteCount printCompressedActionMatrixCpp(  MarginFile &output) const;                                              // return size in bytes
  ByteCount printSuccessorMatrixCpp(         MarginFile &output) const;                                              // return size in bytes
  ByteCount printProductionLengthTableCpp(   MarginFile &output) const;                                              // return size in bytes
  ByteCount printLeftSideTableCpp(           MarginFile &output) const;                                              // return size in bytes
  ByteCount printRightSideTableCpp(          MarginFile &output) const;                                              // return size in bytes
  ByteCount printSymbolNameTableCpp(         MarginFile &output) const;                                              // return size in bytes
  ByteCount printByteArray(                  MarginFile &output, const String &name, const ByteArray &ba, UINT bytesPerLine = 20, const StringArray *linePrefix = nullptr) const;
public:
  GrammarTables(const Grammar &g, const String &tableClassName, const String &parserClassName);
  int  getAction(   UINT state, UINT input)      const override;
  UINT getSuccessor(UINT state, UINT nt   )      const override;

  UINT getProductionLength(UINT prod)            const override {
    return m_productionLength[prod];
  }

  UINT getLeftSymbol(UINT prod)                  const override {
    return m_left[prod];
  }

  const TCHAR *getSymbolName(UINT symbol)        const override {
    return m_symbolName[symbol].cstr();
  }

  void getRightSide(UINT prod, UINT *dst)        const override;

  UINT getTerminalCount()                        const override {
    return m_terminalCount;
  }

  UINT getSymbolCount()                          const override {
    return (UINT)m_symbolName.size();
  }

  UINT getProductionCount()                      const override {
    return (UINT)m_productionLength.size();
  }

  UINT getStateCount()                           const override {
    return (UINT)m_stateActions.size();
  }

  UINT getLegalInputCount(UINT state)            const override {
    return (UINT)m_stateActions[state].size();
  }

  void getLegalInputs(UINT state, UINT *symbols) const override;
  UINT getTableByteCount(   Platform platform  ) const override;

  inline IntegerType  getTerminalType() const { return m_terminalType; }
  inline IntegerType  getActionType()   const { return m_actionType;   }

  BitSet              getLookaheadSet(  UINT state) const;
  RawActionArray      getRawActionArray(UINT state) const;
  BitSet              getNTOffsetSet(   UINT state) const;
  SuccesorArray       getSuccessorArray(UINT state) const;

  const Array<ActionArray> &getStateActions() const {
    return m_stateActions;
  }
  ByteCount getTotalSizeInBytes() const;
  void print(MarginFile &output, Language language, bool useTableCompression) const;
};

inline int stringCmp(const String &s1, const String &s2) {
  return _tcscmp(s1.cstr(), s2.cstr());
}
