#include "stdafx.h"

void SqlCompiler::genInsertSelect(SyntaxNode *n) { // n = insert_stmt
  SyntaxNode *insertwhat = n->child(2);
  _tprintf(_T("select-statement:\n"));
  dumpSyntaxTree(insertwhat);
}

InsertColumnExpression::InsertColumnExpression(StatementTable &ft, unsigned short colindex, SyntaxNode *expr) :
  StatementSymbolInfo(ft,colindex,expr)
{
  m_expr = expr;
}

void InsertColumnExpression::dump(FILE *f) const {
  _ftprintf(f,_T("colindex:%d, colname:%s\n"),m_colIndex, getColumn().m_name.cstr());
//  dumpSyntaxTree(m_expr,f);
}

DbMainType SqlCompiler::checkValueExpressionMainType(const SyntaxNode *expr) {
  DbMainType m1,m2,m3;
  int i;
  switch(expr->token()) {
  case MULT     :
  case DIVOP    :
  case MODOP    :
  case EXPO     :
    m1 = checkValueExpressionMainType(expr->child(0));
    m2 = checkValueExpressionMainType(expr->child(1));
    if(m1 != MAINTYPE_NUMERIC && m1 != MAINTYPE_VOID)
      syntaxError(expr->child(0),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
    if(m2 != MAINTYPE_NUMERIC && m2 != MAINTYPE_VOID)
      syntaxError(expr->child(1),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
    return MAINTYPE_NUMERIC;

  case PLUS     :
    switch(checkValueExpressionMainType(expr->child(0))) {
    case MAINTYPE_NUMERIC:
      m2 = checkValueExpressionMainType(expr->child(1));
      if(m2 != MAINTYPE_NUMERIC && m2 != MAINTYPE_VOID)
        syntaxError(expr->child(1),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
      return MAINTYPE_NUMERIC;

    case MAINTYPE_STRING :
    case MAINTYPE_VARCHAR:
      syntaxError(expr->child(0),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
      return MAINTYPE_NUMERIC;

    case MAINTYPE_DATE   :
      switch(checkValueExpressionMainType(expr->child(1))) {
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

    case MAINTYPE_VOID:
      m2 = checkValueExpressionMainType(expr->child(1));
      if(m2 != MAINTYPE_NUMERIC && m2 != MAINTYPE_VOID)
        syntaxError(expr->child(0),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
      return MAINTYPE_VOID;
    }
    break;
  case MINUS    :
    if(expr->childCount() < 2) {
      switch(checkValueExpressionMainType(expr->child(0))) {
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
      switch(checkValueExpressionMainType(expr->child(0))) {
      case MAINTYPE_NUMERIC:
        if(checkValueExpressionMainType(expr->child(1)) != MAINTYPE_NUMERIC)
          syntaxError(expr->child(1),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
        return MAINTYPE_NUMERIC;

      case MAINTYPE_STRING :
      case MAINTYPE_VARCHAR:
        syntaxError(expr->child(0),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
        return MAINTYPE_NUMERIC;

      case MAINTYPE_DATE   :
        switch(checkValueExpressionMainType(expr->child(1))) {
        case MAINTYPE_NUMERIC:
          return MAINTYPE_DATE;

        case MAINTYPE_STRING :
        case MAINTYPE_VARCHAR:
          syntaxError(expr->child(1),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
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

  case SUBSTRING:
    m1 = checkValueExpressionMainType(expr->child(0));
    m2 = checkValueExpressionMainType(expr->child(1));
    m3 = checkValueExpressionMainType(expr->child(2));
    if(!isCompatibleType(m1,MAINTYPE_STRING))
      syntaxError(expr->child(0),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
    if(!isCompatibleType(m2,MAINTYPE_NUMERIC))
      syntaxError(expr->child(1),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
    if(!isCompatibleType(m3,MAINTYPE_NUMERIC))
      syntaxError(expr->child(2),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
    return MAINTYPE_STRING;

  case CONCAT:
    m1 = checkValueExpressionMainType(expr->child(0));
    m2 = checkValueExpressionMainType(expr->child(1));
    if(!isCompatibleType(m1,MAINTYPE_STRING))
      syntaxError(expr->child(0),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
    if(!isCompatibleType(m2,MAINTYPE_STRING))
      syntaxError(expr->child(1),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
    return MAINTYPE_STRING;

  case TYPEDATE:
    for(i = 0; i < 3; i++) {
      m1 = checkValueExpressionMainType(expr->child(i));
      if(!isCompatibleType(m1,MAINTYPE_NUMERIC))
        syntaxError(expr->child(i),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
    }
    return MAINTYPE_DATE;

  case TYPETIME:
    for(i = 0; i < 3; i++) {
      m1 = checkValueExpressionMainType(expr->child(i));
      if(!isCompatibleType(m1,MAINTYPE_NUMERIC))
        syntaxError(expr->child(i),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
    }
    return MAINTYPE_TIME;

  case TYPETIMESTAMP:
    for(i = 0; i < 6; i++) {
      m1 = checkValueExpressionMainType(expr->child(i));
      if(!isCompatibleType(m1,MAINTYPE_NUMERIC))
        syntaxError(expr->child(i),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
    }
    return MAINTYPE_TIMESTAMP;

  case COUNT    :
    if(expr->child(0)->token() == STAR)
      return MAINTYPE_NUMERIC;
    else
      checkValueExpressionMainType(expr->child(0)->child(0));
    return MAINTYPE_NUMERIC;

  case MIN      :
  case MAX      :
    return checkValueExpressionMainType(expr->child(2));

  case SUM      :
    m1 = checkValueExpressionMainType(expr->child(2));
    if(!isCompatibleType(m1,MAINTYPE_NUMERIC))
      syntaxError(expr->child(1),SQL_INVALID_EXPR_TYPE,_T("Illegal type of operand"));
    return MAINTYPE_NUMERIC;

  case NAME            :
  case DOT             :
    return MAINTYPE_NUMERIC;

  case NUMBER          :
    return MAINTYPE_NUMERIC;

  case STRING          :
    return MAINTYPE_STRING;

  case DATECONST       :
    return MAINTYPE_DATE;

  case TIMECONST       :
    return MAINTYPE_TIME;

  case TIMESTAMPCONST  :
    return MAINTYPE_TIMESTAMP;

  case HOSTVAR         :
    return getMainType(m_bndstmt.m_inHost[expr->getHostVarIndex()].getType());

  case NULLVAL         :
    return MAINTYPE_VOID;

  case CURRENTDATE     :
    return MAINTYPE_DATE;

  case CURRENTTIME     :
    return MAINTYPE_TIME;

  case CURRENTTIMESTAMP:
    return MAINTYPE_TIMESTAMP;

  case CAST            :
    { checkValueExpressionMainType(expr->child(0));
      CastParameter p = genCastParameter(expr->child(1));
      return getMainType(p.getType());
    }
  default       :
    stopcomp(expr);
  }
  return MAINTYPE_VOID;
}

void SqlCompiler::checkExpressionType(InsertColumnExpression &col) {
  const SyntaxNode *p = findFirstTableReference(col.m_expr);
  if(p != NULL)
    syntaxError(p,SQL_INVALID_EXPR_TYPE,_T("Columnnames not allowed in valuelist"));

  p = findFirstAggregateFunction(col.m_expr);
  if(p != NULL)
    syntaxError(p,SQL_INVALID_EXPR_TYPE,_T("Aggregation not allowed in valuelist"));

  DbMainType exprtype = checkValueExpressionMainType(col.m_expr);
  DbMainType coltype  = getMainType(col.getType());
  if(!isCompatibleType(exprtype,coltype)) {
    switch(coltype) {
    case MAINTYPE_NUMERIC  :
      syntaxError(col.m_expr,SQL_INVALID_EXPR_TYPE,_T("Illegal type of expression. Must be numeric"));
      break;
    case MAINTYPE_STRING   :
    case MAINTYPE_VARCHAR  :
      syntaxError(col.m_expr,SQL_INVALID_EXPR_TYPE,_T("Illegal type of expression. Must be String"));
      break;
    case MAINTYPE_DATE     :
      syntaxError(col.m_expr,SQL_INVALID_EXPR_TYPE,_T("Illegal type of expression. Must be Date"));
      break;
    case MAINTYPE_TIME     :
      syntaxError(col.m_expr,SQL_INVALID_EXPR_TYPE,_T("Illegal type of expression. Must be time"));
      break;
    case MAINTYPE_TIMESTAMP:
      syntaxError(col.m_expr,SQL_INVALID_EXPR_TYPE,_T("Illegal type of expression. Must be Timestamp"));
      break;
    default                :
      stopcomp(col.m_expr);
    }
  }
}

SyntaxNode *SqlCompiler::fetchDefaultValueNode(const ColumnDefinition &col) {
  if(col.m_defaultValue.length() == 0) { // has no default-value
    if(col.m_nullAllowed)
      return fetchTokenNode(NULLVAL,NULL);
    else
      return NULL; // no default, nulls not allowed. i.e. syntaxError !
  }
  else { // defaultvalue specified. now find the type an return a syntaxnode of the right type
    switch(getMainType(col.getType())) {
    case MAINTYPE_NUMERIC  :
      return fetchNumberNode(_ttof(col.m_defaultValue.cstr()));
    case MAINTYPE_STRING   :
    case MAINTYPE_VARCHAR  :
      return fetchStringNode(col.m_defaultValue.cstr());
    case MAINTYPE_DATE     :
      return fetchDateNode(Date(col.m_defaultValue.cstr()));
    case MAINTYPE_TIME     :
      return fetchTimeNode(Time(col.m_defaultValue.cstr()));
    case MAINTYPE_TIMESTAMP:
      return fetchTimestampNode(Timestamp(col.m_defaultValue.cstr()));
    default               :
      throwSqlError(SQL_FATAL_ERROR,_T("Unexpected datatype in fetchDefaultValueNode:%d col:<%s>\n"),col.getType(),col.m_name.cstr());
      return NULL; //  to make compiler happy
    }
  }
}

void SqlCompiler::genInsertValues(const SyntaxNode *n,              // n == insert_stmt
                                  StatementTable &table) {
  NodeList    columnlist((n->child(1)->childCount() == 0) ? NULL : n->child(1)->child(0));
  SyntaxNode *insertwhat = n->child(2); // == VALUES
  NodeList    exprlist(insertwhat->child(0));
  Array<InsertColumnExpression> icemap;

  if(columnlist.size() == 0) { // no columnnames specified. use all columns of tabledefinition
    if(exprlist.size() != table.getColumnCount()) {
      syntaxError(insertwhat,SQL_INVALID_EXPRLIST,_T("Invalid number of expressions specified"));
      return;
    }
    for(UINT i = 0; i < table.getColumnCount(); i++)
      icemap.add(InsertColumnExpression(table,i,exprlist[i]));
  }
  else { // columnnames specified
    if(columnlist.size() != exprlist.size()) {
      syntaxError(insertwhat,SQL_INVALID_EXPRLIST,_T("Invalid number of expressions specified"));
      return;
    }
    for(UINT i = 0; i < columnlist.size(); i++) {
      const TCHAR *name = columnlist[i]->name();
      int colindex = table.findColumnIndex(name);
      if(colindex < 0)
        syntaxError(columnlist[i],SQL_INVALID_COLUMNNAME,_T("Invalid columnname:<%s>"),name);
      else {
        bool found = false;
        for(UINT j = 0; j < icemap.size(); j++) { // check not already spec
          if(icemap[j].m_colIndex == colindex) {
            syntaxError(columnlist[i],SQL_COLUMN_ALREADY_DEFINED,_T("Column <%s> already specified"),name);
            found = true;
            break;
          }
        }
        if(!found)
          icemap.add(InsertColumnExpression(table,colindex,exprlist[i]));
      }
    }
    if(columnlist.size() > table.getColumnCount())
      syntaxError(insertwhat,SQL_TOO_MANY_COLUMNS,_T("Too many columns specified"));
    else {
      if(columnlist.size() < table.getColumnCount()) // check, that unspecified columns have nulls-allowed or defaultvalue
        for(UINT i = 0; i < table.getColumnCount(); i++) {
          bool found = false;
          for(UINT j = 0; j < icemap.size(); j++) {
            if(icemap[j].m_colIndex == i) {
              found = true;
              break;
            }
          }
          if(!found) { // unspecified column
            SyntaxNode *defaultvalue = fetchDefaultValueNode(table.getColumn(i));
            if(defaultvalue == NULL)
              syntaxError(n->child(0),SQL_NODEFAULT_OR_NULLALLOWED,_T("Column <%s> has no default-value or null-allowed"),table.getColumn(i).m_name.cstr());
            else
              icemap.add(InsertColumnExpression(table,i,defaultvalue));
          }
        }
    }
  }
  if(ok()) {
    // icemap.size == tableDef.colcount. i.e all columns has an expression
    int hostvarcounter = 0;
    for(UINT i = 0; i < icemap.size(); i++) {
      findHostVarIndex(icemap[i].m_expr,hostvarcounter);
    }
  }
  if(ok()) {
    for(UINT i = 0; i < icemap.size(); i++)
      checkExpressionType(icemap[i]);
  }

  if(ok()) {
    m_code.appendIns2(CODETUPINIT,0,table.getColumnCount());
    for(UINT i = 0; i < icemap.size(); i++) {
      bool dummy;
      genExpression(reduceExpression(icemap[i].m_expr,dummy));
      m_code.appendIns2(CODEPOPTUP,0,icemap[i].m_colIndex);
    }
    m_code.appendIns0(CODETRBEGIN);
    m_code.appendIns1(CODEPUSHCONST,m_code.appendConst(table.getSequenceNo()));
    m_code.appendIns1(CODETUPINSERT,0);
    m_code.appendIns0(CODETRCOMMIT);
  }
#if defined(TRACECOMP)
  m_code.dump();
#endif

//  if(ok()) {
//    FILE *dmp = FOPEN(_T("fisk"),_T("w"));
//    for(int i = 0; i < icemap.size(); i++)
//      icemap[i].dump(dmp);
//    m_code.dump(dmp);
//    fclose(dmp);
//  }
}

void SqlCompiler::genInsert(SyntaxNode *n) { // n == <insert_stmt>
  const TCHAR *tableName = n->child(0)->name();

  StatementTable table(*this,n->child(0));

  if(!ok()) return;
  if(table.getTableType() != TABLETYPE_USER) {
    syntaxError(n->child(0),SQL_INVALID_TABLETYPE,_T("Cannot insert into systemtable <%s>"),tableName);
    return;
  }
  SyntaxNode *insertwhat = n->child(2);
  switch(insertwhat->token()) {
  case VALUES:
    genInsertValues(n,table);
    break;
  case SELECT:
    genInsertSelect(n);
    break;
  default:
    stopcomp(insertwhat);
  }
  m_code.appendIns0(CODERETURN);
}

