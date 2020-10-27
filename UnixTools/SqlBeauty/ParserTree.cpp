#include "stdafx.h"
#include "SqlSymbol.h"
#include "ParserTree.h"

void SqlParser::verror(const SourcePosition &pos, const TCHAR *form, va_list argptr) {
  const String msg = vformat(form,argptr);
  m_errmsg.add(format(_T("%s(%d,%d) : error %s"), m_filename.cstr(), pos.getLineNumber(), pos.getColumn(),msg));
}

ULONG TokenPairHash(const TokenPair &tp) {
  return tp.hashCode();
}

int TokenPairCmp(const TokenPair &key, const TokenPair &tablekey) {
  return (key.m_lastToken == tablekey.m_lastToken)
      && (key.m_thisToken == tablekey.m_thisToken) ? 0 : 1;
}

IndentSize::IndentSize(IndentType type, short insideToken, int size, int lmarg) {
  m_type        = type;
  m_insideToken = insideToken;
  m_size        = size;
  m_lmarg       = lmarg;
}

void SqlParser::insertIndentAlways(short token, int size, int lmarg) {
  m_indentSet.put( token, IndentSize(INDENT_ALWAYS,0,size,lmarg));
  m_noSpaceSet.add(token);
}

void SqlParser::insertIndentInside(short token, bool inside, short insideToken, int size) {
  IndentType type = inside ? INDENT_WHENINSIDE : INDENT_WHENNOTINSIDE;
  m_indentSet.put(token, IndentSize(type,insideToken,size));
  m_inside.put(insideToken, InsideMark());
}

void SqlParser::insertIndentCount(short token, int countMax, short insideToken, int size) {
  m_indentSet.put(token, IndentSize(INDENT_COUNT,insideToken,size));
  m_inside.put(insideToken, InsideMark(countMax));
}

SqlParser::SqlParser()
:LRparser(*SqlTables)
,m_noSpaceSet(SqlTables->getSymbolCount())
,m_indentSet(109)
,m_pairNoSpace(109)
,m_inside(109)
{
  m_noSpaceSet.add(COMMA               );
  m_noSpaceSet.add(SEMI                );
  m_noSpaceSet.add(DOT                 );
  m_noSpaceSet.add(EXCLAMATION         );
  insertPairNoSpace(  LEFT        ,LPAR   );
  insertPairNoSpace(  RIGHT       ,LPAR   );
  insertPairNoSpace(  COUNT       ,LPAR   );
  insertPairNoSpace(  NAME        ,LPAR   );
  insertPairNoSpace(  IIF         ,LPAR   );
  insertPairNoSpace(  DOT         ,NAME   );
  insertPairNoSpace(  DOT         ,STAR   );
  insertPairNoSpace(  EXCLAMATION ,NAME   );
  insertPairNoSpace(  MINUS       ,NUMBER );

  insertIndentAlways(PARAMETERS    , 0);
  insertIndentAlways(INSERT        , 0);
  insertIndentAlways(UPDATE        , 0);
  insertIndentAlways(DELETESYM     , 0);
  insertIndentAlways(TRANSFORM     , 0);
  insertIndentAlways(FROM          , 0);
  insertIndentAlways(WHERE         , 0);
  insertIndentAlways(HAVING        , 0);
  insertIndentAlways(ORDER         , 0);
  insertIndentAlways(GROUp         , 0);
  insertIndentAlways(LEFT          , 2);
  insertIndentAlways(RIGHT         , 2);
  insertIndentAlways(INNER         , 2);
  insertIndentAlways(ON            ,10);
  insertIndentAlways(PIVOT         , 0);
  insertIndentAlways(UNION         , 0);
  insertIndentAlways(SET           , 0);
  insertIndentAlways(parameter     , 2);
  insertIndentAlways(select_elem   , 1);
  insertIndentAlways(insert_elem   , 1);
  insertIndentAlways(transform_stmt, 1,2);
  insertIndentAlways(simple_select , 1,2);
  insertIndentAlways(insert_stmt   , 1,2);
  insertIndentAlways(delete_stmt   , 1,2);
  insertIndentAlways(update_stmt   , 1,2);
  insertIndentAlways(group_elem    , 1);
  insertIndentAlways(order_elem    , 1);
  insertIndentAlways(assign        , 1);

  insertIndentInside(AND, false ,between_condition , 1);
  insertIndentInside(AND, false ,iif_expression    , 1);
  insertIndentInside(OR , false ,iif_expression    , 3);

  insertIndentCount( expr       ,5, set_expr_list  , 4);
  insertIndentCount( constsymbol,5, opt_pivotlist  , 4);
  m_root   = nullptr;
  m_indent = 0;
}

SqlParser::~SqlParser() {
  delete m_root;
}

int SqlParser::reduceAction(UINT prod) {
  const UINT symbol    = getParserTables().getLeftSymbol(prod);
  const UINT prodlen   = getParserTables().getProductionLength(prod);
  SyntaxNode *p = new SyntaxNode(symbol,getParserTables().getSymbolName(symbol),prodlen, false);
  for(UINT i = 0; i < prodlen; i++) {
    p->setChild(i,stacktop(prodlen - i - 1));
  }
  m_dollardollar = p;
  if(prod == 0) {
    m_root = p;
  }
  return 0;
}

void SqlParser::userStackShiftSymbol(UINT symbol) {
  SyntaxNode *p = new SyntaxNode(symbol, getScanner()->getText(), 0, true);
  push(p);
}

void SqlParser::parse(const TCHAR *filename, int lineNumber, const TCHAR *stmt) {
  LexStringStream stream(stmt);
  SqlLex lex(this);
  lex.newStream(&stream,lineNumber);
  setScanner(&lex);
  LRparser::parse();
}

void SqlParser::listErrors() {
  for(size_t i = 0; i < m_errmsg.size(); i++) {
    _ftprintf(stderr,_T("%s\n"), m_errmsg[i].cstr());
  }
}

void dumpTree(SyntaxNodep n, FILE *f, int level) {
  _ftprintf(f,_T("%*.*s%s\n"),level,level,EMPTYSTRING,n->getSymbol());
  for(UINT i = 0; i < n->childCount(); i++) {
    dumpTree(n->getChild(i),f,level+2);
  }
}

void SqlParser::dumpTree(FILE *f) {
  ::dumpTree(m_root,f,0);
}

SyntaxNode::SyntaxNode(int token, const TCHAR *symbol, UINT sons, bool terminal) {
  m_token      = token;
  m_symbol     = symbol;
  m_childCount = sons;
  m_terminal   = terminal;
  if(m_childCount == 0) {
    m_children = nullptr;
  } else {
    m_children = new SyntaxNode*[sons];
    for(UINT i = 0; i < m_childCount; i++) {
      m_children[i] = nullptr;
    }
  }
}

SyntaxNode::~SyntaxNode() {
  if(m_childCount > 0) {
    for(UINT i = 0; i < m_childCount; i++) {
      if(m_children[i]) {
        delete m_children[i];
      }
    }
    delete[] m_children;
  }
}

SyntaxNode *SyntaxNode::getChild(UINT i) {
  if(i >= m_childCount) {
    throwIndexOutOfRangeException(__TFUNCTION__, i, m_childCount);
  }
  return m_children[i];
}

void SyntaxNode::setChild(UINT i, SyntaxNode *v) {
  if(i >= m_childCount) {
    throwIndexOutOfRangeException(__TFUNCTION__, i, m_childCount);
  }
  m_children[i] = v;
}
