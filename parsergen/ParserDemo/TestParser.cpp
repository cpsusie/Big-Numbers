#include "stdafx.h"
#include "TestParser.h"
#include "GRAMMARS.h"

SyntaxNode::SyntaxNode(const TCHAR *symbol, unsigned int childCount, bool terminal, TestParser *parser) {
  m_symbol     = symbol;
  m_childCount = childCount;
  m_terminal   = terminal;
  if(m_childCount == 0) {
    m_children = NULL;
  } else {
    m_children = new SyntaxNodep[childCount];
    for(UINT i = 0; i < m_childCount; i++) {
      m_children[i] = NULL;
    }
  }
  parser->addSyntaxNode(this);
}

SyntaxNode::~SyntaxNode() {
  if(m_childCount > 0) {
    delete[] m_children;
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

static String getLegalInput(const ParserTables &tables, unsigned int state) {
  const UINT n = tables.getLegalInputCount(state);
  UINT *symbols = new UINT[n];
  tables.getLegalInputs(state, symbols);
  StringArray symstr;
  for(UINT i = 0; i < n; i++) {
    symstr.add(tables.getSymbolName(symbols[i]));
  }
  delete[] symbols;

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
  unsigned int run();
};

YaccThread::YaccThread(TestParser &parser) : m_parser(parser) {
}

unsigned int YaccThread::run() {
  m_parser.buildStateArray();
  return 0;
}

TestParser::TestParser() : LRparser(*tablesToTest), m_grammar(CPP, *tablesToTest) {
  m_scanner    = new TestScanner(*this);
  setScanner(m_scanner);
  m_root       = NULL;
  m_userStack  = new SyntaxNodep[getStackSize()];
  m_initThread = new YaccThread(*this);
  m_initThread->resume();
  buildLegalInputArray();
  buildReduceActionArray();
}

TestParser::~TestParser() {
  delete m_scanner;
  delete[] m_userStack;
  if(m_initThread != NULL) {
    delete m_initThread;
    m_initThread = NULL;
  }
  deleteNodeList();
}

void TestParser::deleteNodeList() {
  for(size_t i = 0; i < m_nodeList.size(); i++) {
    delete m_nodeList[i];
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
    delete m_initThread;
    m_initThread = NULL;
  }
}

const String &TestParser::getStateItems(unsigned int state) {
  if(m_initThread != NULL) {
    waitForInitThread();
  }
  return m_stateStr[state];
}

void TestParser::userStackInit() {
  m_stacktop = m_userStack;
  m_root     = NULL;
  deleteNodeList();
}

void TestParser::setStackSize(unsigned int newSize) {
  LRparser::setStackSize(newSize);
  delete[] m_userStack;
  m_userStack = new SyntaxNodep[getStackSize()];
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

void TestParser::userStackShiftSymbol(unsigned int symbol) {
  SyntaxNode *p = new SyntaxNode(m_scanner->getText(), 0, true, this);
  push(p);
}

int TestParser::reduceAction(unsigned int prod) {
  const ParserTables &tables  = getParserTables();
  const unsigned int  symbol  = tables.getLeftSymbol(prod);
  const unsigned int  prodlen = tables.getProductionLength(prod);
  SyntaxNodep         p       = new SyntaxNode(getSymbolName(symbol), prodlen, false, this);
  for(UINT i = 0; i < prodlen; i++) {
    SyntaxNodep child = getStackTop(prodlen - i - 1);
    p->setChild(i, child);
  }
  m_dollardollar = p;
  if(prod == 0) {
    m_root = m_dollardollar ;
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
