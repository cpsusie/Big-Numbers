#pragma once

#include <HashMap.h>
#include <CompactHashMap.h>
#include <MarginFile.h>
#include <AbstractParserTables.h>
#include "ByteCount.h"
#include "SourceText.h"

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

typedef BitSet    SymbolSet;
typedef SymbolSet TermSet;    // capacity always #terminals
typedef BitSet    StateSet;   // capacity always #states
typedef BitSet    NTindexSet; // capacity always #non-terminals

class GrammarSymbol {
public:
  const UINT       m_index;
  const String     m_name;
  const SymbolType m_type;
  bool             m_reachable     : 1; // True if symbol is reachable from startsymbol
  bool             m_terminate     : 1; // True if symbol can terminate
  bool             m_deriveEpsilon : 1; // True if symbol ->* epsilon
  TermSet          m_first1;            // Only for nonterminals;
  int              m_precedence;        // Only for terminals.
  SourcePosition   m_pos;               // Specified at this position in the inputfile
  CompactUIntArray m_leftSideOf;        // Only for nonterminals. List of productions, where this symbol is leftside

  GrammarSymbol(UINT index, const String &name, SymbolType type, int precedence, const SourcePosition &pos, UINT terminalCount);
  const TCHAR *getTypeString() const;
};

class RightSideSymbol {
public:
  UINT           m_index;    // Symbol index
  SymbolModifier m_modifier; // ?, *, +
  inline RightSideSymbol() {
  }
  inline RightSideSymbol(UINT index, SymbolModifier modifier)
    : m_index(   index   )
    , m_modifier(modifier)
  {
  }
  inline operator UINT() const {
    return m_index;
  }
};

class Production {
public:
  const UINT                    m_leftSide;
  CompactArray<RightSideSymbol> m_rightSide;
  SourceText                    m_actionBody;
  short                         m_precedence; // %prec
  const SourcePosition          m_pos;        // Position in the inputfile

  Production(UINT leftSide, const SourcePosition &pos)
    : m_leftSide(leftSide)
    , m_pos(     pos     )
  {
    m_precedence = 0;
  }

  inline UINT getLength() const {
    return (UINT)m_rightSide.size();
  }
};

class LR1Item {  // item = A -> alfa . beta {m_la}, then production[m_prod] = A -> alfa beta, and m_dot = length(alfa)
public:
  // core(item) is defined as [m_prod, m_dot], ignoring m_la
  const bool m_kernelItem;
  const UINT m_prod;
  const UINT m_dot;
  TermSet    m_la;
  int        m_succ; // Must be signed, initialized to -1. if >= 0, then index of successor-state
  inline LR1Item(bool kernelItem, UINT prod, UINT dot, const TermSet &la)
    : m_kernelItem(kernelItem)
    , m_prod(      prod      )
    , m_dot(       dot       )
    , m_la(        la        )
    , m_succ(      -1        )
  {
  }
  inline int getSuccessor() const {
    return m_succ;
  }
  inline ULONG coreHashCode() const {
    return (m_prod << 16) ^ m_dot; //    m_la.hashval(); DONT USE THIS HERE !!!!
  }
};

class LR1State {
public:
  const UINT     m_index;
  BYTE           m_kernelItemCount;
  Array<LR1Item> m_items;
  inline LR1State(UINT index)
    : m_index(          index)
    , m_kernelItemCount(0    )
   {
   }

  LR1Item *findItemWithSameCore(const LR1Item &item);
  inline void addItem(const LR1Item &item) {
    m_items.add(item);
    if(item.m_kernelItem) {
      m_kernelItemCount++;
    }
  }
  void sortItems();
};

class ParserAction {
public:
  USHORT m_term;
  SHORT  m_action; // <  0 reduce by production n. n == -m_action
                   // == 0 accept. reduce by production 0
                   // >  0 shift to state n. n == action

  inline ParserAction() : m_term(0), m_action(0) {
  }
  inline ParserAction(USHORT term, SHORT action) : m_term(term), m_action(action) {
  }
  inline ULONG hashCode() const {
    return *(ULONG*)(&m_term);
  }
  inline bool operator==(const ParserAction &a) const {
    return hashCode() == a.hashCode();
  }
  inline bool operator!=(const ParserAction &a) const {
    return !(*this == a);
  }
  String toString(const SymbolNameContainer &nameContainer) const;
};

typedef CompactShortArray ActionArray;

class SuccessorState {
public:
  USHORT m_nterm;
  USHORT m_newState;

  inline SuccessorState() : m_nterm(0), m_newState(0) {
  }
  inline SuccessorState(USHORT nterm, USHORT newState) : m_nterm(nterm), m_newState(newState) {
  }
  inline ULONG hashCode() const {
    return *(ULONG*)(&m_nterm);
  }
  inline bool operator==(const SuccessorState &a) const {
    return hashCode() == a.hashCode();
  }
  inline bool operator!=(const SuccessorState &a) const {
    return !(*this == a);
  }
  String toString(const SymbolNameContainer &nameContainer) const;
};

typedef CompactUShortArray StateArray;

inline int compactShortArrayCmp(const CompactShortArray &a1, const CompactShortArray &a2) {
  const size_t n = a1.size();
  int          c = sizetHashCmp(n, a2.size());
  if(c) return c;
  return n ? memcmp(a1.begin(), a2.begin(), n * sizeof(short)) : 0;
}

inline int actionArrayCmp(const ActionArray &a1, const ActionArray &a2) {
  return compactShortArrayCmp((const CompactShortArray&)a1, (const CompactShortArray&)a2);
}

inline int stateArrayCmp(const StateArray &a1, const StateArray &a2) {
  return compactShortArrayCmp((const CompactShortArray&)a1, (const CompactShortArray&)a2);
}

inline int parserActionCompareTerm(const ParserAction &p1, const ParserAction &p2) {
  return (int)p1.m_term - (int)p2.m_term;
}

inline int stringCmp(const String &s1, const String &s2) {
  return _tcscmp(s1.cstr(), s2.cstr());
}

class ParserActionArray : public CompactArray<ParserAction> {
public:
  ParserActionArray() {
  }
  ParserActionArray(UINT capacity) : CompactArray(capacity) {
  }
  inline ParserActionArray &sortByTerm() {
    sort(parserActionCompareTerm);
    return *this;
  }
  // Return all legal terminals in array as a BitSet
  TermSet        getLegalTermSet(UINT terminalCount) const;
  // Return all actions (no neccessary distinct) actions in array
  ActionArray    getActionArray()                    const;
  // = size() = number of different terminal symbols in array
  inline UINT    getLegalTermCount()                 const {
    return(UINT)size();
  }
  String         toString(const SymbolNameContainer &nameContainer) const;
};

inline int successorStateCompareNTerm(const SuccessorState &s1, const SuccessorState &s2) {
  return (int)s1.m_nterm - (int)s2.m_nterm;
}

class SuccessorStateArray : public CompactArray<SuccessorState> {
public:
  SuccessorStateArray() {
  }
  SuccessorStateArray(UINT capacity) : CompactArray(capacity) {
  }
  inline SuccessorStateArray &sortByNTerm() {
    sort(successorStateCompareNTerm);
    return *this;
  }
  NTindexSet     getNTindexSet(UINT termCount, UINT symbolCount) const;
  StateArray     getStateArray()                     const;
  inline UINT    getNewStateCount()                  const {
    return(UINT)size();
  }
  String         toString(const SymbolNameContainer &nameContainer) const;
};

class StateResult {
public:
  StringArray m_errors, m_warnings;
  ParserActionArray   m_actions;
  SuccessorStateArray m_succs;
};

class StateHashMap : public HashMap<const LR1State *, UINT> {
public:
  StateHashMap(size_t capacity);
};

#define DUMP_LOOKAHEAD  0x01
#define DUMP_SUCC       0x02
#define DUMP_SHIFTITEMS 0x04
#define DUMP_ACTIONS    0x08
#define DUMP_KERNELONLY 0x10
#define DUMP_WARNINGS   0x20
#define DUMP_ERRORS     0x40

#define DUMP_ALL        DUMP_SHIFTITEMS | DUMP_LOOKAHEAD  | DUMP_SUCC
#define DUMP_DOCFORMAT  DUMP_SHIFTITEMS | DUMP_ACTIONS    | DUMP_ERRORS | DUMP_WARNINGS

class Grammar : public SymbolNameContainer {
private:
  String                       m_name;
  Array<GrammarSymbol>         m_symbols;
  CompactStrHashMap<UINT,300>  m_symbolMap;
  Array<Production>            m_productions;
  Array<LR1State>              m_states;
  StateHashMap                 m_stateMap; // map core(state) -> index (UINT) into m_states
  CompactUIntHashSet<1000>     m_unfinishedSet;
  UINT                         m_termCount, m_startSymbol;


  UINT    addSymbol(            const GrammarSymbol &symbol);
  bool    canTerminate(         const Production    &prod  ) const;
  bool    deriveEpsilon(        const Production    &prod  ) const;
  // Find all nonterminals that derive epsilon
  void    findEpsilonSymbols();
  // Find FIRST1(A) for all nonterminals A
  void    findFirst1Sets();
  void    computeSuccessors(          LR1State      &state );
  void    computeClosure(             LR1State      &state, bool allowNewItems);
  int     findStateWithSameCore(const LR1State      &state ) const;
  // Return index of state
  UINT    addState(             const LR1State      &state );
  bool    mergeLookahead(             LR1State      &dst, const LR1State &src);
  TermSet first1(          const LR1Item       &item  ) const;

  // Is item = "A -> alfa . a beta [la]"
  bool        isShiftItem(     const LR1Item       &item) const;

  // Is item = "A -> alfa . [la]"
  inline bool isReduceItem(    const LR1Item       &item) const {
    return item.m_dot == m_productions[item.m_prod].getLength();
  }

  // Is item start -> alfa . EOI
  inline bool isAcceptItem(    const LR1Item       &item) const {
    return (item.m_prod == 0) && isReduceItem(item);
  }

  // Assume item = "A -> alfa . x beta [la]". Return x (terminal or nonterminal)
  inline UINT getShiftSymbol(  const LR1Item       &item) const {
    return m_productions[item.m_prod].m_rightSide[item.m_dot].m_index;
  }
  void             checkStateIsConsistent(const LR1State &state, StateResult &result);
  ConflictSolution resolveShiftReduceConflict(const GrammarSymbol &terminal, const LR1Item &item) const;

public:
  UINT                   m_warningCount;
  Array<StateResult>     m_result;
  UINT                   m_SRconflicts, m_RRconflicts;         // only for doc-file
  SourceText             m_header, m_driverHead, m_driverTail; // programtext between %{ and %}

  Grammar();
  Grammar(const AbstractParserTables &src);
  ~Grammar() {
  }

  inline UINT                 getSymbolCount()                const final { return (UINT)m_symbols.size();                }
  inline UINT                 getTermCount()                  const final { return m_termCount;                           }
  const  String              &getSymbolName(UINT symbolIndex) const final {
    return getSymbol(symbolIndex).m_name;
  }
  inline UINT                 getProductionCount()            const       { return (UINT)m_productions.size();            }
  inline UINT                 getStateCount()                 const       { return (UINT)m_states.size();                 }
  inline bool                 isTerminal(   UINT symbolIndex) const       { return symbolIndex < m_termCount;             }
  inline bool                 isNonTerminal(UINT symbolIndex) const       { return symbolIndex >= m_termCount;            }
  inline       GrammarSymbol &getSymbol(    UINT symbolIndex)             { return m_symbols[symbolIndex];                }
  inline const GrammarSymbol &getSymbol(    UINT symbolIndex) const       { return m_symbols[symbolIndex];                }
  inline const Production    &getProduction(UINT index      ) const       { return m_productions[index];                  }
  inline const LR1State      &getState(     UINT index      ) const       { return m_states[index];                       }

  // Return index of symbol (terminal or non-terminal) with specified name if it exist
  // or -1, if not found
  int  getSymbolIndex(    const String &name) const;
  // Add terminal symbol to symboltable
  // Assume that no symbol with specified name alrady exist in symboltable
  // Return index, which can be found by getSymbolIndex
  UINT addTerm(       const String &name, SymbolType type, int precedence, const SourcePosition &pos);
  // Add non-terminal symbol to symboltable
  // Assume that no symbol with specified name alrady exist in symboltable
  // Return index, which can be found by getSymbolIndex
  UINT addNTerm(      const String &name, const SourcePosition &pos);
  void addProduction( const Production &production);
  void addClosureProductions();

  // find all symbols that are reachable from startsymbol
  void findReachable();
  // find all symbols that can terminate
  void findTerminate();
  void generateStates();

  const String &getName() const             { return m_name; }
  inline void   setName(const String &name) { m_name = name; }

  inline bool allStatesConsistent() const {
    return (m_SRconflicts == 0) && (m_RRconflicts == 0);
  }

  inline UINT        getStartSymbol()  const { return m_startSymbol;  }
  const  SourceText &getHeader()       const { return m_header;       }
  const  SourceText &getDriverHead()   const { return m_driverHead;   }
  const  SourceText &getDriverTail()   const { return m_driverTail;   }

  // Convert symbolset to String
  String itemToString(     const LR1Item   &item , int flags = DUMP_LOOKAHEAD) const; // flags any combination of {DUMP_LOOKAHEAD,DUMP_SUCC}
  String stateToString(    const LR1State  &state, int flags = DUMP_ALL      ) const; // flags any combination of {DUMP_KERNELONLY,DUMP_SHIFTITEMS,DUMP_ACTIONS} + {flags for itemToString}

  String getRightSide(       UINT prod) const;
  String getProductionString(UINT prod) const;
  UINT   getItemCount()                 const; // return total number of generated LALR(1) items

  void   dump(             const SymbolSet  &set  ,                             MarginFile *f = tostdout) const;
  void   dump(             const Production &prod ,                             MarginFile *f = tostdout) const;
  void   dump(             const LR1Item    &item , int flags = DUMP_LOOKAHEAD, MarginFile *f = tostdout) const; // flags as for itemToString
  void   dump(             const LR1State   &state, int flags = DUMP_ALL,       MarginFile *f = tostdout) const; // flags any combination of {DUMP_ERRORS,DUMP_WARNINGS} + {flags for stateToString}
  void   dumpStates(                                int flags = DUMP_DOCFORMAT, MarginFile *f = tostdout) const; // flags as for dump(const LR1State &state...)
  void   dump(                                                                  MarginFile *f = tostdout) const;
  void   dumpFirst1Sets(FILE *f) const;
};
