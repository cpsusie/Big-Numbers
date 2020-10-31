#pragma once

#include "TemplateWriter.h"
#include "ByteCount.h"

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

inline int stringCmp(const String &s1, const String &s2) {
  return _tcscmp(s1.cstr(), s2.cstr());
}

ByteArray    bitSetToByteArray(const BitSet &set);
void         newLine(MarginFile &output, String &comment = String(_T("")), int minColumn=0);

class ActionsWriter : public KeywordHandler {
private:
  GrammarCoder &m_coder;
public:
  ActionsWriter(GrammarCoder &coder) : m_coder(coder) {}
  void handleKeyword(TemplateWriter &writer, String &line) const override;
};

class TablesWriter : public KeywordHandler {
private:
  GrammarCoder &m_coder;
public:
  TablesWriter(GrammarCoder &coder) : m_coder(coder) {};
  void handleKeyword(TemplateWriter &writer, String &line) const override;
};

class SymbolsWriter : public KeywordHandler {
private:
  GrammarCoder &m_coder;
  bool m_terminals;
  void writeCppSymbols( TemplateWriter &writer) const;
  void writeJavaSymbols(TemplateWriter &writer) const;
public:
  SymbolsWriter(GrammarCoder &coder, bool terminals) : m_coder(coder) { m_terminals = terminals; }
  void handleKeyword(TemplateWriter &writer, String &line) const override;
};

class DocFileWriter : public KeywordHandler {
private:
  GrammarCoder &m_coder;
public:
  DocFileWriter(GrammarCoder &coder) : m_coder(coder) {}
  void handleKeyword(TemplateWriter &writer, String &line) const override;
};
