#include "stdafx.h"
#include <Tokenizer.h>

bool isConstExpression(const SyntaxNode *expr) {
  switch(expr->token()) {
  case NUMBER   :
  case STRING   :
  case DATECONST:
  case TIMECONST:
  case TIMESTAMPCONST:
    return true;

  case NAME     :
  case HOSTVAR  :
  case PARAM    :
    return false;

  default:
    { int sons = expr->childCount();
      for(int i = 0; i < sons; i++) {
        if(!isConstExpression(expr->child(i))) return false;
      }
      return true;
    }
  }
}

const SyntaxNode *findFirstTableReference(const SyntaxNode *expr) {
  switch(expr->token()) {
  case STRING   :
  case NUMBER   :
  case DATECONST:
  case TIMECONST:
  case TIMESTAMPCONST:
    return nullptr;

  case NAME     :
    return expr;

  case HOSTVAR  :
  case PARAM    :
    return nullptr;

  default:
    { int sons = expr->childCount();
      for(int i = 0; i < sons; i++) {
        const SyntaxNode *p = findFirstTableReference(expr->child(i));
        if(p != nullptr) return p;
      }
      return nullptr;
    }
  }
}

const SyntaxNode *findFirstAggregateFunction(const SyntaxNode *expr) {
  switch(expr->token()) {
  case SUM    :
  case COUNT  :
  case MIN    :
  case MAX    :
    return expr;
  case DOT    :
  case NAME   :
  case NUMBER :
  case STRING :
  case DATECONST:
  case TIMECONST:
  case TIMESTAMPCONST:
  case HOSTVAR:
  case PARAM  :
    return nullptr;
  default:
    { int sons = expr->childCount();
      for(int i = 0; i < sons; i++) {
        const SyntaxNode *p = findFirstAggregateFunction(expr->child(i));
        if(p != nullptr) return p;
      }
      return nullptr;
    }
  }
}

static TupleField getTupleField(const SyntaxNode *n) {
  TupleField f;
  switch(n->token()) {
  case NUMBER        :
    f = n->number();
    break;
  case STRING        :
    f = n->str();
    break;
  case DATECONST     :
    f = n->getDate();
    break;
  case TIMECONST     :
    f = n->getTime();
    break;
  case TIMESTAMPCONST:
    f = n->getTimestamp();
    break;
  case NULLVAL       :
    f.setUndefined();
    break;
  default            :
    stopcomp(n);
  }
  return f;
}

int constExpressionCmp(const SyntaxNode *n1, const SyntaxNode *n2) { // types have been checked
  return compare(getTupleField(n1),getTupleField(n2));
}

// is n a simple name and is it belonging to some index
bool SqlCompiler::isSimpleIndexField(const SyntaxNode *n) {
  switch(n->token()) {
  case NAME:
  case DOT :
    { const StatementSymbolInfo *ssi = getInfo(n);
      if(ssi == nullptr) return false; // just in case
      const StatementTable &table = ssi->m_table;
      for(UINT i = 0; i < table.m_indexStat.size(); i++) {
        const IndexDefinition &indexDef = table.m_indexStat[i].m_indexDef;
        if(indexDef.columnIsMember(ssi->m_colIndex)) return true;
      }
      return false;
    }
  default  :
    return false;
  }
}

bool SqlCompiler::isNullPossible(const SyntaxNode *n) { // n = <expr>
  switch(n->token()) {
  case NUMBER   :
  case STRING   :
  case DATECONST:
  case TIMECONST:
  case TIMESTAMPCONST:
    return false;
  case NAME     :
  case DOT      :
    { const StatementSymbolInfo *ssi = getInfo(n);
      if(ssi == nullptr) return false; // just in case
      return ssi->isNullAllowed();
    }
  case HOSTVAR  :
    return isNullAllowed(m_bndstmt.m_inHost[n->getHostVarIndex()].getType());
  case PARAM    :
    return true;
  case MINUS    :
    if(n->childCount() == 1)
      return isNullPossible(n->child(0));
    // continue case
  case PLUS     :
  case MULT     :
  case DIVOP    :
  case MODOP    :
  case EXPO     :
    return isNullPossible(n->child(0)) || isNullPossible(n->child(1));
  case SUBSTRING:
    return isNullPossible(n->child(0)) || isNullPossible(n->child(1)) || isNullPossible(n->child(2));

  case TYPEDATE :
    return isNullPossible(n->child(0)) || isNullPossible(n->child(1)) || isNullPossible(n->child(2));

  case CONCAT   :
    return isNullPossible(n->child(0)) || isNullPossible(n->child(1));

  case COUNT    :
    return false;
  case MIN      :
  case MAX      :
    return true;
  case SUM      :
    return true;
  case NULLVAL:
    return true;
  case CURRENTDATE:
  case CURRENTTIME:
  case CURRENTTIMESTAMP:
    return false;
  case SELECT:
  case UNION:
  case INTERSECT:
  case SETDIFFERENCE:
    return true;
  default:
    stopcomp(n);
    break;
  }
  return true;
}

static int maxFieldLen(const SqlCompiler &compiler, const SyntaxNode *n) {
  switch(n->token()) {
  case NAME:
  case DOT :
    { const StatementSymbolInfo *ssi = compiler.getInfo(n);
      if(ssi == nullptr) return -1; // just in case
      return ssi->len();
    }
  default:
    return -1;
  }
}

static String genMaxColSeqString(const Database &db, int len) {
  TCHAR *s = new TCHAR[len+1];
  _TUCHAR ch = db.getMaxColSeq();
  int i;
  for(i = 0; i < len; i++) {
    s[i] = ch;
  }
  s[i] = '\0';
  String res(s);
  delete[] s;
  return res;
}

static bool charSequence(const TCHAR *from, const TCHAR *to, TCHAR ch) {
  for(const TCHAR *s = from; s <= to; s++) {
    if(*s != ch) {
      return false;
    }
  }
  return true;
}

static int noOfNonWildCardSequences(const TCHAR *str) {
  int count = 0;
  for(Tokenizer tok(str,_T("_%")); tok.hasNext(); tok.next())
    count++;
//  _tprintf(_T("no of noOfNonWildCardSequences for <%s> : %d\n"),str,i);
  return count;
}

// find the minimal length String must have to match str in a like-clause
static int findMinLikeLen(const TCHAR *str) {
  int count = 0;
  for(const TCHAR *s = str; *s; s++) {
    if(*s != '%') {
      count++;
    }
  }
  return count;
}

// converts predicate 'e1 like conststr' to:
// 'e1 between s1 and s2' [and e1 like conststr] if possible
// or
// 'e1 = conststr' if conststr does not contain any wildcards
// or
// 'false' if possible
// or not converted if not possible
static SyntaxNode *reduceLike(SqlCompiler &compiler, const SyntaxNode *e1, const TCHAR *str) {
  int minlength = findMinLikeLen(str);
  int maxlen    = compiler.evaluateMaxStringLength(e1);

  if(maxlen < minlength)
    return compiler.fetchNumberNode(0); // predicate is false

  int len = (UINT)_tcslen(str);
  const TCHAR *firstpercent    = _tcschr( str,'%');
  const TCHAR *lastpercent     = _tcschr(str,'%');
  const TCHAR *firstunderscore = _tcschr( str,'_');
  const TCHAR *firstwildcard = (firstpercent    == nullptr) ? firstunderscore
                             : (firstunderscore == nullptr) ? firstpercent
                             : min(firstpercent,firstunderscore);

  if(firstwildcard == nullptr) // no wildcards in String
    return compiler.fetchTokenNode(EQUAL,e1,compiler.fetchStringNode(str),nullptr);


  if(firstwildcard > str && (noOfNonWildCardSequences(str) > 1 || firstunderscore != nullptr)) { // str is "abcd%ef%... or abcd_..."
    if(compiler.isSimpleIndexField(e1)) {
      size_t    firstwildcardpos = firstwildcard - str;
      String    tmp(str);
      tmp[firstwildcardpos] = '\0';
      String    lower(tmp.cstr());
      String    toUpperCase(lower + genMaxColSeqString(compiler.m_db,(int)(maxlen-lower.length())));
      return compiler.fetchTokenNode(
               AND,
               compiler.fetchTokenNode(
                       AND
                      ,compiler.fetchTokenNode(
                          RELOPLE
                         ,compiler.fetchStringNode(lower.cstr())
                         ,e1
                         ,nullptr
                       )
                      ,compiler.fetchTokenNode(
                          RELOPLE
                         ,e1
                         ,compiler.fetchStringNode(toUpperCase.cstr())
                         ,nullptr
                       )
                      ,nullptr
                    )
               ,compiler.fetchTokenNode(LIKE,e1,compiler.fetchStringNode(str),nullptr)
               ,nullptr);
    }
    else
      return compiler.fetchTokenNode(LIKE,e1,compiler.fetchStringNode(str),nullptr);
  }

  if(firstpercent != nullptr && firstunderscore == nullptr) { // at least one '%' and no '_'
    if((lastpercent - str == (len - 1)) && charSequence(firstpercent,lastpercent,'%')) {
        // str terminate with %%%.. and has no wildcards elsewhere
      size_t firstpercentpos = firstpercent - str;
      if(maxlen > 0) { // actually an error if maxlen <= 0
        String    lower = substr(str,0,firstpercentpos);
        if((UINT)maxlen > lower.length()) {
          String    upper(lower + genMaxColSeqString(compiler.m_db,(int)(maxlen-lower.length())));
          return compiler.fetchTokenNode(
                    AND
                   ,compiler.fetchTokenNode(
                       RELOPLE
                      ,compiler.fetchStringNode(lower.cstr())
                      ,e1
                      ,nullptr
                    )
                   ,compiler.fetchTokenNode(
                       RELOPLE
                      ,e1
                      ,compiler.fetchStringNode(upper.cstr())
                      ,nullptr
                    )
                   ,nullptr
                 );
        }
        else if((UINT)maxlen == lower.length()) {
          return compiler.fetchTokenNode(EQUAL,e1,compiler.fetchStringNode(lower.cstr()),nullptr);
        }
        else // maxlen(e1) < position of first % => predicate is false
          return compiler.fetchNumberNode(0);
      }
    }
  }

  return compiler.fetchTokenNode(LIKE,e1,compiler.fetchStringNode(str),nullptr);
}

int SqlCompiler::evaluateMaxStringLength(const SyntaxNode *n) { // n is stringexpression. returns -1 on undefined
  switch(n->token()) {
  case STRING:
    return (int)_tcslen(n->str());
  case DOT   :
  case NAME  :
    { StatementSymbolInfo *info = getInfo(n);
      const ColumnDefinition &col = info->getColumn();
      if(!isStringType(col.getType()))
        throwSqlError(SQL_FATAL_ERROR,_T("evaluateMaxStringLength:type of %s not String"),col.m_name.cstr());
      return col.m_len;
    }
  case NULLVAL:
    return -1;
  case CONCAT:
    { int l1 = evaluateMaxStringLength(n->child(0));
      if(l1 < 0)
        return -1;
      int l2 = evaluateMaxStringLength(n->child(1));
      if(l2 < 0)
        return -1;
      return l1 + l2;
    }
  case SUBSTRING:
    { int l1 = evaluateMaxStringLength(n->child(0));
      if(l1 < 0)
        return -1;
      if(!isConstExpression(n->child(2)))
        return l1;
      bool arg3known;
      double l3 = evaluateConstNumericExpression(n->child(2),arg3known);
      if(!arg3known)
        return l1;
      else if(l3 < 0)
        return 0;
      else
        return min(l1,(int)l3);
    }
  case HOSTVAR:
    { const HostVarDescription &var = m_bndstmt.m_inHost[n->getHostVarIndex()];
      if(!isStringType(var.getType()))
        throwSqlError(SQL_FATAL_ERROR,_T("evaluateMaxStringLength:type of hostvar <%s> not String"),n->hostvar());
      return var.sqllen;
    }

  case CAST :
    { CastParameter p = genCastParameter(n->child(1));
      if((p.getType() == DBTYPE_CSTRING) || (p.getType() == DBTYPE_WSTRING) || (p.getType() == DBTYPE_VARCHAR)) {
        return p.m_len;
      }
      stopcomp(n); // must be string of varchar
      return 1;
    }
  case SELECT:
  case UNION:
  case INTERSECT:
  case SETDIFFERENCE:
    { const HostVarDescriptionList &var = ((SelectSetOperator*)n->getData())->getDescription();
      return var[0].sqllen;
    }
  default:
    throwSqlError(SQL_FATAL_ERROR,_T("evaluateMaxStringLength called for invalid stringexpression <%s>"),n->toString().cstr());
    return 20;
  }
}

double SqlCompiler::evaluateConstNumericExpression(const SyntaxNode *expr, bool &valueknown) {
  switch(expr->token()) {
  case NUMBER :
    valueknown = true;
    return expr->number();
  case PLUS   :
    { bool e1known;
      double e1 = evaluateConstNumericExpression(expr->child(0),e1known);
      if(!e1known) { valueknown = false; return 0; }
      else
        return e1 + evaluateConstNumericExpression(expr->child(1),valueknown);
    }
    break;
  case MINUS:
    { bool e1known;
      double e1 = evaluateConstNumericExpression(expr->child(0),e1known);
      if(!e1known) { valueknown = false; return 0; }
      if(expr->childCount() > 1)
        return e1 - evaluateConstNumericExpression(expr->child(1),valueknown);
      else
        return -e1;
    }
    break;
  case MULT:
    { bool e1known;
      double e1 = evaluateConstNumericExpression(expr->child(0), e1known);
      if(!e1known) { valueknown = false; return 0; }
      return e1 * evaluateConstNumericExpression(expr->child(1),valueknown);
    }
    break;
  case DIVOP:
    { bool e2known;
      double e2 = evaluateConstNumericExpression(expr->child(1),e2known);
      if(!e2known) { valueknown = false; return 0; }
      if(e2 == 0) {
        syntaxError(expr,SQL_DIVISION_BY_ZERO,_T("Division by zero"));
        valueknown = false;
        return 0;
      }
      return evaluateConstNumericExpression(expr->child(0),valueknown) / e2;
    }
    break;
  case MODOP:
    { bool e2known;
      double e2 = evaluateConstNumericExpression(expr->child(1),e2known);
      if(!e2known) { valueknown = false; return 0; }
      if(e2 == 0) {
        syntaxError(expr,SQL_MODULUS_BY_ZERO,_T("Modulus by zero"));
        valueknown = false;
        return 0;
      }
      return fmod(evaluateConstNumericExpression(expr->child(0),valueknown),e2);
    }
  case EXPO:
    { bool e1known,e2known;
      double e1 = evaluateConstNumericExpression(expr->child(0),e1known);
      if(!e1known) { valueknown = false; return 0; }
      double e2 = evaluateConstNumericExpression(expr->child(1),e2known);
      if(!e2known) { valueknown = false; return 0; }
      try {
        return sqlPow(e1,e2);
      } catch(sqlca ca) {
        syntaxError(expr,ca.sqlcode,_T("%s"),ca.sqlerrmc);
        return 0;
      }
    }
    break;
  default:
    valueknown = false;
    return 0;
  }
}

bool isEmptySelect(const SyntaxNode *n) { // n == union,intersect,setdifference,SelectStmt
  switch(n->token()) {
  case SELECT       :
    { SelectStmt *stmt = (SelectStmt*)n->getData();
      return stmt->isEmptySelect();
    }
  case UNION        :
  case INTERSECT    :
  case SETDIFFERENCE:
    { SelectSetOperator *op = (SelectSetOperator*)n->getData();
      return op->isEmptySelect();
    }
  default:
    stopcomp(n);
    return false;
  }
}

class ReducedExpr {
public:
  ReducedExpr(SqlCompiler &compiler, SyntaxNode *n);
  SyntaxNode *m_expr;
  bool        m_isconst;
  bool isConstNull() const { return m_isconst && m_expr->token() == NULLVAL; }
};

ReducedExpr::ReducedExpr(SqlCompiler &compiler, SyntaxNode *n) {
  m_expr = compiler.reduceExpression(n,m_isconst);
}

class ReducedExprList : public Array<ReducedExpr> {
  SqlCompiler &m_compiler;
public:
  ReducedExprList(SqlCompiler &compiler, const SyntaxNode *n);
  const SyntaxNode *getReducedExprList(int first = 0);
  const SyntaxNode *getReducedExprList(const CompactIntArray &set);
};

ReducedExprList::ReducedExprList(SqlCompiler &compiler, const SyntaxNode *n) : m_compiler(compiler) {
  NodeList list((SyntaxNode*)n);
  for(UINT i = 0; i < list.size();i++)
    add(ReducedExpr(m_compiler,list[i]));
}

const SyntaxNode *ReducedExprList::getReducedExprList(int first) {
  const SyntaxNode *last = (*this)[first].m_expr;
  size_t n = size();
  for(size_t i = first + 1; i < n; i++)
    last = m_compiler.fetchTokenNode(COMMA,last,(*this)[i].m_expr,nullptr);
  return last;
}

const SyntaxNode *ReducedExprList::getReducedExprList(const CompactIntArray &set) {
  const SyntaxNode *last = (*this)[set[0]].m_expr;
  size_t n = set.size();
  for(size_t i = 1; i < n; i++)
    last = m_compiler.fetchTokenNode(COMMA,last,(*this)[set[i]].m_expr,nullptr);
  return last;
}

static SyntaxNode *getSyntaxNode(SqlCompiler *comp, const TupleField &f) {
  if(!f.isDefined())
    return comp->fetchTokenNode(NULLVAL,nullptr);
  switch(getMainType(f.getType())) {
  case MAINTYPE_NUMERIC   :
    { double d;
      f.get(d);
      return comp->fetchNumberNode(d);
    }
  case MAINTYPE_STRING    :
  case MAINTYPE_VARCHAR   :
    { String    s;
      f.get(s);
      return comp->fetchStringNode(s.cstr());
    }
  case MAINTYPE_DATE      :
    { Date d;
      f.get(d);
      return comp->fetchDateNode(d);
    }
  case MAINTYPE_TIME      :
    { Time d;
      f.get(d);
      return comp->fetchTimeNode(d);
    }
  case MAINTYPE_TIMESTAMP :
    { Timestamp d;
      f.get(d);
      return comp->fetchTimestampNode(d);
    }
  case MAINTYPE_VOID      :
    return comp->fetchTokenNode(NULLVAL,nullptr);
  default:
    throwSqlError(SQL_FATAL_ERROR,_T("getSyntaxNode - Invalid DbMainType:%d"),getMainType(f.getType()));
  }
  return nullptr;
}

SyntaxNode *SqlCompiler::reduceExpression(SyntaxNode *n, bool &isconst) {
  bool v1const,v2const,v3const,v4const,v5const,v6const;
  SyntaxNode *v1,*v2,*v3,*v4,*v5,*v6;
  isconst = false;
  switch(n->token()) {
  case PLUS     :
    v1 = reduceExpression(n->child(0),v1const);
    v2 = reduceExpression(n->child(1),v2const);
    if(v1const && v1->token() == NULLVAL) { isconst = true; return v1; }
    if(v2const && v2->token() == NULLVAL) { isconst = true; return v2; }
    if(v1const && v2const) {
      isconst = true;
      if(v1->token() == DATECONST) {
        try {
          return fetchDateNode(v1->getDate() + (int)v2->number());
        } catch(Exception e) {
          syntaxError(n,SQL_DOMAIN_ERROR,_T("%s"),e.what());
          return n;
        }
      }
      else
        return fetchNumberNode(v1->number() + v2->number());
    }
    if(v1const) {    // v2 not const
      if(v1->token() == NUMBER && v1->number() == 0)
        return v2;
    }
    else if(v2const) // v2 const, v1 not const
      if(v2->number() == 0)
        return v1;
                     // both v1 and v2 are non const
    return fetchTokenNode(PLUS,v1,v2,nullptr);

  case MINUS    :
    if(n->childCount() == 1) {
      v1 = reduceExpression(n->child(0),v1const);
      if(v1const && v1->token() == NULLVAL) { isconst = true; return v1; }
      if(v1const) {
        isconst = true;
        return fetchNumberNode(-v1->number());
      }
      return fetchTokenNode(MINUS,v1,nullptr);
    }
    else {           // 2 sons;
      v1 = reduceExpression(n->child(0),v1const);
      v2 = reduceExpression(n->child(1),v2const);
      if(v1const && v1->token() == NULLVAL) { isconst = true; return v1; }
      if(v2const && v2->token() == NULLVAL) { isconst = true; return v2; }
      if(v1const && v2const) {
        isconst = true;
        if(v1->token() == NUMBER && v2->token() == NUMBER)
          return fetchNumberNode(v1->number() - v2->number());
        else
          if(v1->token() == DATECONST && v2->token() == NUMBER) {
            try {
              return fetchDateNode(v1->getDate() - (int)v2->number());
            } catch(Exception e) {
              syntaxError(n,SQL_DOMAIN_ERROR,_T("%s"),e.what());
              return n;
            }
          }
          else
            if(v1->token() == DATECONST && v2->token() == DATECONST)
              return fetchNumberNode(v1->getDate() - v2->getDate());
      }
      if(v1const) {  // v2 not const
        if(v1->token() == NUMBER && v1->number() == 0)
          return fetchTokenNode(MINUS,v2,nullptr);
      }
      else if(v2const) // v2 const, v1 not const
        if(v2->token() == NUMBER && v2->number() == 0)
          return v1;
      // both v1 and v2 are non const
      return fetchTokenNode(MINUS,v1,v2,nullptr);
    }
    break;

  case MULT     :
    v1 = reduceExpression(n->child(0),v1const);
    v2 = reduceExpression(n->child(1),v2const);
    if(v1const && v1->token() == NULLVAL) { isconst = true; return v1; }
    if(v2const && v2->token() == NULLVAL) { isconst = true; return v2; }
    if(v1const && v2const) {
      isconst = true;
      return fetchNumberNode(v1->number() * v2->number());
    }
    if(v1const) { // v2 not const
      if(v1->number() == 0) { // result is 0
        isconst = true;
        return v1;
      }
      if(v1->number() == 1)
        return v2;
    }
    else if(v2const) { // v2 const, v1 not const
      if(v2->number() == 0) { // result is 0
        isconst = true;
        return v2;
      }
      if(v2->number() == 1)
        return v1;
    }
    return fetchTokenNode(MULT,v1,v2,nullptr);

  case DIVOP    :
    v1 = reduceExpression(n->child(0),v1const);
    v2 = reduceExpression(n->child(1),v2const);
    if(v1const && v1->token() == NULLVAL) { isconst = true; return v1; }
    if(v2const && v2->token() == NULLVAL) { isconst = true; return v2; }
    if(v1const && v2const) {
      isconst = true;
      if(v2->number() == 0) {
        syntaxError(n,SQL_DIVISION_BY_ZERO,_T("Division by zero"));
        return v2;
      }
      return fetchNumberNode(v1->number() / v2->number());
    }
    if(v1const) { // v2 not const
      if(v1->number() == 0) { // result is 0
        isconst = true;
        return v1;
      }
    }
    else if(v2const) { // v2 const, v1 not const
      if(v2->number() == 0) { // result is 0
        syntaxError(n,SQL_DIVISION_BY_ZERO,_T("Division by zero"));
        return v2;
      }
      if(v2->number() == 1)
        return v1;
    }
    return fetchTokenNode(DIVOP,v1,v2,nullptr);

  case MODOP    :
    v1 = reduceExpression(n->child(0),v1const);
    v2 = reduceExpression(n->child(1),v2const);
    if(v1const && v1->token() == NULLVAL) { isconst = true; return v1; }
    if(v2const && v2->token() == NULLVAL) { isconst = true; return v2; }
    if(v1const && v2const) {
      isconst = true;
      if(v2->number() == 0) {
        syntaxError(n,SQL_MODULUS_BY_ZERO,_T("Modulus by zero"));
        return v2;
      }
      return fetchNumberNode(fmod(v1->number(), v2->number()));
    }
    if(v1const) { // v2 not const
      if(v1->number() == 0) { // result is 0
        isconst = true;
        return v1;
      }
    }
    else if(v2const) { // v2 const, v1 not const
      if(v2->number() == 0) { // result is 0
        syntaxError(n,SQL_MODULUS_BY_ZERO,_T("Modulus by zero"));
        return v2;
      }
      if(v2->number() == 1)
        return v1;
    }
    return fetchTokenNode(MODOP,v1,v2,nullptr);

  case EXPO     :
    v1 = reduceExpression(n->child(0),v1const);
    v2 = reduceExpression(n->child(1),v2const);
    if(v1const && v1->token() == NULLVAL) { isconst = true; return v1; }
    if(v2const && v2->token() == NULLVAL) { isconst = true; return v2; }
    if(v1const && v2const) {
      isconst = true;
      try {
        return fetchNumberNode(sqlPow(v1->number(),v2->number()));
      } catch(sqlca ca) {
        syntaxError(n,ca.sqlcode,_T("%s"),ca.sqlerrmc);
        return v2;
      }
    }
    if(v1const) { // v2 not const
      if(v1->number() == 1) {
        isconst = true;
        return v1;
      }
    }
    else if(v2const) { // v2 const, v1 not const
      if(v2->number() == 0) { // result is 1 (x ** 0 is 1 for all x)
        isconst = true;
        return fetchNumberNode(1);
      }
      if(v2->number() == 1)
        return v1;
    }
    return fetchTokenNode(EXPO,v1,v2,nullptr);

  case NUMBER   :
  case STRING   :
  case DATECONST:
  case TIMECONST:
  case TIMESTAMPCONST:
    { isconst = true;
      return n;
    }
  case HOSTVAR  :
  case PARAM    :
  case DOT      :
  case NAME     :
    { isconst = false;
      return n;
    }
  case SUBSTRING:
    v1 = reduceExpression(n->child(0),v1const);
    v2 = reduceExpression(n->child(1),v2const);
    v3 = reduceExpression(n->child(2),v3const);
    if(v1const && v1->token() == NULLVAL) { isconst = true; return v1; }
    if(v2const && v2->token() == NULLVAL) { isconst = true; return v2; }
    if(v3const && v3->token() == NULLVAL) { isconst = true; return v3; }
    if(v1const && v2const && v3const) {
      isconst = true;
      return fetchStringNode(sqlSubString(v1->str(),(int)v2->number(),(int)v3->number()).cstr());
    }
    return fetchTokenNode(SUBSTRING,v1,v2,v3,nullptr);

  case TYPEDATE :
    v1 = reduceExpression(n->child(0),v1const);
    v2 = reduceExpression(n->child(1),v2const);
    v3 = reduceExpression(n->child(2),v3const);
    if(v1const && v1->token() == NULLVAL) { isconst = true; return v1; }
    if(v2const && v2->token() == NULLVAL) { isconst = true; return v2; }
    if(v3const && v3->token() == NULLVAL) { isconst = true; return v3; }
    if(v1const && v2const && v3const) {
      isconst = true;
      try {
        return fetchDateNode(Date((int)v1->number(),(int)v2->number(),(int)v3->number()));
      } catch(Exception e) {
        syntaxError(n->child(0),SQL_INVALIDDATE,_T("%s"),e.what());
        return n;
      }
    }
    return fetchTokenNode(TYPEDATE,v1,v2,v3,nullptr);

  case TYPETIME :
    v1 = reduceExpression(n->child(0),v1const);
    v2 = reduceExpression(n->child(1),v2const);
    v3 = reduceExpression(n->child(2),v3const);
    if(v1const && v1->token() == NULLVAL) { isconst = true; return v1; }
    if(v2const && v2->token() == NULLVAL) { isconst = true; return v2; }
    if(v3const && v3->token() == NULLVAL) { isconst = true; return v3; }
    if(v1const && v2const && v3const) {
      isconst = true;
      try {
        return fetchTimeNode(Time((int)v1->number(),(int)v2->number(),(int)v3->number()));
      } catch(Exception e) {
        syntaxError(n->child(0),SQL_INVALIDTIME,_T("%s"),e.what());
        return n;
      }
    }
    return fetchTokenNode(TYPETIME,v1,v2,v3,nullptr);

  case TYPETIMESTAMP :
    v1 = reduceExpression(n->child(0),v1const);
    v2 = reduceExpression(n->child(1),v2const);
    v3 = reduceExpression(n->child(2),v3const);
    v4 = reduceExpression(n->child(3),v4const);
    v5 = reduceExpression(n->child(4),v5const);
    v6 = reduceExpression(n->child(5),v6const);
    if(v1const && v1->token() == NULLVAL) { isconst = true; return v1; }
    if(v2const && v2->token() == NULLVAL) { isconst = true; return v2; }
    if(v3const && v3->token() == NULLVAL) { isconst = true; return v3; }
    if(v4const && v4->token() == NULLVAL) { isconst = true; return v4; }
    if(v5const && v5->token() == NULLVAL) { isconst = true; return v5; }
    if(v6const && v6->token() == NULLVAL) { isconst = true; return v6; }
    if(v1const && v2const && v3const && v4const && v5const && v6const) {
      isconst = true;
      try {
        return fetchTimestampNode(Timestamp((int)v1->number(),(int)v2->number(),(int)v3->number(),
                                            (int)v4->number(),(int)v5->number(),(int)v6->number()
                                           ));
      } catch(Exception e) {
        syntaxError(n->child(0),SQL_INVALIDTIMESTAMP,_T("%s"),e.what());
        return n;
      }
    }
    return fetchTokenNode(TYPETIME,v1,v2,v3,nullptr);

  case CONCAT   :
    v1 = reduceExpression(n->child(0),v1const);
    v2 = reduceExpression(n->child(1),v2const);
    if(v1const && v1->token() == NULLVAL) { isconst = true; return v1; }
    if(v2const && v2->token() == NULLVAL) { isconst = true; return v2; }
    if(v1const && v2const) {
      isconst = true;
      return fetchStringNode((String   (v1->str()) + String   (v2->str())).cstr());
    }
    else
      return fetchTokenNode(CONCAT,v1,v2,nullptr);

  case COUNT    :
    v1 = n->child(0);
    if(v1->token() == STAR)
      return n;
    v2 = reduceExpression(v1->child(0),v2const);
    return fetchTokenNode(COUNT,fetchTokenNode(v1->token(),v2,nullptr),nullptr);

  case MIN      :
  case MAX      :
    v1 = reduceExpression(n->child(1),v1const);
    if(v1const) return v1;
    return fetchTokenNode(n->token(),n->child(0),v1,nullptr);

  case SUM             :
    v1 = reduceExpression(n->child(1),v1const);
    return fetchTokenNode(SUM,n->child(0),v1,nullptr);

  case NULLVAL         :
    isconst = true;
    return n;

  case CURRENTDATE     :
  case CURRENTTIME     :
  case CURRENTTIMESTAMP:
    isconst = false;
    return n;

  case CAST            :
    v1 = reduceExpression(n->child(0),v1const);
    if(v1const) {
      try {
        CastParameter p(genCastParameter(n->child(1)));
        TupleField f(sqlcast(getTupleField(v1),p));
        isconst = true;
        return getSyntaxNode(this,f);
      } catch(sqlca ca) {
        syntaxError(n->child(0),ca.sqlcode,_T("%s"),ca.sqlerrmc);
        return n;
      }
    }
    else
      return fetchTokenNode(CAST,v1,n->child(1),nullptr);
  case SELECT          :
  case UNION           :
  case INTERSECT       :
  case SETDIFFERENCE   :
    return n;
  }
  stopcomp(n);
  return n; // just to make compiler happy
}

PossiblePredicateValues operator||(const PossiblePredicateValues &v1, const PossiblePredicateValues &v2) {
  PossiblePredicateValues res;

  if(v1.falsePossible() && v2.falsePossible())
    res.insert(predfalse);
  if(v1.nullPossible() && (v2.nullPossible() || v2.falsePossible())
   ||v2.nullPossible() && (v1.nullPossible() || v1.falsePossible()) )
    res.insert(predundef);
  if(v1.truePossible()  || v2.truePossible())
    res.insert(predtrue );
  return res;
}

PossiblePredicateValues operator&&(const PossiblePredicateValues &v1, const PossiblePredicateValues &v2) {
  PossiblePredicateValues res;
  if(v1.falsePossible() || v2.falsePossible())
    res.insert(predfalse);
  if(v1.nullPossible() && (v2.nullPossible() || v2.truePossible())
   ||v2.nullPossible() && (v1.nullPossible() || v1.truePossible()) )
    res.insert(predundef);
  if(v1.truePossible()  && v2.truePossible())
    res.insert(predtrue );
  return res;
}

PossiblePredicateValues operator~( const PossiblePredicateValues  &v) {
  PossiblePredicateValues res;
  if(v.falsePossible())
    res.insert(predtrue );
  if(v.nullPossible())
    res.insert(predundef);
  if(v.truePossible())
    res.insert(predfalse);
  return res;
}

char PossiblePredicateValues::bitCount[] = { 0,1,1,2,1,2,2,3 };

void PossiblePredicateValues::dump(FILE *f) {
  _ftprintf(f,_T("possible predicatevalues:") );
  if(falsePossible()) _ftprintf(f,_T("false "));
  if(nullPossible())  _ftprintf(f,_T("undef "));
  if(truePossible())  _ftprintf(f,_T("true")  );
  _ftprintf(f,_T("\n"));
}

// returns const predicates as a number SyntaxNode with
// false     as  0
// true      as  1
// undefined as -1
SyntaxNode *SqlCompiler::reducePredicate(SyntaxNode *n, PossiblePredicateValues &values) {
  values.clear();
  switch(n->token()) {
  case AND    :
    { PossiblePredicateValues v1v,v2v;
      SyntaxNode *v1 = reducePredicate(n->child(0),v1v);
      SyntaxNode *v2 = reducePredicate(n->child(1),v2v);
      values = v1v && v2v;
      if(v1v.isConst())
        switch((int)v1->number()) {
        case  0: return v1;       // result is false
        case  1: return v2;       // result is v2
        case -1:
          if(!v2v.isConst())      // "undef and v2" equals false if v2=false and undef else
            return fetchTokenNode(AND,v1,v2,nullptr);
          switch((int)v2->number()) {
          case  0: return v2;     // result is false
          case  1: return v1;     // result is undef
          case -1: return v1;     // result is undef

          }
        }
                                  // v1const = false
      if(v2v.isConst())
        switch((int)v2->number()) {
        case  0: return v2;       // result is false
        case  1: return v1;       // result is v1
        case -1:                  // "v1 and undef" equals false if v1=false and undef else
          return fetchTokenNode(AND,v1,v2,nullptr);
        }
                                  // both v1 and v2 are non-const
      return fetchTokenNode(AND,v1,v2,nullptr);
    }

  case OR     :
    { PossiblePredicateValues v1v,v2v;
      SyntaxNode *v1 = reducePredicate(n->child(0),v1v);
      SyntaxNode *v2 = reducePredicate(n->child(1),v2v);
      values = v1v || v2v;
//      _tprintf(_T("v1const:%s\nv1:"),boolToStr(v1const));
//      dumpSyntaxTree(v1);
      if(v1v.isConst())
        switch((int)v1->number()) {
        case  0: return v2;       // result is v1
        case  1: return v1;       // result is true
        case -1:
          if(!v2v.isConst())      // "undef or v2" equals true if v2=true and undef else
            return fetchTokenNode(OR,v1,v2,nullptr);
          switch((int)v2->number()) {
          case  0: return v1;     // result is undef
          case  1: return v2;     // result is true
          case -1: return v2;     // result is undef
          }
        }

                                  // v1const = false
      if(v2v.isConst())
        switch((int)v2->number()) {
        case  0: return v1;       // result is v1
        case  1: return v2;       // result is true
        case -1:                  // "v1 or undef" equals true if v1=true and undef else
          return fetchTokenNode(OR,v1,v2,nullptr);
        }
                                  // both v1 and v2 are non-const
      return fetchTokenNode(OR,v1,v2,nullptr);
    }
  case NOT    :
    { PossiblePredicateValues vv;
      SyntaxNode *v = reducePredicate(n->child(0),vv);
      if(vv.isConst()) {
        values = ~vv;
        switch((int)v->number()) {
        case  0: return fetchNumberNode(1); // result is true;
        case  1: return fetchNumberNode(0); // result is false
        case -1: return v;                  // result is undef
        }
      }
      switch(v->token()) {
      case NOT:
        values = vv;
        return v->child(0);
      case NOTEQ:
        values = vv;
        return fetchTokenNode(EQUAL,v->child(0)  ,v->child(1),nullptr);
      case RELOPLE:
        values = vv;
        return fetchTokenNode(RELOPGT,v->child(0),v->child(1),nullptr);
      case RELOPLT:
        values = vv;
        return fetchTokenNode(RELOPGE,v->child(0),v->child(1),nullptr);
      case RELOPGE:
        values = vv;
        return fetchTokenNode(RELOPLT,v->child(0),v->child(1),nullptr);
      case RELOPGT:
        values = vv;
        return fetchTokenNode(RELOPLE,v->child(0),v->child(1),nullptr);
      default:
        values = ~vv;
        return fetchTokenNode(NOT,v,nullptr);
      }
    }
  case LIKE   :
    { bool v1const,v2const;
      SyntaxNode *v1 = reduceExpression(n->child(0), v1const);
      SyntaxNode *v2 = reduceExpression(n->child(1), v2const);
      if(v1const && v1->token() == NULLVAL) {
        values.insert(predundef);
        return fetchNumberNode(-1);  // result is undef
      }
      if(v2const && v2->token() == NULLVAL) {
        values.insert(predundef);
        return fetchNumberNode(-1);  // result is undef
      }

      if(v1const && v2const) {
        bool result = false;
        try {
          result = isLike(v1->str(),v2->str());
        } catch(sqlca ca) {
          syntaxError(n,ca.sqlcode,_T("%s"),ca.sqlerrmc);
        }
        values.insert(result?predtrue:predfalse);
        return fetchNumberNode(result?1:0);
      }
      else if(v2const) { // !v1const
        values = predfalse | predtrue;
        if(isNullPossible(v1))
          values.insert(predundef);
        return reduceLike(*this,v1,v2->str());
      }
      else {
        values = predfalse | predtrue;
        if(isNullPossible(v1) || isNullPossible(v2))
          values.insert(predundef);
        return fetchTokenNode(LIKE,v1,v2,nullptr);
      }
    }
  case ISNULL:
    { bool vconst;
      SyntaxNode *v = reduceExpression(n->child(0),vconst);
      if(vconst) {
        if(v->token() == NULLVAL) {
          values = predtrue;
          return fetchNumberNode(1);
        }
        else {
          values = predfalse;
          return fetchNumberNode(0);
        }
      }
      else
        if(!isNullPossible(v)) {
          values = predfalse;
          return fetchNumberNode(0);
        }
      values = predfalse | predtrue;
      return fetchTokenNode(ISNULL,v,nullptr);
    }
  case EXISTS :
    { if(isEmptySelect(n->child(0))) {
        values = predfalse;
        return fetchNumberNode(0);
      }
      values = predfalse | predtrue;
      return n;
    }
  case INSYM  :
    { bool v1const;
      SyntaxNode *v1 = reduceExpression(n->child(0),v1const);
      if(v1const && v1->token() == NULLVAL) { // result is undef
        values = predundef;
        return fetchNumberNode(-1);
      }
      SyntaxNode *setexpr = n->child(1);
      switch(setexpr->token()) {
      case SELECT       :
      case UNION        :
      case INTERSECT    :
      case SETDIFFERENCE:
        if(isEmptySelect(setexpr)) {
          values = predfalse;
          return fetchNumberNode(0);
        }
        values = predfalse | predtrue | predundef;
        return fetchTokenNode(INSYM,v1,setexpr,nullptr);
      default:
        { ReducedExprList exprlist(*this,setexpr);
          if(v1const) {
            bool setexprisconst      = true;
            bool setexprcontainsnull = false;
            for(UINT i = 0; i < exprlist.size(); i++) {
              ReducedExpr &rexpr = exprlist[i];
              if(!rexpr.m_isconst)
                setexprisconst = false;
              else
                if(rexpr.m_expr->token() == NULLVAL)
                  setexprcontainsnull = true;
                else                        // rexpr is const and defined
                  if(constExpressionCmp(v1,rexpr.m_expr) == 0) { // we have a match. result is true
                    values = predtrue;
                    return fetchNumberNode(1);
                  }
            }
            if(setexprisconst) {            // all expr in set-list are const but there is no match
              if(setexprcontainsnull) {
                values = predundef;
                return fetchNumberNode(-1); // result is undef
              }
              else {
                values = predfalse;
                return fetchNumberNode(0);  // result is false'
              }
            }
          }
          values = predfalse | predtrue | predundef;
          if(exprlist.size() == 1)
            return fetchTokenNode(EQUAL,v1,setexpr,nullptr);
          return fetchTokenNode(INSYM,v1,exprlist.getReducedExprList(),nullptr);
        }
      }
    }
    stopcomp(n);                            // should not come here
  case EQUAL  :
  case NOTEQ  :
    { ReducedExprList exprlist1(*this,n->child(0));
      if(n->child(1)->token() == SELECT) {     // cannot reduce predicates containing subqueries
        values = predfalse | predtrue | predundef;
        return fetchTokenNode(n->token(),exprlist1.getReducedExprList(),n->child(1),nullptr);
      } else {
        ReducedExprList exprlist2(*this,n->child(1));
        CompactIntArray nonconstset;
        bool containsnull = false;
        for(UINT i = 0; i < exprlist1.size(); i++) {
          ReducedExpr &rexpr1 = exprlist1[i];
          ReducedExpr &rexpr2 = exprlist2[i];
          if(rexpr1.isConstNull() || rexpr2.isConstNull()) {
            values = predundef;
            return fetchNumberNode(-1);
          }

          if(!rexpr1.m_isconst || !rexpr2.m_isconst)
            nonconstset.add(i);
          else { // both are const
            if(rexpr1.m_expr->token() == NULLVAL || rexpr2.m_expr->token() == NULLVAL) {
              if(!containsnull)
                nonconstset.add(i);    // add the first undefined subpredicate
              containsnull = true;
            }
            else {                          // both are const AND defined
              int cmp = constExpressionCmp(rexpr1.m_expr,rexpr2.m_expr);
              if(cmp != 0) {
                switch(n->token()) {
                case EQUAL  :
                  values = predfalse;
                  return fetchNumberNode(0);
                case NOTEQ  :
                  values = predtrue;
                  return fetchNumberNode(1);
                default     :
                  stopcomp(n);
                }
              }
            }
          }
        }
        if(nonconstset.size() == 0) {       // then all are const
          if(containsnull) {                // result is undef
            values = predundef;
            return fetchNumberNode(-1);
          }
          else                              // all are const AND defined
            switch(n->token()) {
              case EQUAL  :
                values = predtrue;
                return fetchNumberNode(1);
              case NOTEQ  :
                values = predfalse;
                return fetchNumberNode(0);
              default     :
                stopcomp(n);
            }
        }
        else if(nonconstset.size() == 1 && containsnull) { // there is only 1 subpredicate and it is the one containing null
          values = predundef;
          return fetchNumberNode(-1);       // result is undefined
        }

        // we have at least 1 non-const sub-predicate, but all const are equal
        values = predtrue | predfalse | predundef;
        return fetchTokenNode(n->token()
                             ,exprlist1.getReducedExprList(nonconstset)
                             ,exprlist2.getReducedExprList(nonconstset)
                             ,nullptr
                             );

      }
      stopcomp(n);
    }
  case RELOPLE:
  case RELOPLT:
  case RELOPGE:
  case RELOPGT:
    { ReducedExprList exprlist1(*this,n->child(0));
      if(n->child(1)->token() == SELECT) {     // cannot reduce predicates containing subqueries
        values = predtrue | predfalse | predundef;
        return fetchTokenNode(n->token(),exprlist1.getReducedExprList(),n->child(1),nullptr);
      }
      else {
        ReducedExprList exprlist2(*this,n->child(1));
        for(UINT i = 0; i < exprlist1.size(); i++) {
          ReducedExpr &rexpr1 = exprlist1[i];
          ReducedExpr &rexpr2 = exprlist2[i];
          if(rexpr1.isConstNull() || rexpr2.isConstNull()) {
            values = predundef;
            return fetchNumberNode(-1);
          }
          if(rexpr1.m_isconst && rexpr2.m_isconst) {
            // both are const and defined
            int cmp = constExpressionCmp(rexpr1.m_expr,rexpr2.m_expr);
            if(cmp != 0) {
              switch(n->token()) {
              case RELOPLE:
              case RELOPLT:
                if(cmp < 0) {
                  values = predtrue;
                  return fetchNumberNode(1);
                }
                else {
                  values = predfalse;
                  return fetchNumberNode(0);
                }
              case RELOPGE:
              case RELOPGT:
                if(cmp > 0) {
                  values = predtrue;
                  return fetchNumberNode(1);
                }
                else {
                  values = predfalse;
                  return fetchNumberNode(0);
                }
              default     :
                stopcomp(n);
              }
            }
          }
          else {
            values = predtrue | predfalse | predundef;
            return fetchTokenNode(n->token()
                                ,exprlist1.getReducedExprList(i)
                                ,exprlist2.getReducedExprList(i)
                                ,nullptr
                                );
          }
        }
        // all was const AND defined AND all compare equal
        switch(n->token()) {
          case RELOPLE:
          case RELOPGE:
            values = predtrue;
            return fetchNumberNode(1);
          default:
            values = predfalse;
            return fetchNumberNode(0);
        }
      }
      stopcomp(n);
    }
  case BETWEEN:
    { ReducedExprList exprlistmiddle(*this,n->child(0));
      ReducedExprList exprlistlower( *this,n->child(1));
      ReducedExprList exprlistupper( *this,n->child(2));
      for(UINT i = 0; i < exprlistmiddle.size(); i++) {
        ReducedExpr &rexprmiddle = exprlistmiddle[i];
        ReducedExpr &rexprlower  = exprlistlower[i];
        ReducedExpr &rexprupper  = exprlistupper[i];
        if(rexprmiddle.isConstNull()) {
          values = predundef;
          return fetchNumberNode(-1);
        }
        if(rexprlower.isConstNull() ) {
          values = predundef;
          return fetchNumberNode(-1);
        }
        if(rexprupper.isConstNull() ) {
          values = predundef;
          return fetchNumberNode(-1);
        }
        // all are defined
        if(rexprmiddle.m_isconst && rexprlower.m_isconst && rexprupper.m_isconst) { // middle, lower and upper
          if((constExpressionCmp(rexprlower.m_expr,rexprmiddle.m_expr) > 0)
           ||(constExpressionCmp(rexprmiddle.m_expr,rexprupper.m_expr) > 0)) {
            values = predfalse;
            return fetchNumberNode(0);
          }
          else
            continue; // don't bother about this anymore
        }
        else if(rexprmiddle.m_isconst && rexprlower.m_isconst) { // middle and lower
          if(constExpressionCmp(rexprlower.m_expr,rexprmiddle.m_expr) > 0) {
            values = predfalse;
            return fetchNumberNode(0);
          }
        }
        else if(rexprmiddle.m_isconst && rexprupper.m_isconst) { // middle and upper
          if(constExpressionCmp(rexprmiddle.m_expr,rexprupper.m_expr) > 0) {
            values = predfalse;
            return fetchNumberNode(0);
          }
        }
        else if(rexprlower.m_isconst && rexprupper.m_isconst) { // lower and upper
          if(constExpressionCmp(rexprlower.m_expr,rexprupper.m_expr) > 0) {
            values = predfalse;
            return fetchNumberNode(0);
          }
        }
        values = predtrue | predfalse | predundef;
        return fetchTokenNode(BETWEEN
                             ,exprlistmiddle.getReducedExprList(i)
                             ,exprlistlower.getReducedExprList(i)
                             ,exprlistupper.getReducedExprList(i)
                             ,nullptr
                             );
      }
      // all was const AND defined AND all compare equal
      values = predtrue;
      return fetchNumberNode(1);
    }
    break;
  default:
    stopcomp(n);
    break;
  }
  stopcomp(n);
  return nullptr;
}

// returns const predicates as a number SyntaxNode with
// false     as  0
// true      as  1
// undefined as -1
SyntaxNode *SqlCompiler::reducePredicate1(SyntaxNode *n, bool &isconst) {
  isconst = false;
  switch(n->token()) {
  case AND    :
    { bool v1const,v2const;
      SyntaxNode *v1 = reducePredicate1(n->child(0),v1const);
      SyntaxNode *v2 = reducePredicate1(n->child(1),v2const);
      if(v1const)
        switch((int)v1->number()) {
        case 0:        // result is false
          isconst = true;
          return v1;
        case 1:        // result is v2
          isconst = v2const;
          return v2;
        case -1:
          if(!v2const) // "undef and v2" equals false if v2=false and undef else
            return fetchTokenNode(AND,v1,v2,nullptr);
          switch((int)v2->number()) {
          case 0:      // result is false
            isconst = true;
            return v2;
          case 1:      // result is undef
            isconst = true;
            return v1;
          case -1:     // result is undef
            isconst = true;
            return v1;
          }
        }

                       // v1const = false
      if(v2const)
        switch((int)v2->number()) {
        case 0:        // result is false
          isconst = true;
          return v2;
        case 1:        // result is v1
          isconst = false;
          return v1;
        case -1:       // "v1 and undef" equals false if v1=false and undef else
          return fetchTokenNode(AND,v1,v2,nullptr);
        }
                       // both v1 and v2 are non-const
      return fetchTokenNode(AND,v1,v2,nullptr);
    }
  case OR     :
    { bool v1const,v2const;
      SyntaxNode *v1 = reducePredicate1(n->child(0),v1const);
      SyntaxNode *v2 = reducePredicate1(n->child(1),v2const);
//      _tprintf(_T("v1const:%s\nv1:"),boolToStr(v1const));
//      dumpSyntaxTree(v1);
      if(v1const)
        switch((int)v1->number()) {
        case 0:
          isconst = v2const;
          return v2;
        case 1:
          isconst = true;
          return v1;
        case -1:
          if(!v2const) // "undef or v2" equals true if v2=true and undef else
            return fetchTokenNode(OR,v1,v2,nullptr);
          switch((int)v2->number()) {
          case 0:      // result is undef
            isconst = true;
            return v1;
          case 1:      // result is true
            isconst = true;
            return v2;
          case -1:
            isconst = true;
            return v2;
          }
        }

                       // v1const = false
      if(v2const)
        switch((int)v2->number()) {
        case 0:
          return v1;
        case 1:
          isconst = true;
          return v2;
        case -1:       // "v1 or undef" equals true if v1=true and undef else
          return fetchTokenNode(OR,v1,v2,nullptr);
        }
                       // both v1 and v2 are non-const
      return fetchTokenNode(OR,v1,v2,nullptr);
    }
  case NOT    :
    { bool vconst;
      SyntaxNode *v = reducePredicate1(n->child(0),vconst);
      if(vconst) {
        isconst = true;
        switch((int)v->number()) {
        case 0:
          return fetchNumberNode(1);
        case 1:
          return fetchNumberNode(0);
        case -1:
          return v;
        }
      }
      switch(v->token()) {
      case NOT:
        return v->child(0);
      case NOTEQ:
        return fetchTokenNode(EQUAL,v->child(0)  ,v->child(1),nullptr);
      case RELOPLE:
        return fetchTokenNode(RELOPGT,v->child(0),v->child(1),nullptr);
      case RELOPLT:
        return fetchTokenNode(RELOPGE,v->child(0),v->child(1),nullptr);
      case RELOPGE:
        return fetchTokenNode(RELOPLT,v->child(0),v->child(1),nullptr);
      case RELOPGT:
        return fetchTokenNode(RELOPLE,v->child(0),v->child(1),nullptr);
      default:
        return fetchTokenNode(NOT,v,nullptr);
      }
    }
  case LIKE   :
    { bool v1const,v2const;
      SyntaxNode *v1 = reduceExpression(n->child(0), v1const);
      SyntaxNode *v2 = reduceExpression(n->child(1), v2const);
      if(v1const && v1->token() == NULLVAL) {
        isconst = true;
        return fetchNumberNode(-1);
      }
      if(v2const && v2->token() == NULLVAL) {
        isconst = true;
        return fetchNumberNode(-1);
      }

      if(v1const && v2const) {
        isconst = true;
        int result = false;
        try {
          result = isLike(v1->str(),v2->str()) ? 1 : 0;
        } catch(sqlca ca) {
          syntaxError(n,ca.sqlcode,_T("%s"),ca.sqlerrmc);
        }
        return fetchNumberNode(result);
      }
      else if(v2const)
         return reduceLike(*this,v1,v2->str());
      else
        return fetchTokenNode(LIKE,v1,v2,nullptr);
    }
  case ISNULL:
    { bool vconst;
      SyntaxNode *v = reduceExpression(n->child(0),vconst);
      if(vconst) {
        isconst = true;
        return fetchNumberNode((v->token() == NULLVAL) ? 1 : 0);
      }
      else
        if(!isNullPossible(v)) {
          isconst = true;
          return fetchNumberNode(0);
        }
      return fetchTokenNode(ISNULL,v,nullptr);
    }
  case EXISTS :
    { if(isEmptySelect(n->child(0))) {
        isconst = true;
        return fetchNumberNode(0);
      }
      return n;
    }
  case INSYM  :
    { bool v1const;
      SyntaxNode *v1 = reduceExpression(n->child(0),v1const);
      if(v1const && v1->token() == NULLVAL) { // result is undef
        isconst = true;
        return fetchNumberNode(-1);
      }
      SyntaxNode *setexpr = n->child(1);
      switch(setexpr->token()) {
      case SELECT:
        if(isEmptySelect(setexpr)) {
          isconst = true;
          return fetchNumberNode(0);
        }
        return fetchTokenNode(INSYM,v1,setexpr,nullptr);
      default:
        { ReducedExprList exprlist(*this,setexpr);
          if(v1const) {
            bool setexprisconst      = true;
            bool setexprcontainsnull = false;
            for(UINT i = 0; i < exprlist.size(); i++) {
              ReducedExpr &rexpr = exprlist[i];
              if(!rexpr.m_isconst)
                setexprisconst = false;
              else
                if(rexpr.m_expr->token() == NULLVAL)
                  setexprcontainsnull = true;
                else                        // rexpr is const and defined
                  if(constExpressionCmp(v1,rexpr.m_expr) == 0) { // we have a match. result is true
                    isconst = true;
                    return fetchNumberNode(1);
                  }
            }
            if(setexprisconst) {            // all expr in set-list are const but there is no match
              isconst = true;
              if(setexprcontainsnull)
                return fetchNumberNode(-1); // result is undef
              else
                return fetchNumberNode(0);  // result is false
            }
          }
          if(exprlist.size() == 1)
            return fetchTokenNode(EQUAL,v1,setexpr,nullptr);
          return fetchTokenNode(INSYM,v1,exprlist.getReducedExprList(),nullptr);
        }
      }
    }
    stopcomp(n);                            // should not come here
  case EQUAL  :
  case NOTEQ  :
    { ReducedExprList exprlist1(*this,n->child(0));
      if(n->child(1)->token() == SELECT)      // cannot reduce predicates containing subqueries
        return fetchTokenNode(n->token(),exprlist1.getReducedExprList(),n->child(1),nullptr);
      else {
        ReducedExprList exprlist2(*this,n->child(1));
        CompactIntArray nonconstset;
        bool containsnull = false;
        for(UINT i = 0; i < exprlist1.size(); i++) {
          ReducedExpr &rexpr1 = exprlist1[i];
          ReducedExpr &rexpr2 = exprlist2[i];
          if(rexpr1.isConstNull() || rexpr2.isConstNull()) {
            isconst = true;
            return fetchNumberNode(-1);
          }

          if(!rexpr1.m_isconst || !rexpr2.m_isconst)
            nonconstset.add(i);
          else { // both are const
            if(rexpr1.m_expr->token() == NULLVAL || rexpr2.m_expr->token() == NULLVAL) {
              if(!containsnull)
                nonconstset.add(i);    // add the first undefined subpredicate
              containsnull = true;
            }
            else {                          // both are const AND defined
              int cmp = constExpressionCmp(rexpr1.m_expr,rexpr2.m_expr);
              if(cmp != 0) {
                switch(n->token()) {
                case EQUAL  :
                  isconst = true;
                  return fetchNumberNode(0);
                case NOTEQ  :
                  isconst = true;
                  return fetchNumberNode(1);
                default     :
                  stopcomp(n);
                }
              }
            }
          }
        }
        if(nonconstset.size() == 0) {       // then all are const
          isconst = true;
          if(containsnull)                  // result is undef
            return fetchNumberNode(-1);
          else                              // all are const AND defined
            switch(n->token()) {
              case EQUAL  :
                return fetchNumberNode(1);
              case NOTEQ  :
                return fetchNumberNode(0);
              default     :
                stopcomp(n);
            }
        }
        else if(nonconstset.size() == 1 && containsnull) { // there is only 1 subpredicate and it is the one containing null
          isconst = true;
          return fetchNumberNode(-1);       // result is undefined
        }

        // we have at least 1 non-const sub-predicate, but all const are equal
        return fetchTokenNode(n->token()
                             ,exprlist1.getReducedExprList(nonconstset)
                             ,exprlist2.getReducedExprList(nonconstset)
                             ,nullptr
                             );

      }
      stopcomp(n);
    }
  case RELOPLE:
  case RELOPLT:
  case RELOPGE:
  case RELOPGT:
    { ReducedExprList exprlist1(*this,n->child(0));
      if(n->child(1)->token() == SELECT)      // cannot reduce predicates containing subqueries
        return fetchTokenNode(n->token(),exprlist1.getReducedExprList(),n->child(1),nullptr);
      else {
        ReducedExprList exprlist2(*this,n->child(1));
        for(UINT i = 0; i < exprlist1.size(); i++) {
          ReducedExpr &rexpr1 = exprlist1[i];
          ReducedExpr &rexpr2 = exprlist2[i];
          if(rexpr1.isConstNull() || rexpr2.isConstNull()) {
            isconst = true;
            return fetchNumberNode(-1);
          }
          if(rexpr1.m_isconst && rexpr2.m_isconst) {
            // both are const and defined
            int cmp = constExpressionCmp(rexpr1.m_expr,rexpr2.m_expr);
            if(cmp != 0) {
              isconst = true;
              switch(n->token()) {
              case RELOPLE:
              case RELOPLT:
                return fetchNumberNode((cmp < 0) ? 1 : 0);
              case RELOPGE:
              case RELOPGT:
                return fetchNumberNode((cmp > 0) ? 1 : 0);
              default     :
                stopcomp(n);
              }
            }
          }
          else
            return fetchTokenNode(n->token()
                                ,exprlist1.getReducedExprList(i)
                                ,exprlist2.getReducedExprList(i)
                                ,nullptr
                                );
        }
        // all was const AND defined AND all compare equal
        switch(n->token()) {
          case RELOPLE:
          case RELOPGE:
            isconst = true;
            return fetchNumberNode(1);
          default:
            isconst = true;
            return fetchNumberNode(0);
        }
      }
      stopcomp(n);
    }
  case BETWEEN:
    { ReducedExprList exprlistmiddle(*this,n->child(0));
      ReducedExprList exprlistlower( *this,n->child(1));
      ReducedExprList exprlistupper( *this,n->child(2));
      for(UINT i = 0; i < exprlistmiddle.size(); i++) {
        ReducedExpr &rexprmiddle = exprlistmiddle[i];
        ReducedExpr &rexprlower  = exprlistlower[i];
        ReducedExpr &rexprupper  = exprlistupper[i];
        if(rexprmiddle.isConstNull()) { isconst = true; return fetchNumberNode(-1); }
        if(rexprlower.isConstNull() ) { isconst = true; return fetchNumberNode(-1); }
        if(rexprupper.isConstNull() ) { isconst = true; return fetchNumberNode(-1); }
        // all are defined
        if(rexprmiddle.m_isconst && rexprlower.m_isconst && rexprupper.m_isconst) { // middle, lower and upper
          if((constExpressionCmp(rexprlower.m_expr,rexprmiddle.m_expr) > 0)
           ||(constExpressionCmp(rexprmiddle.m_expr,rexprupper.m_expr) > 0)) {
            isconst = true;
            return fetchNumberNode(0);
          }
          else
            continue; // don't bother about this anymore
        }
        else if(rexprmiddle.m_isconst && rexprlower.m_isconst) { // middle and lower
          if(constExpressionCmp(rexprlower.m_expr,rexprmiddle.m_expr) > 0) {
            isconst = true;
            return fetchNumberNode(0);
          }
        }
        else if(rexprmiddle.m_isconst && rexprupper.m_isconst) { // middle and upper
          if(constExpressionCmp(rexprmiddle.m_expr,rexprupper.m_expr) > 0) {
            isconst = true;
            return fetchNumberNode(0);
          }
        }
        else if(rexprlower.m_isconst && rexprupper.m_isconst) { // lower and upper
          if(constExpressionCmp(rexprlower.m_expr,rexprupper.m_expr) > 0) {
            isconst = true;
            return fetchNumberNode(0);
          }
        }
        return fetchTokenNode(BETWEEN
                             ,exprlistmiddle.getReducedExprList(i)
                             ,exprlistlower.getReducedExprList(i)
                             ,exprlistupper.getReducedExprList(i)
                             ,nullptr
                             );
      }
      // all was const AND defined AND all compare equal
      isconst = true;
      return fetchNumberNode(1);
    }
    break;
  default:
    stopcomp(n);
    break;
  }
  stopcomp(n);
  return nullptr;
}
/*
const SyntaxNode *SqlCompiler::reducePredicate( const SyntaxNode *n) {
  PossiblePredicateValues tmp;
  if(n)
    return reducePredicate(n,tmp);
  return n;
}
*/
SyntaxNode *SqlCompiler::reduceByKeyPredicates(SyntaxNode *n, KeyPredicatesHashMap &hash) {
  switch(n->token()) {
  case AND    :
    { SyntaxNode *v1 = reduceByKeyPredicates(n->child(0),hash);
      SyntaxNode *v2 = reduceByKeyPredicates(n->child(1),hash);
      if(v1 == nullptr) return v2;
      if(v2 == nullptr) return v1;
      return fetchTokenNode(AND,v1,v2,nullptr);
    }
  case OR     :
  case NOT    :
  case EXISTS :
  case INSYM  :
  case NOTEQ  :
  case LIKE   :
    return n;
  case ISNULL:
    return (hash.get(n->child(0)) == nullptr) ? n : nullptr;
  case EQUAL  :
  case RELOPLE:
  case RELOPLT:
  case RELOPGE:
  case RELOPGT:
    { NodeList left(n->child(0));
      NodeList right(n->child(1));
      if((left.size() > 1) && (n->token() != EQUAL))
        return n;

      CompactIntArray nonkeypred;
      for(UINT i = 0; i < left.size(); i++) { // sizes are equal
        if((hash.get(left[i]) == nullptr) && (hash.get(right[i]) == nullptr))
          nonkeypred.add(i);
      }
      if(nonkeypred.size() == 0)
        return nullptr;

      if(nonkeypred.size() == left.size())
        return n;

      return fetchTokenNode(n->token(),left.genlist(*this,nonkeypred),right.genlist(*this,nonkeypred),nullptr);
    }
  case BETWEEN:
    { NodeList middle(n->child(0));
      if(middle.size() > 1) return n;

      NodeList lower( n->child(1));
      NodeList upper( n->child(2));

      if((hash.get(lower[0]) != nullptr) && (hash.get(upper[0]) != nullptr))
        return nullptr;
      else
        return n;
    }
  case NUMBER :
    return n;
  default     :
    stopcomp(n);
    break;
  }
  return nullptr;
}
