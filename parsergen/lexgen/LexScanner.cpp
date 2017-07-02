#include "stdafx.h"
#include <String.h>
#include <assert.h>
#include <ctype.h>
#include "LexScanner.h"

bool LexScanner::nextLine() {
  bool ret = true;
  if(_fgetts(m_lineBuffer, ARRAYSIZE(m_lineBuffer), m_input) == NULL) {
    _tcscpy(m_lineBuffer, EMPTYSTRING);
    ret = false;
  } else {
    m_lineNo++;
  }
  m_next = m_lineBuffer;
  m_col  = 0;
  return ret;
}

void LexScanner::nextChar() {
  if(*m_next == '\n') {
    if(m_collecting) {
      collectChar(*m_next);
    }
    nextLine();
  } else {
    if(m_collecting) {
      collectChar(*m_next);
    }
    m_next++;
    m_col++;
  }
}

void LexScanner::parseString() {
  while(*m_next && *m_next != '"' && *m_next != '\n') {
    if (*m_next == '\\') {
      nextChar();
      switch(*m_next) {
      case '\0':
      case '\n': goto done;
      case 'n' : nextChar(); continue;
      case 'r' : nextChar(); continue;
      case 't' : nextChar(); continue;
      case '\\': nextChar(); continue;
      case '"' : nextChar(); continue;
      default  : nextChar(); continue; // ignore all other
      }
    }
    nextChar();
  }

done:
  if (*m_next == '"') {
    nextChar();
  }
}

void LexScanner::parseName() {
  while(_istalnum(*m_next) || *m_next == '_') {
    nextChar();
  }
}

void LexScanner::skipSpace() {
  while (_istspace(*getInput())) {
    nextInput();
  }
}

Token LexScanner::hnext() {
start: // restart here if we meet a comment /*...*/ of //
  while(_istspace(*m_next)) {
    nextChar();
  }

  m_text = (TCHAR*)m_next;

  if(_istalpha(*m_next) || *m_next == '_') {
    parseName();
    m_token = NAME;
  } else {
    switch( *m_next ) {
    case '"':
      nextChar();
      parseString();
      m_token = STRINGLIT;
      break;
    case '%':
      nextChar();
      switch(*m_next) {
      case '%':
        m_token = PERCENTPERCENT;
        nextChar();
        break;
      case '{':
        m_token = PERCENTLCURL;
        nextChar();
        break;
      case '}':
        m_token = PERCENTRCURL;
        nextChar();
        break;
      default:
        if(_istalpha(*m_next))
          parseName();
        m_token = NAME;
        break;
      }

      break;
    case '/':
      nextChar();
      switch(*m_next) {
      case '/': // comment until endofline
        nextChar();
        while(*m_next && *m_next != '\n')
          nextChar();
        goto start;
      case '*':
        nextChar();
        while(*m_next) {
          while(*m_next && *m_next != '*') nextChar();
          if(*m_next == '*') {
            nextChar();
            if(*m_next == '/') {
              nextChar();
              goto start;
            }
          }
        }
        m_token = EOI;
        break;

      default:
        m_token = UNKNOWN;
        break;
      }
      break;
    case '\0':
      m_token   = EOI;
      break;
    default:
      m_token   = UNKNOWN;
      nextChar();
      break;
    } // end switch
  } // else
  m_length = (int)((TCHAR*)m_next - (TCHAR*)m_text);

  if(m_debug) {
    _tprintf(_T("%s\n"), m_lineBuffer);
    _tprintf(_T("%*.*s^\n"), (int)(m_text - m_lineBuffer), (int)(m_text - m_lineBuffer), EMPTYSTRING);
    _tprintf(_T("lex:lineno:%2d. token:%2d yytext:[%*.*s] yyleng:%d\n")
            , m_lineNo, m_token, m_length, m_length, m_text, m_length );
    pause();
  }

  return m_token;
}

bool LexScanner::nextExpr() {
  // Gets a regular expression and the associated String from input
  // Returns a pointer to the input String normally.
  // Returns NULL on end of file or if a line beginning with % is
  // encountered. All blank lines are discarded and all lines that start with
  // whitespace are concatenated to the previous line.
  // Lineno is set to the line number of the top line of a multiple-line block

  while(m_lineBuffer[0] == '\n') { // skip blank lines
    if(!nextLine()) {
      return false;
    }
  }

  if(m_lineBuffer[0] == '%') { // next line starts with a % sign
    return false;
  }

  m_ruleLineNo = m_lineNo;
  collectInit();
  for(;;) {
    TCHAR *nl;
    if((nl = _tcschr(m_lineBuffer, '\n')) != NULL) *nl = '\0'; // terminate the line at '\n'
    if(m_lineBuffer[0] != 0) { // only collect nonempty lines
      for(TCHAR *cp = m_lineBuffer; *cp; cp++) {
        collectChar(*cp);
      }
      collectChar('\n');
    }
    if(!nextLine())  {
      break;
    }
    if(!_istspace(m_lineBuffer[0])) { // we are at the next rule. stop read ahead
      break;
    }
  }
  collectChar('\0');
  m_next = m_collector.getBuffer();
  return *m_next != '\0'; // have we got more than '\0' into the buffer
}

static const Token tokenmap[] = {
//  ^@  ^A  ^B  ^C  ^D  ^E  ^F  ^G  ^H  ^I  ^J  ^K  ^L  ^M  ^N  ^O
     L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,

//  ^P  ^Q  ^R  ^S  ^T  ^U  ^V  ^W  ^X  ^Y  ^Z  ^[  ^\  ^]  ^^  ^_
     L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,

//  SPACE  !   "   #    $        %   &    '  (   )   *         +     ,  -     .     /
    L,     L,  L,  L,   DOLLAR,  L,  L,   L, LPAR, RPAR, STAR, PLUS, L, DASH, DOT,  L,

//  0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ?
    L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  QUEST,

//  @   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O
    L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,

//  P   Q   R   S   T   U   V   W   X   Y   Z   [   \   ]   ^           _
    L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  LB, L,  RB, CIRCUMFLEX, L,

//  `   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o
    L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,

//  p   q   r   s   t   u   v   w   x   y   z   {   |   }   ~   DEL
    L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L, BAR, L,  L,  L
};

MacroDefinition::MacroDefinition(TCHAR *value) {
  m_value = value;
  m_inUse = false;
}

MacroStackElement::MacroStackElement(MacroDefinition &m, TCHAR *next) : m_macroDefinition(m) {
  m_next = (_TUCHAR*)next;
}

void LexScanner::addMacro() { // assume we have just scanned a name
  String macroName = getText();
  while(_istspace(*m_next)) {
    nextChar();
  }
  TCHAR tmp[1024];
  int i = 0;
  while (*m_next && !_istspace(*m_next)) {
    tmp[i++] = *m_next;
    nextChar();
  }
  tmp[i] = '\0';
//  printf(_T("macro <%s>:<%s>\n"), macroName.cstr(), tmp);
  if (!m_macros.put(macroName, MacroDefinition(tmp))) {
    error(_T("Macro '%s' already defined"), macroName.cstr());
  }
}

MacroDefinition &LexScanner::parseMacro() {
  // assume *m_next is '{'
  // Return a pointer to the contents of a macro having the indicated
  // name. Abort with a message if no macro exists.
  // Advances m_next to point past the '}'

  TCHAR *p;
  m_next++;                                     // skip {
  if(!(p = _tcschr( (TCHAR*)m_next, '}')) ) {     // and find }
    error(_T("Missing } in macro expansion"));
  } else {
    *p = '\0';                                  // overwrite close brace.
    String tmp(m_next);
    *p++ = '}';                                 // restore '}'
    MacroDefinition *mac = m_macros.get(tmp);
//    printf(_T("expanding macro:<%s>\n"), tmp.cstr());
    if(mac == NULL) {
      error(_T("Macro {%s} doesn't exist"), tmp.cstr());
    }
    if(mac->m_inUse) {
      error(_T("Recursive macros {%s} not allowed"), tmp.cstr());
    }
    m_next = p;

    return *mac;
  }
  throw Exception(_T("Internal error in getmacro")); // to make compiler happy
}

void LexScanner::beginRuleSection() {
  nextLine();
  m_token = EOS;
  m_ruleSection = true;
}

void LexScanner::endRuleSection() {
  nextLine();
  m_ruleSection = false;
  m_length = 0;
}

Token LexScanner::nextToken() {                // called when we parse between %% and %%
  if(m_token == EOS) {                         // Get another line
    if(m_inQuote) {
      error(_T("newline in quoted String"));       // doesn't return
    }
    do {                                       // read ahead until a non-blank line is read into the inputarray
      if(!nextExpr()) {                        // then at end of file
        m_token = EOI;
        return m_token;
      }
//      printf(_T("advance:m_next:<%s>\n"), m_next);
      while(_istspace(*m_next)) {                // Ignore leading white space
        nextInput();
      }
    } while(!*m_next );                        // and blank lines.
  }

  while(*m_next == '\0') {
    if(!m_macroStack.isEmpty()) {              // Restore previous input source
      MacroStackElement top = m_macroStack.pop();
      m_next = top.m_next;
      top.m_macroDefinition.m_inUse = false;   // Mark macro as unused
      continue;
    }

    m_token  = EOS;                            // No more input sources to restore
    m_lexeme = 0;                              // ie. you're at the real end of String.
    return m_token;
  }

  if(!m_inQuote) {
    while(*m_next == '{') {                    // Macro expansion required
      MacroDefinition &m = parseMacro();       // Use macro body as input String, and set m_next to point past '}'
      m.m_inUse = true;                        // Mark macro as used
      m_macroStack.push(MacroStackElement(m, (_TUCHAR*)m_next));
      m_next = m.m_value.cstr();
    }
  }

  if(*m_next == '"') {                         // At start or end of a quoted String
                                               // All characters are treated as
    m_inQuote = !m_inQuote;                    // literals while m_inQuote is true
    nextInput();
    if(*m_next == '\0') {
      m_token  = EOS ;
      m_lexeme = 0;
      return m_token;
    }
  }

  bool sawEscape = (*m_next == '\\');

  if(!m_inQuote) {
    if(_istspace(*m_next)) {
      m_token  = EOS ;
      m_lexeme = 0;
      return m_token;
    }
    m_lexeme = escape(m_next);
  } else {
    if(sawEscape && m_next[1] == '"') {
      nextInput();                             // Skip the escaped character
      nextInput();
      m_lexeme = '"';
    } else {
      m_lexeme = *m_next;
      nextInput();
    }
  }

  m_token = (m_inQuote || sawEscape || m_lexeme > 127) ? L : tokenmap[m_lexeme];

  return m_token;
}

String LexScanner::getText() {
  const TCHAR save  = m_text[m_length];
  m_text[m_length] = '\0';
  String result(m_text);
  m_text[m_length] = save;
  return result;
}

void LexScanner::collectBegin() {
  m_collecting = true;
  m_collector.begin(m_text, m_length, SourcePosition(m_absoluteFileName, m_lineNo, 0));
}

void LexScanner::collectChar(TCHAR ch) {
  m_collector.addChar(ch);
}

void LexScanner::getCollected(SourceText &src) {
  src = m_collector.getSourceText(m_length);
}

static void error(const TCHAR *fname, int lineno, const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  String tmp = vformat(format, argptr);
  tmp.replace('\n', ' ');
  va_end(argptr);
  _tprintf(_T("%s(%d) : error %s\n"), fname, lineno, tmp.cstr());
  exit(-1);
}

static void warning(const TCHAR *fname, int lineno, const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  String tmp = vformat(format, argptr);
  tmp.replace('\n', ' ');
  va_end(argptr);
  _tprintf(_T("%s(%d) : warning %s\n"), fname, lineno, tmp.cstr());
}

void LexScanner::verror(const TCHAR *format, va_list argptr) {
  String tmp = vformat(format, argptr);
  tmp.replace('\n', ' ');
  ::error(m_absoluteFileName.cstr(), m_ruleSection ? m_ruleLineNo : m_lineNo, _T("%s"), tmp.cstr());
}

void LexScanner::error(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  verror(format, argptr);
  va_end(argptr);
}

void LexScanner::warning(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  String tmp = vformat(format, argptr);
  tmp.replace('\n', ' ');
  va_end(argptr);
  ::warning(m_absoluteFileName.cstr(), m_ruleSection ? m_ruleLineNo:m_lineNo, _T("%s"), tmp.cstr());
}

LexScanner::LexScanner(const String &fname) : m_macros(101) {
  m_fileName          = fname;
  m_absoluteFileName  = FileNameSplitter(m_fileName).getAbsolutePath();
  m_input             = FOPEN(m_fileName, _T("r"));
  m_lineNo            = 0;
  m_text              = m_lineBuffer;
  m_length            = 0;
  m_collecting        = false;
  m_debug             = false;
  m_inQuote           = false;
  m_ruleSection       = false;
  nextLine();
}

LexScanner::~LexScanner() {
  fclose(m_input);
}
