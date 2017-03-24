#pragma once

#include <Date.h>
#include <BitSet.h>
#include <HashMap.h>
#include <LrParser.h>
#include <Scanner.h>
#include "SqlSymbol.h"
#include "SqlLex.h"

const extern ParserTables *SqlTables;

class SyntaxNode {
private:
  String       m_symbol;
  UINT         m_childCount;
  SyntaxNode **m_children;
  bool         m_terminal;
  int          m_token;
public:
  SyntaxNode(int token, const TCHAR *symbol, UINT sons, bool terminal);
  ~SyntaxNode();
  int          getToken()   const { return m_token;         }
  const TCHAR *getSymbol()  const { return m_symbol.cstr(); }
  bool         isTerminal() const { return m_terminal;      }
  UINT         childCount() const { return m_childCount;    }
  SyntaxNode  *getChild(UINT i);
  void         setChild(UINT i, SyntaxNode *v);
};

typedef SyntaxNode *SyntaxNodep;

void dumpTree(SyntaxNodep n, FILE *f = stdout, int level = 0);

class TokenPair {
public:
  short m_lastToken;
  short m_thisToken;
  TokenPair(short lastToken, short thisToken) { m_lastToken = lastToken; m_thisToken = thisToken; }
  ULONG hashCode() const { return (m_lastToken << 16) | m_thisToken; }
};

ULONG TokenPairHash(const TokenPair &tp);
int TokenPairCmp(const TokenPair &key, const TokenPair &tablekey);

template <class E> class TokenPairHashMap  : public HashMap<TokenPair,E> {
public:
  TokenPairHashMap(ULONG size) : HashMap<TokenPair,E>( TokenPairHash,TokenPairCmp,size) {}
};

typedef enum {
  INDENT_ALWAYS
 ,INDENT_WHENINSIDE
 ,INDENT_WHENNOTINSIDE
 ,INDENT_COUNT
} IndentType;

class IndentSize {
public:
  IndentType m_type;
  short      m_insideToken;
  int        m_size;
  int        m_lmarg;
  IndentSize(IndentType type, short insidetoken, int size, int lmarg = 0);
};

class InsideMark {
public:
  bool m_isinside;
  int  m_counter;
  int  m_countmax;
  InsideMark(int countmax=0) { m_countmax = countmax; setInside(false); }
  void setInside(bool v)   { m_isinside = v; m_counter = 0; }
};

class SqlParser : public LRparser {
private:
  BitSet                      m_noSpaceSet;
  TokenPairHashMap<short>     m_pairNoSpace;
  ShortHashMap<IndentSize>    m_indentSet;
  ShortHashMap<InsideMark>    m_inside;
  int                         m_indent;
  int                         m_lastToken;
  SyntaxNodep                 m_root;
  void insertPairNoSpace( short lasttoken, short thistoken) { m_pairNoSpace.put(TokenPair(lasttoken,thistoken),1); }
  bool memberPairNoSpace( short lasttoken, short thistoken) { return m_pairNoSpace.get(TokenPair(lasttoken,thistoken)) != NULL; }
  void insertIndentAlways(short token, int size, int lmarg = 0);
  void insertIndentInside(short token, bool inside , short insidetoken, int size);
  void insertIndentCount( short token, int countmax, short insidetoken, int size);
  SyntaxNodep m_dollardollar,*m_stacktop,m_userstack[256];
  SyntaxNodep stacktop(int fromtop)  { return m_stacktop[-fromtop];      }
  String nl(int indent) const;
  String sprintTerm(SyntaxNodep n);
  String sprintTree(SyntaxNodep n);
protected:
  int  reduceAction(UINT prod);
  void userStackInit()                    { m_stacktop = m_userstack; delete m_root; m_root = NULL; }
  void userStackShiftSymbol(UINT symbol);
  void push(SyntaxNodep p)                { *(++m_stacktop) = p;              }
  void userStackPopSymbols(UINT count)    { m_stacktop -= count;              } // pop count symbols from userstack
  void userStackShiftDollarDollar()       { push(m_dollardollar);             } // push($$) on userstack
  void defaultReduce(UINT prod)           { m_dollardollar  = *m_stacktop;    } // $$ = $1
public:
  SqlParser();
  ~SqlParser();
  String      m_filename;
  StringArray m_errmsg;
  void   verror(const SourcePosition &pos, const TCHAR *format, va_list argptr);
  bool   ok() const { return m_errmsg.size() == 0; }
  void   listErrors();
  void   dumpTree(FILE *f = stdout);
  String beautify();
  void parse(const TCHAR *filename, int lineno, const TCHAR *stmt);
};
