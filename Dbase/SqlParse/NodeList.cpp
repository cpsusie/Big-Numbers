#include "stdafx.h"
#include "sqlsymbol.h"
#include "SqlParser.h"

NodeList::NodeList(SyntaxNode *n, int token, bool binarytree) {
  findNodes(n,token,binarytree);
};

NodeList::NodeList() {
  findNodes(nullptr);
}

void NodeList::appendcommalist(SyntaxNode *n, int token) {
  if(n == nullptr) return;
  if(n->token() == token) {
    appendcommalist(n->child(0),token);
    add(n->child(1));
  } else {
    add(n);
  }
}

void NodeList::traversetree(SyntaxNode *n, int token) {
  if(n == nullptr) return;
  if(n->token() == token) {
    int count = n->childCount();
    for(int i = 0; i < count; i++)
      traversetree(n->child(i),token);
  } else {
    add(n);
  }
}

void NodeList::findNodes(SyntaxNode *n, int token, bool binarytree) {
  clear();
  if(binarytree) {
    traversetree(n,token);
  } else {
    appendcommalist(n,token);
  }
}

void NodeList::dump(FILE *f, int level) const {
  for(size_t i = 0; i < size(); i++) {
    dumpSyntaxTree((*this)[i],f,level);
  }
}

const SyntaxNode *NodeList::genlist(ParserTree &tree, const CompactIntArray &set) {
  const SyntaxNode *last = (*this)[set[0]];
  const size_t n = set.size();
  for(size_t i = 1; i < n; i++) {
    last = tree.fetchTokenNode(COMMA,last,(*this)[set[i]],nullptr);
  }
  return last;
}
