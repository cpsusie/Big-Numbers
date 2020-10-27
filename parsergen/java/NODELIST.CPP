#include "stdafx.h"
#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include <stdarg.h>
#include "ParserTree.h"

NodeList::NodeList(SyntaxNode *n, int token, bool binarytree) {
  findNodes(n,token,binarytree);
};

NodeList::NodeList() {
  findNodes(nullptr);
}

void NodeList::appendCommaList(SyntaxNode *n, int token) {
  if(n == nullptr)
    return;
  if(n->getToken() == token) {
    appendCommaList(n->getChild(1),token);
    add(n->getChild(2));
  }
  else
    add(n);
}

void NodeList::traverseTree(SyntaxNode *n, int token) {
  if(n == nullptr)
    return;
  if(n->getToken() != token) {
    add(n);
  } else {
    int count = n->getChildCount();
    for(int i = 1; i <= count; i++)
      traverseTree(n->getChild(i),token);
  }
}

void NodeList::findNodes(SyntaxNode *n, int token, bool binarytree) {
  clear();
  if(binarytree)
    traverseTree(n,token);
  else
    appendCommaList(n,token);
}
