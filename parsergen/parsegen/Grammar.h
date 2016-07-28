#pragma once

#include <HashMap.h>
#include <MarginFile.h>
#include <LRParser.h>
#include "StringCollector.h"

typedef enum {
  CPP
 ,JAVA
} Language;

typedef enum {
  LEFTASSOC_TERMINAL  // %left
 ,RIGHTASSOC_TERMINAL // %right
 ,NONASSOC_TERMINAL   // %nonassoc
 ,NONTERMINAL
 ,TERMINAL            // %term or %token
} SymbolType;

typedef enum {
  CHOOSE_SHIFT
 ,CHOOSE_REDUCE
 ,CONFLICT_NOT_RESOLVED
} ConflictSolution;

class GrammarSymbol {
public:
  short          m_index;
  String         m_name;
  SymbolType     m_type;
  bool           m_reachable     : 1; // True if symbol is reachable from startsymbol
  bool           m_terminate     : 1; // True if symbol can terminate
  bool           m_deriveEpsilon : 1; // True if symbol ->* epsilon
  BitSet         m_first1;            // Only for nonterminals;
  short          m_precedence;        // Only for terminals.
  SourcePosition m_pos;               // Specified at this position in the inputfile
  ShortArray     m_leftSideOf;        // Only for nonterminals. List of productions, where this symbol is leftside

  GrammarSymbol(int index, const String &name, SymbolType type, int precedence, const SourcePosition &pos, long terminalCount);
  const TCHAR *getTypeString() const;
};

class RightSideSymbol {
public:
  short          m_index;    // Symbol index
  SymbolModifier m_modifier; // ?, *, +
  RightSideSymbol(int index, SymbolModifier modifier) {
    m_index = index; m_modifier = modifier;
  }
  operator int() const { 
    return m_index;
  }
};

class Production {
public:
  short                   m_leftSide;
  Array<RightSideSymbol>  m_rightSide;
  SourceText              m_actionBody;
  short                   m_precedence; // %prec
  SourcePosition          m_pos;        // Position in the inputfile

  Production(int leftSide, const SourcePosition &pos) {
    m_leftSide   = leftSide;
    m_precedence = 0;
    m_pos        = pos;
  }

  int getLength() const {
    return (int)m_rightSide.size();
  }
};

class LR1Item {  // item = A -> alfa . beta {m_la}, then production[m_prod] = A -> alfa beta, and m_dot = length(alfa)
public:
  short  m_prod;
  short  m_dot;
  BitSet m_la;
  int    m_succ; // Must be signed, initialized to -1. if >= 0, then index of successor-state
  bool   m_kernelItem;
  LR1Item(bool kernelItem, short prod, short dot, const BitSet &la);
  int getSuccessor() const {
    return m_succ;
  }
  unsigned long coreHashCode() const;
};

class LR1State {
public:
  int            m_index;
  Array<LR1Item> m_items;
  short          m_noOfKernelItems;
  LR1State(int index);
  LR1Item *findItem(const LR1Item &item);
  void addItem(const LR1Item &item);
  void sortItems();
};

class ParserAction {
public:
  short m_token;
  short m_action; // <  0 reduce by production n. n == -m_action
                  // == 0 accept. reduce by production 0
                  // >  0 shift to state n. n == action

  inline ParserAction() : m_token(0), m_action(0) {
  }
  inline ParserAction(unsigned short token, short action) : m_token(token), m_action(action) {
  }
  inline unsigned long hashCode() const {
    return *(unsigned long*)(&m_token);
  }
  inline bool operator==(const ParserAction &a) const {
    return hashCode() == a.hashCode();
  }
  inline bool operator!=(const ParserAction &a) const {
    return !(*this == a);
  }
};

typedef CompactArray<ParserAction> ActionArray;

class StateResult {
public:
  StringArray m_errors;
  StringArray m_warnings;
  ActionArray m_actions;
  ActionArray m_succs;
};

typedef HashMap<const LR1State*, int> StateHashMap;

#define DUMP_LOOKAHEAD  0x01
#define DUMP_SUCC       0x02
#define DUMP_SHIFTITEMS 0x04
#define DUMP_ACTIONS    0x08
#define DUMP_KERNELONLY 0x10
#define DUMP_WARNINGS   0x20
#define DUMP_ERRORS     0x40

#define DUMP_ALL        DUMP_SHIFTITEMS | DUMP_LOOKAHEAD  | DUMP_SUCC    
#define DUMP_DOCFORMAT  DUMP_SHIFTITEMS | DUMP_ACTIONS    | DUMP_ERRORS | DUMP_WARNINGS

class Grammar {
private:
  int    addSymbol(const GrammarSymbol &symbol);
  bool   canTerminate( const Production &prod) const;
  bool   deriveEpsilon(const Production &prod) const;
  void   findEpsilonSymbols();        // Find all nonterminals that derive epsilon
  void   findFirst1Sets();            // Find FIRST1(A) for all nonterminals A
  void   computeSuccessors(   LR1State &state);
  void   computeClosure(      LR1State &state, bool allowNewItems);
  int    findState(     const LR1State &state) const;
  int    addState(      const LR1State &state);
  bool   mergeLookahead(      LR1State &dst, const LR1State &src);
  BitSet first1(        const LR1Item &item) const;
  bool   isShiftItem(   const LR1Item &item) const; // Is item = "A -> alfa . a beta [la]"
  bool   isReduceItem(  const LR1Item &item) const; // Is item = "A -> alfa . [la]"
  bool   isAcceptItem(  const LR1Item &item) const; // Is this item start -> alfa . EOI
  int    getShiftSymbol(const LR1Item &item) const; // Assume item = "A -> alfa . x beta [la]". Return x (terminal or nonterminal)
  const Production &getProduction(const LR1Item &item) const {
    return m_productions[item.m_prod];
  }
  void   checkStateIsConsistent(const LR1State &state, StateResult &result);
  ConflictSolution resolveShiftReduceConflict(const GrammarSymbol &terminal, const LR1Item &item) const;
  void   verbose(int level, const TCHAR *format, ...) const;

  Language               m_language;
  String                 m_name;
  Array<GrammarSymbol>   m_symbols;
  StringHashMap<int>     m_symbolMap;
  Array<Production>      m_productions;
  Array<LR1State>        m_states;
  StateHashMap           m_stateMap;
  IntHashSet             m_unfinishedSet;
  int                    m_verboseLevel;
  int                    m_terminalCount, m_startSymbol;

public:
  int                    m_warningCount;
  Array<StateResult>     m_result;
  int                    m_SRconflicts, m_RRconflicts; // only for doc-file
  SourceText             m_header, m_driverHead, m_driverTail; // programtext between %{ and %}

  Grammar(Language language, int verboselevel = 1);
  Grammar(Language language, const ParserTables &src);
  ~Grammar();

  bool isTerminal(int symbolIndex) const {
    return symbolIndex < m_terminalCount;
  }

  bool isNonTerminal(int symbolIndex) const {
    return symbolIndex >= m_terminalCount;
  }

  GrammarSymbol &getSymbol(int symbolIndex) {
    return m_symbols[symbolIndex];
  }
  
  const GrammarSymbol &getSymbol(int symbolIndex) const { 
    return m_symbols[symbolIndex];
  }

  const Production &getProduction(int index) const {
    return m_productions[index];
  }

  const LR1State &getState(int index) const {
    return m_states[index];
  }

  int  findSymbol(    const String &name) const;
  int  addTerminal(   const String &name, SymbolType type, int precedence, const SourcePosition &pos);
  int  addNonTerminal(const String &name, const SourcePosition &pos);
  void addProduction( const Production &production);
  void addClosureProductions();
  
  void findReachable();  // find all symbols that are reachable from startsymbol
  void findTerminate();  // find all symbols that can terminate
  void generateStates();

  const String &getName() const {
    return m_name;
  }

  void setName(const String &name) {
    m_name = name;
  }

  bool allStatesConsistent() const {
    return m_SRconflicts == 0 && m_RRconflicts == 0;
  }

  int getSymbolCount() const {
    return (int)m_symbols.size();
  }

  int getProductionCount() const {
    return (int)m_productions.size();
  }

  int getStateCount() const {
    return (int)m_states.size();
  }

  int getTerminalCount() const {
    return m_terminalCount;
  }

  int getNonTerminalCount() const {
    return getSymbolCount() - getTerminalCount();
  }

  int getStartSymbol() const {
    return m_startSymbol;
  }

  Language getLanguage() const { 
    return m_language;
  }

  const SourceText &getHeader() const {
    return m_header;
  }

  const SourceText &getDriverHead() const {
    return m_driverHead;
  }

  const SourceText &getDriverTail() const {
    return m_driverTail;
  }

  int getVerboseLevel() const {
    return m_verboseLevel;
  }
  
  int getItemCount() const;

  String symbolSetToString(const BitSet &set) const; // convert symbolset to String

  String itemToString(const LR1Item &item, int flags = DUMP_LOOKAHEAD) const;

  String stateToString(const LR1State &state, int flags = DUMP_ALL) const;

  String getRightSide(int prod)        const;
  String getProductionString(int prod) const;
  int    getMaxSymbolNameLength()      const;
  int    getMaxNonTerminalNameLength() const;

  void dump(                                                     MarginFile *f = tostdout) const;
  void dump(const Production &prod ,                             MarginFile *f = tostdout) const;
  void dump(const LR1Item    &item , int flags = DUMP_LOOKAHEAD, MarginFile *f = tostdout) const;
  void dump(const BitSet     &set  ,                             MarginFile *f = tostdout) const;
  void dump(const LR1State   &state, int flags = DUMP_ALL,       MarginFile *f = tostdout) const;
  void dumpStates(                   int flags = DUMP_DOCFORMAT, MarginFile *f = tostdout) const;
  void dumpFirst1Sets(FILE *f) const;
};
