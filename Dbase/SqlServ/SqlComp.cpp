#include "stdafx.h"

void xstopcomp(const SyntaxNode *n, TCHAR *file, int line) {
  _tprintf(_T("----------------------------\n"));
#if defined(TRACECOMP)
  dumpSyntaxTree(n);
#endif
  throwSqlError(SQL_FATAL_ERROR,_T("comp stop in %s:%d"),file,line);
}

/* ---------------------------------------------------- */

void SqlCompiler::vSyntaxError(const SyntaxNode *n, long sqlcode, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  SourcePosition pos = (n->token() == DOT) ? n->child(1)->pos() : n->pos();
  vAppendError(pos,sqlcode,format,argptr);
}

void SqlCompiler::syntaxError(const SyntaxNode *n, long sqlcode, _In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr,format);
  vSyntaxError(n,sqlcode,format,argptr);
  va_end(argptr);
}

void SqlCompiler::findHostVarIndex(SyntaxNode *expr, int &hostvarcounter, int flags ) {
  if(expr == NULL) return;
  switch(expr->token()) {
  case HOSTVAR:
    if(!(flags & HOSTVARSCANNERFLAG_ALLBUTINTO) && (flags & HOSTVARSCANNERFLAG_INTO)) break;
    if(hostvarcounter >= (int)(m_bndstmt.m_stmtHead.m_ninput + m_bndstmt.m_stmtHead.m_noutput)) {
      if(m_bndstmt.m_stmtHead.m_ninput + m_bndstmt.m_stmtHead.m_noutput == 0)
        syntaxError(expr,SQL_NO_HOSTVAR_DEFINED,_T("Hostvar not allowed here"));
      else
        syntaxError(expr,SQL_TOO_MANY_HOSTVAR ,_T("Too many hostvars in expressionlist. inconsistency in bnd-file"));
      return;
    }
    expr->setHostVarIndex(hostvarcounter++);
    break;

  case STRING        :
  case NUMBER        :
  case DATECONST	 :
  case TIMECONST	 :
  case TIMESTAMPCONST:
  case PARAM         :
  case NAME          :
    return;

  case INTO   :
    if((flags & HOSTVARSCANNERFLAG_INTO) && expr->childCount() >= 1)
      findHostVarIndex(expr->child(0), hostvarcounter, HOSTVARSCANNERFLAG_ALL);
    break;

  default     :
    { int sons = expr->childCount();
      for(int i = 0; i < sons;i++)
        findHostVarIndex(expr->child(i), hostvarcounter, flags);
      break;
    }
  }
}

StatementSymbolInfo *SqlCompiler::getInfo(const SyntaxNode *n) const {
  if(n == NULL) throwSqlError(SQL_FATAL_ERROR,_T("getInfo:SyntaxNode is NULL"));
  if(n->token() == NAME)
    return (StatementSymbolInfo*)(n->getData());
  else
    if(n->token() == DOT)
      return (StatementSymbolInfo*)(n->child(1)->getData());
  stopcomp(n);
  return NULL; // just to make compiler happy
}

void SqlCompiler::genUpdate(SyntaxNode *n) {
}

void SqlCompiler::genDelete(SyntaxNode *n) {
}


void SqlCompiler::genDrop(SyntaxNode *n) {
  switch(n->child(0)->token()) {
  case TABLE:
    m_code.appendIns0(CODETRBEGIN);
    m_code.appendIns1(CODEPUSHCONST,m_code.appendConst(n->child(1)->name()));
    m_code.appendIns0(CODEDRTAB);
    m_code.appendIns0(CODETRCOMMIT);
    m_code.appendIns0(CODERETURN);
    break;
  case INDEX:
    m_code.appendIns0(CODETRBEGIN);
    m_code.appendIns1(CODEPUSHCONST,m_code.appendConst(n->child(1)->name()));
    m_code.appendIns0(CODEDRINX);
    m_code.appendIns0(CODETRCOMMIT);
    m_code.appendIns0(CODERETURN);
    break;
  default:
    stopcomp(n);
  }
}

void SqlCompiler::genExplain(SyntaxNode *n) {
}

void SqlCompiler::genFetch(SyntaxNode *n) {
  try {
    VirtualCode vc;
    sqlLoadCode(m_db, m_programid, m_bndstmt.m_stmtHead.m_nr, vc);
    HostVarDescriptionList cursordesc = vc.getDescription();
    m_code.splitCodeAndData(vc);
    int hostvarcounter = 0;
    findHostVarIndex(n,hostvarcounter);
    HostVarDescriptionList fetchdesc(m_bndstmt,false,true);
    NodeList hostvarlist(n->child(1));
    if(cursordesc.size() != fetchdesc.size())
      syntaxError(n->child(0), SQL_INVALID_NO_OF_COLUMNS,_T("Invalid number of columns specified in fetch-statement"));
    else
      if(hostvarlist.size() != fetchdesc.size())
        syntaxError(n->child(0),SQL_INVALID_NO_OF_COLUMNS,_T("Number of columns in statement does not match number of columns in bnd-file"));
    if(ok()) {
      for(UINT i = 0; i < fetchdesc.size(); i++) {
        HostVarDescription &d1 = cursordesc[i];
        HostVarDescription &d2 = fetchdesc[i];
        if(!isCompatibleType(d1.getType(),d2.getType()))
          syntaxError(hostvarlist[i],SQL_INVALID_DATATYPE,_T("Invalid datatype"));
      }
    }
    if(ok())
      m_code.appendDesc(fetchdesc);
  } catch(sqlca) {
    syntaxError(n->child(0),SQL_UNDECLARED_CURSOR,_T("Undeclared cursor"));
  }
}

void SqlCompiler::fixJumpAdressList(const JumpCodeAdressList &jumpcodes, UINT jumpaddress) {
  for(UINT i = 0; i < jumpcodes.size(); i++)
    m_code.fixins1(jumpcodes[i],jumpaddress);
}

static bool evaluationnode(SyntaxNode *expr) {
  switch(expr->token()) {
  case PLUS   :
  case MINUS  :
  case MULT   :
  case DIVOP  :
  case MODOP  :
  case EXPO   :
    return true;
  default     :
    return false;
  }
}

CastParameter SqlCompiler::genCastParameter(const SyntaxNode *n) {
  DbFieldType dbtype;
  int len = 1;
  double strl;
  switch(n->token()) {
  case TYPECHAR	     :
    if(n->childCount() == 2) {
      dbtype = DBTYPE_CSTRING;
      strl = n->child(1)->number();
      if(strl <= 0 || floor(strl) != strl) {
        syntaxError(n->child(1),SQL_SYNTAX_ERROR,_T("Invalid String length"));
      }
      len = (ULONG)strl;
    } else if(n->child(0)->token() == SIGNED) {
      dbtype = DBTYPE_CHAR;
    } else {
      dbtype = DBTYPE_UCHAR;
    }
    break;

  case TYPEWCHAR	     :
    if(n->childCount() == 2) {
      dbtype = DBTYPE_WSTRING;
      strl = n->child(1)->number();
      if(strl <= 0 || floor(strl) != strl) {
        syntaxError(n->child(1),SQL_SYNTAX_ERROR,_T("Invalid String length"));
      }
      len = (ULONG)strl;
    } else if(n->child(0)->token() == SIGNED) {
      dbtype = DBTYPE_SHORT;
    } else {
      dbtype = DBTYPE_USHORT;
    }
    break;

  case TYPESHORT	 :
    if(n->child(0)->token() == SIGNED)
      dbtype = DBTYPE_SHORT;
    else
      dbtype = DBTYPE_USHORT;
    break;

  case TYPEINT 	     :
    if(n->child(0)->token() == SIGNED)
      dbtype = DBTYPE_INT;
    else
      dbtype = DBTYPE_UINT;
    break;

  case TYPELONG	     :
    if(n->child(0)->token() == SIGNED)
      dbtype = DBTYPE_LONG;
    else
      dbtype = DBTYPE_ULONG;
    break;

  case TYPEFLOAT	 :
    dbtype = DBTYPE_FLOAT;
    break;

  case TYPEDOUBLE	 :
    dbtype = DBTYPE_DOUBLE;
    break;

  case TYPEDATE   	 :
    dbtype = DBTYPE_DATE;
    break;

  case TYPETIME   	 :
    dbtype = DBTYPE_TIME;
    break;

  case TYPETIMESTAMP :
    dbtype = DBTYPE_TIMESTAMP;
    break;

  case TYPEVARCHAR       :
    dbtype = DBTYPE_VARCHAR;
    strl = n->child(0)->number();
    if(strl <= 0 || floor(strl) != strl)
      syntaxError(n->child(0),SQL_SYNTAX_ERROR,_T("Invalid varchar length"));
    if(strl > MAXVARCHARLEN)
      syntaxError(n->child(0),SQL_SYNTAX_ERROR,_T("Invalid varchar length. max = %lu"),MAXVARCHARLEN);
    len = (ULONG)strl;
    break;

  default:
    stopcomp(n);
    break;
  }

  return CastParameter(dbtype,len);
}

void SqlCompiler::genTypeCast(const SyntaxNode *n) {
  CastParameter param = genCastParameter(n);
  Packer p;
  p << param;
  m_code.appendIns1(CODECAST,m_code.appendData(p));
}

void SqlCompiler::genExpression(const SyntaxNode *expr) {
  switch(expr->token()) {
  case HOSTVAR       :
    { //unsigned char hosttype = (unsigned char)m_bndstmt.inhost[expr->getHostVarIndex()].type;
      m_code.appendIns1(CODEPUSHHV,expr->getHostVarIndex());
      break;
    }
  case STRING        :
    m_code.appendIns1(CODEPUSHCONST,m_code.appendConst(expr->str()));
    break;
  case NUMBER        :
    m_code.appendIns1(CODEPUSHCONST,m_code.appendConst(expr->number()));
    break;
  case DATECONST     :
    m_code.appendIns1(CODEPUSHCONST,m_code.appendConst(expr->getDate()));
    break;
  case TIMECONST     :
    m_code.appendIns1(CODEPUSHCONST,m_code.appendConst(expr->getTime()));
    break;
  case TIMESTAMPCONST:
    m_code.appendIns1(CODEPUSHCONST,m_code.appendConst(expr->getTimestamp()));
    break;
  case CONCAT        :
    genExpression( expr->child(0));
    genExpression( expr->child(1));
    m_code.appendIns0(CODECONCAT);
    break;
  case SUBSTRING     :
    { for(int i = 0; i < 3; i++)
        genExpression( expr->child(i));
      m_code.appendIns0(CODESUBSTR);
    }
    break;
  case TYPEDATE      :
    { for(int i = 0; i < 3; i++)
        genExpression( expr->child(i));
      m_code.appendIns0(CODEDATE);
    }
    break;
  case TYPETIME      :
    { for(int i = 0; i < 3; i++)
        genExpression( expr->child(i));
      m_code.appendIns0(CODETIME);
    }
    break;
  case TYPETIMESTAMP :
    { for(int i = 0; i < 6; i++)
        genExpression( expr->child(i));
      m_code.appendIns0(CODETIMESTAMP);
    }
    break;
  case PARAM         :
    syntaxError(expr,SQL_INVALID_EXPR_TYPE,_T("Columnnames not allowed in valuelist"));
    return;
  case NAME          :
  case DOT           :
    { StatementSymbolInfo *ssi = getInfo(expr);
      m_code.appendIns2(CODEPUSHTUP,ssi->m_currentTupleReg,ssi->m_currentTupleIndex);
      return;
    }
  case NULLVAL       :
    m_code.appendIns0(CODEPUSHNULL);
    break;

  case CURRENTDATE   :
    m_code.appendIns0(CODEPUSHCURRENTDATE);
    break;

  case CURRENTTIME   :
    m_code.appendIns0(CODEPUSHCURRENTTIME);
    break;

  case CURRENTTIMESTAMP:
    m_code.appendIns0(CODEPUSHCURRENTTIMESTAMP);
    break;

  case PLUS          :
    if(!evaluationnode(expr->child(0)) && evaluationnode(expr->child(1))) { // evaluate son(1) first
      genExpression(expr->child(1));
      genExpression(expr->child(0));
      m_code.appendIns0(CODEADD);
    }
    else {
      genExpression(expr->child(0));
      genExpression(expr->child(1));
      m_code.appendIns0(CODEADD);
    }
    break;
  case MULT          :
    if(!evaluationnode(expr->child(0)) && evaluationnode(expr->child(1))) { // evaluate son(1) first
      genExpression(expr->child(1));
      genExpression(expr->child(0));
      m_code.appendIns0(CODEMULT);
    }
    else {
      genExpression(expr->child(0));
      genExpression(expr->child(1));
      m_code.appendIns0(CODEMULT);
    }
    break;
  case MINUS         :
    if(expr->childCount() < 2) { // unary -
      genExpression(expr->child(0));
      m_code.appendIns0(CODENEG);
      break;
    }
    else { // binary -
      genExpression(expr->child(0));
      genExpression(expr->child(1));
      m_code.appendIns0(CODESUB);
    }
    break;
  case DIVOP         :
    genExpression(expr->child(0));
    genExpression(expr->child(1));
    m_code.appendIns0(CODEDIV);
    break;
  case MODOP         :
    genExpression(expr->child(0));
    genExpression(expr->child(1));
    m_code.appendIns0(CODEMOD);
    break;
  case EXPO          :
    genExpression(expr->child(0));
    genExpression(expr->child(1));
    m_code.appendIns0(CODEEXPO);
    break;
  case CAST          :
    genExpression(expr->child(0));
    genTypeCast(expr->child(1));
    break;
  case UNION         :
  case INTERSECT     :
  case SETDIFFERENCE :
  case SELECT        :
  default            :
    stopcomp(expr);
  }
}

static bool isundefinedpredicate(const SyntaxNode *predicate) {
  return (predicate->token() == NUMBER) && (predicate->number() == -1);
}

void SqlCompiler::genPredicate( const SyntaxNode *predicate) {
  switch(predicate->token()) {
  case NUMBER :
    switch((int)predicate->number()) {
    case 0:
    case 1:
      m_code.appendIns1(CODEPUSHCONST,m_code.appendConst(predicate->number()));
      break;
    case -1:
      m_code.appendIns0(CODEPUSHNULL);
      break;
    default:
      throwSqlError(SQL_FATAL_ERROR,_T("const predicate must be 1,0 or -1"));
    }
    break;
  case AND    :
    genPredicate(predicate->child(0));
    genPredicate(predicate->child(1));
    m_code.appendIns0(CODEAND);
    break;
  case OR     :
    genPredicate(predicate->child(0));
    genPredicate(predicate->child(1));
    m_code.appendIns0(CODEOR);
    break;
  case NOT    :
    genPredicate(predicate->child(0));
    m_code.appendIns0(CODENOT);
    break;
  case EXISTS :
  case INSYM  :
    throwSqlError(SQL_FATAL_ERROR,_T("Predicate exists,insym not implemented"));
    break;
  case LIKE   :
    genExpression(predicate->child(0));
    genExpression(predicate->child(1));
    m_code.appendIns1(CODEISLIKE,predicate->getLikeOperatorIndex());
    break;
  case ISNULL :
    genExpression(predicate->child(0));
    m_code.appendIns0(CODEISNULL);
    break;
  case EQUAL  : // cannot use JMP-instructions here because predicate-values can be undefined
    { NodeList left( predicate->child(0));
      NodeList right(predicate->child(1));
      for(UINT i = 0; i < left.size(); i++) { // sizes are equal
        genExpression(left[i] );
        genExpression(right[i]);
        m_code.appendIns0(CODEEQ);
        if(i > 0)
          m_code.appendIns0(CODEAND);
      }
      break;
    }
  case NOTEQ  : // cannot use JMP-instructions here because predicate-values can be undefined
    { NodeList left( predicate->child(0));
      NodeList right(predicate->child(1));
      for(UINT i = 0; i < left.size(); i++) { // sizes are equal
        genExpression(left[i] );
        genExpression(right[i]);
        m_code.appendIns0(CODENQ);
        if(i > 0)
          m_code.appendIns0(CODEOR);
      }
      break;
    }
  case RELOPGE:
    { NodeList left( predicate->child(0));
      NodeList right(predicate->child(1));
      if(left.size() == 1) {
        genExpression(left[0] );
        genExpression(right[0]);
        m_code.appendIns0(CODEGE);
      }
      else {
        JumpCodeTable      jmp;
        JumpCodeAdressList jmpexitlist;
        for(UINT i = 0; i < left.size(); i++) { // sizes are equal
          genExpression(left[i] );
          genExpression(right[i]);
          jmp.m_undefPart.add(m_code.appendIns1(CODECMP  ,0));
          jmp.m_truePart.add( m_code.appendIns1(CODEJMPGT,0));
          jmp.m_falsePart.add(m_code.appendIns1(CODEJMPLT,0));
        }
        fixJumpAdressList(jmp.m_truePart,m_code.appendIns1(CODEPUSHCONST,m_code.appendConst(1)));
        jmpexitlist.add(m_code.appendIns1(CODEJMP,0));

        fixJumpAdressList(jmp.m_falsePart,m_code.appendIns1(CODEPUSHCONST,m_code.appendConst(0)));
        jmpexitlist.add(m_code.appendIns1(CODEJMP,0));

        fixJumpAdressList(jmp.m_undefPart,m_code.appendIns0(CODEPUSHNULL));

        fixJumpAdressList(jmpexitlist,m_code.currentCodeSize());
      }
      break;
    }
  case RELOPGT:
    { NodeList left( predicate->child(0));
      NodeList right(predicate->child(1));
      if(left.size() == 1) {
        genExpression(left[0] );
        genExpression(right[0]);
        m_code.appendIns0(CODEGT);
      }
      else {
        JumpCodeTable      jmp;
        JumpCodeAdressList jmpexitlist;
        for(UINT i = 0; i < left.size(); i++) { // sizes are equal
          genExpression(left[i] );
          genExpression(right[i]);
          jmp.m_undefPart.add(m_code.appendIns1(CODECMP  ,0));
          jmp.m_truePart.add( m_code.appendIns1(CODEJMPGT,0));
          jmp.m_falsePart.add(m_code.appendIns1(CODEJMPLT,0));
        }
        fixJumpAdressList(jmp.m_falsePart,m_code.appendIns1(CODEPUSHCONST,m_code.appendConst(0)));
        jmpexitlist.add(m_code.appendIns1(CODEJMP,0));

        fixJumpAdressList(jmp.m_truePart,m_code.appendIns1(CODEPUSHCONST,m_code.appendConst(1)));
        jmpexitlist.add(m_code.appendIns1(CODEJMP,0));

        fixJumpAdressList(jmp.m_undefPart,m_code.appendIns0(CODEPUSHNULL));

        fixJumpAdressList(jmpexitlist,m_code.currentCodeSize());
      }
      break;
    }
  case RELOPLE:
    { NodeList left( predicate->child(0));
      NodeList right(predicate->child(1));
      if(left.size() == 1) {
        genExpression(left[0] );
        genExpression(right[0]);
        m_code.appendIns0(CODELE);
      }
      else {
        JumpCodeTable      jmp;
        JumpCodeAdressList jmpexitlist;
        for(UINT i = 0; i < left.size(); i++) { // sizes are equal
          genExpression(left[i] );
          genExpression(right[i]);
          jmp.m_undefPart.add(m_code.appendIns1(CODECMP  ,0));
          jmp.m_truePart.add( m_code.appendIns1(CODEJMPLT,0));
          jmp.m_falsePart.add(m_code.appendIns1(CODEJMPGT,0));
        }
        fixJumpAdressList(jmp.m_truePart,m_code.appendIns1(CODEPUSHCONST,m_code.appendConst(1)));
        jmpexitlist.add(m_code.appendIns1(CODEJMP,0));

        fixJumpAdressList(jmp.m_falsePart,m_code.appendIns1(CODEPUSHCONST,m_code.appendConst(0)));
        jmpexitlist.add(m_code.appendIns1(CODEJMP,0));

        fixJumpAdressList(jmp.m_undefPart,m_code.appendIns0(CODEPUSHNULL));

        fixJumpAdressList(jmpexitlist,m_code.currentCodeSize());
      }
      break;
    }
  case RELOPLT:
    { NodeList left( predicate->child(0));
      NodeList right(predicate->child(1));
      if(left.size() == 1) {
        genExpression(left[0] );
        genExpression(right[0]);
        m_code.appendIns0(CODELE);
      }
      else {
        JumpCodeTable      jmp;
        JumpCodeAdressList jmpexitlist;
        for(UINT i = 0; i < left.size(); i++) { // sizes are equal
          genExpression(left[i] );
          genExpression(right[i]);
          jmp.m_undefPart.add(m_code.appendIns1(CODECMP  ,0));
          jmp.m_truePart.add( m_code.appendIns1(CODEJMPLT,0));
          jmp.m_falsePart.add(m_code.appendIns1(CODEJMPGT,0));
        }
        fixJumpAdressList(jmp.m_falsePart,m_code.appendIns1(CODEPUSHCONST,m_code.appendConst(0)));
        jmpexitlist.add(m_code.appendIns1(CODEJMP,0));

        fixJumpAdressList(jmp.m_truePart,m_code.appendIns1(CODEPUSHCONST,m_code.appendConst(1)));
        jmpexitlist.add(m_code.appendIns1(CODEJMP,0));

        fixJumpAdressList(jmp.m_undefPart,m_code.appendIns0(CODEPUSHNULL));

        fixJumpAdressList(jmpexitlist,m_code.currentCodeSize());
      }
      break;
    }
  case BETWEEN:

  default     :
    stopcomp(predicate);
    break;
  }
}

void SqlCompiler::genCode() {
  switch(m_root->token()) {
  case UPDATE       :
    genUpdate(m_root);
    break;

  case INSERT       :
    genInsert(m_root);
    break;

  case DELETESYM    :
    genDelete(m_root);
    break;

  case SELECT       :
  case UNION        :
  case INTERSECT    :
  case SETDIFFERENCE:
    genSelect(m_root);
    break;

  case DECLARE:
    genDeclare(m_root);
    break;

  case FETCH        : // ie check hostvars has the right type
    genFetch(m_root);
    break;

  case CREATE       :
    genCreate(m_root);
    break;

  case DROP         :
    genDrop(m_root);
    break;

  case EXPLAIN      :
    genExplain(m_root);
    break;

  default           :
    stopcomp(m_root);
    break;
  }
}

void SqlCompiler::getCode(VirtualCode &vc) {
  vc = m_code.getCode();
}

void SqlCompiler::parse() {
  _tprintf(_T("parsing:%s\n"),m_bndstmt.m_stmt); fflush(stdout);
  const SqlApiBindStmtHead &head = m_bndstmt.m_stmtHead;
  SourcePosition pos(head.m_pos.m_line,head.m_pos.m_column);
  ParserTree::parse(head.m_sourceFile,pos,m_bndstmt.m_stmt);
}

SqlCompiler::SqlCompiler(const Database            &db,
                         const SqlApiBindProgramId &programid,
                         const SqlApiBindStmt      &bndstmt
                         ) : m_db(db)
                           , m_bndstmt(bndstmt)
                           , m_programid(programid)
{
  parse();

#if defined(TRACECOMP)
  if(ok()) {
    dumpSyntaxTree(m_root);
    _tprintf(_T("tree:%s\n"),m_root->toString().cstr());
  }
#endif

  if(ok()) genCode();
  if(ok()) m_code.appendDataToCode();
  dumpSyntaxTree(m_root);

#if defined(TRACECOMP)
  if(ok()) {
    FILE *mm = fopen(_T("c:\\temp\\testcode.txt"),_T("a"));
	  if(mm != NULL) {
		  _ftprintf(mm,_T("statement:%s\n"),bndstmt.m_stmt);
		  m_code.dump(mm);
		  fclose(mm);
	  }
  }
#endif

}

SqlCompiler::~SqlCompiler() {
  for(UINT i = 0; i < m_selectOperators.size(); i++)
    delete m_selectOperators[i];
}

bool sqlCompile(   const Database            &db     ,
                   SqlApiBindProgramId       &programid ,
                   const SqlApiBindStmt      &bndstmt,
                   VirtualCode               &vc     ,
                   StringArray               &errmsg ,
                   sqlca                     &ca     ) {
  try {
    SqlCompiler comp(db,programid,bndstmt);

    if(comp.ok())
      comp.getCode(vc);
    else {
      if(comp.m_errmsg.size() == 0)
        errmsg.add(_T("Unknown error"));
      else
        errmsg = comp.m_errmsg;

      ca.seterror(SQL_SYNTAX_ERROR,_T("%s"),errmsg[0].cstr());
    }
    return comp.ok();
  } catch(sqlca exceptionca) {
    errmsg.add(exceptionca.sqlerrmc);
    ca = exceptionca;
    return false;
  } catch(Exception e) {
    ca.seterror(SQL_FATAL_ERROR,_T("Internal compilererror:%s"),e.what());
    errmsg.add(ca.sqlerrmc);
    return false;
  } catch(...) {
    ca.seterror(SQL_FATAL_ERROR,_T("Internal compilererror:Unknown Exception"));
    errmsg.add(ca.sqlerrmc);
    return false;
  }
}

bool sqlCompile(   const Database            &db        ,
                   const String              &stmt      ,
                   VirtualCode               &vc        ,
                   StringArray               &errmsg    ,
                   sqlca                     &ca        ) {
  SqlApiBindStmt bndstmt(stmt.cstr());
  bndstmt.m_stmtHead.m_pos.m_line   = 1;
  bndstmt.m_stmtHead.m_pos.m_column = 0;
  SqlApiBindProgramId programid;
  return sqlCompile(db,programid,bndstmt,vc,errmsg,ca);
}
