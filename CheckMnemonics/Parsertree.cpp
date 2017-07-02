#include "stdafx.h"
#include <MyAssert.h>
#include <String.h>
#include <stdarg.h>
#include "ParserTree.h"
#include "ResourceParser.h"

ParserTree::ParserTree(const String &fileName) {
  m_root            = NULL;
  m_ok              = true;
  m_absolutFileName = FileNameSplitter(fileName).getAbsolutePath();
}

void ParserTree::vAddError(const SourcePosition *pos, const TCHAR *format, va_list argptr) {
  String tmp2;
  String tmp = vformat(format, argptr);
  if(pos != NULL) {
    tmp2 = ::format(_T("(%d,%d) : error --- %s")
                   ,pos->getLineNumber(), pos->getColumn()
                   ,tmp.cstr()
                   );
  } else {
    tmp2 = ::format(_T("Error:%s"), tmp.cstr());
  }
  tmp2.replace('\n',' ');
  m_errors.add(tmp2);
  m_ok = false;
}

void ParserTree::addError(const SourcePosition &pos, const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr, format);
  vAddError(&pos, format, argptr);
  va_end(argptr);
}

void ParserTree::addError(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr, format);
  vAddError(NULL, format, argptr);
  va_end(argptr);
}

void ParserTree::listErrors(FILE *f) const {
  for(size_t i = 0; i < m_errors.size();i++) {
    _ftprintf(f,_T("%s%s\n"), m_absolutFileName.cstr(), m_errors[i].cstr());
  }
  fflush(f);
}

void ParserTree::listErrors(tostream &out) const {
  for(size_t i = 0; i < m_errors.size();i++) {
    out << m_errors[i] << endl;
  }
  out.flush();
}

void ParserTree::listErrors(const TCHAR *fname) const {
  FILE *f = _tfopen(fname,_T("w"));
  if(f == NULL) {
    _ftprintf(stdout,_T("Cannot open %s\n"),fname);
    listErrors(stdout);
  } else {
    listErrors(f);
    fclose(f);
  }
}

static void getListFromTree(const SyntaxNode *n, int delimiterSymbol, NodeArray &list) {
  if(n->getSymbol() == delimiterSymbol) {
    getListFromTree(n->getLeftChild(),delimiterSymbol,list);
    list.add(n->getRightChild());
  } else {
    list.add(n);
  }
}

NodeArray ParserTree::getListFromTree(const SyntaxNode *n, int delimiterSymbol) { // static
  NodeArray result;
  if(n) {
    ::getListFromTree(n, delimiterSymbol, result);
  }
  return result;
}

void ParserTree::addNodesToTable(const SyntaxNode *n) {
  m_nodetable.add(n);
  switch(n->getSymbol()) {
  case NUMBER    :
  case IDENTIFIER:
  case STRING    :
    break;
  default:
    { for(int i = 0; i < n->getChildCount(); i++) {
        addNodesToTable(n->getChild(i));
      }
      break;
    }
  }
}

void ParserTree::releaseAll() {
  for(size_t i = 0; i < m_nodetable.size(); i++) {
    delete m_nodetable[i];
  }
  m_nodetable.clear();
  m_ok       = true;
  m_root     = NULL;
  m_errors.clear();
}

SyntaxNode *ParserTree::fetchNode(const SourcePosition &pos, int symbol,...) {
  va_list argptr;
  va_start(argptr,symbol);
  SyntaxNode *ret = vFetchNode(pos,symbol,argptr);
  va_end(argptr);
  return ret;
}

SyntaxNode *ParserTree::vFetchNode(const SourcePosition &pos, int symbol, va_list argptr) {
  SyntaxNode *result;
  switch(symbol) {
  case NUMBER:
    result = new NumberSyntaxNode(pos,va_arg(argptr,int));
    break;
  case IDENTIFIER  :
    result = new NameSyntaxNode(pos, va_arg(argptr,TCHAR*));
    break;
  case STRING:
    result = new StringSyntaxNode(pos, va_arg(argptr,TCHAR*));
    break;
  default    :
    result = new ResourceNodeTree(pos, symbol, argptr);
    break;
  }
  m_nodetable.add(result);
  return result;
}

ParserTree::~ParserTree() {
  releaseAll();
}

void ParserTree::dumpSyntaxNode(const SyntaxNode *node, FILE *f, int level, bool recurse) { // static
  if(node) {
    const SourcePosition &pos = node->getPos();
    _ftprintf(f,_T("%*.*s (%d,%d) [%s] ")
           ,level,level,_T(" ")
           ,pos.getLineNumber(), pos.getColumn()
           ,ResourceTables->getSymbolName(node->getSymbol())
           );
    switch(node->getSymbol()) {
    case IDENTIFIER:
      _ftprintf(f, _T(":[%s]\n"), node->getName().cstr() );
      break;
    case NUMBER    :
      _ftprintf(f, _T(":[%s]\n"), toString(node->getNumber()).cstr());
      break;
    case STRING:
      _ftprintf(f, _T(":\"%s\"\n"), node->getString().cstr());
      break;

    default        :
      { _ftprintf(f, _T("\n"));
        if(recurse) {
          for(int i = 0; i < node->getChildCount(); i++) {
            dumpSyntaxNode(node->getChild(i), f, level+1, true);
          }
        }
        break;
      }
    }
  }
}

void ParserTree::dumpSyntaxTree(const SyntaxNode *root, FILE *f) { // static
  dumpSyntaxNode(root, f, 0, true);
}

void ParserTree::dumpSyntaxTree(const SyntaxNode *root, const TCHAR *fname) { // static
  FILE *f = _tfopen(fname,_T("w"));
  if(f == NULL) {
    _ftprintf(stdout, _T("Cannot open %s\n"), fname);
    dumpSyntaxTree(root, stdout);
  } else {
    dumpSyntaxTree(root, f);
    fclose(f);
  }
}

void ParserTree::dumpTree(FILE *f) const {
  dumpSyntaxTree(m_root, f);
}

void ParserTree::dumpTree(const TCHAR *fname) const {
  FILE *f = MKFOPEN(fname,_T("w"));
  dumpTree(f);
  fclose(f);
}

void ParserTree::dumpNodeTable(FILE *f) const {
  for(size_t i = 0; i < m_nodetable.size(); i++) {
    const SyntaxNode *n = m_nodetable[i];
    switch(n->getSymbol()) {
    case acceleratorsDefinition :
    case stringTableDefinition   :
    case menuDefinition         :
    case dialogDefinition       :
      dumpSyntaxNode(n, f, 0, true);
      break;
    }
  }
}
