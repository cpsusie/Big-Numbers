#include "stdafx.h"

static void findent(FILE *f, int level) {
  _ftprintf(f,_T("%*.*s"),level,level,EMPTYSTRING);
}

StatementSymbolInfo::StatementSymbolInfo(StatementTable &ft, unsigned short colIndex, SyntaxNode *n) :
  m_table(ft),
  SyntaxNodeData(n) {
  m_colIndex = colIndex;
}

SelectSymbolInfo::SelectSymbolInfo(FromTable &ft, unsigned short colIndex, SyntaxNode *n) :
  StatementSymbolInfo(ft,colIndex,n),
  m_fromTable(ft)
{
}

void StatementSymbolInfo::dump(FILE *f, int level) const {
  findent(f,level+2);
  _ftprintf(f,_T("table:<%s>.<%s> fixedbyconst:%s\n")
    ,m_table.getTableName().cstr()
    ,getColumn().m_name.cstr()
    ,boolToStr(isFixedByConst()));
}

void SelectSymbolInfo::dump(FILE *f, int level) const {
  findent(f,level+2);
  _ftprintf(f,_T("table:<%s> corr:<%s>.<%s> fixedbyconst:%s\n")
    ,m_fromTable.getTableName().cstr()
    ,m_fromTable.m_correlationName.cstr()
    ,getColumn().m_name.cstr()
    ,boolToStr(isFixedByConst()));
}

void SSIMap::append(FromTable &ft, unsigned short colIndex, SyntaxNode *n) {
//  ft.m_compiler.dumpSyntaxTree(n);
  add(SelectSymbolInfo(ft, colIndex, n));
  n->setData(&(*this)[size()-1]);
//  dump();
}

SelectSymbolInfo *SelectStmt::getInfo(const SyntaxNode *n) const {
  if(n == NULL) throwSqlError(SQL_FATAL_ERROR,_T("getInfo:SyntaxNode is NULL"));
  if(n->token() == NAME)
    return (SelectSymbolInfo *)(n->getData());
  else
    if(n->token() == DOT)
      return (SelectSymbolInfo *)(n->child(1)->getData());

  stopcomp(n);
  return NULL; // just to make compiler happy
}

void SSIMap::dump(FILE *f, int level) const {
  for(UINT i = 0; i < size(); i++)
    (*this)[i].dump(f,level);
}

static void defaultstat(SysTableStatData &stat, const IndexDefinition &indexDef) {
  if(indexDef.m_indexName.length() >= ARRAYSIZE(stat.m_indexName))
    throwSqlError(SQL_INVALID_INDEXNAME,_T("Indexname <%s> is too long"),indexDef.m_indexName.cstr());

  _tcscpy(stat.m_indexName,indexDef.m_indexName.cstr());
  stat.m_totalCount = 100;
  int colCount  = indexDef.getColumnCount();
  stat.m_field1 = (colCount >= 1) ? 1.0/3.0  : 0;
  stat.m_field2 = (colCount >= 2) ? 1.0/9.0  : 0;
  stat.m_field3 = (colCount >= 3) ? 1.0/27.0 : 0;
  stat.m_field4 = (colCount >= 4) ? 1.0/81.0 : 0;
}

ColumnAttributes::ColumnAttributes() {
  memset(this,0,sizeof(ColumnAttributes));
}

bool ColumnAttributes::used() const {
  return m_whereUnaggr
      || m_whereAggr
      || m_selUnaggr
      || m_selAggr
      || m_havUnaggr
      || m_havAggr
      || m_orderAggr
      || m_orderUnaggr
      || m_inGroupBy;
};

void ColumnAttributes::setAttribute(int clause, bool inaggr, bool value) {
  if(inaggr) {
    switch(clause) {
    case HAVING: m_havAggr     = value; break;
    case SELECT: m_selAggr     = value; break;
    case WHERE : m_whereAggr   = value; break;
    case ORDER : m_orderAggr   = value; break;
    default    : throwSqlError(SQL_FATAL_ERROR,_T("unknown clause in setAttribute:%d"),clause);
    }
  }
  else { // !inaggr
    switch(clause) {
    case HAVING: m_havUnaggr   = value; break;
    case SELECT: m_selUnaggr   = value; break;
    case WHERE : m_whereUnaggr = value; break;
    case ORDER : m_orderUnaggr = value; break;
    default    : throwSqlError(SQL_FATAL_ERROR,_T("unknown clause in setAttribute:%d"),clause);
    }
  }
}

void ColumnAttributes::dump(const FromTable *t, FILE *f, int level) const {
#define ATTRDUMP(a) if(m_##a) _ftprintf(f,_T("%s,"),_T(#a))
  ATTRDUMP(whereUnaggr);
  ATTRDUMP(whereAggr  );
  ATTRDUMP(selUnaggr  );
  ATTRDUMP(selAggr    );
  ATTRDUMP(havUnaggr  );
  ATTRDUMP(havAggr    );
  ATTRDUMP(orderAggr  );
  ATTRDUMP(orderUnaggr);
  ATTRDUMP(inGroupBy  );
  if(m_fixedBy != NULL)
    _ftprintf(f,_T("fixed by %s"), m_fixedBy->toString().cstr());
  _ftprintf(f,_T("\n"));
}

// Calculates estimated tablesize, ie number of rows in table. always > 0 to prevent div by zero
void StatementTable::calculateTableSize() {
  m_tableSize = 0;
  for(UINT i = 0; i < m_indexStat.size(); i++)
    if(m_indexStat[i].m_stat.m_totalCount > m_tableSize)
      m_tableSize = m_indexStat[i].m_stat.m_totalCount;
  if(m_tableSize == 0)
    m_tableSize = 100;
}

StatementTable::StatementTable(SqlCompiler &compiler, SyntaxNode *n) : m_compiler(compiler)
                                                                     , TableDefinition(compiler.m_db.getTableDefinition(n->name())) {
  m_fixedByConst = NULL;
  m_node         = n;
  m_tableSize    = 100;
  try {
    m_fixedByConst = new BitSet(getColumnCount());

    const TCHAR *tableName = n->name();
    const IndexArray &indexArray = m_compiler.m_db.getIndexDefinitions(tableName);
    for(UINT i = 0; i < indexArray.size(); i++) {
      IndexStatistic ist;
      ist.m_indexDef = indexArray[i];
      if(!m_compiler.m_db.sysTabStatRead(ist.m_indexDef.m_indexName,ist.m_stat)) {
        defaultstat(ist.m_stat,ist.m_indexDef);
      }
      m_indexStat.add(ist);
    }
    calculateTableSize();
  } catch(sqlca ca) {
    m_compiler.syntaxError(n,ca.sqlcode,_T("%s"),ca.sqlerrmc);
  }
}

StatementTable::~StatementTable() {
  if(m_fixedByConst)
    delete m_fixedByConst;
}

FromTable::FromTable(SelectStmt &belongsto, SyntaxNode *n, const TCHAR *correlationname) : StatementTable(belongsto.m_compiler,n), m_belongsTo(belongsto) {
  m_correlationName  = correlationname;
  m_joinSequence     = -1;
  m_usedIndex        = 0;
  m_fixed            = false;
  m_selectivity      = 1;
  m_indexSelectivity = 1;
  m_readOperator     = -1;
  m_cursorReg        = -1;
  m_keyPredicates.m_beginKeyPredicate.m_fromTable = this;
  m_keyPredicates.m_endKeyPredicate.m_fromTable = this;
  for(UINT i = 0; i < getColumnCount(); i++)
    m_attributes.add(ColumnAttributes()); // no columns used yet
  m_countAggr = false;
}

int FromTable::getNumberOfUsedColumns() const {
  int count = 0;
  size_t n = m_attributes.size();
  for(size_t i = 0; i < n; i++)
    if(m_attributes[i].used()) count++;
  return count;
}

void FromTable::dump(FILE *f, int level) const {

  findent(f,level); _ftprintf(f,_T("[%s] [%s] joinseq:%d\n"),getTableName().cstr(),m_correlationName.cstr(),m_joinSequence);
  int fixedbyindex = isFixedByUniqueKey();
  if(fixedbyindex >= 0) {
    findent(f,level);
    _ftprintf(f,_T("Fixed by unique index %s\n"),m_indexStat[fixedbyindex].m_indexDef.m_indexName.cstr());
  }
  else {
    if(m_usedIndex >= 0) {
      findent(f,level);
      if(m_indexStat.size() == 0)
        _ftprintf(f,_T("No index exists "));
      else {
        _ftprintf(f,_T("Accesspath:%s : %s"),getUsedIndex().m_indexDef.m_indexName.cstr(),m_asc ? _T("ASC") : _T("DESC"));
        if(getUsedIndex().m_indexOnly) _ftprintf(f,_T(" Index only"));
      }
      _ftprintf(f,_T("\n"));
      findent(f,level);
      _ftprintf(f,_T("Expect to select %lg%% of the rows after read %lg%% of the table\n")
               ,m_selectivity*100
               ,m_indexSelectivity*100
             );
    }
  }
  findent(f,level); _ftprintf(f,_T("Readoperator:%d cursorreg:%d\n"),m_readOperator, m_cursorReg);
  findent(f,level); _ftprintf(f,_T("Attributes:\n"));
  if(m_countAggr) { findent(f,level); _ftprintf(f,_T("countaggr\n")); }
  for(UINT c = 0; c < m_attributes.size(); c++) {
    const ColumnAttributes &attr = m_attributes[c];
    findent(f,level+2);
    _ftprintf(f,_T("%-15s:"),getColumn(c).m_name.cstr());
    if(m_fixedByConst->contains(c)) _ftprintf(f,_T("fixed "));
    attr.dump(this,f);
  }

  m_keyPredicates.dump(f);
}

void OrderByExpression::dump(FILE *f) const {
  _ftprintf(f,_T("<expr> %s"), m_asc ? _T("asc") : _T("desc"));
}

void UniqueSelectStmtArray::add(SelectStmt *stmt) {
  for(UINT i = 0; i < size(); i++)
    if((*this)[i] == stmt) return; // got it already
  SelectStmtArray::add(stmt);
}

SelectExpressionList::SelectExpressionList() {
}

SelectExpressionList::SelectExpressionList(const NodeList &list) {
  for(UINT i = 0; i < list.size(); i++) {
    add(list[i]);
  }
}

void SelectExpressionList::dump(const SqlCompiler &compiler, FILE *f, int level) const {
  for(UINT i = 0; i < size(); i++) {
    _ftprintf(f,_T("%*.*s%-8s %s\n"),level,level,EMPTYSTRING,getMainTypeString((*this)[i].m_type),(*this)[i].m_expr->toString().cstr());
  }
}

SelectStmt::SelectStmt(SqlCompiler        &compiler,
                       SyntaxNode        *n       ,  // n = <SelectStmt>
                       SelectStmt        *parent  ,
                       SelectStmtPurpose  purpose ,
                       int                outputpipe
                         ) : SelectSetOperator(compiler,purpose, n)
                           , m_selectExprList(NodeList(n->child(1))) {
  // no validation or code-generation in constructor. !!!

  SyntaxNode *p;

  if((p = n->findChild(INTO))   != NULL && p->childCount() >= 1) m_selectIntoList.findNodes(p->child(0));
  if((p = n->findChild(FROM))   != NULL && p->childCount() >= 1) m_fromList.findNodes(      p->child(0));
  if((p = n->findChild(WHERE))  != NULL && p->childCount() >= 1)
    m_whereClause  = p->child(0);
  else
    m_whereClause  = NULL;
  if((p = n->findChild(HAVING)) != NULL && p->childCount() >= 1)
    m_havingClause = p->child(0);
  else
    m_havingClause = NULL;

  if((p = n->findChild(GROUP))  != NULL && p->childCount() >= 1) m_groupByList.findNodes(   p->child(0));
  if((p = n->findChild(ORDER))  != NULL && p->childCount() >= 1) m_orderByList.findNodes(   p->child(0));
  m_parent          = parent;
  m_purpose         = purpose;
  m_noOfFixedTables = 0;
  m_outputPipe      = outputpipe;
  m_noOfLike        = 0;
  if(parent != NULL) {
    parent->m_subSelects.add(this);
    m_name = format(_T("%s.%d"),parent->m_name.cstr(),parent->m_subSelects.size());
  }
  else
    m_name = _T("1");
  // no validation or code-generation in constructor. !!!
}

SelectStmt::~SelectStmt() {
  for(UINT i = 0; i < m_fromTable.size(); i++)
    delete m_fromTable[i];
}

void SelectStmt::syntaxError(const SyntaxNode *n, long sqlcode, TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  m_compiler.vSyntaxError(n,sqlcode,format,argptr);
  va_end(argptr);
}

void SelectStmt::expandStar(SelectExpressionList &expandList, const FromTable *ft) {
  for(UINT i = 0; i < ft->getColumnCount(); i++) {
    SyntaxNode *p = m_compiler.fetchTokenNode(DOT,
                      m_compiler.fetchTokenNode(NAME,ft->m_correlationName.cstr()),
                      m_compiler.fetchTokenNode(NAME,ft->getColumn(i).m_name.cstr()),
                      NULL);
    expandList.add(p);
  }
}

void SelectStmt::findColumnName(SyntaxNode *n) { // n = <name>
  bool found    = false;
  int colIndex, ftindex;
  for(UINT i = 0; i < m_fromTable.size(); i++) {
    int index = m_fromTable[i]->findColumnIndex(n->name());
    if(index >= 0) { // found
      if(found) {    // already found => ambigous name
        syntaxError(n,SQL_AMBIGOUS_COLUMNNAME,_T("Column <%s> is ambigous"),n->name());
      }
      else {
        ftindex  = i;
        colIndex = index;
        found    = true;
      }
    }
  }
  if(!found)
    syntaxError(n,SQL_INVALID_COLUMNNAME,_T("Column <%s> not defined"),n->name());
  else
    m_ssiMap.append(*m_fromTable[ftindex],colIndex,n);
}

FromTable *SelectStmt::findCorrelationName(SyntaxNode *n, bool searchParent) { // n == <name>
  for(UINT i = 0; i < m_fromTable.size(); i++)
    if(m_fromTable[i]->m_correlationName.equalsIgnoreCase(n->name()))
      return m_fromTable[i];
  if(m_parent && searchParent) {
    FromTable *ft = m_parent->findCorrelationName(n,searchParent);
    if(ft) {
      ft->m_belongsTo.m_corSubSelects.add(this);
      m_usedParentSelects.add(&ft->m_belongsTo);
    }
    return ft;
  }
  syntaxError(n,SQL_INVALID_CORRELATION_NAME,_T("Correlationname <%s> not defined"),n->name());
  return NULL;
}

void SelectStmt::findCorrelationTable(SyntaxNode *n) { // n = <expr>
  switch(n->token()) {
  case DOT :
    { FromTable *ft = findCorrelationName(n->child(0),true);
      if(ft) {
        SyntaxNode *column = n->child(1);
        int index = ft->findColumnIndex(column->name());
        if(index < 0)
          syntaxError(column,SQL_INVALID_COLUMNNAME,_T("Column <%s> not defined"),column->name());
        else {
          m_ssiMap.append(*ft,index,column);
        }
      }
      break;
    }

  case NAME:
    findColumnName(n);
    break;
  default:
    stopcomp(n);
    break;
  }
}

const SyntaxNode *SelectStmt::findUnAggregatedSyntaxNode(const SyntaxNode *expr, const FromTable *table, int colIndex) {
  switch(expr->token()) {
  case DOT    :
  case NAME   :
    { const SelectSymbolInfo *ssi = getInfo(expr);
      if(&ssi->m_fromTable == table && ssi->m_colIndex == colIndex)
        return expr;
      else
        return NULL;
      break;
    }
  case NUMBER :
  case STRING :
  case HOSTVAR:
  case PARAM  :
    return NULL;
  case SUM    :
  case COUNT  :
  case MIN    :
  case MAX    :
    return NULL;

  default     :
    { int sons = expr->childCount();
      for(int i = 0; i < sons; i++) {
        const SyntaxNode *n = findUnAggregatedSyntaxNode(expr->child(i),table,colIndex);
        if(n) return n;
      }
    }
    break;
  }
  return NULL;
}

const SyntaxNode *SelectStmt::findUnAggregatedSelectNode(const FromTable *table, int colIndex) {
  for(UINT i = 0; i < m_selectExprList.size(); i++) {
    const SyntaxNode *n = findUnAggregatedSyntaxNode(m_selectExprList[i].m_expr,table,colIndex);
    if(n) return n;
  }
  stopcomp(m_selectExprList[0].m_expr); // should not come here
  return NULL;
}

const SyntaxNode *SelectStmt::findUnAggregatedHavingNode(const FromTable *table, int colIndex) {
  const SyntaxNode *n = findUnAggregatedSyntaxNode(m_havingClause,table,colIndex);
  if(n) return n;
  stopcomp(m_havingClause); // should not come here
  return NULL;
}

const SyntaxNode *SelectStmt::findUnAggregatedOrderByNode(const FromTable *table, int colIndex) {
  for(UINT i = 0; i < m_orderBy.size(); i++) {
    const SyntaxNode *n = findUnAggregatedSyntaxNode(m_orderBy[i].m_expr,table,colIndex);
    if(n) return n;
  }
  stopcomp(m_orderBy[0].m_expr); // should not come here
  return NULL;
}

bool SelectStmt::hasAggregation() const {
  for(UINT i = 0; i < m_fromTable.size(); i++) {
    const FromTable *table = m_fromTable[i];
    if(table->m_countAggr) return true; // there is a count(*)
    for(UINT c = 0; c < table->m_attributes.size(); c++) { // check all columns
      if(table->m_attributes[c].m_selAggr) return true;
    }
  }
  return false;
}

void SelectStmt::selectExpandStar(UINT i ) {
  SyntaxNode *n = m_selectExprList[i].m_expr; // n = <select_elem>
  switch(n->token()) {
  case STAR:
    { SelectExpressionList expandList,newlist;
      for(UINT j = 0; j < m_fromTable.size(); j++)
        expandStar(expandList,m_fromTable[j]);
      for(UINT j = 0; j < i; j++)
        newlist.add(m_selectExprList[j]);
      for(UINT j = 0; j < expandList.size(); j++)
        newlist.add(expandList[j]);
      for(UINT j = i+1; j < m_selectExprList.size(); j++)
        newlist.add(m_selectExprList[j]);
      m_selectExprList = newlist;
    }
    break;
  case DOT :
    { if(n->child(1)->token() == STAR) { // name.star expand
        SyntaxNode *correlation = n->child(0);
        FromTable *ft = findCorrelationName(correlation,false);
        if(ft) {
          SelectExpressionList expandList,newlist;
          expandStar(expandList,ft);
          for(UINT j = 0; j < i; j++)
            newlist.add(m_selectExprList[j]);
          for(UINT j = 0; j < expandList.size(); j++)
            newlist.add(expandList[j]);
          for(UINT j = i+1; j < m_selectExprList.size(); j++)
            newlist.add(m_selectExprList[j]);
          m_selectExprList = newlist;
        }
      }
    }
    break;

  default:
    break;
  }
}

void SelectStmt::getTableDefinition(SyntaxNode *n) { // n = <FromTable>
  switch(n->token()) {
  case DOT:
    m_fromTable.add(new FromTable(*this,n->child(0),n->child(1)->name()));
    break;
  case NAME:
    m_fromTable.add(new FromTable(*this,n,n->name()));
    break;

  default:
    stopcomp(n);
  }
}

void SelectStmt::checkTypeIsNumeric(SyntaxNode *expr,bool inaggr) {
  switch(checkExpressionType(expr,inaggr)) {
  case MAINTYPE_NUMERIC:
  case MAINTYPE_VOID:
    break; // ok
  default:
    syntaxError(expr,SQL_INVALID_EXPR_TYPE,_T("Expression not numeric"));
  }
}

void SelectStmt::checkTypeIsString(SyntaxNode *expr,bool inaggr) {
  switch(checkExpressionType(expr,inaggr)) {
  case MAINTYPE_STRING :
  case MAINTYPE_VARCHAR:
  case MAINTYPE_VOID   :
    break; // ok
  default:
    syntaxError(expr,SQL_INVALID_EXPR_TYPE,_T("Expression not type String"));
  }
}

void SelectStmt::checkCountArgument(SyntaxNode *expr) {
  switch(expr->token()) {
  case STAR:
    { for(UINT i = 0; i < m_fromTable.size(); i++)
        m_fromTable[i]->m_countAggr = true;
      return;
    }
  default  :
    (void)checkExpressionType(expr->child(0),true);
  }
}

void SelectStmt::checkIsOuterRef(SyntaxNode *expr) {
  switch(expr->token()) {
  case STAR :
    break;
  case PLUS :
  case MINUS:
  case MULT :
  case DIVOP:
  case MODOP:
  case EXPO :
    checkIsOuterRef(expr->child(0));
    if(expr->childCount() == 2)
      checkIsOuterRef(expr->child(1));
    break;
  case NUMBER :
  case STRING :
  case HOSTVAR:
    break;
  case DOT    :
  case NAME   :
    { SelectSymbolInfo *ssi = getInfo(expr);
      if(ssi != NULL)
        if(&(ssi->m_fromTable.m_belongsTo) == this)
          aggrInWhereError(expr);
        else // != this
          if(ssi->m_fromTable.m_belongsTo.m_currentClause != HAVING)
            aggrInWhereError(expr);
      break;
    }
  case SUBSTRING:
    checkIsOuterRef(expr->child(0));
    checkIsOuterRef(expr->child(1));
    checkIsOuterRef(expr->child(2));
    break;
  case CONCAT:
    checkIsOuterRef(expr->child(0));
    checkIsOuterRef(expr->child(1));
    break;
  default:
    stopcomp(expr);
  }
}

void SelectStmt::checkAggrInWhere(SyntaxNode *expr) {
  if(m_purpose == MAINSELECT)
    aggrInWhereError(expr);
  else { // this is a subquery. check that all columns in expr are outer references
    switch(expr->token()) {
    case COUNT:
      checkIsOuterRef(expr->child(0));
      break;
    case MIN  :
    case MAX  :
    case SUM  :
      checkIsOuterRef(expr->child(1));
      break;
    }
  }
}

void SelectStmt::aggrInWhereError(SyntaxNode *expr) {
  syntaxError(expr,SQL_AGGR_IN_WHERE,
    _T("An aggregate may not appear in the WHERE clause unless it is in a subquery "
    "contained in a HAVING clause or a select list, and the column being aggregated "
    "is an outer reference")
  );
}

void SelectStmt::aggrInAggrError(SyntaxNode *expr) {
  syntaxError(expr,SQL_AGGR_IN_AGGR,
    _T("Cannot perform an aggregate function on an expression containing an "
       "aggregate or a subquery")
  );
}

DbMainType SelectStmt::checkExpressionType(SyntaxNode *expr, bool inaggr) {
  DbMainType m1,m2,m3;
  switch(expr->token()) {
  case MULT :
  case DIVOP:
  case MODOP:
  case EXPO :
    checkTypeIsNumeric(expr->child(0),inaggr);
    if(expr->childCount() == 2)
      checkTypeIsNumeric(expr->child(1),inaggr);
    return MAINTYPE_NUMERIC;
  case PLUS :
    switch(checkExpressionType(expr->child(0),inaggr)) {
    case MAINTYPE_NUMERIC  :
      m2 = checkExpressionType(expr->child(1),inaggr);
      if(m2 != MAINTYPE_NUMERIC && m2 != MAINTYPE_VOID)
        syntaxError(expr->child(1),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
      return MAINTYPE_NUMERIC;

    case MAINTYPE_STRING   :
    case MAINTYPE_VARCHAR  :
      syntaxError(expr->child(0),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
      return MAINTYPE_NUMERIC;

    case MAINTYPE_DATE     :
      switch(checkExpressionType(expr->child(1),inaggr)) {
      case MAINTYPE_NUMERIC:
        return MAINTYPE_DATE;

      case MAINTYPE_STRING :
      case MAINTYPE_VARCHAR:
        syntaxError(expr->child(0),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
        return MAINTYPE_DATE;

      case MAINTYPE_DATE   :
        syntaxError(expr->child(0),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
        return MAINTYPE_DATE;

      case MAINTYPE_VOID   :
        return MAINTYPE_VOID;
      }
      break;

    case MAINTYPE_VOID     :
      m2 = checkExpressionType(expr->child(1),inaggr);
      if(m2 != MAINTYPE_NUMERIC && m2 != MAINTYPE_VOID)
        syntaxError(expr->child(0),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
      return MAINTYPE_VOID;
    }
    break;
  case MINUS               :
    if(expr->childCount() < 2) {
      switch(checkExpressionType(expr->child(0),inaggr)) {
      case MAINTYPE_NUMERIC:
        return MAINTYPE_NUMERIC;

      case MAINTYPE_STRING :
      case MAINTYPE_VARCHAR:
        syntaxError(expr->child(0),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
        return MAINTYPE_NUMERIC;

      case MAINTYPE_DATE   :
        syntaxError(expr,SQL_INVALID_EXPR_TYPE,_T("Unary minus not allowed on Date"));
        return MAINTYPE_DATE;

      case MAINTYPE_VOID   :
        return MAINTYPE_VOID;
      }
    }
    else {
      switch(checkExpressionType(expr->child(0),inaggr)) {
      case MAINTYPE_NUMERIC:
        if(checkExpressionType(expr->child(1),inaggr) != MAINTYPE_NUMERIC)
          syntaxError(expr->child(1),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
        return MAINTYPE_NUMERIC;

      case MAINTYPE_STRING :
      case MAINTYPE_VARCHAR:
        syntaxError(expr->child(0),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
        return MAINTYPE_NUMERIC;

      case MAINTYPE_DATE   :
        switch(checkExpressionType(expr->child(1),inaggr)) {
        case MAINTYPE_NUMERIC:
          return MAINTYPE_DATE;

        case MAINTYPE_STRING :
        case MAINTYPE_VARCHAR:
          syntaxError(expr->child(0),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
          return MAINTYPE_DATE;

        case MAINTYPE_DATE   :
          return MAINTYPE_NUMERIC;

        case MAINTYPE_VOID   :
          return MAINTYPE_VOID;
        }
        break;

      case MAINTYPE_VOID:
        return MAINTYPE_VOID;
      }
    }
    break;

  case NUMBER        :
    return MAINTYPE_NUMERIC;
  case STRING        :
    return MAINTYPE_STRING;
  case DATECONST     :
    return MAINTYPE_DATE;
  case TIMECONST     :
    return MAINTYPE_TIME;
  case TIMESTAMPCONST:
    return MAINTYPE_TIMESTAMP;
  case HOSTVAR       :
    { int hostVarIndex = expr->getHostVarIndex();

//      _tprintf(_T("hostvar[%d] type:%s (numberofinput:%d\n"),
//        hostVarIndex,
//        getTypeString(m_compiler.m_bndstmt.m_inHost[hostVarIndex].type),
//        m_compiler.m_bndstmt.m_stmtHead.m_ninput
//      );

      if(hostVarIndex > (int)m_compiler.m_bndstmt.m_stmtHead.m_ninput) {
        hostVarIndex -= m_compiler.m_bndstmt.m_stmtHead.m_ninput;
        return getMainType(m_compiler.m_bndstmt.m_outHost[hostVarIndex].getType());
      }
      else
        return getMainType(m_compiler.m_bndstmt.m_inHost[hostVarIndex].getType());
    }
    break;
  case DOT          :
    findCorrelationTable(expr);
    return checkExpressionType(expr->child(1),inaggr); // son(2) contains the ssi.

  case NAME         :
    { if(getInfo(expr) == NULL) findCorrelationTable(expr);
      SelectSymbolInfo *ssi = getInfo(expr);
      if(ssi == NULL) return MAINTYPE_VOID;
      return getMainType(ssi->getType());
    }
  case SUBSTRING    :
    checkTypeIsString( expr->child(0),inaggr);
    checkTypeIsNumeric(expr->child(1),inaggr);
    checkTypeIsNumeric(expr->child(2),inaggr);
    return MAINTYPE_STRING;
  case CONCAT       :
    checkTypeIsString( expr->child(0),inaggr);
    checkTypeIsString( expr->child(1),inaggr);
    return MAINTYPE_STRING;
  case TYPEDATE     :
    m1 = checkExpressionType(expr->child(0),inaggr);
    m2 = checkExpressionType(expr->child(1),inaggr);
    m3 = checkExpressionType(expr->child(2),inaggr);
    if(m1 != MAINTYPE_NUMERIC && m1 != MAINTYPE_VOID)
      syntaxError(expr->child(0),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
    if(m2 != MAINTYPE_NUMERIC && m2 != MAINTYPE_VOID)
      syntaxError(expr->child(1),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
    if(m3 != MAINTYPE_NUMERIC && m3 != MAINTYPE_VOID)
      syntaxError(expr->child(2),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
    return MAINTYPE_DATE;

  case COUNT        :
    if(inaggr) { aggrInAggrError(expr); return MAINTYPE_NUMERIC; }
    checkCountArgument(expr->child(0));
    if(m_currentClause == WHERE) checkAggrInWhere(expr);
    return MAINTYPE_NUMERIC;

  case MIN          :
  case MAX          :
    { if(inaggr) { aggrInAggrError(expr); return MAINTYPE_VOID; }
      DbMainType ret = checkExpressionType(expr->child(1),true);
      if(m_currentClause == WHERE) checkAggrInWhere(expr);
      return ret;
    }
  case SUM          :
    if(inaggr) { aggrInAggrError(expr); return MAINTYPE_NUMERIC; }
    checkTypeIsNumeric(expr->child(1),true);
    if(m_currentClause == WHERE) checkAggrInWhere(expr);
    return MAINTYPE_NUMERIC;
  case NULLVAL      :
    return MAINTYPE_VOID;
  case CURRENTDATE  :
    return MAINTYPE_DATE;
  case CURRENTTIME  :
    return MAINTYPE_TIME;
  case CURRENTTIMESTAMP:
    return MAINTYPE_TIMESTAMP;
  case CAST         :
    { checkExpressionType(expr->child(0),inaggr);
      CastParameter p = m_compiler.genCastParameter(expr->child(1));
      return getMainType(p.getType());
    }
  case UNION        :
  case INTERSECT    :
  case SETDIFFERENCE:
  case SELECT       :
    return checkSubSelect(SUBSELECT_EXPRESSION,expr);
  }
  stopcomp(expr);
  return MAINTYPE_VOID; // just to make compiler happy
}

void SelectStmt::checkExpression(SyntaxNode *expr) {
  (void)checkExpressionType(expr,false);
}

void SelectStmt::markColumnAttributes(int clause, const SyntaxNode *expr, bool inaggr) {
  switch(expr->token()) {
  case DOT    :
  case NAME   :
    { SelectSymbolInfo *ssi = getInfo(expr);
      if(ssi) { // may be null if there was a syntaxError
        ColumnAttributes &attr = ssi->getAttributes();
        attr.setAttribute(clause,inaggr,true);
      }
    }
    break;
  case COUNT  :
  case MIN    :
  case MAX    :
  case SUM    :
    { int sons = expr->childCount();
      for(int i = 0; i < sons; i++)
        markColumnAttributes(clause, expr->child(i),true);
    }
    break;
  case NUMBER :
  case STRING :
  case HOSTVAR:
  case PARAM  :
    break;
  default     :
    { int sons = expr->childCount();
      for(int i = 0; i < sons; i++)
        markColumnAttributes(clause, expr->child(i),inaggr);
    }
  }
}

void SelectStmt::clearColumnAttributes(int clause, bool inaggr) {
  for(UINT i = 0; i < m_fromTable.size(); i++) {
    FromTable *table = m_fromTable[i];
    for(UINT j = 0; j < table->m_attributes.size(); j++)
      table->m_attributes[j].setAttribute(clause,inaggr,false);
  }
}

void SelectStmt::checkExpressionList(NodeList &left, NodeList &right) {
  if(left.size() != right.size()) {
    syntaxError(right[0],SQL_INVALID_EXPRLIST,_T("Invalid number of expressions"));
    return;
  }
  for(UINT i = 0; i < left.size(); i++) {
    DbMainType mtleft  = checkExpressionType(left[i] ,false);
    DbMainType mtright = checkExpressionType(right[i],false);
    if(!isCompatibleType(mtleft,mtright))
      syntaxError(right[i],SQL_INVALID_EXPR_TYPE,_T("Type mismatch in expression"));
  }
}

static SelectStmtPurpose getStmtPurpose(SyntaxNode *expr) {
  switch(expr->token()) {
  case UNION        : return UNION_PART;
  case INTERSECT    : return INTERSECT_PART;
  case SETDIFFERENCE: return DIFFERENCE_PART;
  default           : stopcomp(expr);
  }
  return MAINSELECT;
}

void SelectStmt::checkSetExpr(SyntaxNode *expr, DbMainType exprtype, bool neg) { // expr = <set_expr>
  switch(expr->token()) {
  case UNION        :
  case INTERSECT    :
  case SETDIFFERENCE:
    { SelectStmtPurpose purpose = getStmtPurpose(expr);
      SelectSetOperator *s1 = checkSubSelectOperator(purpose, expr->child(0));
      SelectSetOperator *s2 = checkSubSelectOperator(purpose, expr->child(1));
      SelectSetOperator *s  = new SelectSetOperator(m_compiler, neg ? SUBSELECT_NOT_IN_SET : SUBSELECT_IN_SET, expr, s1, s2);
      s->checkCompatibleSelectLists();
      if(!isCompatibleType(s->getExpressionType(0),exprtype))
        syntaxError(expr,SQL_INVALID_EXPR_TYPE,_T("Type mismatch in expression"));
      break;
    }
  case SELECT       :
    { SelectStmtPurpose purpose = neg ? SUBSELECT_NOT_IN_SET : SUBSELECT_IN_SET;
      if(!isCompatibleType(checkSubSelect(purpose,expr),exprtype))
        syntaxError(expr,SQL_INVALID_EXPR_TYPE,_T("Type mismatch in expression"));
      break;
    }
  default:
    { NodeList exprlist(expr);
      for(UINT i = 0; i < exprlist.size(); i++)
        if(!isCompatibleType(checkExpressionType(exprlist[i],false),exprtype))
          syntaxError(exprlist[i],SQL_INVALID_EXPR_TYPE,_T("Type mismatch in expression"));
    }
  }
}

SelectSetOperator *SelectStmt::checkSubSelectOperator(SelectStmtPurpose purpose, SyntaxNode *expr, bool allowmany) {
  switch(expr->token()) {
  case UNION        :
  case INTERSECT    :
  case SETDIFFERENCE:
    { SelectStmtPurpose purpose = getStmtPurpose(expr);
      SelectSetOperator *s1 = checkSubSelectOperator(purpose, expr->child(0),allowmany);
      SelectSetOperator *s2 = checkSubSelectOperator(purpose, expr->child(1),allowmany);
      SelectSetOperator *s  = new SelectSetOperator(m_compiler, purpose, expr, s1, s2);
      if(m_compiler.ok())
        s->checkCompatibleSelectLists();
      return s;
    }

  case SELECT       :
    { SelectStmt *subselect = new SelectStmt(m_compiler,expr,this,purpose,m_compiler.newPipe());
      subselect->typeCheck();
      if(m_compiler.ok()) {
        SelectExpressionList &subselectexpr = subselect->m_selectExprList;
        if(subselectexpr.size() != 1 && !allowmany)
          syntaxError(subselectexpr[0].m_expr,SQL_INVALID_EXPRLIST,_T("Only 1 expression allowed in subselect"));
      }
      return subselect;
    }
  default:
    stopcomp(expr);
  }
  return NULL;
}

DbMainType SelectStmt::checkSubSelect(SelectStmtPurpose purpose, SyntaxNode *expr) { // expr = SELECT, UNION, INTERSECT, SETDIFFERENCE
  SelectSetOperator *s = checkSubSelectOperator(purpose, expr);
  return m_compiler.ok() ? s->getExpressionType(0) : MAINTYPE_VOID;
}

void SelectStmt::checkPredicate(SyntaxNode *pred, bool neg) {
  if(pred == NULL) return;
  switch(pred->token()) {
  case OR :
  case AND:
    checkPredicate(pred->child(0));
    checkPredicate(pred->child(1));
    return;
  case NOT:
    checkPredicate(pred->child(0),!neg);
    return;
  case RELOPLE:
  case RELOPLT:
  case RELOPGE:
  case RELOPGT:
  case EQUAL  :
  case NOTEQ  :
    { NodeList left(pred->child(0));
      NodeList right(pred->child(1));
      checkExpressionList(left,right);
    }
    return;
  case EXISTS :
    { SelectStmtPurpose purpose = neg ? SUBSELECT_PURPOSE_NOT_EXIST : SUBSELECT_PURPOSE_EXIST;
      checkSubSelectOperator(purpose,pred->child(0),true);
    }
    return; // ??

  case BETWEEN:
    { NodeList middle(pred->child(0));
      NodeList lower(pred->child(1));
      NodeList upper(pred->child(2));
      checkExpressionList(middle,lower);
      checkExpressionList(middle,upper);
    }
    return;
  case INSYM  :
    { NodeList exprlist(pred->child(0)); // see comment in sql.y about shift/reduce-conflicts
      if(exprlist.size() != 1)
        syntaxError(exprlist[0],SQL_INVALID_EXPRLIST,_T("Only 1 expression allowed in set-predicate"));
      checkSetExpr(pred->child(1),checkExpressionType(exprlist[0],false),neg);
    }
    return;
  case LIKE   :
    { NodeList exprlist(pred->child(0)); // see comment in sql.y about shift/reduce-conflicts
      if(exprlist.size() != 1)
        syntaxError(exprlist[0],SQL_INVALID_EXPRLIST,_T("Only 1 expression allowed in like-predicate"));
      else {
        checkTypeIsString(exprlist[0] ,false);
        checkTypeIsString(pred->child(1),false);
        pred->setLikeOperatorIndex(m_noOfLike++);
      }
    }
    return;

  case ISNULL:
    { NodeList exprlist(pred->child(0)); // see comment in sql.y about shift/reduce-conflicts
      if(exprlist.size() != 1)
        syntaxError(exprlist[0],SQL_INVALID_EXPRLIST,_T("Only 1 expression allowed in is-predicate"));
      else {
        checkExpression(exprlist[0]);
      }
    }
    return;
  }
  stopcomp(pred);
}

void SelectStmt::checkSelectList() {
  if(m_purpose != MAINSELECT && m_selectIntoList.size() > 0) {
    syntaxError(m_selectIntoList[0],
         SQL_INVALID_INTO_LIST,_T("select into [hostvar-list] not allowed in subselect"));
    return;
  }
  bool usedcolumn = m_purpose != SUBSELECT_PURPOSE_EXIST
                 && m_purpose != SUBSELECT_PURPOSE_NOT_EXIST;

  if(m_compiler.ok())
    for(UINT i = 0; i < m_selectExprList.size(); i++) {
//    dumpSyntaxTree(m_selectExprList[i]);
      m_selectExprList[i].m_type = checkExpressionType(m_selectExprList[i].m_expr,false);
      markColumnAttributes(SELECT,m_selectExprList[i].m_expr);
    }

    // selectintolist.size == 0 || selectintolist.size == SelectExpressionList.size
  if(m_compiler.ok())
    if(m_selectIntoList.size() > 0) {
      if(m_selectIntoList.size() != m_selectExprList.size()) {
        SyntaxNode *p;
        if(m_selectIntoList.size() > m_selectExprList.size())
          p = m_selectIntoList[m_selectExprList.size()];
        else
          p = m_selectExprList[m_selectIntoList.size()].m_expr;
        syntaxError(p,SQL_INVALID_INTO_LIST,_T("Number of columns in INTO-clause doesn't match number of columns in selectlist"));
      }
      else {
        for(UINT i = 0; i < m_selectIntoList.size(); i++) {
          int hostVarIndex = m_selectIntoList[i]->getHostVarIndex();
          hostVarIndex -= m_compiler.m_bndstmt.m_stmtHead.m_ninput;
          if(hostVarIndex < 0) {
            syntaxError(m_selectIntoList[i],
              SQL_INVALID_INTO_LIST,_T("The index of this output-hostvar (%d) is less than the specified number of input-hostvars (%d) in the bnd-file"),
              m_selectIntoList[i]->getHostVarIndex(),m_compiler.m_bndstmt.m_stmtHead.m_ninput);
            break;
          }
          if(!isCompatibleType(m_selectExprList[i].m_type,getMainType(m_compiler.m_bndstmt.m_outHost[hostVarIndex].getType())))
            syntaxError(m_selectIntoList[i],
              SQL_INVALID_HOSTVAR_TYPE,_T("Invalid type of hostvar"));
        }
      }
    }
}

void SelectStmt::checkOrderBy() {
  for(UINT i = 0; i < m_orderByList.size(); i++) {
    SyntaxNode *by  = m_orderByList[i];
    SyntaxNode *n   = by->child(0);         // n = <order_elem>
    bool         asc = by->child(1)->token() == ASCENDING;
    if(n->token() == NUMBER) { // refers to select_element
      double v = n->number();
      if(v < 1 || v > m_selectExprList.size() || (v != floor(v))) {
        syntaxError(n,SQL_INVALID_ORDERBY,_T("Invalid order by"));
        continue;
      }
      int colIndex = (int)v - 1; // they are indexed from 0..count-1 in C++ but from 1..count in sql
      n = m_selectExprList[colIndex].m_expr;
    }
    else
      checkExpression(n);
    markColumnAttributes(ORDER,n);
    m_orderBy.add(OrderByExpression(n,asc));
  }
}

void SelectStmt::checkGroupBy() {
  for(UINT i = 0; i < m_groupByList.size(); i++) {
    SyntaxNode *n = m_groupByList[i]; // n == <group_elem>
    if(n->token() == NUMBER) { // refers to select_element
      double v = n->number();
      if(v < 1 || v > m_selectExprList.size() || (v != floor(v))) {
        syntaxError(n,SQL_INVALID_GROUPBY,_T("Invalid group by"));
        continue;
      }
      else {
        int colIndex = (int)v - 1; // they are indexed from 0..count-1 in C++ but from 1..count in sql
        n = m_selectExprList[colIndex].m_expr;
      }
    }
    else
      checkExpression(n);

    const SyntaxNode *p = findFirstAggregateFunction(n);
    if(p != NULL) {
      syntaxError(p,SQL_INVALID_GROUPBY,
        _T("Cannot use an aggregate or a subquery in an expression used for the group by"
           " list of a GROUP BY clause"));
    } else {
      switch(n->token()) {
      case DOT :
      case NAME:
        { SelectSymbolInfo *ssi = getInfo(n);
          if(ssi) { // may be null if there was a syntaxError
            ColumnAttributes &attr = ssi->getAttributes();
            attr.m_inGroupBy = true;
          }
          break;
        }
      default: // dont mark groupby attributes in expressions; only for columns alone in the groupby should be marked
        break;
      }
      m_groupBy.add(n);
    }
  }
}

const SyntaxNode *SelectStmt::findUnGroupedUnAggregatedColumn(const SyntaxNode *expr) {
  switch(expr->token()) {
  case DOT    :
  case NAME   :
    { SelectSymbolInfo *ssi = getInfo(expr);
      if(!ssi) return NULL; // may be null if there was a syntaxError
      ColumnAttributes &attr = ssi->getAttributes();
      return attr.m_inGroupBy ? NULL : expr;
    }

  case COUNT  :
  case MIN    :
  case MAX    :
  case SUM    :
    return NULL;
  case NUMBER :
  case STRING :
  case HOSTVAR:
  case PARAM  :
    return NULL;
  default     :
    { UINT n = expr->childCount();
      for(UINT i = 0; i < n; i++) {
        const SyntaxNode *p = findUnGroupedUnAggregatedColumn(expr->child(i));
        if(p) return p;
      }
      return NULL;
    }
  }
  return NULL;
}

bool SelectStmt::isTreeEqual(const SyntaxNode *t1, const SyntaxNode *t2) const {
  if(t1 != NULL && t2 == NULL) return false;
  if(t1 == NULL && t2 != NULL) return false;
  if(t1->token() != t2->token()) return false;
  switch(t1->token()) {
  case DOT    :
  case NAME   :
    { SelectSymbolInfo *ssi1 = getInfo(t1);
      SelectSymbolInfo *ssi2 = getInfo(t2);
      if(ssi1 == NULL || ssi2 == NULL) return false; // may be null if there was a syntaxError
      return (*ssi1 == *ssi2);
    }
  case NUMBER :
    return t1->number() == t2->number();
  case STRING :
    return _tcscmp(t1->str(),t2->str()) == 0;         // _tcscmp here
  case HOSTVAR:
    return _tcscmp(t1->hostvar(),t2->hostvar()) == 0; // _tcscmp here
  case PARAM  :
    return false; // ???? what to do here
  default     :
    { UINT n1 = t1->childCount();
      UINT n2 = t2->childCount();
      if(n1 != n2) return false;
      for(UINT i = 0; i < n1; i++)
        if(!isTreeEqual(t1->child(i),t2->child(i))) return false;
    }
  }
  return true;
}

bool SelectStmt::isInGroupBy(const SyntaxNode *expr) const {
  for(UINT i = 0; i < m_groupBy.size(); i++)
    if(isTreeEqual(m_groupBy[i],expr))
      return true;
  return false;
}

void SelectStmt::checkGroupedOrAggregatedExpression(const SyntaxNode *expr, const TCHAR *clause) {
  const SyntaxNode *tmpnode;
  if((tmpnode = findUnGroupedUnAggregatedColumn(expr)) && !isInGroupBy(expr)) {
    SelectSymbolInfo *ssi = getInfo(tmpnode);
    syntaxError(
        tmpnode
       ,SQL_NO_AGGR_NO_GROUPBY
       ,_T("Column '%s.%s' is invalid in the %s because it "
           "is not contained in either an aggregate function or the GROUP BY clause")
       ,ssi->m_fromTable.m_correlationName.cstr()
       ,ssi->m_fromTable.getColumn(ssi->m_colIndex).m_name.cstr()
       ,clause
    );
  }
}

void SelectStmt::checkGroupedOrAggregatedExpressionList(const NodeList &list, const TCHAR *clause) {
  for(UINT i = 0; i < list.size(); i++)
    checkGroupedOrAggregatedExpression(list[i],clause);
}

void SelectStmt::checkGroupedOrAggregatedPredicate(const SyntaxNode *pred, const TCHAR *clause) {
  switch(pred->token()) {
  case OR :
  case AND:
    checkGroupedOrAggregatedPredicate(pred->child(0),clause);
    checkGroupedOrAggregatedPredicate(pred->child(1),clause);
    break;
  case NOT:
    checkGroupedOrAggregatedPredicate(pred->child(0),clause);
    break;
  case RELOPLE:
  case RELOPLT:
  case RELOPGE:
  case RELOPGT:
  case EQUAL  :
  case NOTEQ  :
    if(pred->child(1)->token() == SELECT)
      checkGroupedOrAggregatedExpression(pred->child(0),clause);
    else {
      checkGroupedOrAggregatedExpressionList(NodeList(pred->child(0)),clause);
      checkGroupedOrAggregatedExpressionList(NodeList(pred->child(1)),clause);
    }
    break;
  case BETWEEN:
    checkGroupedOrAggregatedExpressionList(NodeList(pred->child(0)),clause);
    checkGroupedOrAggregatedExpressionList(NodeList(pred->child(1)),clause);
    checkGroupedOrAggregatedExpressionList(NodeList(pred->child(2)),clause);
    break;
  case INSYM  :
    checkGroupedOrAggregatedExpression(pred->child(0),clause);
    switch(pred->child(1)->token()) {
    case SELECT: // do nothing
      break;
    default:
      checkGroupedOrAggregatedExpressionList(NodeList(pred->child(1)),clause);
      break;
    }
    break;
  case LIKE   :
    checkGroupedOrAggregatedExpression(pred->child(0),clause);
    checkGroupedOrAggregatedExpression(pred->child(1),clause);
    break;;

  case ISNULL:
    checkGroupedOrAggregatedExpression(pred->child(0),clause);
    break;
  }
}

void SelectStmt::checkAggregation() {
  if(m_groupByList.size() == 0) {
    if(hasAggregation()) { // check that there is no unaggregated columns in the selectlist
      for(UINT i = 0; i < m_fromTable.size(); i++) {
        const FromTable *table = m_fromTable[i];
        for(UINT c = 0; c < table->m_attributes.size(); c++) {
          if(table->m_attributes[c].m_selUnaggr)
            syntaxError(
               findUnAggregatedSelectNode(table,c)
               ,SQL_NO_AGGR_NO_GROUPBY
               ,_T("Column '%s.%s' is invalid in the select list because it "
                   "is not contained in an aggregate function and there is no GROUP BY clause")
               ,table->m_correlationName.cstr()
               ,table->getColumn(c).m_name.cstr()
            );
          if(table->m_attributes[c].m_havUnaggr)
            syntaxError(
               findUnAggregatedHavingNode(table,c)
               ,SQL_NO_AGGR_NO_GROUPBY
               ,_T("Column '%s.%s' is invalid in the HAVING clause because it "
                   "is not contained in an aggregate function and there is no GROUP BY clause")
               ,table->m_correlationName.cstr()
               ,table->getColumn(c).m_name.cstr()
            );
          if(table->m_attributes[c].m_orderUnaggr)
            syntaxError(
               findUnAggregatedOrderByNode(table,c)
               ,SQL_NO_AGGR_NO_GROUPBY
               ,_T("Column '%s.%s' is invalid in the ORDER BY clause because it "
                   "is not contained in an aggregate function and there is no GROUP BY clause")
               ,table->m_correlationName.cstr()
               ,table->getColumn(c).m_name
            );
        }
      }
    }
  }
  else { // groupbylist.size > 0
    UINT i;
    for(i = 0; i < m_selectExprList.size(); i++)
      checkGroupedOrAggregatedExpression(m_selectExprList[i].m_expr,_T("SELECT LIST"));
    if(m_havingClause != NULL)
      checkGroupedOrAggregatedPredicate(m_havingClause,_T("HAVING CLAUSE"));

    for(i = 0; i < m_orderBy.size(); i++)
      checkGroupedOrAggregatedExpression(m_orderBy[i].m_expr,_T("ORDER BY"));
  }
}

void SelectStmt::typeCheck() {
  UINT i,j;
  for(i = 0; i < m_fromList.size(); i++)
    getTableDefinition( m_fromList[i] );

  for(i = 0; i < m_fromTable.size(); i++) // check that correlationnames are unique
    for(j = i+1; j < m_fromTable.size(); j++)
      if(m_fromTable[i]->m_correlationName.equalsIgnoreCase(m_fromTable[j]->m_correlationName))
        syntaxError(m_fromTable[j]->m_node,SQL_DUPLICATE_CORRELATIONNAME,
             _T("Tablename or correlationname %s already used"),m_fromTable[j]->m_correlationName.cstr());

  if(!m_compiler.ok()) return;
  for(i = 0; i < m_selectExprList.size(); i++)
    selectExpandStar(i);

  if(!m_compiler.ok()) return;
  m_currentClause = SELECT;
  checkSelectList();

  if(!m_compiler.ok()) return;
  m_currentClause = ORDER;
  checkOrderBy();

  if(!m_compiler.ok()) return;
  m_currentClause = GROUP;
  checkGroupBy();

  if(!m_compiler.ok()) return;
  m_currentClause = WHERE;
  checkPredicate(m_whereClause);
  if(!m_compiler.ok()) return;
  m_currentClause = HAVING;
  checkPredicate(m_havingClause);

  if(!m_compiler.ok()) return;
  if(m_whereClause)
    markColumnAttributes(WHERE ,m_whereClause );
  if(!m_compiler.ok()) return;
  if(m_havingClause) {
    markColumnAttributes(HAVING,m_havingClause);
  }
  if(!m_compiler.ok()) return;
  checkAggregation();

#ifdef TRACECOMP
  if(!m_compiler.ok()) return;
  if(m_whereClause != NULL) {
    _tprintf(_T("unreduced where <%s>:\n"),m_name.cstr());
    dumpSyntaxTree(m_whereClause);
  }
  if(!m_compiler.ok()) return;
  if(m_havingClause != NULL) {
    _tprintf(_T("unreduced having <%s>:\n"),m_name.cstr());
    dumpSyntaxTree(m_havingClause);
  }
#endif

  if(!m_compiler.ok()) return;
  if(m_whereClause)
    m_whereClause  = m_compiler.reducePredicate(m_whereClause ,m_possibleWhere );
  if(!m_compiler.ok()) return;
  if(m_havingClause)
    m_havingClause = m_compiler.reducePredicate(m_havingClause,m_possibleHaving);

#ifdef TRACECOMP
  if(m_whereClause != NULL) {
    _tprintf(_T("reduced where <%s>:\n"),m_name.cstr());
    dumpSyntaxTree(m_whereClause);
    _tprintf(_T("where %s\n"),m_whereClause->toString().cstr());
    _tprintf(_T("possiblewhere:")); m_possibleWhere.dump();
  }
  if(m_havingClause != NULL) {
    _tprintf(_T("reduced having <%s>:\n"),m_name.cstr());
    dumpSyntaxTree(m_havingClause);
    _tprintf(_T("having %s\n"),m_havingClause->toString().cstr());
    _tprintf(_T("possiblehaving:")); m_possibleHaving.dump();
  }
#endif

  if(!m_compiler.ok()) return;
  findJoinSequence();

#ifdef TRACECOMP
  if(m_whereClause != NULL) {
    _tprintf(_T("reducedbykeypredicates where <%s>:\n"),m_name.cstr());
    dumpSyntaxTree(m_whereClause);
    _tprintf(_T("where %s\n"),m_whereClause->toString().cstr());
  }
  else {
    _tprintf(_T("no more whereclause\n"));
  }
#endif

  if(!m_compiler.ok()) return;
  clearColumnAttributes(WHERE,false);
  clearColumnAttributes(WHERE,true );

  if(m_whereClause)
    markColumnAttributes(WHERE ,m_whereClause );

#ifdef TRACECOMP
  for(i = 0; i < m_fromTable.size(); i++)
    m_fromTable[i]->dump();
#endif

}

static TCHAR *purposeString(SelectStmtPurpose purpose) {
  switch(purpose) {
  case MAINSELECT                 :return _T("Mainselect");
  case SUBSELECT_PURPOSE_EXIST    :return _T("Subselect Exist");
  case SUBSELECT_PURPOSE_NOT_EXIST:return _T("Subselect Not Exist");
  case SUBSELECT_IN_SET           :return _T("Subselect In set");
  case SUBSELECT_NOT_IN_SET       :return _T("Subselect Not In set");
  case SUBSELECT_EXPRESSION       :return _T("Subselect Expression");
  case UNION_PART                 :return _T("Part of a Union");
  case DIFFERENCE_PART            :return _T("Part of a Setdifference");
  case INTERSECT_PART             :return _T("Part of an Intersection");
  default                         :return _T("Unknown Purpose!!");
  }
}


void SelectStmt::dump(FILE *f, int level) const {
  UINT i;
  findent(f,level); _ftprintf(f,_T("select <%s>: purpose:%s\n"),m_name.cstr(),purposeString(m_purpose));
  if(m_usedParentSelects.size() > 0) {
    findent(f,level); _ftprintf(f,_T("used parents:"));
    for(i = 0; i < m_usedParentSelects.size(); i++)
      _ftprintf(f,_T("<%s>,"),m_usedParentSelects[i]->m_name.cstr());
    _ftprintf(f,_T("\n"));
  }
  if(m_corSubSelects.size() > 0) {
    findent(f,level); _ftprintf(f,_T("correlated subselects:"));
    for(i = 0; i < m_corSubSelects.size(); i++)
      _ftprintf(f,_T("<%s>,"),m_corSubSelects[i]->m_name.cstr());
    _ftprintf(f,_T("\n"));
  }
  m_selectExprList.dump(m_compiler,f,level+2);
  if(m_selectIntoList.size() > 0) {
    findent(f,level); _ftprintf(f,_T("into\n"));
    m_selectIntoList.dump(f,level+2);
  }

  findent(f,level); _ftprintf(f,_T("from:\n"));
  for(i = 0; i < m_fromTable.size(); i++)
    m_fromTable[i]->dump(f,level);

  if(m_whereClause != NULL) {
    findent(f,level); _ftprintf(f,_T("where:\n"));
    dumpSyntaxTree(m_whereClause,f,level+2);
  }
  if(m_havingClause != NULL) {
    findent(f,level); _ftprintf(f,_T("having:\n"));
    dumpSyntaxTree(m_havingClause,f,level+2);
  }
  if(m_groupBy.size() > 0) {
    findent(f,level); _ftprintf(f,_T("group:"));
    for(i = 0; i < m_groupBy.size(); i++)
      _ftprintf(f,_T("%s,"),m_groupBy[i]->toString().cstr());
    _ftprintf(f,_T("\n"));
  }
  if(m_orderBy.size() > 0) {
    findent(f,level); _ftprintf(f,_T("order:"));
    for(i = 0; i < m_orderBy.size(); i++) {
      m_orderBy[i].dump(f); _ftprintf(f,_T(","));
    }
    _ftprintf(f,_T("\n"));
  }
  findent(f,level); _ftprintf(f,_T("SSIMap:\n"));
//  m_ssiMap.dump(f,level+2);
  if(m_subSelects.size() != 0) {
    findent(f,level); _ftprintf(f,_T("subselects:%zd\n"),m_subSelects.size());
    for(i = 0; i < m_subSelects.size(); i++) {
      findent(f,level); _ftprintf(f,_T("subselect:\n"));
      m_subSelects[i]->dump(f,level+4);
    }
  }
}

RelationType token2relationtype(int reloptoken) {
  switch(reloptoken) {
  case EQUAL   : return RELOP_EQ;
  case RELOPLE : return RELOP_LE;
  case RELOPLT : return RELOP_LT;
  case RELOPGE : return RELOP_GE;
  case RELOPGT : return RELOP_GT;
  default      :
    throwSqlError(SQL_FATAL_ERROR,_T("Unknown reloptoken in token2relationtype:%d"),reloptoken);
  }
  return RELOP_EQ; // just to make compiler happy
}

ProjectMap::ProjectMap(FromTable &table) : m_table(table) {
  int counter = 0;
  for(UINT i = 0; i < table.m_attributes.size(); i++) {
    if(table.m_attributes[i].used()) {
      m_fieldSet.add(i);
      m_fieldMap.add(counter++);
    }
    else
      m_fieldMap.add(-1);
  }
}

void SelectStmt::setCurrentTupleReg(const ProjectMap &map, int tuplereg) {
  for(UINT i = 0; i < m_ssiMap.size(); i++) {
    SelectSymbolInfo *ssi = &m_ssiMap[i];
    if(&ssi->m_fromTable == &map.m_table) {
      ssi->m_currentTupleReg = tuplereg;
      ssi->m_currentTupleIndex = map.m_fieldMap[ssi->m_colIndex];
    }
  }
}

void SelectStmt::genScanOperator(FromTable &t, int outputpipe) {
  int start = m_compiler.m_code.currentCodeSize();
  m_compiler.m_operatorStart.add(start);
  TableCursorParam param;
  param.m_sequenceNo      = t.getSequenceNo();
  param.m_indexName       = t.getUsedIndex().m_indexDef.m_indexName;
  param.m_dir             = t.m_asc ? SORT_ASCENDING : SORT_DESCENDING;
  param.m_indexOnly       = t.getUsedIndex().m_indexOnly;
  param.m_beginFieldCount = (UINT)t.m_keyPredicates.m_beginKeyPredicate.m_expr.size();
  param.m_endFieldCount   = (UINT)t.m_keyPredicates.m_endKeyPredicate.m_expr.size();
  if(param.m_beginFieldCount == 0)
    param.m_beginRelOp = RELOP_TRUE;
  else
    param.m_beginRelOp = token2relationtype(t.m_keyPredicates.m_beginKeyPredicate.m_relOpToken);

  if(param.m_endFieldCount == 0)
    param.m_endRelOp = RELOP_TRUE;
  else
    param.m_endRelOp = token2relationtype(t.m_keyPredicates.m_endKeyPredicate.m_relOpToken);

  ProjectMap map(t);
  param.m_fieldSet = map.m_fieldSet;

  m_compiler.m_code.appendIns2(CODETUPINIT,0,param.m_beginFieldCount);
  for(UINT i = 0; i < param.m_beginFieldCount; i++) {
    int col = t.getUsedIndex().m_indexDef.getColumn(i).m_col;
    m_compiler.genExpression(t.m_keyPredicates.m_beginKeyPredicate.m_expr[i]);
    m_compiler.m_code.appendIns2(CODEPOPTUP,0,i);
  }

  m_compiler.m_code.appendIns2(CODETUPINIT,1,param.m_endFieldCount);
  for(UINT i = 0; i < param.m_endFieldCount; i++) {
    int col = t.getUsedIndex().m_indexDef.getColumn(i).m_col;
    m_compiler.genExpression(t.m_keyPredicates.m_endKeyPredicate.m_expr[i]);
    m_compiler.m_code.appendIns2(CODEPOPTUP,1,i);
  }

  Packer pack;
  pack << param;
//  param.dump();

  m_compiler.m_code.appendIns1(CODEPUSHADR,m_compiler.m_code.appendData(pack));
  m_compiler.m_code.appendIns1(CODEOPENCURSOR,0);

  m_compiler.m_code.appendIns2(CODETUPINIT,0,(UINT)param.m_fieldSet.size());
  m_compiler.m_code.appendIns2(CODETUPINIT,1,(UINT)m_selectExprList.size());

  int loopstart = m_compiler.m_code.appendIns2(CODEFETCHCURSOR,0,0);
  int breakaddr = m_compiler.m_code.appendIns1(CODEJMPNQ,0);

  setCurrentTupleReg(map,0);

  if(m_whereClause != NULL) {
    m_compiler.genPredicate(m_whereClause);

    m_compiler.m_code.appendIns1(CODECMPTRUE,loopstart);
    m_compiler.m_code.appendIns1(CODEJMPNQ,loopstart);
  }

  for(UINT i = 0; i < m_selectExprList.size(); i++) {
    m_compiler.genExpression(m_selectExprList[i].m_expr);
    m_compiler.m_code.appendIns2(CODEPOPTUP,1,i);
  }

  m_compiler.m_code.appendIns2(CODESENDTUP,1,outputpipe);

  m_compiler.m_code.appendIns1(CODEJMP,loopstart);
  int afterloop = m_compiler.m_code.appendIns1(CODESENDEOF,outputpipe);
  m_compiler.m_code.appendIns0(CODERETURN);
  m_compiler.m_code.fixins1(breakaddr,afterloop);
}

void SelectStmt::genJoinOperator(FromTable &t1, FromTable &t2, int outputpipe) {
  int start = m_compiler.m_code.currentCodeSize();
  m_compiler.m_operatorStart.add(start);

}

void SelectStmt::genJoinOperator(FromTable &t, int inputpipe, int outputpipe) {
  int start = m_compiler.m_code.currentCodeSize();
  m_compiler.m_operatorStart.add(start);
}

void SelectStmt::genNullOperator(int outputpipe) {
  int start = m_compiler.m_code.currentCodeSize();
  m_compiler.m_operatorStart.add(start);
  m_compiler.m_code.appendIns1(CODESENDEOF,outputpipe);
  m_compiler.m_code.appendIns0(CODERETURN);
}

void SelectStmt::genCode() {
  if(isEmptySelect()) {
    genNullOperator(m_outputPipe);
    return;
  }
  if(m_fromTable.size() == 1) {
    m_fromTable[0]->m_readOperator = 0;
    m_fromTable[0]->m_cursorReg    = 0;
    genScanOperator(*m_fromTable[0],m_outputPipe);
  }
  else {
    if(m_fromTable.size() == 2) {
      m_fromTable[0]->m_readOperator = 0;
      m_fromTable[0]->m_cursorReg    = 0;
      m_fromTable[1]->m_readOperator = 0;
      m_fromTable[1]->m_cursorReg    = 1;
      genJoinOperator(*m_fromTable[0],*m_fromTable[1],m_outputPipe);
    }
    else {
      m_fromTable[0]->m_readOperator = 0;
      m_fromTable[0]->m_cursorReg    = 0;
      m_fromTable[1]->m_readOperator = 0;
      m_fromTable[1]->m_cursorReg    = 1;
      int currentpipe = m_compiler.newPipe();
      genJoinOperator(*m_fromTable[0],*m_fromTable[1],currentpipe);
      UINT n = (UINT)m_fromTable.size();
      for(UINT i = 2; i < n; i++) {
        m_fromTable[i]->m_readOperator = i - 1;
        m_fromTable[i]->m_cursorReg    = 0;
        int currentoutputpipe = (i == n - 1) ? m_outputPipe : m_compiler.newPipe();
        genJoinOperator(*m_fromTable[i],currentpipe,currentoutputpipe);
        currentpipe = currentoutputpipe;
      }
    }
  }
}

SelectSetOperator *SqlCompiler::genSimpleSelect(SyntaxNode *n) { // n == <simple_select>
  int outputpipe = newPipe();
  SelectStmt *stmt = new SelectStmt(*this,n,NULL,MAINSELECT,outputpipe);


  if(ok())
    stmt->typeCheck();

  if(ok()) {
    stmt->genCode();
  }
//  stmt.dump();
  return stmt;
}

SyntaxNode *findExpression(SelectSetOperator *op, int i, DbMainType type) {
  SyntaxNode *expr;
  if(op->m_son1) {
    expr = findExpression(op->m_son1,i,type);
    if(expr) return expr;
  }
  if(op->m_son2) {
    expr = findExpression(op->m_son2,i,type);
    if(expr) return expr;
  }
  if(op->m_node->token() == SELECT) {
    SelectStmt *stmt = (SelectStmt*)op;
    if(stmt->m_selectExprList[i].m_type == type)
      return stmt->m_selectExprList[i].m_expr;
  }
  return NULL;
}

void SelectSetOperator::checkCompatibleSelectLists() {
  if(m_son1 == NULL || m_son2 == NULL) stopcomp(m_node);
  HostVarDescriptionList d1 = m_son1->getDescription();
  HostVarDescriptionList d2 = m_son2->getDescription();
  if(d1.size() != d2.size()) {
    m_compiler.syntaxError(m_node,SQL_INVALID_EXPRLIST,_T("Number of expressions in two select connected with %s differ"),
      SqlTables->getSymbolName(m_node->token()));
    return;
  }
  for(UINT i = 0; i < d1.size(); i++) {
    if(!isCompatibleType(d1[i].getType(),d2[i].getType())) {
      SyntaxNode *e1 = findExpression(this,i,getMainType(d1[i].getType()));
      SyntaxNode *e2 = findExpression(this,i,getMainType(d2[i].getType()));
      m_compiler.syntaxError(e1,SQL_INVALID_EXPR_TYPE,_T("Type of expressions in 2 selects connected with %s is not compatible"),
      SqlTables->getSymbolName(m_node->token()));
      m_compiler.syntaxError(e2,SQL_INVALID_EXPR_TYPE,_T("Type of expressions in 2 selects connected with %s is not compatible"),
      SqlTables->getSymbolName(m_node->token()));
    }
  }
}

SelectSetOperator::SelectSetOperator(SqlCompiler &compiler, SelectStmtPurpose purpose, SyntaxNode *n, SelectSetOperator *son1, SelectSetOperator *son2) :
  m_compiler(compiler),
  SyntaxNodeData(n)
{
  switch(n->token()) {
  case UNION:
  case INTERSECT:
  case SETDIFFERENCE:
    break;
  default:
    stopcomp(n);
  }
  compiler.m_selectOperators.add(this);
  m_purpose = purpose;
  m_son1    = son1;
  m_son2    = son2;
  m_desc    = NULL;
}

SelectSetOperator::SelectSetOperator(SqlCompiler &compiler, SelectStmtPurpose purpose, SyntaxNode *n) : // n
  m_compiler(compiler),
  SyntaxNodeData(n)
{
  if(n->token() != SELECT)
    stopcomp(n);
  compiler.m_selectOperators.add(this);
  m_purpose = purpose;
  m_son1    = m_son2 = NULL;
  m_desc    = NULL;
}

// SelectSetOperator *SqlCompiler::create_subselect(SelectStmtPurpose purpose, SyntaxNode *n,

DbMainType SelectSetOperator::getExpressionType(int i) {
  return getMainType(getDescription()[i].getType());
}

void SelectSetOperator::dump(FILE *f, int level) const {
  findent(f,level);
  _ftprintf(f,_T("%s : purpose:%s\n"), SqlTables->getSymbolName(m_node->token()), purposeString(m_purpose));
  getDescription().dump();
}

SelectSetOperator::~SelectSetOperator() {
  if(m_desc) delete m_desc;
}

const HostVarDescriptionList &SelectSetOperator::getDescription() const {
  if(m_desc) return *m_desc;
  HostVarDescriptionList desc1,desc2,desc;
  switch(m_node->token()) {
  case UNION        :
    { desc1 = m_son1->getDescription();
      desc2 = m_son2->getDescription();
      for(UINT i = 0; i < desc1.size(); i++) {
        switch(getMainType(desc1[i].getType())) {
        case MAINTYPE_STRING :
        case MAINTYPE_VARCHAR:
          desc.add(HostVarDescription(desc1[i].getType(),max(desc1[i].sqllen,desc2[i].sqllen)));
          break;
        default:
          desc.add(desc1[i]);
          break;
        }
      }
    }
    break;
  case INTERSECT    :
    { desc1 = m_son1->getDescription();
      desc2 = m_son2->getDescription();
      for(UINT i = 0; i < desc1.size(); i++) {
        switch(getMainType(desc1[i].getType())) {
        case MAINTYPE_STRING :
        case MAINTYPE_VARCHAR:
          desc.add(HostVarDescription(desc1[i].getType(),min(desc1[i].sqllen,desc2[i].sqllen)));
          break;
        default:
          desc.add(desc1[i]);
          break;
        }
      }
    }
    break;
  case SETDIFFERENCE:
    { desc1 = m_son1->getDescription();
      desc2 = m_son2->getDescription();
      for(UINT i = 0; i < desc1.size(); i++) {
        switch(getMainType(desc1[i].getType())) {
        case MAINTYPE_STRING :
        case MAINTYPE_VARCHAR:
          desc.add(HostVarDescription(desc1[i].getType(),desc1[i].sqllen));
          break;
        default:
          desc.add(desc1[i]);
          break;
        }
      }
    }
    break;
  }
  m_desc = new HostVarDescriptionList(desc);
  return *m_desc;
}

const HostVarDescriptionList &SelectStmt::getDescription() const {
  if(m_desc) return *m_desc;
  HostVarDescriptionList desc;
  for(UINT i = 0; i < m_selectExprList.size(); i++) {
    DbMainType type = m_selectExprList[i].m_type;
    switch(type) {
    case MAINTYPE_NUMERIC:
      desc.add(HostVarDescription(DBTYPE_DOUBLE,sizeof(double)));
      break;
    case MAINTYPE_STRING :
      { int l = m_compiler.evaluateMaxStringLength(m_selectExprList[i].m_expr);
        if(l < 0) l = 0;
        desc.add(HostVarDescription(DBTYPE_WSTRING,l));
        break;
      }
    case MAINTYPE_VARCHAR:
      { int l = m_compiler.evaluateMaxStringLength(m_selectExprList[i].m_expr);
        if(l < 0) l = 0;
        desc.add(HostVarDescription(DBTYPE_VARCHAR,l));
        break;
      }
    case MAINTYPE_DATE   :
      desc.add(HostVarDescription(DBTYPE_DATE,sizeof(Date)));
      break;
    case MAINTYPE_TIME:
      desc.add(HostVarDescription(DBTYPE_TIME,sizeof(Time)));
      break;
    case MAINTYPE_TIMESTAMP:
      desc.add(HostVarDescription(DBTYPE_TIMESTAMP,sizeof(Timestamp)));
      break;

    case MAINTYPE_VOID   :
      desc.add(HostVarDescription(DBTYPE_UNKNOWN,0));
      break;
    default:
      throwSqlError(SQL_FATAL_ERROR,_T("getDescription:Unknown DbMainType:%d"), type );
    }
  }
  m_desc = new HostVarDescriptionList(desc);
  return *m_desc;
}

SelectSetOperator *SqlCompiler::genSelect1(SyntaxNode *n) { // n == SelectStmt, UNION,INTERSECT,SETDIFFERENCE
  SelectSetOperator *s1,*s2,*s;
  switch(n->token()) {
  case UNION        :
  case INTERSECT    :
  case SETDIFFERENCE:
    s1 = genSelect1(n->child(0));
    s2 = genSelect1(n->child(1));
    s = new SelectSetOperator(*this, MAINSELECT, n,s1,s2);
    if(ok()) s->checkCompatibleSelectLists();
    break;
  case SELECT       :
    s = genSimpleSelect(n);
    break;
  default:
    stopcomp(n);
    return NULL;
  }
  return s;
}

void SqlCompiler::genSelect(SyntaxNode *n) { // n == <SelectStmt>
  int hostVarCounter = 0;
  findHostVarIndex(n,hostVarCounter,HOSTVARSCANNERFLAG_ALLBUTINTO);
  findHostVarIndex(n,hostVarCounter,HOSTVARSCANNERFLAG_INTO      );

#ifdef TRACECOMP
  dumpSyntaxTree(n);
#endif

  m_noofpipes = 0;
  int pipaddr = m_code.appendIns1(CODEINITPIPES,0);
  int jmpaddr = m_code.appendIns1(CODEJMP      ,0);

  genSelect1(n);

  if(ok()) {
    m_code.fixins1(pipaddr,m_noofpipes);
    int jmpto = -1;
    for(UINT i = 0; i < m_operatorStart.size(); i++) {
      int addr = m_code.appendIns1(CODENEWOPERATOR,m_operatorStart[i]);
      if(i == 0) jmpto = addr;
    }
    UINT retaddr = m_code.appendIns0(CODERETURN);
    if(jmpto == -1) jmpto = retaddr;
    m_code.fixins1(jmpaddr,jmpto);
    m_code.appendDesc(m_selectOperators[0]->getDescription());
  }

#ifdef TRACECOMP
  if(ok())
    for(UINT i = 0; i < m_selectOperators.size(); i++)
      m_selectOperators[i]->dump();
#endif

}

void SqlCompiler::genDeclare(SyntaxNode *n) { // n == declare name for <SelectStmt>
  genSelect(n->child(1));
}

