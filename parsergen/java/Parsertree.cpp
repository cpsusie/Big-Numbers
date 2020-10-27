#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include <stdarg.h>
#include <MyUtil.h>
#include "Java5Parser.h"

void ParserTree::vAppendError(const char *format, va_list argptr) {
  char tmp[256];
  vsprintf(tmp,format,argptr);
  m_errmsg.add(tmp);
  m_ok = false;
}

void ParserTree::appendError(const char *format,...) {
  va_list argptr;
  va_start(argptr,format);
  vAppendError(format,argptr);
  va_end(argptr);
}

void ParserTree::listErrors(FILE *f) {
  for(unsigned int i = 0; i < m_errmsg.size();i++)
     fprintf(f,"%s\n", m_errmsg[i].cstr());
  fflush(f);
}

void ParserTree::listErrors(char *fileName) {
  FILE *f = fopen(fileName,"w");
  if(f == nullptr) {
    fprintf(stdout,"Cannot open %s\n",fileName);
    listErrors(stdout);
  }
  else {
    listErrors(f);
    fclose(f);
  }
}

ParserTree::ParserTree() {
  m_ok   = true;
  m_root = nullptr;
}

void ParserTree::parse(const char *fileName) {
  m_nodeArray.clear();
  FileStream inputStream(fileName);
  JavaLex lex;
  lex.newStream(&inputStream);
  Java5Parser parser(*this,fileName,&lex);
  parser.parse();
}

ParserTree::~ParserTree() {
}

SyntaxNode *ParserTree::vFetchTokenNode(int token, va_list argptr) {
  switch(token) {
    case STRINGLITERAL:
      { char *s = va_arg(argptr,char*);
        return new SyntaxNode(s,STRINGLITERAL);
      }
    case IDENTIFIER  :
      { char *s = va_arg(argptr,char*);
        return new SyntaxNode(s,IDENTIFIER);
      }
    case NUMBER:
      { double d = va_arg(argptr,double);
        return new SyntaxNode(d);
        break;
      }
    default:
      return new SyntaxNode(token,argptr);
  }
}

String ParserTree::nodeToString(SyntaxNode *n) {
  char tmp[20];
  switch(n->getToken()) {
  case STRINGLITERAL : return n->getString();
  case IDENTIFIER      : return n->getName();
  case NUMBER    : sprintf(tmp,"%lg", n->getNumber() );
                   return tmp;
  default        : sprintf(tmp,"%s",tokenName(n->getToken()));
                   return tmp;
  }
}

void dumpSyntaxTree( SyntaxNode *node, FILE *f, int indent) {
  if( node ) {
    fprintf(f,"%*.*s %2d pos:(%2d,%d) token:%s ",
	  indent,indent," ",indent,
	  node->getPosition().m_lineno,node->getPosition().m_col, tokenName(node->getToken()) );
    switch(node->getToken()) {
    case STRINGLITERAL : fprintf(f,":%s\n",node->getString());
                     break;
    case IDENTIFIER      : fprintf(f,":%s\n",node->getName());
                     break;
    case NUMBER    : fprintf(f,":[%18.15lg]\n", node->getNumber() );
                     break;
    default        : { fprintf(f,"\n");
                       int count = node->getChildCount();
                       for(int i = 1; i <= count; i++)
                         dumpSyntaxTree(node->getChild(i),f,indent+1);
                       break;
                     }
    }
  }
}

void dumpSyntaxTree(SyntaxNode *root, char *fileName, int indent) {
  FILE *f = fopen(fileName,"w");
  if(f == nullptr) {
    fprintf(stdout,"Cannot open %s\n",fileName);
    dumpSyntaxTree(root,stdout,indent);
  }
  else {
    dumpSyntaxTree(root,f,indent);
    fclose(f);
  }
}

void ParserTree::dumpTree(char *fileName) {
  FILE *f = fopen(fileName,"w");
  if(f == nullptr) return;
  dumpSyntaxTree(m_root,f);
  fclose(f);
}

void ParserTree::verror(const SourcePosition &pos, const char *format, va_list argptr) {
  char tmp[1024],tmp2[1024];
  vsprintf(tmp,format,argptr);
  sprintf(tmp2,"Error in line %d: %s", pos.m_lineno, tmp );
  printf("%s",tmp2);
  m_ok = false;
}

