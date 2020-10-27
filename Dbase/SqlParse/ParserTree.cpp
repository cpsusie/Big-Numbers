#include "stdafx.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sqlapi.h>
#include "SqlParser.h"

SyntaxNode *ParserTree::fetchStringNode(const TCHAR *str, int token) {
  SyntaxNode *p = new SyntaxNode(str,token);
  m_nodetable.add(p);
  return p;
}

SyntaxNode *ParserTree::fetchNumberNode(double number) {
  SyntaxNode *p = new SyntaxNode(number);
  m_nodetable.add(p);
  return p;
}

SyntaxNode *ParserTree::fetchDateNode(const Date &d) {
  SyntaxNode *p = new SyntaxNode(d);
  m_nodetable.add(p);
  return p;
}

SyntaxNode *ParserTree::fetchTimeNode(const Time &d) {
  SyntaxNode *p = new SyntaxNode(d);
  m_nodetable.add(p);
  return p;
}

SyntaxNode *ParserTree::fetchTimestampNode(const Timestamp &d) {
  SyntaxNode *p = new SyntaxNode(d);
  m_nodetable.add(p);
  return p;
}

SyntaxNode *ParserTree::fetchSyntaxNodeWithChildren(int token, va_list argptr) {
  SyntaxNode *p = new SyntaxNode(token,argptr);
  m_nodetable.add(p);
  return p;
}

static TCHAR *escape(TCHAR *dst, const TCHAR *src) {
  src++;
  const TCHAR *sss = src;
  TCHAR *t;
  for(t = dst; *src; src++) {
    if(*src == _T('\\')) {
      src++;
      *(t++) = *src;
    } else if(*src == _T('"')) {
      break;
    } else {
      *(t++) = *src;
    }
  }
  *(t++) = 0;
  return dst;
}

SyntaxNode *ParserTree::vFetchTokenNode(int token, va_list argptr) {
  TCHAR tmpstr[4096];
  SyntaxNode *p;
  switch(token) {
  case STRING:
    { TCHAR *s = va_arg(argptr, TCHAR *);
      p = fetchStringNode(escape(tmpstr,s));
      break;
    }

  case PARAM :
  case NAME  :
  case HOSTVAR :
    { TCHAR *s = va_arg(argptr, TCHAR *);
      p = fetchStringNode(s,token);
      break;
    }

  case NUMBER:
    { double d = va_arg(argptr, double);
      p = fetchNumberNode(d);
      break;
    }

  case DATECONST:
    { Date &d = va_arg(argptr, Date);
      p = fetchDateNode(d);
      break;
    }

  case TIMECONST:
    { Time &d = va_arg(argptr, Time);
      p = fetchTimeNode(d);
      break;
    }

  case TIMESTAMPCONST:
    { Timestamp &d = va_arg(argptr, Timestamp);
      p = fetchTimestampNode(d);
      break;
    }

  default:
    { p = fetchSyntaxNodeWithChildren(token, argptr);
      break;
    }
  }

  return p;
}

SyntaxNode *ParserTree::fetchTokenNode(int token,...) {
  va_list argptr;
  va_start(argptr, token);
  SyntaxNode *p = vFetchTokenNode(token, argptr);
  va_end(argptr);
  return p;
}

void ParserTree::vAppendError(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  m_errmsg.add(vformat(format, argptr));
  m_ok = false;
}

void ParserTree::appendError(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr, format);
  vAppendError(format, argptr);
  va_end(argptr);
}

void ParserTree::vAppendError(const SourcePosition &pos, long sqlcode, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  String tmp = vformat(format, argptr);
  if(m_filename.length() == 0) {
    appendError(_T("Error %ld in line %d:%s"), sqlcode, pos.getLineNumber(), tmp.cstr());
  } else {
    appendError(_T("%s(%d,%d) : error %d : %s")
               ,m_filename.cstr()
               ,pos.getLineNumber()
               ,pos.getColumn()
               ,sqlcode
               ,tmp.cstr() );
  }
}

void ParserTree::listErrors(FILE *f) {
  for(size_t i = 0; i < m_errmsg.size();i++) {
    _ftprintf(f,_T("%s\n"), m_errmsg[i].cstr());
  }
  fflush(f);
}

void ParserTree::listErrors(const String &fname) {
  FILE *f = fopen(fname,_T("w"));
  if(f == nullptr) {
    _ftprintf(stdout,_T("Cannot open %s\n"),fname.cstr());
    listErrors(stdout);
  } else {
    listErrors(f);
    fclose(f);
  }
}

void ParserTree::releaseAll() {
  for(size_t i = 0; i < m_nodetable.size(); i++) {
    delete m_nodetable[i];
  }
  m_errmsg.clear();
  m_root = nullptr;
  m_ok   = true;
}

ParserTree::ParserTree(TCHAR *filename) {
  m_ok       = true;
  m_root     = nullptr;
  m_filename = filename;
}

ParserTree::~ParserTree() {
  releaseAll();
}

void ParserTree::parse(const String &sourcefile, const SourcePosition &pos, const String &stmt) {
  releaseAll();
  setFilename(sourcefile);
  SqlParser parser(*this);
  parser.parse(pos,stmt);
}

void dumpSyntaxTree( const SyntaxNode *n, FILE *f, int level ) {
  if(n) {
    const SourcePosition &pos = n->pos();
    _ftprintf(f, _T("(%2d,%2d) %*.*s %2d token:[%s] ")
              , pos.getLineNumber(), pos.getColumn()
              , level, level, _T(" "), level
              , SqlParser::getTables().getSymbolName(n->token()));
    switch(n->token()) {
    case NAME          : _ftprintf(f,_T(":[%s]\n"           ), n->name()                           ); break;
    case PARAM         : _ftprintf(f,_T(":[%s]\n"           ), n->param()                          ); break;
    case HOSTVAR       : _ftprintf(f,_T(":[%s] index:[%d]\n"), n->hostvar(),n->getHostVarIndex()   ); break;
    case STRING        : _ftprintf(f,_T(":[%s]\n"           ), n->str()                            ); break;
    case NUMBER        : _ftprintf(f,_T(":[%lg]\n"          ), n->number()                         ); break;
    case DATECONST     : _ftprintf(f,_T(":[%s]\n"           ), n->getDate().toString().cstr()      ); break;
    case TIMECONST     : _ftprintf(f,_T(":[%s]\n"           ), n->getTime().toString().cstr()      ); break;
    case TIMESTAMPCONST: _ftprintf(f,_T(":[%s]\n"           ), n->getTimestamp().toString().cstr() ); break;
    default	           :
      { _ftprintf(f,_T("\n"));
        for(UINT i = 0; i < n->childCount(); i++)
          dumpSyntaxTree(n->child(i),f,level+1);
		    break;
	    }
    }
  }
}

void dumpSyntaxTree(const SyntaxNode *node, const String &filename ) {
  FILE *f = fopen(filename,_T("w"));
  dumpSyntaxTree(node, f);
  fclose(f);
}

void ParserTree::dumpTree(FILE *f) const {
  dumpSyntaxTree(m_root, f);
}

void ParserTree::dumpTree(const String &filename) const {
  FILE *f = fopen(filename,_T("w"));
  if(f == nullptr) {
    return;
  }
  dumpTree(f);
  fclose(f);
}

String SyntaxNode::toString() const {
  switch(token()) {
  case STRING         : return String(_T("\"")) + str() + _T("\"");
  case NUMBER         : return format(_T("%lg"), number());
  case NAME           : return name();
  case DATECONST      : return String(_T("#")) + getDate().toString()      + _T("#");
  case TIMECONST      : return String(_T("#")) + getTime().toString()      + _T("#");
  case TIMESTAMPCONST : return String(_T("#")) + getTimestamp().toString() + _T("#");
  case PARAM          : return param();
  case HOSTVAR        : return hostvar();
  case PLUS           : return String(_T("(")) + child(0)->toString() + _T(" + ") + child(1)->toString() + _T(")");
  case MINUS          : if(childCount() == 1) {
                          return String(_T("-(")) + child(0)->toString() + _T(")");
                        } else {
                          return String(_T("(")) + child(0)->toString() + _T(" - ")  + child(1)->toString() + _T(")");
                        }
  case MULT           : return String(_T("(")) + child(0)->toString()   + _T(" * ")  + child(1)->toString() + _T(")");
  case DIVOP          : return String(_T("(")) + child(0)->toString()   + _T(" / ")  + child(1)->toString() + _T(")");
  case MODOP          : return String(_T("(")) + child(0)->toString()   + _T(" % ")  + child(1)->toString() + _T(")");
  case EXPO           : return String(_T("(")) + child(0)->toString()   + _T(" ** ") + child(1)->toString() + _T(")");
  case DOT            : return child(0)->toString() + _T(".") + child(1)->toString();
  case CONCAT         : return String(_T("(")) + child(0)->toString()   + _T(" || ") + child(1)->toString() + _T(")");
  case RELOPLT        :
  case RELOPLE        :
  case NOTEQ          :
  case RELOPGT        :
  case RELOPGE        :
  case EQUAL          : return child(0)->toString() + relopstring(token()) + child(1)->toString();
  case AND            : return String(_T("(")) + child(0)->toString() + _T(" AND ") + child(1)->toString() + _T(")");
  case OR             : return String(_T("(")) + child(0)->toString() + _T(" OR ")  + child(1)->toString() + _T(")");
  case NOT            : return String(_T("not (")) + child(0)->toString() + _T(")");
  case COMMA          : return child(0)->toString() + _T(",") + child(1)->toString();
  case SUBSTRING      : return String(_T("substring("))
                             + child(0)->toString() + _T(",")
                             + child(1)->toString() + _T(",")
                             + child(2)->toString()+
                               _T(")");
  case NULLVAL        : return _T("null");
  case LIKE           : if(childCount() == 2) {
                          return child(0)->toString() + _T(" like ") + child(1)->toString();
                        } else {
                          return _T(" like ");
                        }
  case ISNULL         : return child(0)->toString() + _T(" is null");
  case BETWEEN        : return child(0)->toString() + _T(" between ") +
                               child(1)->toString() + _T(" and ") +
                               child(2)->toString();
  case INSYM          : return child(0)->toString() + _T(" in ") + child(1)->toString();
  case STAR           : return _T("*");
  case UNION          :
    return String(_T("(")) + child(0)->toString() +  ((childCount()==3)?_T(" union all "):_T(" union ")) + child(1)->toString() + _T(")");
  case INTERSECT      :
    return String(_T("(")) + child(0)->toString() + _T(" intersect ")     + child(1)->toString() + _T(")");
  case SETDIFFERENCE  :
    return String(_T("(")) + child(0)->toString() + _T(" setdifference ") + child(1)->toString() + _T(")");
  case WHERE          :
  case INTO           :
  case HAVING         :
  case GROUP          :
  case ORDER          :
  case ACCESSS        :
  case MODE           :
    if(childCount() == 0) {
      return EMPTYSTRING;
    }
    break;
  case KEY            :
    return String(_T("key ("))    + child(0)->toString() + _T(")");
  case TYPEVARCHAR        :
    return String(_T("varchar(")) + child(0)->toString() + _T(")");
  }
  String s = SqlParser::getTables().getSymbolName(token());
  int sons = childCount();
  for(int i = 0; i < sons; i++) {
    String c = child(i)->toString();
    if(c.length() > 0) {
      s += String(_T(" ")) + c;
    }
  }
  return s;
}

const TCHAR *relopstring(int token) {
  switch(token) {
  case RELOPLE: return _T("<=");
  case RELOPLT: return _T("<");
  case RELOPGE: return _T(">=");
  case RELOPGT: return _T(">");
  case EQUAL  : return _T("=");
  case NOTEQ  : return _T("<>");
  default     : return _T("?");
  }
}
