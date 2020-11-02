#pragma once

#include "ByteCount.h"
#include "CodeFlags.h"
#include "KeywordHandler.h"

class TemlateWriter;

class GrammarCode {
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
  GrammarCode(const String  &templateName
             ,const Grammar &grammar
             ,const String  &outputDirImpl
             ,const String  &outputDirHeaders
             ,const String  &nameSpace
             ,CodeFlags      flags);
  ~GrammarCode() {
  }
  void generateParser();
  void generateDocFile()                   const;
  void generateDocFile(MarginFile &output) const;

  const Grammar   &getGrammar()            const { return m_grammar;         }
  const String    &getSourceName()         const { return m_sourceName;      }
  const String    &getParserClassName()    const { return m_parserClassName; }
  const String    &getTablesClassName()    const { return m_tablesClassName; }
  const CodeFlags &getFlags()              const { return m_flags;           }
  const ByteCount &getByteCount()          const { return m_tablesByteCount; }

  void setByteCount(const ByteCount &count) {
    m_tablesByteCount = count;
  }
};

inline int stringCmp(const String &s1, const String &s2) {
  return _tcscmp(s1.cstr(), s2.cstr());
}

ByteArray    symbolSetToByteArray(const SymbolSet &set);
void         outputBeginArrayDefinition(MarginFile &output, const TCHAR *tableName, IntegerType elementType, UINT size); // size = #elements in array
ByteCount    outputEndArrayDefinition(  MarginFile &output,                         IntegerType elementType, UINT size, bool addNewLine=false);
void         newLine(MarginFile &output, String &comment = String(_T("")), int minColumn=0);

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
  bool m_terminals;
  void writeCppSymbols( TemplateWriter &writer) const;
  void writeJavaSymbols(TemplateWriter &writer) const;
public:
  SymbolsWriter(const GrammarCode &coder, bool terminals) : m_coder(coder) {
    m_terminals = terminals;
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
