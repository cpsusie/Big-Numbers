#pragma once

#include <grammar.h>
#include <Thread.h>

class TestParser;

class SyntaxNode {
private:
  String       m_symbol;
  unsigned int m_childCount;
  SyntaxNode **m_children;
  bool         m_terminal;
public:
  SyntaxNode(const TCHAR *symbol, unsigned int childCount, bool terminal, TestParser *parser);
  ~SyntaxNode();
  void setChild(unsigned int i, SyntaxNode *n) { m_children[i] = n;      }
  unsigned int getChildCount() const  { return m_childCount;    }
  SyntaxNode *getChild(int i)         { return m_children[i];   }
  const TCHAR *getSymbol() const       { return m_symbol.cstr(); }
  bool isTerminal() const             { return m_terminal;      }
};

typedef SyntaxNode *SyntaxNodep;

class ParserHandler {
public:
  virtual int  handleReduction(unsigned int prod) = 0;
  virtual void handleError(const SourcePosition &pos, const TCHAR *format, va_list argptr) = 0;
  virtual void handleDebug(const SourcePosition &pos, const TCHAR *format, va_list argptr) = 0;
};


class TestParser : public LRparser {
private:
  LexStringStream    m_inputStream;
  Scanner           *m_scanner;
  Grammar            m_grammar;
  ParserHandler     *m_handler;
  bool               m_ok;
  SyntaxNodep        m_dollardollar,*m_stacktop;
  SyntaxNodep       *m_userStack;
  Array<SyntaxNodep> m_nodeList;
  SyntaxNodep        m_root;
  StringArray        m_legalLookahead;
  StringArray        m_reduceActionStr;
  StringArray        m_stateStr;
  Thread            *m_initThread;
  void deleteNodeList();
  void buildLegalInputArray();
  void buildReduceActionArray();
  void waitForInitThread();
public:
  TestParser();
  ~TestParser();
  Grammar &getGrammar()                             { return m_grammar;                 }
  void setHandler(ParserHandler *handler)           { m_handler = handler;              }
  void setNewInput(const TCHAR *String);
  bool accept()                       const         { return m_ok;                      }
  String getLegalInput()              const         { return m_legalLookahead[state()]; }
  String getActionString()            const;
  const String &getStateItems(unsigned int state);
  SyntaxNodep getRoot()                             { return m_root;                    }
  void addSyntaxNode(SyntaxNodep p)                 { m_nodeList.add(p);                }
  void vdebug(const TCHAR *format, va_list argptr);
  void verror(const SourcePosition &pos, const TCHAR *format, va_list argptr);
  void setDebugScanner(bool newvalue)               { m_scanner->setDebug(newvalue);    }
  void setStackSize(unsigned int newsize);
  const SyntaxNodep getStackTop(int fromtop)  const { return m_stacktop[-fromtop];      }
  const SyntaxNodep *getUserStack()           const { return m_userStack;               }
  void buildStateArray();
  void userStackInit();
  int  reduceAction(unsigned int prod);
  void userStackShiftSymbol(unsigned int symbol);
  void userStackPopSymbols(unsigned int count)       { m_stacktop -= count;              } // pop count symbols from userstack
  void push(SyntaxNodep p)                           { *(++m_stacktop) = p;              } // push p onto userstack
  void userStackShiftDollarDollar()                  { push(m_dollardollar);             } // push($$) onto userstack
  void defaultReduce(unsigned int prod)              { m_dollardollar  = *m_stacktop;    } // $$ = $1
};
