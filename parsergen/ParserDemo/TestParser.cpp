#include "stdafx.h"
#include "TestParser.h"
#include "GRAMMARS.h"

SyntaxNode::SyntaxNode(const TCHAR *symbol, UINT childCount, bool terminal, TestParser *parser) {
  m_symbol     = symbol;
  m_childCount = childCount;
  m_terminal   = terminal;
  if(m_childCount == 0) {
    m_children = NULL;
  } else {
    m_children = new SyntaxNodep[childCount]; TRACE_NEW(m_children);
    for(UINT i = 0; i < m_childCount; i++) {
      m_children[i] = NULL;
    }
  }
  parser->addSyntaxNode(this);
}

SyntaxNode::~SyntaxNode() {
  if(m_childCount > 0) {
    SAFEDELETEARRAY(m_children);
  }
}

class TestScanner : public ScannerToTest {
private:
  LRparser &m_parser;

  void verror(const SourcePosition &pos, const TCHAR *format, va_list argptr) {
    m_parser.verror(pos, format, argptr);
  }

  void vdebug(const TCHAR *format, va_list argptr) {
    m_parser.vdebug(format, argptr);
  }

public:
  TestScanner(LRparser &parser) : m_parser(parser) {}
};

static String getLegalInput(const ParserTables &tables, UINT state) {
  const UINT n       = tables.getLegalInputCount(state);
  UINT      *symbols = new UINT[n]; TRACE_NEW(symbols);
  tables.getLegalInputs(state, symbols);
  StringArray symstr;
  for(UINT i = 0; i < n; i++) {
    symstr.add(tables.getSymbolName(symbols[i]));
  }
  SAFEDELETEARRAY(symbols);

  String result;
  for(size_t i = 0; i < symstr.size(); i++) {
    if(i > 0) {
      result += _T(" ");
    }
    result += symstr[i];
  }
  return result;
}

class YaccThread : public Thread {
private:
  TestParser &m_parser;
public:
  YaccThread(TestParser &parser);
  UINT run();
};

YaccThread::YaccThread(TestParser &parser)
: Thread(_T("YaccThread"))
, m_parser(parser)
{
}

UINT YaccThread::run() {
  m_parser.buildStateArray();
  return 0;
}

TestParser::TestParser() : LRparser(*tablesToTest), m_grammar(CPP, *tablesToTest) {
  m_scanner    = new TestScanner(*this);          TRACE_NEW(m_scanner   );
  setScanner(m_scanner);
  m_root       = NULL;
  m_userStack  = new SyntaxNodep[getStackSize()]; TRACE_NEW(m_userStack );
  m_initThread = new YaccThread(*this);           TRACE_NEW(m_initThread);
  m_initThread->resume();
  buildLegalInputArray();
  buildReduceActionArray();
}

TestParser::~TestParser() {
  SAFEDELETE(     m_scanner   );
  SAFEDELETEARRAY(m_userStack );
  SAFEDELETE(     m_initThread);
  deleteNodeList();
}

void TestParser::deleteNodeList() {
  for(size_t i = 0; i < m_nodeList.size(); i++) {
    SAFEDELETE(m_nodeList[i]);
  }
  m_nodeList.clear();
  m_root = NULL;
}

void TestParser::buildLegalInputArray() {
  const ParserTables &tables     = getParserTables();
  const UINT          stateCount = tables.getStateCount();
  for(UINT i = 0; i < stateCount; i++) {
    m_legalLookahead.add(::getLegalInput(tables, i));
  }
}

void TestParser::buildReduceActionArray() {
  const ParserTables &tables = getParserTables();
  for(UINT prod = 0; prod < tables.getProductionCount(); prod++) {
    m_reduceActionStr.add(format(_T("Reduce by (%d) %s -> %s")
                                ,prod
                                ,getSymbolName(tables.getLeftSymbol(prod))
                                ,tables.getRightString(prod).cstr()
                                )
                         );
  }
}

void TestParser::buildStateArray() {
  m_grammar.generateStates();
  Grammar &g = getGrammar();
  for(int i = 0; i < g.getStateCount(); i++) {
    m_stateStr.add(g.stateToString(g.getState(i)).replace('\n', _T("\r\n")));
  }
}

void TestParser::waitForInitThread() {
  if(m_initThread != NULL) {
    while(m_initThread->stillActive()) {
      Sleep(300);
    }
    SAFEDELETE(m_initThread);
  }
}

const String &TestParser::getStateItems(UINT state) {
  if(m_initThread != NULL) {
    waitForInitThread();
  }
  return m_stateStr[state];
}

void TestParser::userStackInit() {
  m_cycleCount = 0;
  m_stacktop   = m_userStack;
  m_root       = NULL;
  deleteNodeList();
}

void TestParser::setStackSize(UINT newSize) {
  LRparser::setStackSize(newSize);
  SAFEDELETEARRAY(m_userStack);
  m_userStack = new SyntaxNodep[getStackSize()]; TRACE_NEW(m_userStack);
}

void TestParser::setNewInput(const TCHAR *string) {
  m_inputStream.open(string);
  m_scanner->newStream(&m_inputStream, 1);
  m_ok   = true;
  m_root = NULL;
}

void TestParser::verror(const SourcePosition &pos, const TCHAR *format, va_list argptr) {
  m_ok = false;
  m_handler->handleError(pos, format, argptr);
}

void TestParser::vdebug(const TCHAR *format, va_list argptr) {
  m_handler->handleDebug(m_scanner->getPos(), format, argptr);
}

void TestParser::userStackShiftSymbol(UINT symbol) {
  m_cycleCount++;
  SyntaxNode *p = new SyntaxNode(m_scanner->getText(), 0, true, this); TRACE_NEW(p);
  push(p);
}

int TestParser::reduceAction(UINT prod) {
  m_cycleCount++;
  const ParserTables &tables  = getParserTables();
  const UINT  symbol  = tables.getLeftSymbol(prod);
  const UINT  prodlen = tables.getProductionLength(prod);
  SyntaxNodep         p       = new SyntaxNode(getSymbolName(symbol), prodlen, false, this);
  for(UINT i = 0; i < prodlen; i++) {
    SyntaxNodep child = getStackTop(prodlen - i - 1);
    p->setChild(i, child);
  }
  m_leftSide = p;
  if(prod == 0) {
    m_root = m_leftSide;
  }
  m_handler->handleReduction(prod);
  return 0;
}

String TestParser::getActionString() const {
  const int action = getNextAction();
  if(action == _ParserError) {
    return _T("Error");
  } else if(action > 0) {
    return format(_T("Shift to state %d"), action);
  } else { // action <= 0 : prod = -action
    return m_reduceActionStr[-action];
  }
}
