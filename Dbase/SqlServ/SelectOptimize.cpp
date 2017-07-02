#include "stdafx.h"

#define TRACEMODUL

#ifdef TRACEMODUL
#define TRACECOST
//#define TRACEORDERCOST
#define TRACESUBCOST

#ifdef TRACECOST
static long costcount;
#endif

#endif

// is this a simple name belonging to table
int SelectStmt::expressionColumnIndex(FromTable *table, const SyntaxNode *n) {
  switch(n->token()) {
  case NAME:
  case DOT:
    { SelectSymbolInfo *ssi = getInfo(n);
      return (&ssi->m_fromTable == table) ? ssi->m_colIndex : -1;
    }
  default:
    return -1;
  }
}

// return true if an expression is fixed, ie. doesn't change while select is being executed.
// NOT the same as isConstExpression
bool SelectStmt::isFixedExpression(const SyntaxNode *expr) {
  switch(expr->token()) {
  case STRING   :
  case NUMBER   :
  case HOSTVAR  :
  case PARAM    :
  case DATECONST:
    return true;
  case DOT      :
  case NAME     :
    { SelectSymbolInfo *ssi = getInfo(expr);
      if(&ssi->m_fromTable.m_belongsTo != this) return true; // is the name an outer reference
      return ssi->isFixedByConst(); // here is the transitive closure
    }
  case MINUS    :
    if(expr->childCount() == 1) {
      return isFixedExpression(expr->child(0));
    }
    // NB continue case !!
  case PLUS     :
  case MULT     :
  case DIVOP    :
  case MODOP    :
  case EXPO     :
  case CONCAT   :
    return isFixedExpression(expr->child(0)) && isFixedExpression(expr->child(1));
  case SUBSTRING:
    return isFixedExpression(expr->child(0)) && isFixedExpression(expr->child(1)) && isFixedExpression(expr->child(2));
  case TYPEDATE :
    return isFixedExpression(expr->child(0)) && isFixedExpression(expr->child(1)) && isFixedExpression(expr->child(2));
  case MIN      : // occurs in having-clause, or where-clause with all aggregated columns as outer reference
  case MAX      :
  case SUM      :
    return isFixedExpression(expr->child(1));
  default       :
    return false;
  }
}

static bool singleConstantSet(const SyntaxNode *n) { // n = set_expr
  return (n->token() != SELECT) && (n->token() != COMMA);
}

BitSet SelectStmt::findFieldsFixedByConstPredicate(FromTable *table, const SyntaxNode *pred, bool neg) {
  BitSet res(table->getColumnCount());
  if(pred == NULL) return res;
  switch(pred->token()) {
  case OR :
    // if the user has specified the same Predicate in left and right subpredicate,
    // ie (f=1 and (...)) or (f=1 and (...)) we could do better
    // but this is a rare case
    break;
  case AND:
    { BitSet s1 = findFieldsFixedByConstPredicate(table,pred->child(0),neg);
      BitSet s2 = findFieldsFixedByConstPredicate(table,pred->child(1),neg);
      res = s1 + s2; // fixed fields is union of s1 and s2
    }
    break;
  case NOT:
    return findFieldsFixedByConstPredicate(table, pred->child(0),!neg);
  case EQUAL  :
  case NOTEQ  :
    { if(pred->token() == EQUAL) {
        if(neg) {
          return res; // return empty set
        }
      } else {// we have a double negation
        if(!neg) {
          return res;  // return empty set
        }
      }

      if(pred->child(1)->token() == SELECT) { // check if it's a correlated subselect
        SelectStmt *subsel = (SelectStmt*)pred->child(1)->getData();
        if(subsel->m_usedParentSelects.size() != 0) { // it's correlated
          return res; // return empty set
        } else {
          int colindex = expressionColumnIndex(table,pred->child(0));
          if(colindex >= 0) {
            res += colindex;
          }
          return res;
        }
      }
      NodeList left(pred->child(0));
      NodeList right(pred->child(1)); // we assume that left.size == right.size. has been checked in checkPredicate
      for(UINT i = 0; i < left.size(); i++) {
        const SyntaxNode *l  = left[i];
        const SyntaxNode *r  = right[i];
        int leftColumnIndex  = expressionColumnIndex(table,l);
        int rightColumnIndex = expressionColumnIndex(table,r);
        if(leftColumnIndex >= 0) { // dont remove { } !!
          if(isFixedExpression(r)) {
            res += leftColumnIndex;
            table->m_attributes[leftColumnIndex].m_fixedBy = right[i];
          }
        }
        else
          if(rightColumnIndex >= 0)
            if(isFixedExpression(l)) {
              res += rightColumnIndex;
              table->m_attributes[rightColumnIndex].m_fixedBy = left[i];
            }
      }
      return res;
    }
    break;

  case BETWEEN:
    return res; // empty set
  case INSYM  :
    { int colindex = expressionColumnIndex(table,pred->child(0));
      if(!neg && (colindex >= 0) && singleConstantSet(pred->child(1)))
        res += colindex;
    }
    break;
  case LIKE   :
    return res; // empty set

  case ISNULL:
    { int colindex = expressionColumnIndex(table,pred->child(0));
      if(colindex >= 0)
        res += colindex;
      break;
    }
  }
  return res;
}

void FromTable::checkFixedByUniqueKey() {
  for(size_t i = 0; i < m_indexStat.size(); i++) {
    const IndexDefinition &indexDef = m_indexStat[i].m_indexDef;
    if(indexDef.m_indexType == INDEXTYPE_NON_UNIQUE) continue;
    UINT c;
    for(c = 0; c < indexDef.getColumnCount(); c++) {
      if(!m_fixedByConst->contains(indexDef.m_columns[c].m_col)) {
        break;
      }
    }
    if(c == indexDef.getColumnCount()) { // we have a unique index, where all fields are fixed by const. This index is a good accesspath
      m_fixedByConst->clear();
      m_fixedByConst->invert(); // insert all members;
      m_joinSequence = m_belongsTo.m_noOfFixedTables++;
      m_fixed        = true;
      m_usedIndex    = (int)i;
      m_asc          = true; // don't bother about this
      m_keyPredicates.m_beginKeyPredicate.m_relOpToken = EQUAL;
      m_keyPredicates.m_endKeyPredicate.m_relOpToken   = EQUAL;
      for(UINT j = 0; j < c; j++) {
        const IndexColumn &indexColumn = indexDef.getColumn(j);
        m_keyPredicates.m_beginKeyPredicate.m_expr.add(m_attributes[indexColumn.m_col].m_fixedBy);
        m_keyPredicates.m_endKeyPredicate.m_expr.add(m_attributes[indexColumn.m_col].m_fixedBy);
      }
      return;
    }
  }
}

// returns true if any fields where added to the set of fixed fields
bool SelectStmt::addFieldsFixedByConstPredicate(const SyntaxNode *pred) {
  bool changed = false;
  for(size_t i = 0; i < m_fromTable.size(); i++) {
    FromTable *table = m_fromTable[i];
    BitSet fixedfields = findFieldsFixedByConstPredicate(table,pred,false);
    if(!(fixedfields - *(table->m_fixedByConst)).isEmpty()) {
      (*table->m_fixedByConst) += fixedfields;
      table->checkFixedByUniqueKey();
      changed = true;
    }
  }
  return changed;
}

static int turnInequality(int token) { // not negation
  switch(token) {
  case RELOPLE: return RELOPGE;
  case RELOPLT: return RELOPGT;
  case RELOPGE: return RELOPLE;
  case RELOPGT: return RELOPLT;
  default:
    throwSqlError(SQL_FATAL_ERROR,_T("turnInequality:invalid token:%d"),token);
    return RELOPGT; // just to make compiler happy
  }
}

void FromTable::calulateIndexOnly() {
  for(UINT i = 0; i < m_indexStat.size(); i++) {
    bool indexOnly = true;
    for(UINT c = 0; indexOnly && c < m_attributes.size(); c++) {
      if(m_attributes[c].used() && !m_indexStat[i].m_indexDef.columnIsMember(c)) {
        indexOnly = false;
      }
    }
    m_indexStat[i].m_indexOnly = indexOnly;
  }
}

// Calculate the maximal joinsequence of all columns used in subselect (and subsub...) relative to this
// -1 if no columns from this is used
int SelectStmt::subSelectMaxJoinSequence(SelectStmt *subselect) const {
  int maxseq = -1;
  for(size_t i = 0; i < subselect->m_ssiMap.size(); i++) {
    SelectSymbolInfo &ssi = subselect->m_ssiMap[i];
    if(&ssi.m_fromTable.m_belongsTo == this) {
      int seq = ssi.m_fromTable.m_joinSequence;
      if(seq > maxseq) maxseq = seq;
    }
    // else ssi belongs to subselect or parent. which is -1
  }
  for(size_t i = 0; i < subselect->m_subSelects.size(); i++) {
    int subseq = this->subSelectMaxJoinSequence(subselect->m_subSelects[i]); // use this ! not subselect->subSelectMaxJoinSequence !!!!
    if(subseq > maxseq) maxseq = subseq;
  }
//  _tprintf(_T("joinsequence for <%s>:%d\n"),subselect->m_name.cstr(),maxseq);
  return maxseq;
}

// Calculate the maximal joinsequence of all columns used in subselect (and subsub...) relative to this
// -1 if no columns from this is used
int SelectStmt::selectOperatorMaxJoinSequence(SelectSetOperator *op) const {
  switch(op->m_node->token()) {
  case SELECT       :
    return subSelectMaxJoinSequence((SelectStmt*)op);

  case UNION        :
  case INTERSECT    :
  case SETDIFFERENCE:
    { int js1 = selectOperatorMaxJoinSequence(op->m_son1);
      int js2 = selectOperatorMaxJoinSequence(op->m_son2);
      return max(js1,js2);
    }
  default:
    stopcomp(op->m_node);
  }
  return -1;
}

int SelectStmt::setMaxJoinSequence(SyntaxNode *n) const { // n = <setExpr>
  switch(n->token()) {
  case SELECT       :
  case UNION        :
  case INTERSECT    :
  case SETDIFFERENCE:
    return selectOperatorMaxJoinSequence((SelectSetOperator*)n->getData());

  default           :
    int maxseq = -1;
    NodeList exprlist(n);
    for(size_t i = 0; i < exprlist.size(); i++) {
      int exprseq = expressionMaxJoinSequence(exprlist[i]);
      if(exprseq > maxseq) {
        maxseq = exprseq;
      }
    }
    return maxseq;
  }
}

double SelectStmt::rowsSelected(double &total) const {
  total = 1;
  for(size_t i = 0; i < m_fromTable.size(); i++)
    total *= m_fromTable[i]->getTableSize();
  return m_selectivity * total;
}

double SelectSetOperator::rowsSelected(double &total) const {
  double n1,n2,t1,t2,d1,d2;
  switch(m_node->token()) {
  case SELECT       :
    return ((SelectStmt*)(this))->rowsSelected(total);

  case UNION        :
    n1 = m_son1->rowsSelected(t1);
    n2 = m_son2->rowsSelected(t2);
    d1 = n1 / t1;
    d2 = n2 / t2;
    if(m_node->childCount() == 3) { // union all
      return (d1 + d2 - d1*d2) * (t1+t2) / 2;
    } else {
      return (d1 + d2) * (t1+t2) / 2;
    }

  case INTERSECT    :
  { n1 = m_son1->rowsSelected(t1);
    n2 = m_son2->rowsSelected(t2);
    d1 = n1 / t1;
    d2 = n2 / t2;
    return d1 * d2 * (t1+t2) / 2;
  }

  case SETDIFFERENCE:
    n1 = m_son1->rowsSelected(t1);
    n2 = m_son2->rowsSelected(t2);
    return (n1 > n2) ? (n1 - n2) : 5;

  default           :
    stopcomp(m_node);

  }
  return 0;
}

double SelectStmt::getSetSize(SyntaxNode *n) const { // n = <setExpr>
  double total;
  switch(n->token()) {
  case SELECT       :
    return ((SelectStmt*)n->getData())->rowsSelected(total);

  case UNION        :
  case INTERSECT    :
  case SETDIFFERENCE:
    return ((SelectSetOperator*)n->getData())->rowsSelected(total);

  default           :
    { NodeList exprList(n);
      return (double)exprList.size();
    }
  }
}

int SelectStmt::expressionMaxJoinSequence(const SyntaxNode *expr) const {
  switch(expr->token()) {
  case NAME         :
  case DOT          :
    { SelectSymbolInfo *ssi = getInfo(expr);
      if(&ssi->m_fromTable.m_belongsTo != this) {
        return -1;
      } else {
        return ssi->m_fromTable.m_joinSequence;
      }
    };

  case PARAM        :
  case HOSTVAR      :
  case STRING       :
  case NUMBER       :
  case DATECONST    :
    return -1;

  case SELECT       : // expr relop subselect
  case UNION        :
  case INTERSECT    :
  case SETDIFFERENCE:
    return selectOperatorMaxJoinSequence((SelectSetOperator*)expr->getData());

  default           :
    { int sons = expr->childCount();
      int seq = -1;
      for(int i = 0; i < sons; i++) {
        int tmpseq = expressionMaxJoinSequence(expr->child(i));
        if(tmpseq > seq) {
          seq = tmpseq;
        }
      }
      return seq;
    }
  }
}

void Predicate::dump(FILE *f) const {
  _ftprintf(f,_T("Predicate:\"%s\" %s")
             ,relopstring(m_relOpToken),m_colIndex.toStringBasicType().cstr());
#ifdef TRACECOMP
  if(m_pred != NULL) {
    dumpSyntaxTree(m_pred);
  }
#endif
}

bool Predicate::columnIsMember(short col) const {
  for(size_t i = 0; i < m_colIndex.size(); i++) {
    if(m_colIndex[i] == col) {
      return true;
    }
  }
  return false;
}

SyntaxNode *Predicate::findOpposite(FromTable *fromTable, int col) {
  NodeList left(m_pred->child(0));
  NodeList right(m_pred->child(1));

//  dump();

  for(size_t i = 0; i < left.size(); i++) {
    if(fromTable->m_belongsTo.expressionColumnIndex(fromTable,left[i]) == col) {
      return right[i];
    }
    if(fromTable->m_belongsTo.expressionColumnIndex(fromTable,right[i]) == col) {
      return left[i];
    }
  }
  throwSqlError(SQL_FATAL_ERROR,_T("internal error in Predicate::findOpposite:no matching expression found"));
  return NULL;
}

void PredicateList::dump(FILE *f) const {
  for(size_t i = 0; i < size(); i++) {
    (*this)[i].dump(f);
  }
}

void KeyPredicate::dump(FILE *f) const {
  _ftprintf(f,_T("fieldcount :%zd relop:'%s'\n"),m_expr.size(),relopstring(m_relOpToken));
#ifdef TRACECOMP
  for(size_t i = 0; i < m_expr.size(); i++) {
    dumpSyntaxTree(m_expr[i],f);
  }
#endif
}

void KeyPredicates::init() {
  m_beginKeyPredicate.m_begin = true;
  m_beginKeyPredicate.m_expr.clear();
  m_endKeyPredicate.m_begin = false;
  m_endKeyPredicate.m_expr.clear();
}

void KeyPredicate::getKeyPredicateExpression(int length, const IndexDefinition &indexDef, PredicateList &predicateList, BitSet &set) {
  for(int f = 0; f < length; f++) {
    int indexCol = indexDef.getColumn(f).m_col;
    for(Iterator<size_t> it = set.getIterator(); it.hasNext(); ) {
      size_t i = it.next();
      Predicate &pred = predicateList[i];
//      pred.dump();
      if(predicateList[i].columnIsMember(indexCol)) {
        m_expr.add(predicateList[i].findOpposite(m_fromTable,indexCol));
        break;
      }
    }
  }
}

void KeyPredicates::find(const IndexDefinition &indexDef, PredicateList &predicateList) {
  FromTable *fromTable = m_beginKeyPredicate.m_fromTable;
//  predicateList.dump();
  init();

  int n = (int)predicateList.size();
  int i;
  int lastEqualKeyField = -1;
  BitSet equalPrefix(indexDef.getColumnCount());
  BitSet equalPredicates(predicateList.size());
  for(i = 0; i < n; i++) {
    switch(predicateList[i].m_relOpToken) {
    case EQUAL  :
      { const CompactShortArray &list = predicateList[i].m_colIndex;
        for(UINT j = 0; j < list.size(); j++) {
          short int keyFieldIndex = indexDef.getFieldIndex(list[j]);
          if(keyFieldIndex >= 0) {
            equalPrefix.add(keyFieldIndex);
            equalPredicates.add(i);
            if(keyFieldIndex > lastEqualKeyField) lastEqualKeyField = keyFieldIndex;
          }
        }
      }
      break;
    case INSYM:
      _tprintf(_T("KeyPredicates INSYM\n"));
      break;
    }
  }
  for(i = 0; i < lastEqualKeyField; i++)
    if(!equalPrefix.contains(i)) {
      equalPrefix.remove(i,indexDef.getColumnCount()-1);
      lastEqualKeyField = i - 1;
      break;
    }

  if(!equalPrefix.isEmpty()) {
    for(i = 0; i < n; i++) {
      switch(predicateList[i].m_relOpToken) {
      case EQUAL  :
        { short int keyFieldIndex = indexDef.getFieldIndex(predicateList[i].m_colIndex[0]);
          if(keyFieldIndex >= 0 && equalPrefix.contains(keyFieldIndex))
            predicateList[i].m_keyPredicate = true;
        }
        break;
      default: continue;
      }
    }
  }
  // equalPrefix.dump(stdout);
  // equalPrefix now contains the longest indexprefix for operator =, maybe empty
  // Now find the best beginkeyprefix concat(equalPrefix,gefields) for operators >,>=,< and <=
  int bestPrefix = lastEqualKeyField;
  int relOpToken = EQUAL;
  BitSet beginKeyPredicates(predicateList.size());
  for(i = 0; i < n; i++) {
    int currrentLastKeyField = lastEqualKeyField;
    int lastRelOpToken       = EQUAL;
    switch(predicateList[i].m_relOpToken) {
    case RELOPGE:
    case RELOPGT:
      { const CompactShortArray &list = predicateList[i].m_colIndex;
        int lastFieldIndex = lastEqualKeyField;
        size_t j;
        for(j = 0; j < list.size(); j++) {
          short int keyFieldIndex = indexDef.getFieldIndex(list[j]);
          if(keyFieldIndex < 0) break;
          if(((keyFieldIndex == lastFieldIndex) || (keyFieldIndex == lastFieldIndex + 1)) &&
            (fromTable->m_asc == indexDef.getColumn(keyFieldIndex).m_asc))
            lastFieldIndex = keyFieldIndex;
          else
            break;
        }
        if(lastFieldIndex > currrentLastKeyField) {
          predicateList[i].m_keyPredicate = true;
          currrentLastKeyField = lastFieldIndex;
          if(j == list.size())
            lastRelOpToken = predicateList[i].m_relOpToken;
          else
            lastRelOpToken = RELOPGE;
        }
      }
      break;

    case RELOPLE:
    case RELOPLT:
      { const CompactShortArray &list = predicateList[i].m_colIndex;
        int lastFieldIndex = lastEqualKeyField;
        size_t j;
        for(j = 0; j < list.size(); j++) {
          short int keyFieldIndex = indexDef.getFieldIndex(list[j]);
          if(keyFieldIndex < 0) break;
          if(((keyFieldIndex == lastFieldIndex) || (keyFieldIndex == lastFieldIndex + 1)) &&
            (fromTable->m_asc != indexDef.getColumn(keyFieldIndex).m_asc))
            lastFieldIndex = keyFieldIndex;
          else
            break;
        }
        if(lastFieldIndex > currrentLastKeyField) {
          predicateList[i].m_keyPredicate = true;
          currrentLastKeyField = lastFieldIndex;
          if(j == list.size())
            lastRelOpToken = predicateList[i].m_relOpToken;
          else
            lastRelOpToken = RELOPLE;
        }
      }
      break;
    }
    if(currrentLastKeyField > bestPrefix) {
      bestPrefix = currrentLastKeyField;
      relOpToken = lastRelOpToken;
      beginKeyPredicates = equalPredicates;
      beginKeyPredicates += i;
    }
  }
  if(beginKeyPredicates.isEmpty())
    beginKeyPredicates = equalPredicates;

//  beginKeyPredicates.dump(stdout);
  m_beginKeyPredicate.m_relOpToken = relOpToken;
  if(bestPrefix >= 0)
    m_beginKeyPredicate.getKeyPredicateExpression(bestPrefix + 1,indexDef,predicateList,beginKeyPredicates);

  // Now find the best endkeyprefix concat(equalPrefix,lefields) for operators '<' and '<='
  bestPrefix = lastEqualKeyField;
  relOpToken = EQUAL;
  BitSet endKeyPredicates(predicateList.size());
  for(i = 0; i < n; i++) {
    int currrentLastKeyField = lastEqualKeyField;
    int lastRelOpToken       = EQUAL;
    switch(predicateList[i].m_relOpToken) {
    case RELOPLE:
    case RELOPLT:
      { const CompactShortArray &list = predicateList[i].m_colIndex;
        int lastFieldIndex = lastEqualKeyField;
        size_t j;
        for(j = 0; j < list.size(); j++) {
          short int keyFieldIndex = indexDef.getFieldIndex(list[j]);
          if(keyFieldIndex < 0) break;
          if(((keyFieldIndex == lastFieldIndex) || (keyFieldIndex == lastFieldIndex + 1)) &&
            (fromTable->m_asc == indexDef.getColumn(keyFieldIndex).m_asc))
            lastFieldIndex = keyFieldIndex;
          else
            break;
        }
        if(lastFieldIndex > currrentLastKeyField) {
          predicateList[i].m_keyPredicate = true;
          currrentLastKeyField = lastFieldIndex;
          if(j == list.size())
            lastRelOpToken = predicateList[i].m_relOpToken;
          else
            lastRelOpToken = RELOPLE;
        }
      }
      break;
    case RELOPGE:
    case RELOPGT:
      { const CompactShortArray &list = predicateList[i].m_colIndex;
        int lastFieldIndex = lastEqualKeyField;
        size_t j;
        for(j = 0; j < list.size(); j++) {
          short int keyFieldIndex = indexDef.getFieldIndex(list[j]);
          if(keyFieldIndex < 0) break;
          if(((keyFieldIndex == lastFieldIndex) || (keyFieldIndex == lastFieldIndex + 1)) &&
            (fromTable->m_asc != indexDef.getColumn(keyFieldIndex).m_asc)) {
            lastFieldIndex = keyFieldIndex;
          } else {
            break;
          }
        }
        if(lastFieldIndex > currrentLastKeyField) {
          predicateList[i].m_keyPredicate = true;
          currrentLastKeyField = lastFieldIndex;
          if(j == list.size())
            lastRelOpToken = predicateList[i].m_relOpToken;
          else
            lastRelOpToken = RELOPGE;
        }
      }
      break;
    }
    if(currrentLastKeyField > bestPrefix) {
      bestPrefix = currrentLastKeyField;
      relOpToken = lastRelOpToken;
      endKeyPredicates = equalPredicates;
      endKeyPredicates += i;
    }
  }
  if(endKeyPredicates.isEmpty())
    endKeyPredicates = equalPredicates;
  m_endKeyPredicate.m_relOpToken = relOpToken;
//  endKeyPredicates.dump(stdout);
  if(bestPrefix >= 0)
    m_endKeyPredicate.getKeyPredicateExpression(bestPrefix + 1,indexDef,predicateList,endKeyPredicates);
}

void KeyPredicates::dump(FILE *f) const {
  _ftprintf(f,_T("  beginkey-Predicate:")); m_beginKeyPredicate.dump(f);
  _ftprintf(f,_T("  endkey-Predicate  :")); m_endKeyPredicate.dump(f);
}

double IndexStatistic::selectivity(PredicateList &predicateList, KeyPredicates &predicates) const {
//  _tprintf(_T("table <%-15s> index:<%-15s>:\n"),m_indexDef.m_tablename,m_indexDef.m_indexName);
  if(predicateList.size() > 0)
    predicates.find(m_indexDef,predicateList);
  else
    predicates.init();
#ifdef TRACECOST
//  predicates.dump();
#endif
  UINT beginKeyPredicateSize = (UINT)predicates.m_beginKeyPredicate.m_expr.size();
  UINT endKeyPredicateSize   = (UINT)predicates.m_endKeyPredicate.m_expr.size();

  if(beginKeyPredicateSize == 0 && endKeyPredicateSize   == 0) {
    return 1; // tablescan
  }

  if(beginKeyPredicateSize == 0 || endKeyPredicateSize   == 0) {
    return 1.0 / 3.0;
  }
  int minFieldCount = min(beginKeyPredicateSize,endKeyPredicateSize);
  if(predicates.m_beginKeyPredicate.m_relOpToken == EQUAL ||
     predicates.m_endKeyPredicate.m_relOpToken   == EQUAL)
    return m_stat.selectivity(minFieldCount);
  else {
    double res = m_stat.selectivity(minFieldCount);
    if(res >= 0.5) return res;
    else
      return res * 2;
  }
}

void FromTable::findEqualPredicates(const SyntaxNode *n, const NodeList &left, const NodeList &right, PredicateList &predicateList) const { // sizes are equal !!
  for(UINT i = 0; i < left.size(); i++) {
    const SyntaxNode *leftexpr        = left[i];
    const SyntaxNode *rightexpr       = right[i];
    SelectSymbolInfo *leftSymbolInfo  = NULL;
    SelectSymbolInfo *rightSymbolInfo = NULL;
    switch(leftexpr->token()) {
    case NAME:
    case DOT :
      leftSymbolInfo = m_belongsTo.getInfo(leftexpr);
      break;
    }
    switch(rightexpr->token()) {
    case NAME:
    case DOT :
      rightSymbolInfo = m_belongsTo.getInfo(rightexpr);
      break;
    }
    if(leftSymbolInfo && (&leftSymbolInfo->m_fromTable == this)
      && m_joinSequence > m_belongsTo.expressionMaxJoinSequence(rightexpr)) {
        Predicate pred(EQUAL,n);
        pred.m_colIndex.add(leftSymbolInfo->m_colIndex);
        predicateList.add(pred);
    }
    else
      if(rightSymbolInfo && (&rightSymbolInfo->m_fromTable == this)
        && m_joinSequence > m_belongsTo.expressionMaxJoinSequence(leftexpr)) {
          Predicate pred(EQUAL,n);
          pred.m_colIndex.add(rightSymbolInfo->m_colIndex);
          predicateList.add(pred);
      }
  }
}

void FromTable::findInEqualPredicates(const SyntaxNode *n, const NodeList &left, const NodeList &right, int relOpToken, PredicateList &predicateList) const { // sizes are equal !!
  Predicate leftPredicate(relOpToken,n);
  Predicate rightPredicate(turnInequality(relOpToken),n);
  bool contleft  = true;
  bool contright = true;
  for(UINT i = 0; i < left.size() && (contleft || contright); i++) {
    const SyntaxNode *leftexpr        = left[i];
    const SyntaxNode *rightexpr       = right[i];
    SelectSymbolInfo *leftSymbolInfo  = NULL;
    SelectSymbolInfo *rightSymbolInfo = NULL;
    switch(leftexpr->token()) {
    case NAME:
    case DOT :
      if(contleft) {
        leftSymbolInfo = m_belongsTo.getInfo(leftexpr);
        if(&leftSymbolInfo->m_fromTable != this) {
          contleft       = false;
          leftSymbolInfo = NULL;
        }
      }
      break;
    default:
      contleft = false;
      break;
    }

    switch(rightexpr->token()) {
    case NAME:
    case DOT :
      if(contright) {
        rightSymbolInfo = m_belongsTo.getInfo(rightexpr);
        if(&rightSymbolInfo->m_fromTable != this) {
          contright       = false;
          rightSymbolInfo = NULL;
        }
      }
      break;
    default:
      contright = false;
      break;
    }

    if( contleft && m_joinSequence > m_belongsTo.expressionMaxJoinSequence(rightexpr))
      leftPredicate.m_colIndex.add(leftSymbolInfo->m_colIndex);
    else
      contleft = false;
    if( contright && m_joinSequence > m_belongsTo.expressionMaxJoinSequence(leftexpr))
      rightPredicate.m_colIndex.add(rightSymbolInfo->m_colIndex);
    else
      contright = false;
  }

  if(leftPredicate.m_colIndex.size() > 0)
    predicateList.add(leftPredicate);
  if(rightPredicate.m_colIndex.size() > 0)
    predicateList.add(rightPredicate);
}

void FromTable::findSetPredicate(const SyntaxNode *n, PredicateList &predicateList) const { // n = <Predicate>
  SyntaxNode *member  = n->child(0);
  SyntaxNode *setExpr = n->child(1);

  switch(member->token()) {
  case NAME:
  case DOT :
    { SelectSymbolInfo *ssi = m_belongsTo.getInfo(member);
      if(&ssi->m_fromTable == this && m_joinSequence > m_belongsTo.setMaxJoinSequence(setExpr)) {
        double estimatedSetSize = m_belongsTo.getSetSize(setExpr);
        Predicate pred(INSYM,n,estimatedSetSize);
        pred.m_colIndex.add(ssi->m_colIndex);
        predicateList.add(pred);
      }
    };
    break;

  default  :
    return; // can do no more
  }
}

void FromTable::findPredicates(const NodeList &andlist, PredicateList &predicateList ) const {
  for(UINT i = 0; i < andlist.size(); i++) {
    const SyntaxNode *rel = andlist[i];
    switch(rel->token()) {
    case RELOPLE:
    case RELOPLT:
    case RELOPGE:
    case RELOPGT:
      { NodeList left(rel->child(0));
        NodeList right(rel->child(1));
        findInEqualPredicates(rel,left,right,rel->token(),predicateList);
      }
      break;
    case EQUAL  :
      { NodeList left(rel->child(0));
        NodeList right(rel->child(1));
        findEqualPredicates(rel,left,right,predicateList);
      }
      break;
    case BETWEEN:
      { NodeList middle(rel->child(0));
        NodeList lower(rel->child(1));
        NodeList upper(rel->child(2));
        findInEqualPredicates(rel,lower ,middle, RELOPLE,predicateList);
        findInEqualPredicates(rel,middle,upper , RELOPLE,predicateList);
      }
      break;
    case INSYM:
      findSetPredicate(rel,predicateList);
      break;
    case OR :
      predicateList.add(Predicate(OR,rel));
      break;
    case AND:
      predicateList.add(Predicate(AND,rel));
      break;
    }
  }
}

void FromTable::predicateSelectivity(NodeList &andlist, double &indexSelectivity, double &selectivity) {
//  _tprintf(_T("table <%s> joinsequence:%d\n"),m_correlationName.cstr(),m_joinSequence);
  PredicateList predicateList;
  findPredicates(andlist,predicateList);
  indexSelectivity = getUsedIndex().selectivity(predicateList,m_keyPredicates);
  selectivity      = indexSelectivity;
  for(UINT i = 0; i < predicateList.size(); i++) {
    Predicate &pred = predicateList[i];
    if(pred.m_keyPredicate) continue;
    switch(pred.m_relOpToken) {
    case EQUAL  : selectivity *= 0.5; break;
    case RELOPLE:
    case RELOPLT:
    case RELOPGE:
    case RELOPGT:
      { UINT n = (UINT)pred.m_colIndex.size();
        for(UINT j = 0; j < n; j++)
          selectivity *= 0.80;
        break;
      }
    case OR     :
      { double tmpread,tmpsel;
        searchConditionSelectivity(pred.m_pred,tmpread,tmpsel);
        selectivity *= tmpsel;
        break;
      }
    case AND    :
      { double tmpread,tmpsel;
        booleanTermSelectivity(pred.m_pred,tmpread,tmpsel);
        selectivity *= tmpsel;
        break;
      }
    default:
      dumpSyntaxTree(andlist[i]);
      break;
    }
  }
/*
  _tprintf(_T("table <%s> indexSelectivity:%lg selectivity:%lg\n"),
    m_correlationName.cstr(),indexSelectivity,selectivity);
*/
}

void FromTable::booleanTermSelectivity( const SyntaxNode *n, double &indexSelectivity, double &selectivity) {
  NodeList andlist((SyntaxNode*)n,AND,true);
  predicateSelectivity(andlist, indexSelectivity, selectivity);
}

// estimates the fraction of a table that needs to be read and probably will be selected
void FromTable::searchConditionSelectivity( const SyntaxNode *n, double &indexSelectivity, double &selectivity) {
  if(isFixedByUniqueKey() >= 0) {
    indexSelectivity = selectivity = 1.0 / getTableSize();
  }
  else {
    NodeList orlist((SyntaxNode*)n,OR,true);
    double tmpinxsel1,tmpsel1;
    booleanTermSelectivity(orlist[0],tmpinxsel1,tmpsel1);
    for(UINT i = 1; i < orlist.size(); i++) {
      double tmpinxsel2,tmpsel2;
      booleanTermSelectivity(orlist[i],tmpinxsel2,tmpsel2);
      tmpinxsel1 += tmpinxsel2 - tmpinxsel1 * tmpinxsel2;
      tmpsel1    += tmpsel2    - tmpsel1  * tmpsel2;
    }
    indexSelectivity = tmpinxsel1;
    selectivity      = tmpsel1;
  }
}

void FromTable::findTableSelectivity(const SyntaxNode *n, double &indexSelectivity, double &selectivity) {
  searchConditionSelectivity(n,m_indexSelectivity,m_selectivity);
  indexSelectivity = m_indexSelectivity;
  selectivity      = m_selectivity;
}

void SelectStmt::statementSelectivity(double &indexSelectivity, double &selectivity) {
  indexSelectivity = 1;
  selectivity      = 1;
  if(m_whereClause != NULL) {
    for(UINT i = 0; i < m_fromTable.size(); i++) {
      FromTable *table = m_fromTable[i];
      double tmpinxsel,tmpsel;
#ifdef TRACECOST
      _tprintf(_T("table <%-15s> index:<%-15s %s>:\n")
       ,table->m_correlationName.cstr()
       ,table->getUsedIndex().m_indexDef.m_indexName.cstr()
       ,table->m_asc?_T("ASCENDING"):_T("DESCENDING"));
#endif
      table->findTableSelectivity(m_whereClause, tmpinxsel, tmpsel);
#ifdef TRACECOST
      _tprintf(_T("read (%.2lf%%,%.2lf%%) rows from %s\n"),tmpinxsel*100,tmpsel*100,m_fromTable[i]->m_correlationName.cstr());
#endif
      indexSelectivity *= tmpinxsel;
      selectivity      *= tmpsel;
    }
  }
}

double SelectStmt::estimateGroupByCost(double elementsSelected) {
  if(m_groupBy.size() == 0) return 0; // no group by
  _tprintf(_T("estimate group by not implemented yet\n"));
  return 0;
}

typedef enum {
  MONOTONITY_ASC
 ,MONOTONITY_DESC
 ,MONOTONITY_CONST
 ,MONOTONITY_NON
} Monotonity;

class OrderGivenElement {
public:
  FromTable     *m_fromTable;
  int            m_tabcolindex;
  int            m_keycolindex;
  bool           m_asc;
  OrderGivenElement(FromTable *fromTable, int tabcolindex, int keycolindex, bool asc) {
    m_fromTable  = fromTable;
    m_tabcolindex = tabcolindex;
    m_keycolindex = keycolindex;
    m_asc         = asc;
  }
  Monotonity findMonotonity(SyntaxNode *expr);
  void dump(FILE *f = stdout) const;
};

static Monotonity changeMonotonity(Monotonity mono) {
  switch(mono) {
  case MONOTONITY_ASC   : return MONOTONITY_DESC;
  case MONOTONITY_DESC  : return MONOTONITY_ASC;
  case MONOTONITY_CONST : return MONOTONITY_CONST;
  case MONOTONITY_NON   : return MONOTONITY_NON;
  default:
    throwSqlError(SQL_FATAL_ERROR,_T("Unknown Monotonity:%d"),mono);
    return MONOTONITY_NON; // just to make compiler happy
  }
}

#ifdef TRACEORDERCOST
static TCHAR *monotonityString(Monotonity mono) {
  switch(mono) {
  case MONOTONITY_ASC   : return _T("MONOTONITY_ASC");
  case MONOTONITY_DESC  : return _T("MONOTONITY_DESC");
  case MONOTONITY_CONST : return _T("MONOTONITY_CONST");
  case MONOTONITY_NON   : return _T("MONOTONITY_NON");
  default: return _T("Unknown Monotonity");
  }
}
#endif

void OrderGivenElement::dump(FILE *f) const {
  _ftprintf(f,_T("OrderGivenElement[%d]:%s.%s %s (index:%s)"),
    m_keycolindex,
    m_fromTable->m_correlationName.cstr(),
    m_fromTable->getColumn(m_tabcolindex).m_name.cstr(),
    m_asc ? _T("ASC") : _T("DESC"),
    m_fromTable->getUsedIndex().m_indexDef.m_indexName.cstr()
  );
}

Monotonity OrderGivenElement::findMonotonity(SyntaxNode *expr) {
  SelectStmt &stmt     = m_fromTable->m_belongsTo;
  SqlCompiler &compiler = stmt.m_compiler;

// Finds the Monotonity of an expresion relative to this.
// Actually some kind of differentiation
  switch(expr->token()) {
    case DOT      :
    case NAME     :
      { SelectSymbolInfo *ssi = stmt.getInfo(expr);
        if(ssi->isFixedByConst()) return MONOTONITY_CONST;
        if(&ssi->m_fromTable == m_fromTable) {
          if(ssi->m_colIndex == m_tabcolindex)
            return m_asc ? MONOTONITY_ASC : MONOTONITY_DESC;
          else {
            return MONOTONITY_NON;
/*
            FromTable *fromTable = ssi->m_fromTable;
            int keyFieldIndex = fromTable->getUsedIndex().m_indexDef.getFieldIndex(ssi->m_colIndex);
            if(keyFieldIndex < 0) return MONOTONITY_NON; // not keyfield
            if(keyFieldIndex < m_keycolindex) return MONOTONITY_CONST;
            return MONOTONITY_NON;
*/
          }
        }
        else // fromtables differ. Check joinsequence
          if(ssi->m_fromTable.m_joinSequence < m_fromTable->m_joinSequence)
            return MONOTONITY_CONST;
          else
            return MONOTONITY_NON;
      }
      break;

    case STRING        :
    case NUMBER        :
    case DATECONST     :
    case TIMECONST     :
    case TIMESTAMPCONST:
    case HOSTVAR       :
    case PARAM         :
      return MONOTONITY_CONST;

    case CURRENTDATE   :
    case CURRENTTIME   :
    case CURRENTTIMESTAMP:
      return MONOTONITY_ASC;

    case PLUS     :
      { if(isConstExpression(expr->child(0))) return findMonotonity(expr->child(1));
        if(isConstExpression(expr->child(1))) return findMonotonity(expr->child(0));
      }
      break;

    case MINUS    :
      { if(expr->childCount() == 1)
          return changeMonotonity(findMonotonity(expr->child(0)));
        if(isConstExpression(expr->child(0))) return changeMonotonity(findMonotonity(expr->child(1)));
        if(isConstExpression(expr->child(1))) return findMonotonity(expr->child(0));
      }
      break;

    case MULT     : // be aware of sign
      { bool value1known,value2known;
        double value1 = compiler.evaluateConstNumericExpression(expr->child(0), value1known);
        double value2 = compiler.evaluateConstNumericExpression(expr->child(1), value2known);
        if(value1known) {
          if(value1 >  0) return findMonotonity(expr->child(1));
          if(value1 == 0) return MONOTONITY_CONST;
          if(value1 <  0) return changeMonotonity(findMonotonity(expr->child(1)));
        }
        if(value2known) {
          if(value2 >  0) return findMonotonity(expr->child(0));
          if(value2 == 0) return MONOTONITY_CONST;
          if(value2 <  0) return changeMonotonity(findMonotonity(expr->child(0)));
        }
        return MONOTONITY_NON;
      }

    case DIVOP    :
      { bool value1known,value2known;
        double value1 = compiler.evaluateConstNumericExpression(expr->child(0), value1known);
        double value2 = compiler.evaluateConstNumericExpression(expr->child(1), value2known);
        if(value2known) {
          if(value2 >  0) return findMonotonity(expr->child(0));
          if(value2 == 0) {
            compiler.syntaxError(expr,SQL_DIVISION_BY_ZERO,_T("Division by zero"));
            return MONOTONITY_NON;
          }
          if(value2 <  0) return changeMonotonity(findMonotonity(expr->child(0)));
        }
        if(value1known) {
          if(value1 >  0) return changeMonotonity(findMonotonity(expr->child(1)));
          if(value1 == 0) return MONOTONITY_CONST;
          if(value1 <  0) return findMonotonity(expr->child(1));
        }
        return MONOTONITY_NON;
      }
      return MONOTONITY_NON;
      break;

    case MODOP    :
      return MONOTONITY_NON;

    case EXPO     :
      return MONOTONITY_NON;

    case SUBSTRING:
      { bool startvalueknown,endvalueknown;
        double substringstart = compiler.evaluateConstNumericExpression(expr->child(1), startvalueknown);
        double substringend   = compiler.evaluateConstNumericExpression(expr->child(2), endvalueknown  );
        if(!startvalueknown || !endvalueknown) return MONOTONITY_NON;
        switch(findMonotonity(expr->child(0))) {
        case MONOTONITY_CONST: return MONOTONITY_CONST;
        case MONOTONITY_NON  : return MONOTONITY_NON;
        case MONOTONITY_ASC  : return (substringstart == 0) ? MONOTONITY_ASC  : MONOTONITY_NON;
        case MONOTONITY_DESC : return (substringstart == 0) ? MONOTONITY_DESC : MONOTONITY_NON;
        default              : return MONOTONITY_NON; // actually an error
        }
      }
      break;

    case TYPEDATE:
      return MONOTONITY_NON;

    case CONCAT:
      { if(isConstExpression(expr->child(0)))
          return findMonotonity(expr->child(1));
        else
          return MONOTONITY_NON;
      }
      break;

    case SUM  :
    case COUNT:
    case MIN  :
    case MAX  :
      return MONOTONITY_NON;

    default:
      stopcomp(expr);
  }
  return MONOTONITY_NON;
}

class MonotonityMatrix {
private:
  Array<OrderGivenElement> &m_orderGivenByIndex;
  Array<OrderByExpression> &m_orderBy;
  SelectStmt              *m_stmt;
  bool findOrderPath(UINT match, UINT from);
public:
  Monotonity **matrix;
  MonotonityMatrix(SelectStmt *stmt,Array<OrderGivenElement> &ordergivenbyindex, Array<OrderByExpression> &orderby);
  ~MonotonityMatrix();
  bool findOrderPath();
};

MonotonityMatrix::MonotonityMatrix(SelectStmt *stmt, Array<OrderGivenElement> &ordergivenbyindex, Array<OrderByExpression> &orderby)
 :m_orderGivenByIndex(ordergivenbyindex)
 ,m_orderBy(orderby)
{
  m_stmt = stmt;
}

MonotonityMatrix::~MonotonityMatrix() {
}

static void findent(FILE *f, int level) {
  _ftprintf(f,_T("%*.*s"),level,level,EMPTYSTRING);
}

// try to match orderby with position=match, using index-columns [from..ordergivenbyindex.size()-1]
bool MonotonityMatrix::findOrderPath(UINT match, UINT from) {
  if(match == m_orderBy.size()) {
#ifdef TRACEORDERCOST
    _tprintf(_T("############# found full orderpath (a) #################\n"));
#endif
    return true;
  }

  SyntaxNode *expr = m_orderBy[match].m_expr;
//  m_stmt->m_compiler.dumpSyntaxTree(expr);
  UINT n = (UINT)m_orderGivenByIndex.size();
  if(from >= n) { // we have matched all the fields, given by index,
                  // but there are still requested orderby-fields. These must all be const
                  // in order to give a valid path
    for(UINT i = match; i < m_orderBy.size(); i++)
      if(!m_stmt->isFixedExpression(m_orderBy[i].m_expr))
        return false;
#ifdef TRACEORDERCOST
    _tprintf(_T("############# found full orderpath (b) #################\n"));
#endif
    return true;
  }
  for(UINT i = from; i < n; i++) {
    OrderGivenElement &ordergiven = m_orderGivenByIndex[i];
#ifdef TRACEORDERCOST
    findent(stdout,match*2); _tprintf(_T("level:%d %s\n"),match, m_orderBy[match].m_asc?_T("ASC"):_T("DESC"));
    m_stmt->m_compiler.dumpSyntaxTree(expr,stdout,match);
    findent(stdout,match*2); ordergiven.dump();
#endif
    Monotonity mono = ordergiven.findMonotonity(expr);
#ifdef TRACEORDERCOST
    _tprintf(_T(" %s\n"),monotonityString(mono));
#endif
    switch(mono) {
    case MONOTONITY_ASC   :
      if(!m_orderBy[match].m_asc) return false;
      return findOrderPath(match+1,i+1);
      break;
    case MONOTONITY_DESC  :
      if(m_orderBy[match].m_asc) return false;
      return findOrderPath(match+1,i+1);
      break;
    case MONOTONITY_CONST :
      if(findOrderPath(match+1,i) || findOrderPath(match+1,i+1)) return true;
      break;
    case MONOTONITY_NON   :
      return false;
    }
  }
  return false;
}

bool MonotonityMatrix::findOrderPath() {
  UINT from;
  for(from = 0; from < m_orderGivenByIndex.size(); from++) {
    OrderGivenElement &ordergiven = m_orderGivenByIndex[from];
    if(!ordergiven.m_fromTable->m_fixedByConst->contains(ordergiven.m_tabcolindex))
      break;
  }
  return findOrderPath(0, from);
}

double SelectStmt::estimateOrderByCost(double elementsSelected) {
  if(m_orderBy.size() == 0) return 0; // no orderby

  Array<OrderGivenElement> ordergivenbyindex;

  for(UINT i = 0; i < m_fromTable.size(); i++) {
    FromTable &table = *m_fromTable[i];
    if(table.isFixedByUniqueKey() >= 0) continue;
    const IndexDefinition &indexDef = table.getUsedIndex().m_indexDef;
    for(UINT col = 0; col < indexDef.getColumnCount(); col++) {
      const IndexColumn &indexColumn = indexDef.getColumn(col);
      ordergivenbyindex.add(
                           OrderGivenElement(
                             &table,
                              indexColumn.m_col,
                              col,
                              table.m_asc == indexColumn.m_asc
                           )
                        );
    }
  }

  if(elementsSelected < 5) elementsSelected = 5; // just to be sure we dont get a math-Exception
  double sortcost = elementsSelected * 2 * log(elementsSelected);
  MonotonityMatrix m(this,ordergivenbyindex,m_orderBy);
  return m.findOrderPath() ? 0 : sortcost;
}

double SelectStmt::estimateCost(double &ordercost) {
  UINT i;
#ifdef TRACECOST
  costcount++;
  _tprintf(_T("cost begin---------------------------------\n"));
  for(i = 0; i < m_fromTable.size(); i++) {
    FromTable &table = *m_fromTable[i];
    _tprintf(_T("table <%s> <%s> %s joinseq:%d\n"),
    table.m_correlationName.cstr(),
    table.getUsedIndex().m_indexDef.m_indexName.cstr(),
    table.m_asc ? _T("ASC") : _T("DESC"),
    table.m_joinSequence);
  }
#endif
  // missing distinct !!
  double indexSelectivity,selectivity;
  statementSelectivity(indexSelectivity,selectivity);
  double rowsSelected = selectivity;
  double rowsread     = indexSelectivity;
  for(i = 0; i < m_fromTable.size(); i++) {
    const FromTable      &table     = *m_fromTable[i];
    const IndexStatistic &indexStat = table.getUsedIndex();
    double factor = indexStat.m_indexOnly ? 1 : 3;
    rowsSelected *= table.getTableSize() * factor;
    rowsread     *= table.getTableSize() * factor;
  }
  double totalcost = rowsread;
  for(i = 0; i < m_subSelects.size(); i++) {
    double subselectcost = m_subSelects[i]->m_totalCost;
    int subselectjoinsequence = subSelectMaxJoinSequence(m_subSelects[i]);
    // The subselect is executed as early as possible in the PipeLine
    // that is just after the table with joinsequence = subSelectMaxJoinSequence(subselect) is fetched
    if(subselectjoinsequence < 0) // this subselect is executed only once, ie at the startuptime of the select
      totalcost += subselectcost;
    else { // how many times is this subselect executed
      double timesexecuted = 1;
      int j;
      for(j = 0; j < subselectjoinsequence; j++) {
        timesexecuted *= m_fromTable[j]->getTableSize() * m_fromTable[j]->m_selectivity;
      }
      timesexecuted *= m_fromTable[j]->getTableSize() * m_fromTable[j]->m_indexSelectivity;
#ifdef TRACESUBCOST
      _tprintf(_T("subselect <%s> (js:%d) will be executed %lg times each with a cost of %lg\n")
               ,m_subSelects[i]->m_name.cstr()
               ,subselectjoinsequence
               ,timesexecuted
               ,subselectcost
              );
#endif
      totalcost += subselectcost * timesexecuted;
    }
  }
  if(m_totalCost >= 0 && totalcost >= m_totalCost) return totalcost; // no need to calculate the rest

  totalcost += estimateGroupByCost(rowsSelected);

  ordercost = estimateOrderByCost(rowsSelected);
  totalcost += ordercost;
#ifdef TRACECOST
  _tprintf(_T("rowsread:%lg rowsSelected:%lg ordercost:%lg total cost:%lg\n"),
    rowsread,rowsSelected,ordercost,totalcost);
  _tprintf(_T("cost end---------------------------------\n"));
#endif
//  PAUSE();
  return totalcost; // incl. order by, group by, subselects
}

TableReadMethod::TableReadMethod(FromTable *table) {
  m_table               = table;
  m_index               = table->m_usedIndex;
  m_asc                 = table->m_asc;
  m_indexSelectivity    = table->m_indexSelectivity;
  m_selectivity         = table->m_selectivity;
  m_keyPredicates       = table->m_keyPredicates;
}

void SelectStmt::estimateAndSaveCost() {
  double tmpordercost;
  double tmptotalcost = estimateCost(tmpordercost);
  if(m_totalCost < 0 || tmptotalcost < m_totalCost) {
    m_totalCost = tmptotalcost;
    m_orderCost = tmpordercost;
    m_bestReadMethod.clear();
    for(UINT i = 0; i < m_fromTable.size(); i++) {
      FromTable &table = *m_fromTable[i];
      m_bestReadMethod.add(TableReadMethod(&table));
    }
  }
/*
    _tprintf(_T("saved bestjoinsequence (cost:%lg):"),m_cost);
    for(i = 0; i < m_bestjoinsequence.size(); i++)
      _tprintf(_T("%d:<%s>,"),m_bestjoinsequence[i],m_fromTable[i]->m_correlationName.cstr());
    _tprintf(_T("\n"));
*/
}

void SelectStmt::combinateReadDirection(int tableindex) {
  if(tableindex < m_noOfFixedTables)
    estimateAndSaveCost();
  else
  if(m_orderBy.size() == 0) { // no orderby. Set all tables readdirection to asc
    for(UINT i = 0; i < m_fromTable.size(); i++)
      m_fromTable[i]->m_asc = true;
    estimateAndSaveCost();
  }
  else {
    FromTable &table = *m_fromTable[tableindex];
    if(table.isFixedByUniqueKey() >= 0) // use asc only on this table
      combinateReadDirection(tableindex-1);
    else { // try both asc and desc read on each table
      table.m_asc = true;
      combinateReadDirection(tableindex-1);
      table.m_asc = false;
      combinateReadDirection(tableindex-1);
    }
  }
}

void SelectStmt::combinateUsedIndex(int tableindex) {
  if(tableindex < m_noOfFixedTables) {
    combinateReadDirection((int)m_fromTable.size()-1);
  } else {
    FromTable &table = *m_fromTable[tableindex];
    for(UINT i = 0; i < table.m_indexStat.size(); i++) {
      table.m_usedIndex = i;
      combinateUsedIndex(tableindex-1);
    }
  }
}

void SelectStmt::permuterJoinSequence(int nelem) {
  if(nelem <= m_noOfFixedTables + 1) {
    combinateUsedIndex((int)m_fromTable.size()-1);
  } else {
    nelem--;
    permuterJoinSequence(nelem);
    FromTable **p2 = &m_fromTable[nelem];
    for(int i = m_noOfFixedTables; i < nelem; i++) {
      FromTable **p1 = &m_fromTable[i];
      std::swap((*p1)->m_joinSequence,(*p2)->m_joinSequence);
      std::swap(*p1,*p2);
      permuterJoinSequence( nelem );
      std::swap(*p1,*p2);
      std::swap((*p1)->m_joinSequence,(*p2)->m_joinSequence);
    }
  }
}

// place the fixed tables first in joinsequence order
static int joinseqcmp(FromTable * const &t1, FromTable * const &t2) {
  int c = t2->m_fixed - t1->m_fixed;
  if(c) return c;
  return t1->m_joinSequence - t2->m_joinSequence;
}

void SelectStmt::findJoinSequence() {
#ifdef TRACECOMP
  _tprintf(_T("findJoinSequence for <%s>\n"),m_name.cstr());
#endif
  if(m_compiler.ok()) {
    for(;;) {
      // this is a transitive closure function. ie if a = b and b = :f then both a and b are fixed by const :f
      if(!addFieldsFixedByConstPredicate(m_whereClause) && !addFieldsFixedByConstPredicate(m_havingClause))
        break;
    }
  }

  UINT i;
//  for(i = 0; i < m_fromTable.size(); i++)
//    m_fromTable[i]->dump();

  CompactArray<FromTable*> reducedFromTable;
  // first remove tables not used
  for(i = 0; i < m_fromTable.size(); i++) {
    if(m_fromTable[i]->getNumberOfUsedColumns() == 0 && !m_fromTable[i]->m_countAggr) {
      delete m_fromTable[i];
      m_fromTable[i] = NULL;
      continue;
    }
    else
      reducedFromTable.add(m_fromTable[i]);
  }

  if(reducedFromTable.size() != m_fromTable.size())
    m_fromTable = reducedFromTable;

  for(i = 0; i < m_fromTable.size(); i++)
    m_fromTable[i]->calulateIndexOnly();

  m_fromTable.sort(joinseqcmp);
/*
  _tprintf(_T("joinsequence before permuter\n"));
  for(i = 0; i < m_fromTable.size(); i++) {
    FromTable &f = *m_fromTable[i];
    _tprintf(_T("%d:%s index<%s> <%s>\n"),
             m_fromTable[i]->m_joinSequence
            ,m_fromTable[i]->m_correlationName.cstr()
            ,m_fromTable[i]->getUsedIndex().m_indexDef.m_indexName.cstr()
            ,m_fromTable[i]->m_asc ? _T("asc") : _T("desc")
          );
  }
  PAUSE();
*/
  for(i = m_noOfFixedTables; i < m_fromTable.size(); i++)
    m_fromTable[i]->m_joinSequence = i;
  m_totalCost = -1; // negative initial value to be sure to catch the first real cost
#ifdef TRACECOST
  if(m_purpose == MAINSELECT) costcount = 0;
#endif
  if(isEmptySelect()) {
    m_totalCost = 0;
    m_orderCost = 0;
  }
  else {
    permuterJoinSequence((int)m_fromTable.size());
    for(i = 0; i < m_fromTable.size(); i++) {
      m_fromTable[i]          = m_bestReadMethod[i].m_table;
      FromTable &table        = *m_fromTable[i];
      table.m_usedIndex        = m_bestReadMethod[i].m_index;
      table.m_asc              = m_bestReadMethod[i].m_asc;
      table.m_indexSelectivity = m_bestReadMethod[i].m_indexSelectivity;
      table.m_selectivity      = m_bestReadMethod[i].m_selectivity;
      table.m_joinSequence     = i;
      table.m_keyPredicates    = m_bestReadMethod[i].m_keyPredicates;
    }

    m_fromTable.sort(joinseqcmp);

    reduceByKeyPredicates();


#ifdef TRACEMODUL
#ifdef TRACECOST
    _tprintf(_T("Best joinsequence <%s> (costcount:%ld)::\n"),m_name.cstr(),costcount);
#else
    _tprintf(_T("Best joinsequence <%s>\n"),m_name.cstr());
#endif
    for(i = 0; i < m_fromTable.size(); i++) {
      _tprintf(_T("%d:%s index<%s> <%s> expect to select %lg%% of the rows after read %lg%% of the table\n"),
               m_fromTable[i]->m_joinSequence
              ,m_fromTable[i]->m_correlationName.cstr()
              ,m_fromTable[i]->getUsedIndex().m_indexDef.m_indexName.cstr()
              ,m_fromTable[i]->m_asc ? _T("ASC") : _T("DESC")
              ,m_fromTable[i]->m_selectivity*100
              ,m_fromTable[i]->m_indexSelectivity*100
            );
      _tprintf(_T("KeyPredicates.\n")); m_fromTable[i]->m_keyPredicates.dump();
    }
#endif
  }
#ifdef TRACEMODUL
  _tprintf(_T("total cost:%lg cost of order by:%lg\n"),m_totalCost,m_orderCost);
#endif
}

bool SelectStmt::isEmptySelect() const {
  return ((m_whereClause  != NULL) && (m_whereClause->token()  == NUMBER) && (m_whereClause->number()  < 1))
      || ((m_havingClause != NULL) && (m_havingClause->token() == NUMBER) && (m_havingClause->number() < 1));
}

bool SelectSetOperator::isEmptySelect() const {
  switch(m_node->token()) {
  case SELECT       :
    return ((SelectStmt*)(this))->isEmptySelect();
  case UNION        :
    return m_son1->isEmptySelect() && m_son2->isEmptySelect();
  case INTERSECT    :
    return m_son1->isEmptySelect() || m_son2->isEmptySelect();
  case SETDIFFERENCE:
    return m_son1->isEmptySelect();
  default:
    stopcomp(m_node);
    return false;
  }
}

static ULONG syntaxNodeHash(const SyntaxNodeP &k ) {
#ifdef _M_X64
  return uint64Hash((UINT64)k);
#else
  return ulongHash((ULONG)k);
#endif
}

static int syntaxNodeCmp( const SyntaxNodeP &key, const SyntaxNodeP &tablekey ) {
  return sign(key - tablekey);
}

KeyPredicatesHashMap::KeyPredicatesHashMap()
: HashMap<SyntaxNodeP,int>(syntaxNodeHash,syntaxNodeCmp) {
}

void SelectStmt::reduceByKeyPredicates() {
  if(m_whereClause == NULL || m_whereClause->token() == NUMBER) return;
  KeyPredicatesHashMap hash;
  size_t i;
  for(i = 0; i < m_fromTable.size(); i++) {
    FromTable &table = *m_fromTable[i];
    size_t j;
    for(j = 0; j < table.m_keyPredicates.m_beginKeyPredicate.m_expr.size(); j++)
      hash.put(table.m_keyPredicates.m_beginKeyPredicate.m_expr[j],0);
    for(j = 0; j <table.m_keyPredicates.m_endKeyPredicate.m_expr.size(); j++)
      hash.put(table.m_keyPredicates.m_endKeyPredicate.m_expr[j],0);
  }
//  _tprintf(_T("hashsize:%d\n"),hash.count());


#ifdef TRACECOMP
  i = 0;
  for(Iterator<SyntaxNodeP> it = hash.keySet().getIterator(); it.hasNext();) {
    _tprintf(_T("pred[%zd]:%s"),i++, it.next()->toString().cstr());
    _tprintf(_T("\n"));
  }
#endif

  m_whereClause = m_compiler.reduceByKeyPredicates(m_whereClause,hash);
}
