#pragma once

#include <TreeMap.h>
#include "TemplateWriter.h"

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
  unsigned int     m_tablesByteCount;
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

  const String &getTablesClassName() const {
    return m_tablesClassName;
  }

  const CodeFlags &getFlags() const {
    return m_flags;
  }

  void setByteCount(unsigned int count) {
    m_tablesByteCount = count;
  }

  unsigned int getByteCount() const {
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
  String                       m_tablesClassName;
  mutable BitSet               m_compressibleStateSet;
  mutable unsigned int         m_countTableBytes;
  mutable unsigned int         m_uncompressedStateBytes;
  mutable unsigned int         m_compressedLASetBytes;

  bool tableTypeIsShort() const {
    return getStateCount() >= 128 || getSymbolCount() >= 128 || getProductionCount() >= 128 || getMaxInputCount() >= 128;
  }
  int getTableTypeSize() const {
    return tableTypeIsShort() ? sizeof(short) : sizeof(char);
  }
  static int wordAlignedSize(int size);
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
  int  printActionMatrixCpp(           MarginFile &output) const;                      // return size in bytes
  int  printCompressedActionsCpp(      MarginFile &output) const;
  int  printUncompressedActionsCpp(    MarginFile &output) const;                      // return size in bytes
  int  printUncompressedActionArrayCpp(MarginFile &output, unsigned int state) const;  // return size in bytes
  int  printSuccessorMatrixCpp(        MarginFile &output) const;                      // return size in bytes
  int  printSuccessorArrayCpp(         MarginFile &output, unsigned int state) const;  // return size in bytes
  int  printProductionLengthTableCpp(  MarginFile &output) const;                      // return size in bytes
  int  printLeftSideTableCpp(          MarginFile &output) const;                      // return size in bytes
  int  printRightSideTableCpp(         MarginFile &output) const;                      // return size in bytes
  int  printSymbolNameTableCpp(        MarginFile &output) const;                      // return size in bytes
  int  printByteArray(                 MarginFile &output, const String &name, const ByteArray &ba, UINT bytesPerLine = 20, const StringArray *linePrefix = NULL) const;
public:
  GrammarTables(const Grammar &g, const String &tableClassName);
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
  unsigned int getTotalSizeInBytes(bool useTableCompression) const;
  void print(MarginFile &output, Language language, bool useTableCompression) const;
};
