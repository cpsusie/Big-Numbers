#pragma once

#include <HashMap.h>
#include "CodeFlags.h"
#include "KeywordHandler.h"

class KeywordTrigger {
private:
  const String    m_verboseString;
  KeywordHandler &m_handler;
  inline const String &getVerboseString() const {
    return m_verboseString;
  }
  inline bool hasVerboseString() const {
    return m_verboseString.length() > 0;
  }
public:
  KeywordTrigger(KeywordHandler &handler, const String &verboseString);
  inline const KeywordHandler &getHandler() const {
    return m_handler;
  }
  void verbose() const;
};

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
  void           closeOutput();
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
  void addKeywordHandler(const String &keyword, KeywordHandler &handler, const String &verboseString=EMPTYSTRING);
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
  static       String                  createTempFileName(const String &ext);
  void                                 openOutput(const String &fileName);
  void                                 printf(_In_z_ _Printf_format_string_  TCHAR const * const format,...);
  void                                 writeSourceText(   const SourceText &sourceText);
  void                                 writeLineDirective(const String &sourceName, int lineNumber);

  inline const SourcePositionWithName &getCurrentSourcePos() const { return m_currentPos;             }
  inline const CodeFlags              &getFlags()            const { return m_flags;                  }
  inline const String                 &getImplOutputDir()    const { return m_implOutputDir;          }
  inline const String                 &getHeaderOutputDir()  const { return m_headerOutputDir;        }
  inline const SourcePositionWithName &getPos()              const { return m_currentPos;             }
  inline       int                     getLineNumber()       const { return getPos().getLineNumber(); }
};

class SourceTextWriter : public KeywordHandler {
private:
  SourceText  m_sourceText;
public:
  SourceTextWriter(const SourceText &sourceText);
  void handleKeyword(TemplateWriter &writer, String &line) const override;
};
