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

  LexScanner(const LexScanner &src);            // Not defined. Class not cloneable
  LexScanner &operator=(const LexScanner &src); // Not defined. Class not cloneable

  bool nextLine();
  void nextChar();
  void nextInput() {
    m_next++;
  }
  // Gets a regular expression and the associated String from input
  // Returns a pointer to the input String normally.
  // Returns nullptr on end of file or if a line beginning with % is
  // encountered. All blank lines are discarded and all lines that start with
  // whitespace are concatenated to the previous line.
  // Lineno is set to the line number of the top line of a multiple-line block
  bool nextExpr();
  void parseName();
  void parseString();
  // assume *m_next is '{'
  // Return a pointer to the contents of a macro having the indicated
  // name. Abort with a message if no macro exists.
  // Advances m_next to point past the '}'
  MacroDefinition &parseMacro();
public:
  LexScanner(const String &fname);
  virtual ~LexScanner();
  TCHAR *getfname() {
    return m_fileName.cstr();
  }
  // get the text just scanned. used before first %%
  String getText();
  // next to be used before first %%
  Token hnext();
  // get next token. to be used between %% and %%
  Token nextToken();
  inline Token getToken() const {
    return m_token;
  }
  inline const _TUCHAR *getInput() const {
    return m_next;
  }
  inline SourcePosition getInputPos() const {
    return SourcePosition(m_ruleLineNo, (int)(m_next - m_collector.getBuffer()));
  }
  inline UINT getLexeme() const {
    return m_lexeme;
  }
  void skipSpace();
  void beginRuleSection();
  void endRuleSection();
  void collectChar(TCHAR ch);
  inline void collectInit() {
    m_collector.init();
  }
  void collectBegin();
  inline void collectEnd() {
    m_collecting = false;
  }
  void getCollected(SourceText &src);
  // assume we have just scanned a name
  void addMacro();
  UINT getLineno() const {
    return m_lineNo;
  }
  inline SourcePositionWithName getRulePosition() const {
    return SourcePositionWithName(m_absoluteFileName, m_ruleLineNo, 0);
  }
  void verror( _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
  void error(  _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  void warning(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);
};

// scan escape sequences, return equivalent ASCII character. f.ex. "\n" = 10
UINT escape(const _TUCHAR *&s);
