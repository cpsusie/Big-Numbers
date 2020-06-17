#pragma once

#include <grammar.h>
#include <SafeRunnable.h>

class TestParser;

class SyntaxNode {
private:
  String       m_symbol;
  UINT         m_childCount;
  SyntaxNode **m_children;
  bool         m_terminal;
public:
  SyntaxNode(const TCHAR *symbol, UINT childCount, bool terminal, TestParser *parser);
  ~SyntaxNode();
  void         setChild(UINT i, SyntaxNode *n) { m_children[i] = n;      }
  UINT         getChildCount() const           { return m_childCount;    }
  SyntaxNode  *getChild(UINT i)                { return m_children[i];   }
  const TCHAR *getSymbol() const               { return m_symbol.cstr(); }
  bool         isTerminal() const              { return m_terminal;      }
};

typedef SyntaxNode *SyntaxNodep;

class ParserHandler {
public:
  virtual int  handleReduction(UINT prod) = 0;
  virtual void handleError(const SourcePosition &pos, const TCHAR *format, va_list argptr) = 0;
  virtual void handleDebug(const SourcePosition &pos, const TCHAR *format, va_list argptr) = 0;
};

class TestParser : public LRparser {
private:
  LexStringStream           m_inputStream;
  Scanner                  *m_scanner;
  Grammar                   m_grammar;
  ParserHandler            *m_handler;
  bool                      m_ok;
  UINT                      m_cycleCount;
  SyntaxNodep               m_leftSide,*m_stacktop;
  SyntaxNodep              *m_userStack;
  CompactArray<SyntaxNodep> m_nodeList;
  SyntaxNodep               m_root;
  StringArray               m_legalLookahead;
  StringArray               m_reduceActionStr;
  StringArray               m_stateStr;
  SafeRunnable             *m_yaccJob;
  void deleteNodeList();
  void buildLegalInputArray();
  void buildReduceActionArray();
  void waitForYaccJob();
public:
  TestParser();
  ~TestParser();
  Grammar      &getGrammar()                        { return m_grammar;                 }
  void          setHandler(ParserHandler *handler)  { m_handler = handler;              }
  void          setNewInput(const TCHAR *String);
  inline bool   accept()              const         { return m_ok;                      }
  inline UINT   getCycleCount()       const         { return m_cycleCount;              }
  const String &getLegalInput()       const         { return m_legalLookahead[state()]; }
  String        getActionString()     const;
  const String &getStateItems(UINT state);
  inline SyntaxNodep getRoot()                      { return m_root;                    }
  void addSyntaxNode(SyntaxNodep p)                 { m_nodeList.add(p);                }
  void vdebug(                           const TCHAR *format, va_list argptr);
  void verror(const SourcePosition &pos, const TCHAR *format, va_list argptr);
  void setDebugScanner(bool newvalue)               { m_scanner->setDebug(newvalue);    }
  void setStackSize(UINT newsize);
  const SyntaxNodep  getStackTop(int fromtop) const { return m_stacktop[-fromtop];      }
  const SyntaxNodep *getUserStack()           const { return m_userStack;               }
  void buildStateArray();
  void userStackInit();
  int  reduceAction(        UINT prod  );
  void userStackShiftSymbol(UINT symbol);
  void userStackPopSymbols( UINT count )             { m_stacktop -= count;             } // pop count symbols from userstack
  void push(SyntaxNodep p)                           { *(++m_stacktop) = p;             } // push p onto userstack
  void userStackShiftLeftSide()                      { push(m_leftSide);                } // push($$) onto userstack
  void defaultReduce(UINT prod)                      { m_leftSide = *m_stacktop;        } // $$ = $1
};
