#include "stdafx.h"
#include <ThreadPool.h>
#include "TestParser.h"
#include "GRAMMARS.h"

SyntaxNode::SyntaxNode(const TCHAR *symbol, UINT childCount, bool terminal, TestParser *parser) {
  m_symbol     = symbol;
  m_childCount = childCount;
  m_terminal   = terminal;
  if(m_childCount == 0) {
    m_children = nullptr;
  } else {
    m_children = new SyntaxNodep[childCount]; TRACE_NEW(m_children);
    for(UINT i = 0; i < m_childCount; i++) {
      m_children[i] = nullptr;
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

  void verror(const SourcePosition &pos, const TCHAR *format, va_list argptr) override {
    m_parser.verror(pos, format, argptr);
  }

  void vdebug(const TCHAR *format, va_list argptr) override {
    m_parser.vdebug(format, argptr);
  }

public:
  TestScanner(LRparser &parser) : m_parser(parser) {}
};

static String getLegalInput(const ParserTables &tables, UINT state) {
  UINT      *symbols = NULL;
  const UINT n       = tables.getLegalInputCount(state);
  try {
    symbols = new UINT[n]; TRACE_NEW(symbols);
    tables.getLegalInputs(state, symbols);
    String result;
    for(UINT i = 0; i < n; i++) {
      if(i > 0) {
        result += _T(" ");
      }
      result += tables.getSymbolName(symbols[i]);
    }
    SAFEDELETEARRAY(symbols);
    return result;
  } catch(...) {
    SAFEDELETEARRAY(symbols);
    throw;
  }
}

class YaccJob : public SafeRunnable {
private:
  TestParser &m_parser;
public:
  YaccJob(TestParser &parser);
  UINT safeRun() override;
};

YaccJob::YaccJob(TestParser &parser) : m_parser(parser) {
}

UINT YaccJob::safeRun() {
  SETTHREADDESCRIPTION(_T("YaccJob"));
  m_parser.buildStateArray();
  return 0;
}

TestParser::TestParser() : LRparser(*tablesToTest), m_grammar(*tablesToTest) {
  m_scanner    = new TestScanner(*this);          TRACE_NEW(m_scanner  );
  setScanner(m_scanner);
  m_root       = nullptr;
  m_userStack  = new SyntaxNodep[getStackSize()]; TRACE_NEW(m_userStack);
  m_yaccJob    = new YaccJob(*this);              TRACE_NEW(m_yaccJob  );
  ThreadPool::executeNoWait(*m_yaccJob);
  buildLegalInputArray();
  buildReduceActionArray();
}

TestParser::~TestParser() {
  SAFEDELETE(     m_scanner  );
  SAFEDELETEARRAY(m_userStack);
  SAFEDELETE(     m_yaccJob  );
  deleteNodeList();
}

void TestParser::deleteNodeList() {
  for(size_t i = 0; i < m_nodeList.size(); i++) {
    SAFEDELETE(m_nodeList[i]);
  }
  m_nodeList.clear();
  m_root = nullptr;
}

void TestParser::buildLegalInputArray() {
  const ParserTables &tables     = getParserTables();
  const UINT          stateCount = tables.getStateCount();
  m_legalLookahead.setCapacity(stateCount);
  for(UINT i = 0; i < stateCount; i++) {
    m_legalLookahead.add(::getLegalInput(tables, i));
  }
}

void TestParser::buildReduceActionArray() {
  const ParserTables &tables    = getParserTables();
  const UINT          prodCount = tables.getProductionCount();
  m_reduceActionStr.setCapacity(prodCount);
  for(UINT prod = 0; prod < prodCount; prod++) {
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
  Grammar   &g          = getGrammar();
  const UINT stateCount = g.getStateCount();
  m_stateStr.setCapacity(stateCount);
  for(UINT i = 0; i < stateCount; i++) {
    m_stateStr.add(g.stateToString(g.getState(i)).replace('\n', _T("\r\n")));
  }
}

void TestParser::waitForYaccJob() {
  SAFEDELETE(m_yaccJob);
}

const String &TestParser::getStateItems(UINT state) {
  waitForYaccJob();
  return m_stateStr[state];
}

void TestParser::userStackInit() {
  m_cycleCount = 0;
  m_stacktop   = m_userStack;
  m_root       = nullptr;
  deleteNodeList();
}

void TestParser::setStackSize(UINT newSize) {
  __super::setStackSize(newSize);
  SAFEDELETEARRAY(m_userStack);
  m_userStack = new SyntaxNodep[getStackSize()]; TRACE_NEW(m_userStack);
}

void TestParser::setNewInput(const TCHAR *string) {
  m_inputStream.open(string);
  m_scanner->newStream(&m_inputStream, 1);
  m_ok   = true;
  m_root = nullptr;
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

String TestParser::getActionMatrixDump() const {
  const ParserTables &tables     = getParserTables();
  const UINT          stateCount = tables.getStateCount();
  const UINT          termCount  = tables.getTerminalCount();
  String              result;
  result = _T("     | ");
  for(UINT t = 0; t < termCount; t++) {
    result += format(_T("%4u "), t);
  }

  result += '\n';
  result += spaceString(result.length(), _T('_'));
  result += '\n';

  for(UINT s = 0; s < stateCount; s++) {
    String line;
    line = format(_T("%4u | "), s);
    for(UINT t = 0; t < termCount; t++) {
      int a = tables.getAction(s, t);
      if(a == _ParserError) {
        line += _T("   E ");
      } else {
        line += format(_T("%4d "), a);
      }
    }
    result += line;
    result += '\n';
  }
  return result;
}
