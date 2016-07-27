#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include <stdarg.h>
#include <MyUtil.h>
#include "Java5parser.h"

char *SyntaxNode::getName() {
  if(m_token != IDENTIFIER)
    throwException( "SyntaxNode not type IDENTIFIER. token = %d",m_token);
  return u.m_str;
}

char *SyntaxNode::getString() {
  if(m_token != STRINGLITERAL)
    throwException( "SyntaxNode not type STRING. token = %d",m_token);
  return u.m_str;
}

double SyntaxNode::getNumber() {
  if(m_token != NUMBER)
    throwException( "SyntaxNode not type NUMBER. token = %d",m_token);
  return u.m_number;
}

static void throwNoSonError(const SyntaxNode *n, int i) {
  throwException("No son(%d) in SyntaxNode token %s",i,tokenName(n->getToken()));
}

SyntaxNode *SyntaxNode::getChild(int i) {
  if(i == 0) throwNoSonError(this,i);

  switch(getToken()) {
  case IDENTIFIER      : 
  case STRINGLITERAL : 
  case NUMBER    : 
    throwNoSonError(this,i);
    break;
  }
  return u.m_children.m_child[i-1];
}

int SyntaxNode::getChildCount() const {
  switch(getToken()) {
  case IDENTIFIER      : 
  case STRINGLITERAL : 
  case NUMBER    : 
    return 0;
  }
  return u.m_children.m_count;
}

SyntaxNode::~SyntaxNode() {
  switch(m_token) {
  case STRINGLITERAL:
  case IDENTIFIER     :
    free(u.m_str);
    break;
  case NUMBER   :
    break;
  default:
    delete[] u.m_children.m_child;
    break;
  }
}

SyntaxNode::SyntaxNode(int token, va_list argptr) {
  SyntaxNode *tmp[100]; // we expect no more than 100 parameters !!
  m_token = token;
  int n = 0;
  for(SyntaxNode *help = va_arg(argptr,SyntaxNode *); help; help = va_arg(argptr,SyntaxNode *))
    tmp[n++] = help;
  SyntaxNode **children = n ? new SyntaxNode*[n] : NULL;
  for(int i = 0; i < n; i++)
    children[i] = tmp[i];
  u.m_children.m_count = n;
  u.m_children.m_child = children;
}

SyntaxNode::SyntaxNode(const char *str, int token) {
  m_token = token;
  u.m_str = strdup(str);
}

SyntaxNode::SyntaxNode(double number) {
  m_token    = NUMBER;
  u.m_number = number;
}

void SyntaxNode::setPosition(const SourcePosition &pos) {
  m_pos = pos;
}
