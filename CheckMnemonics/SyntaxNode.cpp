#include "stdafx.h"
#include <String.h>
#include <stdarg.h>
#include "ResourceParser.h"
#include "SyntaxNode.h"

SyntaxNode::SyntaxNode(const SourcePosition &pos, int symbol) {
  m_pos    = pos;
  m_symbol = symbol;
}

SyntaxNode::SyntaxNode(int symbol) {
  m_symbol = symbol;
}

Exception SyntaxNode::createException(const String &attribute) const {
  String msg = format(_T("Cannot get %s of syntaxNode with symbol=%s ,pos:(%d,%d)"),attribute.cstr(),getSymbolName().cstr(), m_pos.getLineNumber(), m_pos.getColumn());
  return Exception(msg.cstr());
}

String SyntaxNode::getSymbolName() const {
  const ParserTables &tables = ResourceParser::getTables();
  if(m_symbol < 0 || m_symbol >= (int)tables.getSymbolCount()) {
    return format(_T("Unknown symbol (=%d)"),m_symbol);
  }
  return tables.getSymbolName(m_symbol);
}

class NameChecker : public SyntaxNodeHandler {
private:
  const String &m_name;
  bool          m_nameFound;

public:
  NameChecker(const String &name) : m_name(name) {
    m_nameFound = false;
  }

  void handleNode(const SyntaxNode *n) {
    if(n->isName() && n->getName() == m_name) {
      m_nameFound = true;
    }
  }

  bool isFound() const {
    return m_nameFound;
  }
};

class NodeCounter : public SyntaxNodeHandler {
private:
  int m_count;

public:
  NodeCounter() {
    m_count = 0;
  }

  void handleNode(const SyntaxNode *n) {
    m_count++;
  }

  int getCount() const {
    return m_count;
  }
};

int SyntaxNode::getNodeCount() const {
  NodeCounter nodeCounter;
//  traverseExpression(nodeCounter);
  return nodeCounter.getCount();
}

ResourceNodeTree::ResourceNodeTree(const SourcePosition &pos, int symbol, va_list argptr) : SyntaxNode(pos,symbol) {
  va_list tmp = argptr;
  int count = 0;
  SyntaxNode *p = va_arg(tmp,SyntaxNode*);
  for(; p; p = va_arg(tmp,SyntaxNode*)) {
    count++;
  }
  m_childCount = count;
  m_child = new SyntaxNode*[count]; TRACE_NEW(m_child);
  for(count = 0,tmp = argptr, p = va_arg(tmp,SyntaxNode*); p; p = va_arg(tmp,SyntaxNode*)) {
    m_child[count++] = p;
  }
}

ResourceNodeTree::ResourceNodeTree(const ResourceNodeTree *src) : SyntaxNode(src->getPos(), src->getSymbol()) {
  m_childCount = src->getChildCount();
  m_child = new SyntaxNode*[m_childCount]; TRACE_NEW(m_child);
  for(int i = 0; i < m_childCount; i++) {
    m_child[i] = src->getChild(i)->clone();
  }
}

ResourceNodeTree::~ResourceNodeTree() {
  SAFEDELETEARRAY(m_child);
}

SyntaxNode *ResourceNodeTree::clone() const {
  SyntaxNode *copy = new ResourceNodeTree(this); TRACE_NEW(copy);
  return copy;
}

SyntaxNode *ResourceNodeTree::getChild(UINT i) const {
  if(i >= m_childCount) {
    ParserTree::dumpSyntaxTree(this);
    throwException(_T("Cannot get child %u from treenode. ChildCount=%d"), i, m_childCount);
  }
  return m_child[i];
}

String ResourceNodeTree::toString() const {
  return _T("ResourceNodeTree::toString() not implemented");
}
