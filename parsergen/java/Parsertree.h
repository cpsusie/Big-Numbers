#pragma once

#include <Array.h>
#include <MyString.h>
#include <String.h>
#include <Scanner.h>
#include "Java5Symbol.h"

class SyntaxNode {
private:
  int            m_symbol;
  SourcePosition m_pos;
public:
  SyntaxNode(int symbol, const SourcePosition &pos);
  virtual ~SyntaxNode();

  int getSymbol() const {
    return m_symbol;
  }

  SourcePosition getPosition() const {
    return m_pos;
  }

  void setPosition(const SourcePosition &pos);

  virtual const String &getName() const;
  virtual const String &getString() const;
  virtual double        getNumber() const;
  virtual SyntaxNode   *getChild(int i);
  virtual int           getChildCount() const;

  bool isThisName(const String &name) {
    return getSymbol() == IDENTIFIER && getName() == name;
  }
};

class SyntaxNodeName : public SyntaxNode {
private:
  String m_name;
public:
  SyntaxNodeName(const SourcePosition &pos, const String &name);
  const String &getName() const {
    return m_name;
  }
};

class SyntaxNodeString : public SyntaxNode {
private:
  String m_string;
public:
  SyntaxNodeString(const SourcePosition &pos, const String &string);
  const String &getString() const {
    return m_string;
  }
};

class SyntaxNodeTree : public SyntaxNode {
private:
  int         m_childCount;
  SyntaxNode *m_children;
public:
  SyntaxNodeTree(int symbol, const SourcePosition &pos, ...);
  SyntaxNodeTree(int symbol, const SourcePosition &pos, va_list argptr);

  int getChildCount() const {
    return m_childCount;
  }
  SyntaxNode *getChild(int index);
};

typedef Array<SyntaxNode*> SyntaxNodeArray;

class ParserTree {
private:
  bool            m_ok;
  SyntaxNodeArray m_nodeArray;
  Array<String>   m_errmsg;
  SyntaxNode     *m_root;
public:
  ParserTree();
  virtual ~ParserTree();
  void parse(const char *fileName);
  void verror(const SourcePosition &pos, const char *format, va_list argptr);
  SyntaxNode *fetchNameNode(  const SourcePosition &pos, const String &name);
  SyntaxNode *fetchStringNode(const SourcePosition &pos, const String &string);
  SyntaxNode *fetchNumberNode(const SourcePosition &pos, double number);
  SyntaxNode *fetchTreeNode( int symbol, const SourcePosition &pos, ...);
  SyntaxNode *vFetchTreeNode(int symbol, const SourcePosition &pos, va_list argptr);
  void appendError( const char *format,...);
  void vAppendError(const char *format, va_list argptr);
  void listErrors(FILE *f = stdout);
  void listErrors(char *fileName);
  bool ok() const {
    return m_ok;
  }
  void dumpTree(char *fileName);
  String nodeToString(SyntaxNode *n);
};


class NodeList : public SyntaxNodeArray {
  void appendCommaList(SyntaxNode *n, int delimiter);
  void traverseTree(SyntaxNode *n, int delimiter);
public:
  NodeList();
  NodeList(SyntaxNode *n, int delimiter = COMMA, bool binarytree = false);
  void findNodes(SyntaxNode *n, int delimiter = COMMA, bool binarytree = false);
};

void dumpSyntaxTree(SyntaxNode *n, char *fileName  , int indent = 0);
void dumpSyntaxTree(SyntaxNode *n, FILE *f = stdout, int indent = 0);

