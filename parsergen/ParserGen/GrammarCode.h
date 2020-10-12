#pragma once

#include <TreeMap.h>
#include <LRParser.h>
#include "TemplateWriter.h"

class ByteCount {
private:
  UINT m_countx86;
  UINT m_countx64;
public:
  inline ByteCount() : m_countx86(0), m_countx64(0) {
  }
  inline ByteCount(UINT countx86, UINT countx64) : m_countx86(countx86), m_countx64(countx64) {
  }
  inline void reset() {
    m_countx86 = m_countx64 = 0;
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
  ByteCount getAlignedSize() const {
    const int restx86 = m_countx86%4, restx64 = m_countx64%8;
    return ByteCount(restx86 ? (m_countx86 + (4-restx86)) : m_countx86
                    ,restx64 ? (m_countx64 + (8-restx64)) : m_countx64);
  }
  inline String toString() const {
    return format(_T("%s(x86)/%s(x64) bytes")
                 ,format1000(m_countx86).cstr()
                 ,format1000(m_countx64).cstr());
  }
  inline UINT getByteCount(Platform platform) const {
    return (platform == PLATFORM_X86) ? m_countx86 : m_countx64;
  }
  static const ByteCount s_pointerSize;
};

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

typedef enum {
  INDEXTYPE_BYTE
 ,INDEXTYPE_USHORT
 ,INDEXTYPE_UINT
} IndexType;

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
  mutable ByteCount            m_uncompressedStateBytes;
  mutable ByteCount            m_compressedLASetBytes;
  mutable IndexType            m_succIndexType;

  static const TCHAR *getTableTypeName(bool      isShort );
  static inline UINT  getTableTypeSize(bool      isShort ) {
    return isShort ? sizeof(short) : sizeof(char);
  }
  static IndexType    findIndexType(   UINT      maxValue);
  static const TCHAR *getIndexTypeName(IndexType type    );
  static UINT         getIndexTypeSize(IndexType type    );

  inline bool         isTableTypeShort() const {
    return getStateCount() >= 128 || getSymbolCount() >= 128 || getProductionCount() >= 128 || getMaxInputCount() >= 128;
  }
  inline const TCHAR *getTableTypeName() const {
    return getTableTypeName(isTableTypeShort());
  }
  inline UINT         getTableTypeSize() const {
    return getTableTypeSize(isTableTypeShort());
  }

  inline const TCHAR *getIndexTypeName() const {
    return getIndexTypeName(m_succIndexType);
  }
  inline UINT         getIndexTypeSize() const {
    return getIndexTypeSize(m_succIndexType);
  }

  static ByteCount    wordAlignedSize(UINT size);
  static ByteCount    wordAlignedSize(const ByteCount &c, UINT n); // for arrays with n elements, each of size s
  UINT                getMaxInputCount() const;
  void                initCompressibleStateSet();
  bool                calcIsCompressibleState(UINT state) const;
  inline bool         isCompressibleState(    UINT state) const {
    return m_compressibleStateSet.contains(state);
  }
  inline bool         isOneItemState(UINT state) const {
    return m_stateActions[state].size() == 1;
  }

  static ByteArray    bitSetToByteArray(const BitSet &set);
  BitSet              getLookaheadSet(UINT state) const;

  void printCpp( MarginFile &output, bool useTableCompression) const;
  void printJava(MarginFile &output, bool useTableCompression) const;
  ByteCount printActionMatrixCpp(           MarginFile &output) const;                                              // return size in bytes
  ByteCount printCompressedActionsCpp(      MarginFile &output, StringArray &defines) const;
  ByteCount printUncompressedActionsCpp(    MarginFile &output, StringArray &defines) const;                        // return size in bytes
  void      printUncompressedActionArrayCpp(MarginFile &output, UINT state, UINT startindex) const;                 // return size in bytes
  ByteCount printSuccessorMatrixCpp(        MarginFile &output) const;                                              // return size in bytes
  void      printSuccessorArrayCpp(         MarginFile &output, UINT state, UINT startIndex) const;
  ByteCount printProductionLengthTableCpp(  MarginFile &output) const;                                              // return size in bytes
  ByteCount printLeftSideTableCpp(          MarginFile &output) const;                                              // return size in bytes
  ByteCount printRightSideTableCpp(         MarginFile &output) const;                                              // return size in bytes
  ByteCount printSymbolNameTableCpp(        MarginFile &output) const;                                              // return size in bytes
  ByteCount printByteArray(                 MarginFile &output, const String &name, const ByteArray &ba, UINT bytesPerLine = 20, const StringArray *linePrefix = NULL) const;
public:
  GrammarTables(const Grammar &g, const String &tableClassName, const String &parserClassName);
  int  getAction(   UINT state, UINT input)      const override;
  int  getSuccessor(UINT state, UINT nt   )      const override;

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

  ByteCount getTotalSizeInBytes() const;
  void print(MarginFile &output, Language language, bool useTableCompression) const;
};
