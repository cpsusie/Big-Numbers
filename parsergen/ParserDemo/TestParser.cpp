#include "stdafx.h"
#include <ThreadPool.h>
#include <Semaphore.h>
#include <Grammar.h>
#include <OptimizedBitSetPermutation.h>
#include <ParserTransitionMatrix.h>
#include <GrammarResult.h>
#include "TestParser.h"
#include "GRAMMARS.h"

using namespace LRParsing;

SyntaxNode::SyntaxNode(const String &symbol, UINT childCount, bool terminal, TestParser *parser)
: m_symbol(    symbol    )
, m_childCount(childCount)
, m_terminal(  terminal  )
{
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
  void verror(const SourcePosition &pos, const TCHAR *format, va_list argptr) final {
    m_parser.verror(pos, format, argptr);
  }
  void vdebug(const TCHAR *format, va_list argptr) final {
    m_parser.vdebug(format, argptr);
  }
public:
  TestScanner(LRparser &parser) : m_parser(parser) {
  }
};

class YaccJob : public SafeRunnable {
private:
  const AbstractParserTables &m_tables;
  Grammar                     m_grammar;
  mutable Semaphore           m_lock;
public:
  YaccJob(const AbstractParserTables &tables);
  UINT safeRun() final;
  const Grammar &getGrammar() const;
};

YaccJob::YaccJob(const AbstractParserTables &tables)
  : m_tables(tables)
  , m_grammar(tables)
  , m_lock(0)
{
}

const Grammar &YaccJob::getGrammar() const {
  m_lock.wait();
  return m_grammar;
}

class TablesFromGrammar : public AbstractParserTables {
private:
  const Grammar &m_grammar;
public:
  TablesFromGrammar(const Grammar &grammar) : m_grammar(grammar) {
  }
  UINT          getSymbolCount()                                const final {
    return m_grammar.getSymbolCount();
  }
  UINT          getTermCount()                                  const final {
    return m_grammar.getTermCount();
  }
  const String &getSymbolName(       UINT symbolIndex)          const final {
    return m_grammar.getSymbolName(symbolIndex);
  }
  Action        getAction(           UINT state, UINT term    ) const final {
    return m_grammar.getResult().m_stateResult[state].m_termActionArray.getAction(term);
  }
  // Return >= 0 if new state exist, or -1 if no next state with the given combination of state,nterm exist
  int           getSuccessor(        UINT state, UINT nterm   ) const final {
    return m_grammar.getResult().m_stateResult[state].m_ntermNewStateArray.getNewState(nterm);
  }
  UINT          getProductionLength( UINT prod                ) const final {
    return m_grammar.getProduction(prod).getLength();
  }
  UINT          getLeftSymbol(       UINT prod                ) const final {
    return m_grammar.getProduction(prod).m_leftSide;
  }
  void          getRightSide(        UINT prod, UINT *dst     ) const final;
  UINT          getProductionCount()                            const final {
    return m_grammar.getProductionCount();
  }
  UINT          getStateCount()                                 const final {
    return m_grammar.getStateCount();
  }
  UINT          getStartState()                                 const final {
    return m_grammar.getStartState();
  }
  UINT          getTableByteCount(Platform platform) const final {
    return 0;
  }
};

void TablesFromGrammar::getRightSide(UINT prod, UINT *dst) const {
  const CompactArray<RightSideSymbol> &ra = m_grammar.getProduction(prod).m_rightSide;
  for(auto s : ra) {
    *(dst++) = s.m_index;
  }
}


class StatePermutation: public UIntPermutation {
public:
  StatePermutation(const AbstractParserTables &fromTables, const AbstractParserTables &toTables);
};

StatePermutation::StatePermutation(const AbstractParserTables &fromTables, const AbstractParserTables &toTables) : UIntPermutation(fromTables.getStateCount()) {
  const UINT symbolCount = toTables.getSymbolCount();
  const UINT termCount   = toTables.getTermCount();

  if(fromTables.getStateCount() != toTables.getStateCount()) {
    throwInvalidArgumentException(__TFUNCTION__, _T("fromTables.stateCount=%u, toTables.stateCount=%u"), fromTables.getStateCount(), toTables.getStateCount());
  }
  if(fromTables.getSymbolCount() != symbolCount) {
    throwInvalidArgumentException(__TFUNCTION__, _T("fromTables.getSymbolCount=%u, toTables.symbolCount=%u"), fromTables.getSymbolCount(), symbolCount);
  }
  if(fromTables.getTermCount() != termCount) {
    throwInvalidArgumentException(__TFUNCTION__, _T("fromTables.termCount=%u, toTables.termCount=%u"), fromTables.getTermCount(), termCount);
  }
  UIntPermutation &perm = *this;
  CompactStack<StatePair> stack;
  stack.push(StatePair(fromTables.getStartState(), toTables.getStartState()));
  UINT count = 0;
  while(!stack.isEmpty()) {
    const StatePair p = stack.pop();
    if(perm[p.m_state] == -1) {
      perm[p.m_state] = p.m_newState;
      count++;
      for(UINT symbol = 0; symbol < symbolCount; symbol++) {
        const int fromNewState = fromTables.getNewState(p.m_state, symbol);
        if((fromNewState >= 0) && ((int)perm[fromNewState] < 0)) {
          const int toNewState = toTables.getNewState(p.m_newState, symbol);
          assert(toNewState >= 0);
          stack.push(StatePair(fromNewState, toNewState));
        }
      }
    }
  }
  assert(count == perm.size());
}

UINT YaccJob::safeRun() {
  SETTHREADDESCRIPTION(_T("YaccJob"));
  m_grammar.generateStates();
  StatePermutation permutation(TablesFromGrammar(m_grammar), m_tables);
  m_grammar.reorderStates(permutation);
  m_lock.notify();
  return 0;
}

TestParser::TestParser()
  : LRparser(*tablesToTest)
  , m_ok(    true)
  , m_cycleCount(0)
  , m_root(nullptr)
{
  m_scanner    = new TestScanner(*this);          TRACE_NEW(m_scanner  );
  setScanner(m_scanner);
  m_userStack  = new SyntaxNodep[getStackSize()]; TRACE_NEW(m_userStack);
  m_yaccJob    = new YaccJob(getParserTables());  TRACE_NEW(m_yaccJob  );
  ThreadPool::executeNoWait(*m_yaccJob);
  buildLegalTermStringArray();
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

void TestParser::buildLegalTermStringArray() {
  const AbstractParserTables &tables = getParserTables();
  const FullActionMatrix      am(tables);
  const UINT                  stateCount = tables.getStateCount();
  m_legalLookahead.setCapacity(stateCount);
  for(UINT s = 0; s < stateCount; s++) {
    m_legalLookahead.add(am.getLegalTermString(s));
  }
}

void TestParser::buildReduceActionArray() {
  const AbstractParserTables &tables    = getParserTables();
  const UINT                  prodCount = tables.getProductionCount();
  m_reduceActionStr.setCapacity(prodCount);
  for(UINT prod = 0; prod < prodCount; prod++) {
    m_reduceActionStr.add(format(_T("Reduce by (%d) %s -> %s")
                                ,prod
                                ,tables.getLeftSymbolName(prod).cstr()
                                ,tables.getRightString(prod).cstr()
                                )
                         );
  }
}

const String &TestParser::getStateItems(UINT state) {
  if(m_yaccJob) {
    const Grammar &g = m_yaccJob->getGrammar();
    const UINT stateCount = g.getStateCount();
    m_stateStr.setCapacity(stateCount);
    for(UINT i = 0; i < stateCount; i++) {
      m_stateStr.add(g.getState(i).toString().replace('\n', _T("\r\n")));
    }
    SAFEDELETE(m_yaccJob);
  }
  return m_stateStr[state];
}

void TestParser::userStackInit() {
  m_cycleCount = 0;
  m_stacktop   = m_userStack;
  memset(m_userStack, 0, sizeof(m_userStack[0]) * getStackSize());
  m_root       = nullptr;
  deleteNodeList();
}

void TestParser::setStackSize(UINT newSize) {
  __super::setStackSize(newSize);
  SAFEDELETEARRAY(m_userStack);
  m_userStack = new SyntaxNodep[getStackSize()]; TRACE_NEW(m_userStack);
}

void TestParser::setNewInput(const TCHAR *string, bool makeDerivatonTree) {
  m_inputStream.open(string);
  m_scanner->newStream(&m_inputStream, 1);
  m_ok                 = true;
  m_makeDerivationTree = makeDerivatonTree;
  m_root               = nullptr;
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
  if(m_makeDerivationTree) {
    SyntaxNode *p = new SyntaxNode(m_scanner->getText(), 0, true, this); TRACE_NEW(p);
    push(p);
  } else {
    push(nullptr);
  }
}

int TestParser::reduceAction(UINT prod) {
  m_cycleCount++;
  if(!m_makeDerivationTree) {
    m_leftSide = nullptr;
  } else {
    const AbstractParserTables &tables  = getParserTables();
    const UINT                  symbol  = tables.getLeftSymbol(prod);
    const UINT                  prodlen = tables.getProductionLength(prod);
    SyntaxNodep                 p = new SyntaxNode(getSymbolName(symbol), prodlen, false, this); TRACE_NEW(p);
    for(UINT i = 0; i < prodlen; i++) {
      SyntaxNodep child = getStackTop(prodlen - i - 1);
      p->setChild(i, child);
    }
    m_leftSide = p;
    if(prod == 0) {
      m_root = m_leftSide;
    }
  }
  return 0;
}

String TestParser::getActionString() const {
  const Action action = getNextAction();
  switch(action.getType()) {
  case PA_SHIFT : return format(_T("Shift to state %u"), action.getNewState());
  case PA_REDUCE: return m_reduceActionStr[action.getReduceProduction()];
  case PA_ERROR : return _T("Error");
  default       : return format(_T("Unknown actionType:%d"), action.getType());
  }
}

String TestParser::getActionMatrixDump() const {
  const AbstractParserTables &tables = getParserTables();
  String                      result;
  result += FullActionMatrix(getParserTables()).toString();
  result += _T("\n\n\n");
  result += TransposedShiftMatrix(tables).toString();
  return result;
}

String TestParser::getSuccessorMatrixDump() const {
  const AbstractParserTables &tables = getParserTables();
  String                      result;
  result += FullSuccessorMatrix(tables).toString();
  result += _T("\n\n\n");
  result += TransposedSuccessorMatrix(tables).toString();
  return result;
}
