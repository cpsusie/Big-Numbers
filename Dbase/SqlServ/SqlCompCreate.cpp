#include "stdafx.h"
#include <float.h>

void SqlCompiler::genKeyDefinition(const SyntaxNode      *n, // n = <keyfield>
                                   const TableDefinition &tableDef,
                                   IndexDefinition       &indexDef
                                  ) {
  const TCHAR *name = n->child(0)->name();
  bool orderAscending = n->child(1)->token() == ASCENDING ? true : false;

  if(indexDef.getColumnCount() >= MAXKEYFIELDCOUNT) {
    syntaxError(n,SQL_KEY_TOO_BIG,_T("Too many fields in primary key of %s. Max number of fields=%d"),tableDef.getTableName().cstr(), MAXKEYFIELDCOUNT-1);
    return;
  }
  for(UINT i = 0; i < tableDef.getColumnCount();i++) {
    if(_tcsicmp(tableDef.getColumn(i).m_name.cstr(),name) == 0) {
      if(tableDef.getColumn(i).m_nullAllowed) {
        syntaxError(n,SQL_KEYFIELD_NULL,_T("Field <%s> can be null"), name);
        return;
      }
      for(UINT j = 0; j < indexDef.getColumnCount(); j++) {
        if(indexDef.getColumn(j).m_col == i) {
          syntaxError(n,SQL_DUPLICATE_FIELDNAME,_T("Duplicate column in index:<%s>"),name);
          return;
        }
      }
      indexDef.m_columns.add(IndexColumn(i,orderAscending));
      return;
    }
  }
  syntaxError(n,SQL_INVALID_COLUMNNAME,_T("Column <%s> not defined"),name);
}

void SqlCompiler::checkNumberRange(SyntaxNode *n, double min, double max, bool integer) {
  double v = n->number();
  if(v < min || v > max) {
    syntaxError(n,SQL_INVALID_DEFAULT,_T("Default value exceeds range of fieldtype. Valid range=[%lg,%lg]"),min,max);
  }
  if(integer && (v != floor(v))) {
    syntaxError(n,SQL_INVALID_DEFAULT,_T("Default value must be integer"));
  }
}

void SqlCompiler::checkNumberRange(SyntaxNode *n, unsigned char dbtype) {
  double value = n->number();
  switch(dbtype) {
  case DBTYPE_CHAR      :
  case DBTYPE_CHARN     :
    checkNumberRange(n,    DBMINCHAR  ,  DBMAXCHAR  ,true ); break;
  case DBTYPE_UCHAR     :
  case DBTYPE_UCHARN    :
    checkNumberRange(n,    DBMINUCHAR ,  DBMAXUCHAR ,true ); break;
  case DBTYPE_SHORT     :
  case DBTYPE_SHORTN    :
    checkNumberRange(n,    DBMINSHORT ,  DBMAXSHORT ,true ); break;
  case DBTYPE_USHORT    :
  case DBTYPE_USHORTN   :
    checkNumberRange(n,    DBMINUSHORT,  DBMAXUSHORT,true ); break;
  case DBTYPE_INT       :
  case DBTYPE_INTN      :
    checkNumberRange(n,    DBMININT   ,  DBMAXINT   ,true ); break;
  case DBTYPE_UINT      :
  case DBTYPE_UINTN     :
    checkNumberRange(n,    DBMINUINT  ,  DBMAXUINT  ,true ); break;
  case DBTYPE_LONG      :
  case DBTYPE_LONGN     :
    checkNumberRange(n,    DBMINLONG  ,  DBMAXLONG  ,true ); break;
  case DBTYPE_ULONG     :
  case DBTYPE_ULONGN    :
    checkNumberRange(n,    DBMINULONG ,  DBMAXULONG ,true ); break;
  case DBTYPE_FLOAT     :
  case DBTYPE_FLOATN    :
    checkNumberRange(n,    DBMINFLOAT ,  DBMAXFLOAT ,false); break;
  case DBTYPE_DOUBLE    :
  case DBTYPE_DOUBLEN   :
    checkNumberRange(n,    DBMINDOUBLE,  DBMAXDOUBLE,false); break;
  default:stopcomp(n);
  }
}

void SqlCompiler::checkDefault(SyntaxNode *n, ColumnDefinition &col) {
  if(n->childCount() == 0) {
    col.m_defaultValue = _T("");
    return;
  }
  SyntaxNode *defaultValue = n->child(0);
  UINT l;
  switch(col.getType()) {
  case DBTYPE_CSTRING     :
  case DBTYPE_CSTRINGN    :
  case DBTYPE_WSTRING     :
  case DBTYPE_WSTRINGN    :
  case DBTYPE_VARCHAR    :
  case DBTYPE_VARCHARN   :
    switch(defaultValue->token()) {
    case STRING:
      l = (UINT)_tcslen(defaultValue->str());
      if(l > col.m_len || l > MAXDEFAULTVALUE) {
        syntaxError(defaultValue,SQL_INVALID_DEFAULT,_T("Invalid length of defaultValue. Max length=%d"),min(col.m_len,MAXDEFAULTVALUE));
      } else {
        col.m_defaultValue = defaultValue->str();
      }
      break;
    default:
      syntaxError(defaultValue,SQL_INVALID_DEFAULT,_T("Invalid type of defaultvalue. Must be string"));
      break;
    }
    break;
  case DBTYPE_CHAR       :
  case DBTYPE_CHARN      :
  case DBTYPE_UCHAR      :
  case DBTYPE_UCHARN     :
  case DBTYPE_SHORT      :
  case DBTYPE_SHORTN     :
  case DBTYPE_USHORT     :
  case DBTYPE_USHORTN    :
  case DBTYPE_INT        :
  case DBTYPE_INTN       :
  case DBTYPE_UINT       :
  case DBTYPE_UINTN      :
  case DBTYPE_LONG       :
  case DBTYPE_LONGN      :
  case DBTYPE_ULONG      :
  case DBTYPE_ULONGN     :
  case DBTYPE_FLOAT      :
  case DBTYPE_FLOATN     :
  case DBTYPE_DOUBLE     :
  case DBTYPE_DOUBLEN    :
    switch(defaultValue->token()) {
    case NUMBER:
      checkNumberRange(defaultValue,col.getType());
      if(ok()) {
        col.m_defaultValue = format(_T("%lf"),defaultValue->number());
      }
      break;

    default:
      syntaxError(defaultValue,SQL_INVALID_DEFAULT,_T("Invalid type of defaultvalue. Must be numeric"));
      break;
    }
    break;
  case DBTYPE_DATE       :
  case DBTYPE_DATEN      :
    if(defaultValue->token() != DATECONST) {
      syntaxError(defaultValue,SQL_INVALID_DEFAULT,_T("Invalid type of defaultvalue. Must be date"));
    }
    if(ok()) {
      col.m_defaultValue = defaultValue->getDate().toString();
    }
    break;
  case DBTYPE_TIME       :
  case DBTYPE_TIMEN      :
    if(defaultValue->token() != TIMECONST) {
      syntaxError(defaultValue,SQL_INVALID_DEFAULT,_T("Invalid type of defaultvalue. Must be time"));
    }
    if(ok()) {
      col.m_defaultValue = defaultValue->getTime().toString();
    }
    break;
  case DBTYPE_TIMESTAMP  :
  case DBTYPE_TIMESTAMPN :
    if(defaultValue->token() != TIMESTAMPCONST) {
      syntaxError(defaultValue,SQL_INVALID_DEFAULT,_T("Invalid type of defaultvalue. Must be timestamp"));
    }
    if(ok()) {
      col.m_defaultValue = defaultValue->getTimestamp().toString();
    }
    break;
  default:
    syntaxError(defaultValue,SQL_FATAL_ERROR,_T("Unknown fieldtype:%d"),col.getType());
    break;
  }
}

void SqlCompiler::getType(const SyntaxNode *n, ColumnDefinition &col) {
  SyntaxNode *t = n->child(1);
  double       strl;

  col.m_nullAllowed = n->child(2)->token() == NULLVAL;

  switch(t->token()) {
  case TYPECHAR  :
    if(t->childCount() >= 2 && t->child(1)->token() == NUMBER) {
      col.m_type = col.m_nullAllowed ? DBTYPE_CSTRINGN : DBTYPE_CSTRING;
      strl = t->child(1)->number();
      if(strl <= 0 || floor(strl) != strl) {
        syntaxError(t->child(1),SQL_SYNTAX_ERROR,_T("Invalid string length. Must be postive integer"));
      }
      col.m_len = (ULONG)strl;
    } else if(t->child(0)->token() == SIGNED) {
      col.m_type = col.m_nullAllowed ? DBTYPE_CHARN  : DBTYPE_CHAR;
    } else {
      col.m_type = col.m_nullAllowed ? DBTYPE_UCHARN : DBTYPE_UCHAR;
    }
    break;

  case TYPEWCHAR  :
    if(t->childCount() >= 2 && t->child(1)->token() == NUMBER) {
      col.m_type = col.m_nullAllowed ? DBTYPE_WSTRINGN : DBTYPE_WSTRING;
      strl = t->child(1)->number();
      if(strl <= 0 || floor(strl) != strl) {
        syntaxError(t->child(1),SQL_SYNTAX_ERROR,_T("Invalid string length. Must be postive integer"));
      }
      col.m_len = (ULONG)strl;
    } else if(t->child(0)->token() == SIGNED) {
      col.m_type = col.m_nullAllowed ? DBTYPE_SHORTN  : DBTYPE_SHORT;
    } else {
      col.m_type = col.m_nullAllowed ? DBTYPE_USHORTN : DBTYPE_USHORT;
    }
    break;

  case TYPESHORT :
    if(t->child(0)->token() == SIGNED) {
      col.m_type = col.m_nullAllowed ? DBTYPE_SHORTN   : DBTYPE_SHORT;
    } else {
      col.m_type = col.m_nullAllowed ? DBTYPE_USHORTN  : DBTYPE_USHORT;
    }
    break;

  case TYPEINT   :
    if(t->child(0)->token() == SIGNED) {
      col.m_type = col.m_nullAllowed ? DBTYPE_INTN     : DBTYPE_INT;
    } else {
      col.m_type = col.m_nullAllowed ? DBTYPE_UINTN    : DBTYPE_UINT;
    }
    break;

  case TYPELONG  :
    if(t->child(0)->token() == SIGNED) {
      col.m_type = col.m_nullAllowed ? DBTYPE_LONGN    : DBTYPE_LONG;
    } else {
      col.m_type = col.m_nullAllowed ? DBTYPE_ULONGN   : DBTYPE_ULONG;
    }
    break;

  case TYPEFLOAT :
    col.m_type   = col.m_nullAllowed ? DBTYPE_FLOATN   : DBTYPE_FLOAT ;
    break;

  case TYPEDOUBLE:
    col.m_type   = col.m_nullAllowed ? DBTYPE_DOUBLEN  : DBTYPE_DOUBLE; 
    break;

  case TYPEVARCHAR:
    col.m_type   = col.m_nullAllowed ? DBTYPE_VARCHARN : DBTYPE_VARCHAR;
    strl = t->child(0)->number();
    if(strl <= 0 || floor(strl) != strl) {
      syntaxError(t->child(0),SQL_SYNTAX_ERROR,_T("Invalid varchar length. Must be positive integer"));
    }
    if(strl > MAXVARCHARLEN) {
      syntaxError(t->child(0),SQL_SYNTAX_ERROR,_T("Invalid varchar length. Max length=%lu"),MAXVARCHARLEN);
    }
    col.m_len  = (unsigned long)strl;
    break;

  case TYPEDATE  :
    col.m_type   = col.m_nullAllowed ? DBTYPE_DATEN : DBTYPE_DATE; 
    break;

  case TYPETIME  :
    col.m_type   = col.m_nullAllowed ? DBTYPE_TIMEN : DBTYPE_TIME; 
    break;

  case TYPETIMESTAMP: 
    col.m_type   = col.m_nullAllowed ? DBTYPE_TIMESTAMPN : DBTYPE_TIMESTAMP; 
    break;

  default        : stopcomp(t);
  }
  checkDefault(n->child(3), col);
}

void SqlCompiler::genColumnDefinition(const SyntaxNode *n, TableDefinition &tableDef) {
  if(n->token() == COLUMN) {
    ColumnDefinition col;
    const TCHAR *name = n->child(0)->name();
    if(_tcslen(name) >= ARRAYSIZE(col.m_name)) {
      syntaxError(n->child(0),SQL_INVALID_COLUMNNAME,_T("Fieldname <%s> too long. Max length=%d"),name, ARRAYSIZE(col.m_name)-1);
      return;
    }
    for(UINT i = 0; i < tableDef.getColumnCount();i++) {
      if(_tcsicmp(tableDef.getColumn(i).m_name.cstr(),name) == 0) {
        syntaxError(n,SQL_DUPLICATE_FIELDNAME,_T("Column %s already defined"),name);
        return;
      }
    }
    col.m_name = name;
    getType(n,col);

    try {
      tableDef.addColumn(col);
    } catch(sqlca ca) {
      syntaxError(n,ca.sqlcode,_T("%s"),ca.sqlerrmc);
    }
  }
}

void SqlCompiler::genCreateTableAs(SyntaxNode *n) { // n = create_stmt
  const TCHAR    *name = n->child(2)->name();
  IndexDefinition indexDef;

  if(_tcslen(name) > MAXTABLENAME) {
    syntaxError(n,SQL_INVALID_TABLENAME,_T("Tablename <%s> too long. Max length=%d"), name, MAXTABLENAME);
    return;
  }

  TableDefinition tableDef(TABLETYPE_USER,name);
  
  indexDef.m_indexType = INDEXTYPE_PRIMARY;
  indexDef.m_tableName = tableDef.getTableName();

  NodeList columns(n->child(3));
  NodeList keys(n->child(1)->child(0));

  for(size_t i = 0; i < columns.size(); i++) {
    genColumnDefinition(columns[i],tableDef);
  }
  for(size_t i = 0; i < keys.size(); i++) {
    genKeyDefinition(keys[i],tableDef,indexDef);
  }

  KeyFileDefinition keydef;
  try {
    keydef = tableDef.getKeyFileDefinition(indexDef);
  } catch(sqlca ca) {
    syntaxError(n->child(1),ca.sqlcode,_T("Primary key of %s exceeds %d bytes")
                                      ,tableDef.getTableName().cstr(),MAXKEYSIZE);
  }

  if(ok()) {
    Packer pack;
    pack << tableDef << indexDef;
    m_code.appendIns0(CODETRBEGIN);
    m_code.appendIns1(CODEPUSHADR,m_code.appendData(pack));
    m_code.appendIns0(CODECRTAB);
    m_code.appendIns0(CODETRCOMMIT);
    m_code.appendIns0(CODERETURN);
  }
/*
  tableDef.dump();
  indexDef.dump();
  keydef.dump();
*/
}

void SqlCompiler::genCreateTableLike(SyntaxNode *n) { // n = create_stmt
  const TCHAR *tableName = n->child(2)->name();
  const TCHAR *likename  = n->child(3)->name();

  if(_tcslen(tableName) > MAXTABLENAME) {
    syntaxError(n,SQL_INVALID_TABLENAME,_T("Tablename <%s> is too long. Max length=%d"), tableName, MAXTABLENAME);
    return;
  }
  if(_tcslen(likename) > MAXTABLENAME) {
    syntaxError(n->child(3),SQL_INVALID_TABLENAME,_T("Tablename <%s> is too long. Maxlength=%d"), likename, MAXTABLENAME);
    return;
  }
  if(ok()) {
    m_code.appendIns0(CODETRBEGIN);
    m_code.appendIns1(CODEPUSHCONST,m_code.appendConst(tableName));
    m_code.appendIns1(CODEPUSHCONST,m_code.appendConst(likename));
    m_code.appendIns0(CODECRLIK);
    m_code.appendIns0(CODETRCOMMIT);
    m_code.appendIns0(CODERETURN);
  }
}

void SqlCompiler::genCreateTable(SyntaxNode *n) { // n = create_stmt
  switch(n->child(1)->token()) {
  case LIKE:
    genCreateTableLike(n);
    break;
  default:
    genCreateTableAs(n);
    break;
  }
}

void SqlCompiler::genCreateIndex(SyntaxNode *n) { // n = create_stmt
  bool  unique          = n->child(1)->token() == UNIQUE;
  const TCHAR *indexName = n->child(2)->name();
  const TCHAR *tableName = n->child(3)->name();
  NodeList columns(n->child(4));

  if(_tcslen(indexName) > MAXINDEXNAME) {
    syntaxError(n->child(2),SQL_INVALID_INDEXNAME,_T("Indexname <%s> is too long. Max length=%d"),indexName, MAXINDEXNAME);
  }
  if(_tcslen(tableName) > MAXTABLENAME) {
    syntaxError(n->child(3),SQL_INVALID_TABLENAME,_T("Tablename <%s> is too long. Max length=%d"),tableName, MAXTABLENAME);
  }
  if(columns.size() > MAXKEYFIELDCOUNT) {
    syntaxError(n->child(4),SQL_KEY_TOO_BIG,_T("Too many fields in index %s. Max number of fiels in index=%d"),indexName, MAXKEYFIELDCOUNT);
  }

  for(size_t i = 0; i < columns.size(); i++) {
    SyntaxNode *col = columns[i]->child(0);
    if(_tcslen(col->name()) > MAXCOLUMNNAME) {
      syntaxError(col,SQL_INVALID_COLUMNNAME,_T("Fieldname <%s> too long. Max length=%d"),col->name(), MAXCOLUMNNAME);
    }
  }

  for(size_t i = 0; i < columns.size(); i++) {
    for(size_t j = 0; j < i; j++) {
      SyntaxNode *coli = columns[i]->child(0);
      SyntaxNode *colj = columns[j]->child(0);
      if(_tcsicmp(coli->name(),colj->name()) == 0)
        syntaxError(coli,SQL_DUPLICATE_FIELDNAME,_T("Duplicate column in index:<%s>"),coli->name());
    }
  }

  if(ok()) {
    CreateIndexData crinx;
    crinx.m_indexName = indexName;
    crinx.m_tableName = tableName;
    crinx.m_unique = unique;
    for(size_t i = 0; i < columns.size(); i++) {
      IndexColumnName col;
      const TCHAR *colname = columns[i]->child(0)->name();
      bool orderAscending = columns[i]->child(1)->token() == ASCENDING ? true : false;
      col.m_colName = colname;
      col.m_asc     = orderAscending;
      crinx.m_columns.add(col);
    }

    Packer pack;
    pack << crinx;
    m_code.appendIns0(CODETRBEGIN);
    m_code.appendIns1(CODEPUSHADR,m_code.appendData(pack));
    m_code.appendIns0(CODECRINX);
    m_code.appendIns0(CODETRCOMMIT);
    m_code.appendIns0(CODERETURN);
//    data.dump();
  }
}

void SqlCompiler::genCreate(SyntaxNode *n) { // n = create_stmt
  switch(n->child(0)->token()) {
  case TABLE   : 
    genCreateTable(n);
    break;

  case INDEX   :
    genCreateIndex(n);
    break;

  default      :
    stopcomp(n);
  }
}
