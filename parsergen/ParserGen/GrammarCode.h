#pragma once

#include <TreeMap.h>
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
  static ByteCount s_pointerSize;
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

class GrammarTables : public ParserTables {
private:
  unsigned int                 m_terminalCount;
  CompactArray<unsigned short> m_productionLength, m_left;
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

  bool tableTypeIsShort() const {
    return getStateCount() >= 128 || getSymbolCount() >= 128 || getProductionCount() >= 128 || getMaxInputCount() >= 128;
  }
  int getTableTypeSize() const {
    return tableTypeIsShort() ? sizeof(short) : sizeof(char);
  }
  static ByteCount wordAlignedSize(int size);
  static ByteCount wordAlignedSize(const ByteCount &c, UINT n); // for arrays with n elements, each of size s
  int getMaxInputCount() const;

  void initCompressibleStateSet();
  bool calcIsCompressibleState(unsigned int state) const;
  inline bool isCompressibleState(   unsigned int state) const {
    return m_compressibleStateSet.contains(state);
  }
  inline bool isOneItemState(unsigned int state) const {
    return m_stateActions[state].size() == 1;
  }

  static ByteArray bitSetToByteArray(const BitSet &set);
  BitSet getLookaheadSet(unsigned int state) const;
  ByteArray getCompressedStateSetAsByteArray() const {
    return bitSetToByteArray(m_compressibleStateSet);
  }

  void printCpp( MarginFile &output, bool useTableCompression) const;
  void printJava(MarginFile &output, bool useTableCompression) const;
  ByteCount printActionMatrixCpp(           MarginFile &output) const;                      // return size in bytes
  ByteCount printCompressedActionsCpp(      MarginFile &output) const;
  ByteCount printUncompressedActionsCpp(    MarginFile &output) const;                      // return size in bytes
  ByteCount printUncompressedActionArrayCpp(MarginFile &output, unsigned int state) const;  // return size in bytes
  ByteCount printSuccessorMatrixCpp(        MarginFile &output) const;                      // return size in bytes
  ByteCount printSuccessorArrayCpp(         MarginFile &output, unsigned int state) const;  // return size in bytes
  ByteCount printProductionLengthTableCpp(  MarginFile &output) const;                      // return size in bytes
  ByteCount printLeftSideTableCpp(          MarginFile &output) const;                      // return size in bytes
  ByteCount printRightSideTableCpp(         MarginFile &output) const;                      // return size in bytes
  ByteCount printSymbolNameTableCpp(        MarginFile &output) const;                      // return size in bytes
  ByteCount printByteArray(                 MarginFile &output, const String &name, const ByteArray &ba, UINT bytesPerLine = 20, const StringArray *linePrefix = NULL) const;
public:
  GrammarTables(const Grammar &g, const String &tableClassName, const String &parserClassName);
  int getAction(   unsigned int state, int input) const;
  int getSuccessor(unsigned int state, int nt   ) const;

  unsigned int getProductionLength(unsigned int prod) const {
    return m_productionLength[prod];
  }

  unsigned int getLeftSymbol(unsigned int prod) const {
    return m_left[prod];
  }

  const TCHAR *getSymbolName(unsigned int symbol) const {
    return m_symbolName[symbol].cstr();
  }

  void getRightSide(unsigned int prod, unsigned int *dst) const;

  unsigned int getTerminalCount() const {
    return m_terminalCount;
  }

  unsigned int getSymbolCount() const {
    return (int)m_symbolName.size();
  }

  unsigned int getProductionCount() const {
    return (int)m_productionLength.size();
  }

  unsigned int getStateCount() const {
    return (int)m_stateActions.size();
  }

  unsigned int getLegalInputCount(unsigned int state) const {
    return (int)m_stateActions[state].size();
  }

  void getLegalInputs(unsigned int state, unsigned int *symbols) const;
  ByteCount getTotalSizeInBytes(bool useTableCompression) const;
  void print(MarginFile &output, Language language, bool useTableCompression) const;
};
