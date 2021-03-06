#pragma once

#include "KeywordHandler.h"
#include "ByteCount.h"

class MarginFile;
class BitSetInterval;
class Grammar;
class TemlateWriter;

class GrammarCode {
private:
  Grammar       &m_grammar;
  const String   m_sourceName;
  const String   m_grammarName;
  const String   m_parserClassName;
  const String   m_tablesClassName;
  const String   m_docFileName;
  ByteCount      m_tablesByteCount;

public:
  GrammarCode(Grammar &grammar);
  ~GrammarCode() {
  }
  void generateParser();
  void generateDocFile()                   const;
  void generateDocFile(MarginFile &output) const;
  // try various combinations of compression-parameters and print tables memory usage to stdout. then exit(0)
  void listCompressionCombination();

  const Grammar   &getGrammar()            const { return m_grammar;         }
  const String    &getSourceName()         const { return m_sourceName;      }
  const String    &getParserClassName()    const { return m_parserClassName; }
  const String    &getTablesClassName()    const { return m_tablesClassName; }
  const ByteCount &getByteCount()          const { return m_tablesByteCount; }
  void setByteCount(const ByteCount &count) {
    m_tablesByteCount = count;
  }
};

ByteArray    bitSetToByteArray(               const BitSet &bitSet, UINT capacity = 0);
// Convert bitSet to ByteArray. if(capacity = 0), then the bitSet's capacity is used. if capacity is specified
// it is checked, that bitSet doesn't contain any 1-bits at positions outside range [interval.from..interval.to-1],
// and then only the bytes needed is added to byteArray (using function getSizeofBitSet(capacity) to calculate the size)
// if this check fails, an exception is thrown
ByteArray    bitSetToByteArray(               const BitSet &bitSet, const BitSetInterval &interval);
ByteArray    dynamicBitSetToByteArray(        const BitSet &bitSet);
// size = #elements in array
void         outputBeginArrayDefinition(      MarginFile &output, const TCHAR *tableName, IntegerType elementType, UINT size);
ByteCount    outputEndArrayDefinition(        MarginFile &output,                         IntegerType elementType, UINT size, bool addNewLine=false);
// capacity = capacity of each bitSet, count = #bitsets in table, elements always unsigned char
UINT         outputBeginBitSetTableDefinition(MarginFile &output, const TCHAR *tableName, const BitSetInterval &interval, UINT count);
ByteCount    outputEndBitSetTableDefinition(  MarginFile &output, UINT size, bool addNewLine=false);
void         newLine(MarginFile &output, String &comment = String(_T("")), int minColumn=0);

static constexpr UINT commentWidth = 54;

class ActionsWriter : public KeywordHandler {
private:
  const GrammarCode &m_coder;
public:
  ActionsWriter(const GrammarCode &coder) : m_coder(coder) {
  }
  void handleKeyword(TemplateWriter &writer, String &line) const override;
};

class TablesWriter : public KeywordHandler {
private:
  GrammarCode &m_coder;
public:
  TablesWriter(GrammarCode &coder) : m_coder(coder) {
  }
  void handleKeyword(TemplateWriter &writer, String &line) const override;
};

class SymbolsWriter : public KeywordHandler {
private:
  const GrammarCode &m_coder;
  const bool         m_writeTerminals;
  void writeCppSymbols( TemplateWriter &writer) const;
  void writeJavaSymbols(TemplateWriter &writer) const;
public:
  SymbolsWriter(const GrammarCode &coder, bool writeTerminals)
    : m_coder(coder)
    , m_writeTerminals(writeTerminals)
  {
  }
  void handleKeyword(TemplateWriter &writer, String &line) const override;
};

class DocFileWriter : public KeywordHandler {
private:
  const GrammarCode &m_coder;
public:
  DocFileWriter(const GrammarCode &coder) : m_coder(coder) {}
  void handleKeyword(TemplateWriter &writer, String &line) const override;
};
