#include "pch.h"
#include <String.h>
#include <stdarg.h>
#include <LRparser.h>

// try this one: select max(sdf* + from my,df where sdf * 2 =;

LRparser::LRparser(const ParserTables &tables, Scanner *scanner, UINT stackSize) : m_tables(tables), m_scanner(scanner) {
  m_parserStack   = NULL;
  parserStackCreate(stackSize);

  initialize();
  m_maxErrorCount = 100;
  m_cascadeCount  = 5;
  m_debug         = false;
  m_done          = true;
}

LRparser::~LRparser() {
  parserStackDestroy();
}

void LRparser::initialize() {
  m_state         = 0;
  m_suppressError = 0;
  m_errorCount    = 0;
  m_done          = false;
  m_input         = 0;
  m_text          = EMPTYSTRING;
  m_textLength    = 0;
  parserStackInit();
}

void LRparser::parserStackDestroy() {
  if(m_parserStack != NULL) {
    delete[] m_parserStack;
    m_parserStack = NULL;
    m_stackSize   = 0;
  }
}

void LRparser::parserStackCreate(UINT stackSize) {
  m_stackSize   = max(256, stackSize);
  m_parserStack = new ParserStackElement[m_stackSize];
}

void LRparser::setStackSize(UINT newSize) {
  parserStackDestroy();
  parserStackCreate(newSize);
  m_done = true;
  m_errorCount++;
}

int LRparser::getNextAction() const {
  return done() ? (accept() ? 0 : _ParserError) : m_tables.getAction(m_state, m_input);
}

void LRparser::stackOverflow() {
  if(m_debug) {
    debug(_T("Stackoverflow"));
  }
  error(m_pos, _T("Stackoverflow. stacksize=%u. increment stacksize"), m_stackSize);
  m_errorCount++;
  m_done = true;
}

void LRparser::dumpState() {
  debug(_T("--------New state:%-4u. Input=%-15s ('%s')")
       ,state()
       ,getSymbolName(input())
       ,getScanner()->getText());
}

bool LRparser::recover() {
  if(m_suppressError == 0) {
    m_errorCount++;
    if(m_input) {
      error( m_scanner->getPos(), _T("Unexpected symbol:'%s'"), m_scanner->getText());
    } else { // EOI - current lexeme is empty
      error( m_scanner->getPos(), _T("Unexpected end of input"));
    }
    if(m_errorCount > m_maxErrorCount) {
      error(m_scanner->getPos(), _T("Too many errors, aborting"));
      return false;
    }
  }

  const UINT startHeight  = getStackHeight();
  do {
    while(!stackEmpty() && m_tables.getAction(getParserStackTop().m_state, m_input) == _ParserError) {
      parserStackPop(1);
    }

    if(!stackEmpty()) { // Recovered successfully
      const int poppedSymbols = startHeight - getStackHeight();

      if(m_debug && (poppedSymbols != 0)) {
        String poppedString;
        const TCHAR *delim = NULL;
        for(UINT i = getStackHeight(); i < startHeight; i++) {
          if(delim) poppedString += delim; else delim = _T(" ");
          poppedString += getSymbolName(m_parserStack[i].m_symbol);
        }
        debug(_T("Recovering. Popping %d symbols from stack [%s]"), poppedSymbols,poppedString.cstr());
      }

      userStackPopSymbols(poppedSymbols);
      m_state = getParserStackTop().m_state;

      if(m_debug) {
        dumpState();
      }
      return true; // Recovered successfully
    }

    if(m_debug) {
      debug(_T("Recovering. Skipping bad input %s ('%s')"), getSymbolName(m_input), m_scanner->getText());
    }

    parserStackRestore(startHeight);
    m_input = m_scanner->getNextLexeme();
    m_scanner->markPrevious();
  } while(m_input != 0 /*EOI*/ );

  return false; // Recover failed
}

SourcePosition LRparser::getPos() const {
  return SourcePosition(m_pos.getFileName(),m_pos.getLineNumber(),m_pos.getColumn()-m_textLength);
}

const SourcePosition &LRparser::getPos(int i) const {
  if(m_done) {
    return m_pos;
  }
  return (m_productionLength == 0) ? m_scanner->getPreviousPos() : getParserStackTop(m_productionLength-i).m_pos;
}

int LRparser::parseStep() { // return 0 on continue, != 0 terminate parse
  if(m_done) {
    return -1; // parse has been terminated. Dont try to continue
  }
  const int action = m_tables.getAction(m_state,m_input);
  if(m_suppressError) {
     m_suppressError--;
  }
  if(action == _ParserError) {

    if(m_debug) {
      debug(_T("Error in state %d. Input=%s ('%s')")
           ,m_state
           ,getSymbolName(m_input)
           ,m_scanner->getText());
    }

    if(!recover()) {
      m_done = true;

      if(m_debug) {
        debug(_T("Recover failed"));
        debug(_T("Dont accept"));
      }

      return -1;
    }
    m_suppressError = m_cascadeCount;
  } else if(action > 0) { // shift
    m_state = action;
    m_pos   = m_scanner->getPreviousPos();

    if(m_debug) {
      debug(_T("Shift %s ('%s') to stack.")
           ,getSymbolName(m_input)
           ,m_scanner->getText());
/*
      debug(_T("parser.m_pos:(%s)  scanner:pos:%s length:%d,   prevPos:(%s) , prevLength:%d")
           ,m_pos.toString().cstr()
           ,m_scanner->getPos().toString().cstr()        , m_scanner->getLength()
           ,m_scanner->getPreviousPos().toString().cstr(), m_scanner->getPreviousLength());
*/
    }
    parserStackShift(m_state, m_input, m_scanner->getPos());
    userStackShiftSymbol(m_input);
    m_scanner->markPrevious();
    m_input = m_scanner->getNextLexeme();

    if(m_debug) {
      dumpState();
    }
  } else if(action <= 0) { // reduce by production (= -action)
    const UINT reduceProduction = -action;
    m_productionLength   = m_tables.getProductionLength(reduceProduction);

    if(m_debug) {
      debug(_T("Reduce by %-3u :%s -> %s.")
           ,reduceProduction
           ,m_tables.getLeftSymbolName(reduceProduction)
           ,m_tables.getRightString(reduceProduction).cstr());
    }

    defaultReduce(reduceProduction);    // $$ = $1
    TCHAR termchar;
    bool gotptoken;
    m_pos = m_scanner->getPreviousPos();
    if(m_text = (TCHAR*)m_scanner->getPreviousText()) {
      m_textLength = (int)m_scanner->getPreviousLength();
      termchar     = m_text[m_textLength];
      m_text[m_textLength] = 0;
      gotptoken    = true;
//      printf(_T("<%s> at (%d,%d)\n"),m_yyText,m_yyPos.m_lineno,m_yyPos.m_col);
    } else { // no previous token
      m_text       = EMPTYSTRING;
      m_textLength = 0;
      gotptoken    = false;
    }

    const int c = reduceAction(reduceProduction);
    if(gotptoken) {
      m_text[m_textLength] = termchar;
    }
    if(c) {
      m_done = true;

      if(m_debug) {
        debug(_T("reduceAction returned nonzero (%d). Terminate parse."), c);
      }
      return c;
    }

    const SourcePosition &pos = getPos(1);

    parserStackPop(m_productionLength);
    userStackPopSymbols(m_productionLength);
    userStackShiftDollarDollar();

    if(reduceProduction == 0) {
      const UINT nt = m_tables.getLeftSymbol(reduceProduction);
      parserStackShift(m_state, nt, pos);
      m_done = true;
      if(m_debug) {
        debug(_T("%s"),accept() ? _T("Accept"):_T("Dont accept"));
      }
    } else {
      m_state = getParserStackTop().m_state;
      const UINT nt = m_tables.getLeftSymbol(reduceProduction);
      m_state = m_tables.getSuccessor(m_state,nt);
      parserStackShift(m_state, nt, pos);
    }

    if(m_debug) {
      dumpState();
    }
  }
  return 0;
}

static const TCHAR *noScannerText = _T("No scanner specified for LRparser");

int LRparser::parseBegin() { // return 0 on ok. < 0 on error
  if(m_scanner == NULL) {
    m_errorCount++;
    m_done = true;
    error(SourcePosition(),_T("%s"), noScannerText);

    if(m_debug) {
      debug(_T("%s"), noScannerText);
    }
    return -1;
  }

  initialize();
  userStackInit();
  m_input = m_scanner->getNextLexeme();

  if(m_debug) {
    debug(_T("Initialize"));
    dumpState();
  }

  parserStackShift(m_state, 0, m_scanner->getPos()); // push state0, eoi - ie stackbottom
  return 0;
}

int LRparser::parse() {
  if(parseBegin() != 0) {
    return -1;
  }
  while(!m_done) {
    const int c = parseStep();
    if(c) {
      return c;
    }
  }
  return (m_errorCount == 0) ? 0 : -1;
}

Scanner *LRparser::setScanner(Scanner *scanner) {
  Scanner *old = m_scanner;
  m_scanner = scanner;
  return old;
}

void LRparser::error(const SourcePosition &pos, const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  verror(pos,format,argptr);
  va_end(argptr);
}

void LRparser::debug(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  vdebug(format,argptr);
  va_end(argptr);
}

void LRparser::verror(const SourcePosition &pos, const TCHAR *format, va_list argptr) {
  _tprintf(_T("error in line %d:"), pos. getLineNumber());
  _vtprintf(format, argptr);
  _tprintf(_T("\n")); // we default append a newline.
}

void LRparser::vdebug(const TCHAR *format, va_list argptr) {
  _vtprintf(format, argptr);
  _tprintf(_T("\n"));
}

String ParserTables::getRightString(UINT prod) const {
  const UINT length = getProductionLength(prod);
  if(length == 0) {
    return _T("epsilon");
  } else {
    UINT symbols[1000];
    getRightSide(prod, symbols);
    String result;
    for(UINT i = 0; i < length; i++) {
      if(i > 0) {
        result += _T(" ");
      }
      result += getSymbolName(symbols[i]);
    }
    return result;
  }
}
