#pragma once

#include <Scanner.h>
#include <LRparser.h>
#include <Grammar.h>
#include <SafeRunnable.h>

class TestParser;

class SyntaxNode {
private:
  const String m_symbol;
  const UINT   m_childCount : 31;
  const bool   m_terminal   : 1;
  SyntaxNode **m_children;
public:
  SyntaxNode(const String &symbol, UINT childCount, bool terminal, TestParser *parser);
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
  virtual void handleError(const SourcePosition &pos, const TCHAR *format, va_list argptr) = 0;
  virtual void handleDebug(const SourcePosition &pos, const TCHAR *format, va_list argptr) = 0;
};

class YaccJob;

class TestParser : public LRparser {
private:
  LexStringStream           m_inputStream;
  Scanner                  *m_scanner;
  ParserHandler            *m_handler;
  bool                      m_makeDerivationTree;
  bool                      m_ok;
  UINT                      m_cycleCount;
  SyntaxNodep               m_leftSide,*m_stacktop;
  SyntaxNodep              *m_userStack;
  CompactArray<SyntaxNodep> m_nodeList;
  SyntaxNodep               m_root;
  StringArray               m_legalLookahead;
  StringArray               m_reduceActionStr;
  StringArray               m_stateStr;
  YaccJob                  *m_yaccJob;
  void deleteNodeList();
  void buildLegalTermStringArray();
  void buildReduceActionArray();
public:
  TestParser();
  ~TestParser() override;
  void          setHandler(ParserHandler *handler)  { m_handler = handler;                  }
  void          setNewInput(const TCHAR *String, bool makeDerivatonTree);
  inline bool   accept()                 const      { return m_ok && (getCycleCount() > 0); }
  inline UINT   getCycleCount()          const      { return m_cycleCount;                  }
  const String &getLegalInput()          const      { return m_legalLookahead[state()];     }
  String        getActionString()        const;
  String        getActionMatrixDump()    const;
  String        getSuccessorMatrixDump() const;
  const String &getStateItems(UINT state);
  Action        getNextAction()          const {
    return done() ? (accept() ? Action(PA_REDUCE,0) : Action()) : getParserTables().getAction(state(), input());
  }

  inline SyntaxNodep getRoot()                      { return m_root;                    }
  void addSyntaxNode(SyntaxNodep p)                 { m_nodeList.add(p);                }
  void vdebug(                           const TCHAR *format, va_list argptr) override;
  void verror(const SourcePosition &pos, const TCHAR *format, va_list argptr) override;
  void setDebugScanner(bool newvalue)               { m_scanner->setDebug(newvalue);    }
  void setStackSize(UINT newsize);
  const SyntaxNodep  getStackTop(UINT fromTop) const {
    assert(fromTop < getStackHeight());
    return *(m_stacktop - fromTop);
  }
  const SyntaxNodep *getUserStack()      const {
    return m_userStack;
  }
  void userStackInit()                   final;
  int  reduceAction(        UINT prod  ) final;
  void userStackShiftSymbol(UINT symbol) final;
  void userStackPopSymbols( UINT count ) final   { m_stacktop -= count;              } // pop count symbols from userstack
  void push(SyntaxNodep p)                       { *(++m_stacktop) = p;              } // push p onto userstack
  void userStackShiftLeftSide()          final   { push(m_leftSide);                 } // push($$) onto userstack
  void defaultReduce(       UINT prod  ) final   { m_leftSide = *m_stacktop;         } // $$ = $1
};
