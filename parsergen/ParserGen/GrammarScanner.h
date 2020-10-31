#pragma once

#include "StringCollector.h"

//char        *stripesc(    char *dst, char *src);

#define MAXSTRINGLEN 130
#define MAXNAMELEN   128

typedef enum {
  EOI
 ,BAR
 ,COLON
 ,DOLLAR
 ,DOLLARDOLLAR
 ,LCURL
 ,LEFT
 ,NAME
 ,NONASSOC
 ,NUMBER
 ,PERCENTLCURL
 ,PERCENTPERCENT
 ,PERCENTRCURL
 ,PLUS
 ,PREC
 ,QUEST
 ,SEMI
 ,STAR
 ,RCURL
 ,RIGHT
 ,STRINGLIT
 ,TERM
 ,UNKNOWN
} Token;

class GrammarScanner {
private:
  const String    m_fileName;
  const int       m_tabSize;
  TCHAR           m_lineBuffer[256];
  StringCollector m_collector;
  bool            m_collecting;
  String          m_absoluteFileName;
  SourcePosition  m_sourcePos;
  SourcePosition  m_currentPos;
  bool            m_debug;
  bool            m_ok;
  int             m_length;
  TCHAR          *m_next;
  TCHAR          *m_text;
  FILE           *m_input;
  double          m_number;
  TCHAR           m_string[256];
  void collectChar();
  void nextLine();
  void parseNumber();
  Token parseName();
  void parseString();
  void advance();
public:
  GrammarScanner(const String &fileName, int tabSize);
  virtual ~GrammarScanner();
  String getText();
  inline double getNumber() const {
    return m_number;
  }
  Token next();
  void collectBegin();
  void collectEnd() {
    m_collecting = false;
  }
  void getCollected(SourceText &sourceText);
  inline UINT getLineNumber() const {
    return m_sourcePos.getLineNumber();
  }
  inline SourcePosition getSourcePos() const {
    return m_sourcePos;
  }
  inline const String &getAbsoluteFileName() const {
    return m_absoluteFileName;
  }
  inline int getTabSize() const {
    return m_tabSize;
  }
  void error(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  void error(const SourcePosition &pos, _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  void warning(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  void warning(const SourcePosition &pos, _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  inline bool ok() {
    return m_ok;
  }
};

Token nameOrKeyWord(const TCHAR *name);
