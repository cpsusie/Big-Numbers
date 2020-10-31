#pragma once

#include <HashMap.h>

class TemplateWriter;

class CodeFlags {
public:
  bool m_lineDirectives       : 1;
  bool m_generateBreaks       : 1;
  bool m_generateActions      : 1;
  bool m_generateLookahead    : 1;
  bool m_generateNonTerminals : 1;
  bool m_useTableCompression  : 1;
  bool m_dumpStates           : 1;
  bool m_DFAVerbose           : 1;
  bool m_skipIfEqual          : 1;
  bool m_callWizard           : 1;
  bool m_verbose              : 1;
  CodeFlags();
};

class KeywordHandler {
public:
  virtual void handleKeyword(TemplateWriter &writer, String &line) const = 0;
  virtual ~KeywordHandler() {
  }
};

class KeywordTrigger {
private:
  String              m_verboseString;
  KeywordHandler     &m_handler;

public:
  KeywordTrigger(KeywordHandler &handler, const String &verboseString);

  inline const KeywordHandler &getHandler() const {
    return m_handler;
  }

  inline const String &getVerboseString() const {
    return m_verboseString;
  }
};

typedef enum {
  STR_EQ    // '=='
 ,STR_NE    // '!="
} StringRelation;

class TemplateWriter {
private:
  String                         m_templateName;
  String                         m_implOutputDir;
  String                         m_headerOutputDir;
  MarginFile                    *m_output;
  StringHashMap<KeywordTrigger*> m_keywords;
  StringHashMap<String>          m_macroes;
  SourcePositionWithName         m_currentPos;
  const CodeFlags                m_flags;
  String                         m_wantedOutputName;
  bool                           m_outputIsTemp;
  CompactArray<KeywordHandler*>  m_defaultHandlers;
  void           checkChar(     const TCHAR *&s, char expected) const;
  String         parseMacro(    const TCHAR *&s    ) const; // assume *s = '$'
  String         expandMacroes( const String &line ) const;
  void closeOutput();
  inline MarginFile &getOutput() {
    return *m_output;
  }
  // expand macores and handle commands in text. recursive
  void outputText(const String &text);
  // output #line <m_currentPos> if m_linedirective is enabled
  inline void outputLineDirective() {
    if(m_flags.m_lineDirectives) {
      writeLineDirective(m_currentPos.getName(), m_currentPos.getLineNumber());
    }
  }
  void createDefaultHandlers();
  void destroyDefaultHandlers();
public:
  TemplateWriter(const String &templateName, const String &implOutputDir, const String &headerOutputDir, const CodeFlags &flags);
  virtual ~TemplateWriter();
  void addKeywordHandler(const String &keyword, KeywordHandler &handler, const String &verboseString="");
  void addMacro(const String &macro, const String &value);
  void generateOutput();
  void incrLeftMargin(int incr) {
    const int old = getOutput().getLeftMargin();
    getOutput().setLeftMargin(old+incr);
  }
  void decrLeftMargin(int decr) {
    const int old = getOutput().getLeftMargin();
    getOutput().setLeftMargin(old-decr);
  }
  void printf(const TCHAR *format,...);
  void writeSourceText(const SourceText &sourceText);
  void writeLineDirective(const String &sourceName, int lineNumber);
  inline const SourcePositionWithName &getCurrentSourcePos() const {
    return m_currentPos;
  }

  const CodeFlags &getFlags() const {
    return m_flags;
  }
  static String createTempFileName(const String &ext);

  inline const String &getImplOutputDir() const {
    return m_implOutputDir;
  }

  inline const String &getHeaderOutputDir() const {
    return m_headerOutputDir;
  }

  inline const SourcePositionWithName &getPos() const {
    return m_currentPos;
  }

  inline int getLineNumber() const {
    return getPos().getLineNumber();
  }

  void openOutput(const String &fileName);
};

class SourceTextWriter : public KeywordHandler {
private:
  SourceText  m_sourceText;
public:
  SourceTextWriter(const SourceText &sourceText);
  void handleKeyword(TemplateWriter &writer, String &line) const override;
};
