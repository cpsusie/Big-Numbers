#pragma once

#include <HashMap.h>
#include <MarginFile.h>
#include "ByteCount.h"
#include "SourceText.h"
#include "BitSetParameters.h"
#include "BitSetInterval.h"

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
  UINT             m_index;
  const String     m_name;
  const SymbolType m_type;
  bool             m_reachable     : 1; // True if symbol is reachable from startsymbol
  bool             m_terminate     : 1; // True if symbol can terminate
  bool             m_deriveEpsilon : 1; // True if symbol ->* epsilon
  TermSet          m_first1;            // Only for nonterminals;
  int              m_precedence;        // Only for terminals.
  SourcePosition   m_pos;               // Specified at this position in the inputfile
  CompactUIntArray m_leftSideOf;        // Only for nonterminals. List of productions, where this symbol is leftside

  GrammarSymbol(UINT index, const String &name, SymbolType type, int precedence, const SourcePosition &pos, UINT termCount);
  inline const String &getName() const {
    return m_name;
  }
  inline SymbolType getType() const {
    return m_type;
  }
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
  static const TCHAR *getModifierString(SymbolModifier modifier);
  inline const TCHAR *getModifierString() const {
    return getModifierString(m_modifier);
  }
  String toString(const AbstractSymbolNameContainer &nameContainer) const;
};

class Grammar;

class Production {
private:
  const Grammar                &m_grammar;
public:
  const UINT                    m_leftSide;
  CompactArray<RightSideSymbol> m_rightSide;
  SourceText                    m_actionBody;
  short                         m_precedence; // %prec
  const SourcePosition          m_pos;        // Position in the inputfile

  Production(const Grammar *grammar, UINT leftSide, const SourcePosition &pos)
    : m_grammar(*grammar )
    , m_leftSide(leftSide)
    , m_pos(     pos     )
  {
    m_precedence = 0;
  }
  Production(const Grammar *grammar, const Production &prod);

  inline UINT getLength()          const {
    return (UINT)m_rightSide.size();
  }
  bool        canTerminate()       const;
  bool        deriveEpsilon()      const;

  String      getRightSideString() const;
  String      toString()           const;
};

class LR1Item {  // item = A -> alfa . beta {m_la}, then production[m_prod] = A -> alfa beta, and m_dot = length(alfa)
private:
  const Grammar &m_grammar;
public:
  // core(item) is defined as [m_prod, m_dot], ignoring m_la
  bool           m_kernelItem;
  UINT           m_prod;
  UINT           m_dot;
  TermSet        m_la;
  int            m_newState; // Must be signed, initialized to -1. if >= 0, then index of successor-state
  LR1Item(const Grammar *grammar, bool kernelItem, UINT prod, UINT dot, const TermSet &la);
  inline LR1Item &setValues(bool kernelItem, UINT prod, UINT dot, const TermSet &la) {
    m_kernelItem = kernelItem;
    m_prod       = prod      ;
    m_dot        = dot       ;
    m_la         = la        ;
    m_newState   = -1        ;
    return *this;
  }
  inline int getNewState()            const {
    return m_newState;
  }
  inline ULONG hashCode()             const {
    return (m_prod << 16) ^ m_dot; // m_la.hashval(); DONT USE THIS HERE !!!!
  }
  inline bool hasSameCore(const LR1Item *item) const {
    return (m_prod == item->m_prod) && (m_dot == item->m_dot);
  }

  // Is item = "A -> alfa . a beta [la]"
  // if so, set term to a
  bool        isShiftItem(UINT &term) const;
  // Is item = "A -> alfa . a beta [la]"
  bool        isShiftItem()           const;
  // Is item = "A -> alfa . [la]"
  bool        isReduceItem()          const;
  // Is item start -> alfa . EOI
  inline bool isAcceptItem()          const {
    return (m_prod == 0) && isReduceItem();
  }
  // Assume item = "A -> alfa . x beta [la]". Return x (terminal or nonterminal)
  UINT        getShiftSymbol()        const;
  TermSet     first1()                const;

  // flags any combination of {DUMP_LOOKAHEAD,DUMP_SUCC}
  String      toString(UINT flags = DUMP_LOOKAHEAD) const;
};

class LR1State {
private:
  const Grammar         &m_grammar;
public:
  UINT                   m_index;
  BYTE                   m_kernelItemCount;
  CompactArray<LR1Item*> m_items;
  inline LR1State(const Grammar *grammar, UINT index)
    : m_grammar(*grammar)
  {
    setValues(index);
  }
  inline LR1State &setValues(UINT index) {
    m_index           = index;
    m_kernelItemCount = 0;
    return *this;
  }
  LR1Item *findItemWithSameCore(const LR1Item *item) const;
  // compare CORE(this) with CORE(state),
  // Assume, items have been sorted
  // Return true if #kernelitems(this) = kernelitems(state) and this->item[i].hasSameCore(state.item[i]) for all i in [0..#kernelitems[
  bool  hasSameCore(const LR1State &state) const;
  ULONG hashCode() const;
  inline void addItem(LR1Item *item) {
    m_items.add(item);
    if(item->m_kernelItem) {
      m_kernelItemCount++;
    }
  }
  bool mergeLookahead(const LR1State *src);
  inline UINT getItemCount() const {
    return (UINT)m_items.size();
  }
  // sort Items. put kernelitems first, then sort by m_prod, m_dot-position (ignoring m_la, m_newState)
  void sortItems();
  // flags any combination of {DUMP_KERNELONLY,DUMP_SHIFTITEMS,DUMP_ACTIONS} + {flags for itemToString}
  String toString(UINT flags = DUMP_ALL) const;
};

class UIntPermutation;
class OptimizedBitSetPermutation;
class OptimizedBitSetPermutation2;
class GrammarResult;
class LR1StateList;
class LR1ItemList;

class SymbolMap : public CompactStrHashMap<UINT, 300> {
public:
  static constexpr size_t defaultCapacity = 1001;
  SymbolMap(size_t capacity = defaultCapacity) : CompactStrHashMap(capacity) {
  }
};

class StateHashMap : public HashMap<const LR1State *, UINT> {
public:
  static constexpr size_t defaultCapacity = 4001;
  StateHashMap(size_t capacity = defaultCapacity);
};

class StateMap : public CompactArray<LR1State*> {
private:
  StateHashMap m_map; // map core(state) -> index (UINT) into array
public:
  inline void clear() {
    __super::clear();
    m_map.clear();
  }

  // Return index of state with same core if founjd, or -1 if not found
  inline int  getIndex(const LR1State *state) const {
    const UINT *i = m_map.get(state);
    return i ? *i : -1;
  }
  // Return index of state
  UINT        add(LR1State *state);
  inline UINT getStateCount() const {
    return (UINT)size();
  }
  void reorderStates(const UIntPermutation &permutation);
};

class UnfinishedStateSet : public CompactUIntHashSet<1000> {
public:
  static constexpr size_t defaultCapacity = 1001;
  UnfinishedStateSet(size_t capacity = defaultCapacity) : CompactUIntHashSet(capacity) {
  }
};

class Grammar : public AbstractSymbolNameContainer {
  friend class GrammarParser;
  friend class LR1State;
private:
  String                  m_name;
  UINT                    m_termCount, m_startSymbol, m_startState;
  mutable LR1ItemList    *m_itemList;
  mutable LR1StateList   *m_stateList;
  Array<GrammarSymbol>    m_symbolArray;
  SymbolMap               m_symbolMap;
  Array<Production>       m_productionArray;
  StateMap                m_stateArray;
  UnfinishedStateSet      m_unfinishedSet;
  bool                    m_termPermutationDone;
  UINT                    m_termBitSetCapacity;
  bool                    m_statePermutationDone;
  BitSetInterval          m_shiftStateSetInterval, m_succStateSetInterval;
  SourceText              m_header, m_driverHead, m_driverTail; // programtext between %{ and %}
  GrammarResult          *m_result;

  Grammar &operator=(const Grammar &src); // not implemented

  void        setName(              const String         &name  ) {
    m_name = name;
  }
  UINT        addSymbol(            const GrammarSymbol  &symbol);
  // Find all nonterminals that derive epsilon
  void        findEpsilonSymbols();
  // Find FIRST1(A) for all nonterminals A
  void        findFirst1Sets();
  void        computeSuccessors(          LR1State       *state );
  void        computeClosure(             LR1State       *state , bool allowNewItems);
  inline UINT addState(                   LR1State       *state ) {
    const UINT index = m_stateArray.add(state);
    m_succStateSetInterval.setTo( index + 1);
    m_shiftStateSetInterval.setTo(index + 1);
    return index;
  }
  void        init();
  void        initFreeLists();
  LR1Item    *fetchItem(bool kernelItem, UINT prod, UINT dot, const TermSet &la) const;
  LR1Item    *fetchItem(            const LR1Item        *src   ) const;
  void        releaseItem(                LR1Item        *item  ) const;
  void        clearItemList()                                     const;
  LR1State   *fetchState()                                        const;
  LR1State   *fetchState(           const LR1State       *src   ) const;
  void        releaseState(               LR1State       *state ) const;
  void        clearStateList()                                    const;

  // Add terminal symbol to symboltable
  // Assume that no symbol with specified name alrady exist in symboltable
  // Return index, which can be found by getSymbolIndex
  UINT        addTerm(              const String         &name, SymbolType type, int precedence, const SourcePosition &pos);
  // Add non-terminal symbol to symboltable
  // Assume that no symbol with specified name alrady exist in symboltable
  // Return index, which can be found by getSymbolIndex
  UINT        addNTerm(             const String         &name, const SourcePosition &pos);
  void        addProduction(        const Production     &production);
  void        addClosureProductions();

  // Find all symbols that are reachable from startsymbol
  void        findReachable();
  // Find all symbols that can terminate
  void        findTerminate();

  void        checkStateIsConsistent(UINT stateIndex);
  ConflictSolution resolveShiftReduceConflict(const GrammarSymbol &terminal, const LR1Item &item) const;
  void        checkAllStates();
public:
  Grammar();
  Grammar(const Grammar              &src);
  Grammar(const AbstractParserTables &src);
  virtual ~Grammar();

  void                         generateStates();
         UINT                  getSymbolCount()                const final { return (UINT)m_symbolArray.size();            }
         UINT                  getTermCount()                  const final { return m_termCount;                           }
  const  String               &getSymbolName(UINT symbolIndex) const final { return getSymbol(symbolIndex).m_name;         }
  inline UINT                  getProductionCount()            const       { return (UINT)m_productionArray.size();        }
  inline UINT                  getStateCount()                 const       { return (UINT)m_stateArray.size();             }
  inline       GrammarSymbol  &getSymbol(    UINT symbolIndex)             { return m_symbolArray[symbolIndex];            }
  inline const GrammarSymbol  &getSymbol(    UINT symbolIndex) const       { return m_symbolArray[symbolIndex];            }
  inline const Production     &getProduction(UINT index      ) const       { return m_productionArray[index];              }
  inline const LR1State       &getState(     UINT index      ) const       { return *m_stateArray[index];                  }

  // Return index of symbol (terminal or non-terminal) with specified name if it exist
  // or -1, if not found
  inline int                   getSymbolIndex(    const String &name) const {
    const UINT *id = m_symbolMap.get(name.cstr());
    return (id != nullptr) ? *id : -1;
  }

// ------------------------------------- helper functions for code generation ---------------------------------

  const  String               &getName()                       const { return m_name;                   }
  const  GrammarResult        &getResult()                     const { return *m_result;                }
         GrammarResult        &getResult()                           { return *m_result;                }
  inline UINT                  getStartSymbol()                const { return m_startSymbol;            }
  inline UINT                  getStartState()                 const { return m_startState;             }
  inline const SourceText     &getHeader()                     const { return m_header;                 }
  inline const SourceText     &getDriverHead()                 const { return m_driverHead;             }
  inline const SourceText     &getDriverTail()                 const { return m_driverTail;             }

  BitSetParameters             getBitSetParam(ElementType type) const;

  void                         reorderTerminals(const OptimizedBitSetPermutation  &permutation);
  void                         reorderStates(   const OptimizedBitSetPermutation2 &permutation);
  void                         reorderStates(   const UIntPermutation             &permutation);
  void                         disableReorderTerminals()             { m_termPermutationDone  = true;   }
  void                         disableReorderStates()                { m_statePermutationDone = true;   }
  bool                         getTermReorderingDone()         const { return m_termPermutationDone;    }
  bool                         getStateReorderingDone()        const { return m_statePermutationDone;   }
  inline UINT                  getTermBitSetCapacity()         const { return m_termBitSetCapacity;     }
  const BitSetInterval        &getShiftStateBitSetInterval()   const { return m_shiftStateSetInterval;  }
  const BitSetInterval        &getSuccStateBitSetInterval()    const { return m_succStateSetInterval;   }

  // Return total number of generated LALR(1) items
  UINT                         getItemCount()                  const;

  void  dump(                  const SymbolSet  &set  ,                              MarginFile *f = tostdout) const;
  void  dump(                  const Production &prod ,                              MarginFile *f = tostdout) const;
  // flags as for item::toString
  void  dump(                  const LR1Item    &item , UINT flags = DUMP_LOOKAHEAD, MarginFile *f = tostdout) const;
  // flags any combination of {DUMP_ERRORS,DUMP_WARNINGS} + {flags for state::toString}
  void  dump(                  const LR1State   &state, UINT flags = DUMP_ALL,       MarginFile *f = tostdout) const;
  // flags as for dump(const LR1State &state...)
  void  dumpStates(                                     UINT flags = DUMP_DOCFORMAT, MarginFile *f = tostdout) const;
  void  dump(                                                                        MarginFile *f = tostdout) const;
  void  dumpFirst1Sets(FILE *f) const;
};
