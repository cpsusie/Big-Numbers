#pragma once

#include <HashMap.h>
#include <StringCollector.h>

typedef enum {
  EOI                 //  end of input
 ,STRINGLIT
 ,NAME
 ,PERCENTPERCENT
 ,PERCENTLCURL
 ,PERCENTRCURL
 ,EOS                 //  end of String
 ,DOT                 //  .
 ,CIRCUMFLEX          //  ^
 ,DOLLAR              //  $
 ,LB                  //  [
 ,RB                  //  ]
 ,LPAR                //  (
 ,RPAR                //  )
 ,STAR                //  *
 ,DASH                //  -
 ,L                   //  literal character
 ,QUEST               //  ?
 ,BAR                 //  |
 ,PLUS                //  +
 ,UNKNOWN
} Token;

class MacroDefinition {
public:
  String m_value;
  bool   m_inUse;
  MacroDefinition(TCHAR *value);
};

class MacroStackElement {
public:
  MacroDefinition &m_macroDefinition;
  _TUCHAR         *m_next;
  MacroStackElement(MacroDefinition &m, TCHAR *next);
};

class LexScanner {
private:
  StringHashMap<MacroDefinition> m_macros;
  Stack<MacroStackElement>       m_macroStack;
  bool            m_inQuote;     // are we in a quoted String
  TCHAR           m_lineBuffer[8192];
  String          m_fileName, m_absoluteFileName;
  const _TUCHAR  *m_next;
  StringCollector m_collector;
  bool            m_collecting;
  bool            m_debug;
  unsigned int    m_lineNo;      // current lineno of inputfile
  unsigned int    m_col;         // current column of inputfile
  unsigned int    m_ruleLineNo;  // use this linenumber in error-messages when we are in rulesection
  bool            m_ruleSection; // are we in the rulesection of input
  int             m_length;
  TCHAR          *m_text;
  FILE           *m_input;
  Token           m_token;       // current token  (almost = tokmap[m_lexeme])
  unsigned int    m_lexeme;      // current lexeme (character read)
  bool nextLine();
  void nextChar();
  void nextInput() {
    m_next++;
  }
  bool nextExpr();
  void parseName();
  void parseString();
  MacroDefinition &parseMacro();
public:
  LexScanner(const String &fname);
  LexScanner(const LexScanner &src);            // not defined
  LexScanner &operator=(const LexScanner &src); // not defined
  virtual ~LexScanner();
  TCHAR *getfname() {
    return m_fileName.cstr();
  }
  String getText();             // get the text just scanned. used before first %%
  Token hnext();                // next to be used before first %%
  Token nextToken();            // get next token. to be used between %% and %%
  Token getToken() const {
    return m_token;
  }
  const _TUCHAR *getInput() const {
    return m_next;
  }
  unsigned int getLexeme() const {
    return m_lexeme;
  }
  void skipSpace();
  void beginRuleSection();
  void endRuleSection();
  void collectChar(TCHAR ch);
  void collectInit() {
    m_collector.init();
  }
  void collectBegin();
  void collectEnd() {
    m_collecting = false;
  }
  void getCollected(SourceText &src);
  void addMacro();
  unsigned int getLineno() const {
    return m_lineNo;
  }
  SourcePosition getRulePosition() const {
    return SourcePosition(m_absoluteFileName, m_ruleLineNo, 0);
  }
  void verror( const TCHAR *format, va_list argptr);
  void error(  const TCHAR *format, ...);
  void warning(const TCHAR *format, ...);
};

unsigned int escape(const _TUCHAR *&s); // scan escape sequences, return equivalent ASCII character. f.ex. "\n" = 10
