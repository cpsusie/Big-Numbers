#pragma once

#include <MyUtil.h>
#include <HashMap.h>
#include <TreeMap.h>
#include <Scanner.h>
#include "ResourceSymbol.h"
#include "SyntaxNode.h"

typedef CompactArray<const SyntaxNode*> NodeArray;

class ParserTree {
private:
  SyntaxNode     *m_root;
  bool            m_ok;
  NodeArray       m_nodetable;
  StringArray     m_errors;
  String          m_absolutFileName;
  void addNodesToTable(const SyntaxNode *n);

protected:
  void releaseAll();
public:
  ParserTree(const String &fileName);
  ParserTree(const ParserTree &src);             // not defined
  ParserTree &operator=(const ParserTree &rhs);  // not defined
  virtual ~ParserTree();
  const SyntaxNode *getRoot() const {
    return m_root;
  }
  
  SyntaxNode *getRoot() {
    return m_root;
  }
  
  void setRoot(SyntaxNode *n) {
    m_root = n;
  }
  
  bool isOk() const {
    return m_ok;
  }

  static NodeArray getListFromTree(const SyntaxNode *n, int delimiterSymbol=COMMA);

  SyntaxNode *vFetchNode(const SourcePosition &pos, int symbol, va_list argptr);
  SyntaxNode *fetchNode( const SourcePosition &pos, int symbol, ...);
  void vAddError(const SourcePosition *pos, const TCHAR *format, va_list argptr);
  void addError(const SourcePosition &pos, const TCHAR *format,...);
  void addError(const TCHAR *format,...);
  const StringArray &getErrors() const { return m_errors; }
  void listErrors(FILE *f = stdout) const;
  void listErrors(tostream &out) const;
  void listErrors(const TCHAR *fname) const;
  static void dumpSyntaxNode(const SyntaxNode *node, FILE *f, int level, bool recurse);
  static void dumpSyntaxTree(const SyntaxNode *root, FILE *f = stdout);
  static void dumpSyntaxTree(const SyntaxNode *root, const TCHAR *fname);
  void dumpTree(      FILE *f = stdout) const;
  void dumpTree(      const TCHAR *fname) const;
  void dumpNodeTable( FILE *f = stdout) const;
  
};
