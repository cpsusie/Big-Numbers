#include "stdafx.h"
#include <String.h>
#include <assert.h>
#include <ctype.h>
#include <FileNameSplitter.h>
#include "LexScanner.h"

LexScanner::LexScanner(const String &fname) : m_macros(101) {
  m_fileName = fname;
  m_absoluteFileName = FileNameSplitter(m_fileName).getAbsolutePath();
  m_input = FOPEN(m_fileName, _T("r"));
  m_lineNo = 0;
  m_text = m_lineBuffer;
  m_length = 0;
  m_collecting = false;
  m_debug = false;
  m_inQuote = false;
  m_ruleSection = false;
  nextLine();
}

LexScanner::~LexScanner() {
  fclose(m_input);
}

bool LexScanner::nextLine() {
  bool ret = true;
  if(_fgetts(m_lineBuffer, ARRAYSIZE(m_lineBuffer), m_input) == nullptr) {
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
    if((nl = _tcschr(m_lineBuffer, '\n')) != nullptr) *nl = '\0'; // terminate the line at '\n'
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

void LexScanner::addMacro() {
  const String macroName = getText();
  while(_istspace(*m_next)) {
    nextChar();
  }
  String macroValue;
  while(*m_next && !_istspace(*m_next)) {
    macroValue += *m_next;
    nextChar();
  }
//  printf(_T("macro <%s>:<%s>\n"), macroName.cstr(), tmp);
  if (!m_macros.put(macroName, MacroDefinition(macroValue))) {
    error(_T("Macro '%s' already defined"), macroName.cstr());
  }
}

const MacroDefinition &LexScanner::parseMacro() {
  TCHAR *p;
  m_next++;                                     // skip {
  if(!(p = _tcschr( (TCHAR*)m_next, '}')) ) {   // and find }
    error(_T("Missing } in macro expansion"));
  } else {
    *p = '\0';                                  // overwrite close brace.
    const String macroName(m_next);
    *p++ = '}';                                 // restore '}'
    const MacroDefinition *macroDef = m_macros.get(macroName);
//    printf(_T("expanding macro:<%s>\n"), tmp.cstr());
    if(macroDef == nullptr) {
      error(_T("Macro {%s} doesn't exist"), macroName.cstr());
    } else {
      if(macroDef->m_inUse) {
        error(_T("Recursive macros {%s} not allowed"), macroName.cstr());
      }
      m_next = p;
      return *macroDef;
    }
  }
  throwException(_T("Internal error in %s"),__TFUNCTION__);
  return *m_macroStack.top().m_macroDefinition;  // to make compiler happy
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
      error(_T("Newline in quoted String"));   // doesn't return
    }
    do {                                       // read ahead until a non-blank line is read into the inputarray
      if(!nextExpr()) {                        // then at end of file
        m_token = EOI;
        return m_token;
      }
//      printf(_T("advance:m_next:<%s>\n"), m_next);
      while(_istspace(*m_next)) {              // Ignore leading white space
        nextInput();
      }
    } while(!*m_next );                        // and blank lines.
  }

  while(*m_next == '\0') {
    if(!m_macroStack.isEmpty()) {              // Restore previous input source
      const MacroStackElement top = m_macroStack.pop();
      m_next = top.m_next;
      top.m_macroDefinition->m_inUse = false;  // Mark macro as unused
      continue;
    }

    m_token  = EOS;                            // No more input sources to restore
    m_lexeme = 0;                              // ie. you're at the real end of String.
    return m_token;
  }

  if(!m_inQuote) {
    while(*m_next == '{') {                    // Macro expansion required
      const MacroDefinition &m = parseMacro(); // Use macro body as input String, and set m_next to point past '}'
      m.m_inUse = true;                        // Mark macro as used
      m_macroStack.push(MacroStackElement(m, m_next));
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
  m_collector.begin(m_text, m_length, SourcePositionWithName(m_absoluteFileName, m_lineNo, 0));
}

void LexScanner::collectChar(TCHAR ch) {
  m_collector.addChar(ch);
}

void LexScanner::getCollected(SourceText &src) {
  src = m_collector.getSourceText(m_length);
}

static void error(const TCHAR *fname, int lineno, _In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  String tmp = vformat(format, argptr);
  tmp.replace('\n', ' ');
  va_end(argptr);
  _tprintf(_T("%s(%d) : error %s\n"), fname, lineno, tmp.cstr());
  exit(-1);
}

static void warning(const TCHAR *fname, int lineno, _In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  String tmp = vformat(format, argptr);
  tmp.replace('\n', ' ');
  va_end(argptr);
  _tprintf(_T("%s(%d) : warning %s\n"), fname, lineno, tmp.cstr());
}

void LexScanner::verror(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  String tmp = vformat(format, argptr);
  tmp.replace('\n', ' ');
  ::error(m_absoluteFileName.cstr(), m_ruleSection ? m_ruleLineNo : m_lineNo, _T("%s"), tmp.cstr());
}

void LexScanner::error(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  verror(format, argptr);
  va_end(argptr);
}

void LexScanner::warning(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  String tmp = vformat(format, argptr);
  tmp.replace('\n', ' ');
  va_end(argptr);
  ::warning(m_absoluteFileName.cstr(), m_ruleSection ? m_ruleLineNo:m_lineNo, _T("%s"), tmp.cstr());
}
