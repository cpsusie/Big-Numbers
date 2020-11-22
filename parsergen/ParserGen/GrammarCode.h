#pragma once

#include "KeywordHandler.h"

class TemlateWriter;

// Must match elements in BitSetParam::s_elementName
typedef enum {
  SYMBOL_BITSET
 ,TERM_BITSET
 ,NTINDEX_BITSET
 ,PRODUCTION_BITSET
 ,STATE_BITSET
} BitSetType;

class BitSetParam {
private:
  static const TCHAR *s_elementName[][2];
  const BitSetType m_type;
  const UINT       m_capacity;
public:
  BitSetParam(BitSetType type, UINT capacity)
    : m_type(       type       )
    , m_capacity(   capacity   )
  {
  }
  inline BitSetType getType() const {
    return m_type;
  }
  inline UINT getCapacity() const {
    return m_capacity;
  }
  static inline const TCHAR *getElementName(BitSetType type, bool plur) {
    return s_elementName[type][plur?1:0];
  }
  inline const TCHAR *getElementName(bool plur) const {
    return getElementName(getType(), plur);
  }
};

class UsedByBitSet : public BitSet {
private:
  const BitSetType m_type;
public:
  UsedByBitSet(const BitSetParam &param) : BitSet(param.getCapacity()), m_type(param.getType()) {
  }
  String toString() const;
};

class GrammarCode {
private:
  const Grammar   &m_grammar;
  const String     m_sourceName;
  const String     m_grammarName;
  const String     m_parserClassName;
  const String     m_tablesClassName;
  const String     m_docFileName;
  ByteCount        m_tablesByteCount;

public:
  GrammarCode(const Grammar &grammar);
  ~GrammarCode() {
  }
  void generateParser();
  void generateDocFile()                   const;
  void generateDocFile(MarginFile &output) const;

  const Grammar   &getGrammar()            const { return m_grammar;         }
  const String    &getSourceName()         const { return m_sourceName;      }
  const String    &getParserClassName()    const { return m_parserClassName; }
  const String    &getTablesClassName()    const { return m_tablesClassName; }
  const ByteCount &getByteCount()          const { return m_tablesByteCount; }
  BitSetParam      getBitSetParam(BitSetType type) const;
  void setByteCount(const ByteCount &count) {
    m_tablesByteCount = count;
  }
};

ByteArray    bitSetToByteArray(   const BitSet    &set);
void         outputBeginArrayDefinition(MarginFile &output, const TCHAR *tableName, IntegerType elementType, UINT size); // size = #elements in array
ByteCount    outputEndArrayDefinition(  MarginFile &output,                         IntegerType elementType, UINT size, bool addNewLine=false);
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
