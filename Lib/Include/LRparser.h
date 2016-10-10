#pragma once

#include "Scanner.h"

typedef enum {
  NO_MODIFIER
 ,ZEROORONE     // ?
 ,ZEROORMANY    // *
 ,ONEORMANY     // +
} SymbolModifier;
 
class ParserTables {
public:
  virtual int          getAction(           UINT state, int input) const = 0; // > 0:shift, <=0:reduce, _ParserError:Error
  virtual int          getSuccessor(        UINT state, int nt   ) const = 0;
  virtual UINT         getProductionLength( UINT prod  ) const = 0;
  virtual UINT         getLeftSymbol(       UINT prod  ) const = 0;
  virtual const TCHAR *getSymbolName(       UINT symbol) const = 0;
  const   TCHAR       *getLeftSymbolName(   UINT prod  ) const {
    return getSymbolName(getLeftSymbol(prod));
  }
          String       getRightString(      UINT prod  ) const;
  virtual void         getRightSide(        UINT prod, UINT *dst) const = 0;
  virtual UINT         getTerminalCount()   const = 0;
  virtual UINT         getSymbolCount()     const = 0;
  virtual UINT         getProductionCount() const = 0;
  virtual UINT         getStateCount()      const = 0;
  virtual UINT         getLegalInputCount(  UINT state ) const = 0;
  virtual void         getLegalInputs(      UINT state, UINT *symbols) const = 0;
  virtual ~ParserTables() {
  }
};

#define _ParserError 0xffff

template <class Type> class ParserTablesTemplate : public ParserTables {
private:
  const BYTE          *m_compressedSet;
  const BYTE          *m_compressedLasets;
  const void  * const *m_action;
  const Type  * const *m_successor;
  const unsigned char *m_productionLength;
  const Type          *m_leftSide;
  const Type          *m_rightSideTable;
  const TCHAR        **m_symbolName;
  const unsigned short m_terminalCount;
  const unsigned short m_symbolCount;
  const unsigned short m_productionCount;
  const unsigned short m_stateCount;
  mutable const Type **m_rightSides;

  static inline bool contains(const BYTE *bitset, UINT v) {
    return (bitset[v/8]&(1<<(v%8))) != 0;
  }

  int findElementUncompressed(const Type *state, int token) const {
    int l = 0;
    int r = *(state++)-1;
    while(l <= r) { // binary search
      const int m = (l+r)>>1;
      const int cmp = state[m<<1] - token;
      if(cmp < 0) {
        l = m + 1;
      } else if(cmp > 0) {
        r = m - 1;
      } else {
        return state[(m<<1)|1];
      }
    }
    return _ParserError;
  }

  inline bool isSingleItemActionState(unsigned long statev) const {
    return (statev & 0x8000) == 0;
  }
  inline const BYTE *getCompressedLaset(unsigned long statev) const {
    return m_compressedLasets + (statev&0x7fff);
  }
  inline int getCompressedAction(unsigned long statev) const {
    return (long)statev >> 16;
  }

  inline int getActionCompressedSingleItem(unsigned long statev, int token) const {
    return ((statev&0x7fff) == (unsigned long)token) ? getCompressedAction(statev) : _ParserError;
  }

  inline int getActionCompressedMultiItem(unsigned long statev, int token) const {
    return contains(getCompressedLaset(statev), token) ? getCompressedAction(statev) : _ParserError;
  }
  inline int findActionCompressed(unsigned long statev, int token) const {
    return isSingleItemActionState(statev)
         ? getActionCompressedSingleItem(  statev, token)
         : getActionCompressedMultiItem(statev, token);
  }

  inline void getLegalInputsCompressedSingleItem(unsigned long statev, UINT *symbols) const {
    *symbols = (statev&0x7fff);
  }

  void getLegalInputsCompressedMultiItem(unsigned long statev, UINT *symbols) const {
    const BYTE *set = getCompressedLaset(statev);
    for(int token = 0; token < m_terminalCount; token++) {
      if(contains(set, token)) {
        *(symbols++) = token;
      }
    }
  }

  int getLegalInputCountCompressedMultiItem(unsigned long statev) const {
    const BYTE *set = getCompressedLaset(statev);
    int         sum = 0;
    for(int byteCount = (m_terminalCount-1)/8 + 1; byteCount--;) {
      for(BYTE b = *(set++); b; b &= (b-1)) {
        sum++;
      }
    }
    return sum;
  }

  void getLegalInputsCompressed(unsigned long statev, UINT *symbols) const {
    if(isSingleItemActionState(statev)) {
      getLegalInputsCompressedSingleItem(statev, symbols);
    } else {
      getLegalInputsCompressedMultiItem(statev, symbols);
    }
  }

  int getLegalInputCountCompressed(unsigned long statev) const {
    return isSingleItemActionState(statev) 
         ? 1
         : getLegalInputCountCompressedMultiItem(statev);
  }

  void getLegalInputsUncompressed(const Type *state, UINT *symbols) const {
    for(int n = *(state++); n--; state+=2) {
      *(symbols++) = *state;
    }
  }

  inline int getLegalInputCountUncompressed(const Type *state) const {
    return state[0];
  }

  const Type **getRightSides() const {
    if(m_rightSides == NULL) {
      m_rightSides = new const Type*[m_productionCount];
      int index = 0;;
      for(int p = 0; p < m_productionCount; p++) {
        const UINT prodLen = m_productionLength[p];
        m_rightSides[p] = prodLen ? (m_rightSideTable + index) : NULL;
        index += prodLen;
      }
    }
    return m_rightSides;
  }

  inline bool isCompressedState(UINT state) const {
    return contains(m_compressedSet, state);
  }

public:
#pragma warning(push)
#pragma warning(disable:4311 4302)

  inline int getAction(UINT state, int token) const { // token is terminal. return > 0:shift, <=0:reduce, _ParserError:Error
    return isCompressedState(state) 
         ? findActionCompressed(   (unsigned long)m_action[state], token) 
         : findElementUncompressed((const Type  *)m_action[state], token);
  }

  inline int getSuccessor(UINT state, int nt) const { // nt is nonterminal
    return findElementUncompressed(m_successor[state], nt);
  };

  inline UINT getProductionLength(UINT prod) const {
    return m_productionLength[prod];
  }

  inline UINT getLeftSymbol(UINT prod) const {
    return m_leftSide[prod];
  }

  inline const TCHAR *getSymbolName(UINT symbol) const {
    return m_symbolName[symbol];
  }

  void getRightSide(UINT prod, UINT *dst) const {
    UINT l = getProductionLength(prod);
    if(l == 0) {
      return;
    }
    const Type *rightSide = getRightSides()[prod];
    while(l--) {
      *(dst++) = *(rightSide++);
    }
  }

  inline UINT getTerminalCount() const {
    return m_terminalCount;
  }

  inline UINT getSymbolCount() const {
    return m_symbolCount;
  }

  inline UINT getProductionCount() const {
    return m_productionCount;
  }

  inline UINT getStateCount() const {
    return m_stateCount;
  }
  inline UINT getLegalInputCount(UINT state) const {
    return isCompressedState(state)
         ? getLegalInputCountCompressed(  (unsigned long)(m_action[state]))
         : getLegalInputCountUncompressed((const Type  *)(m_action[state]));
  }

  void getLegalInputs(UINT state, UINT *symbols) const {
    if(isCompressedState(state)) {
      getLegalInputsCompressed(  (unsigned long)(m_action[state]), symbols);
    } else {
      getLegalInputsUncompressed((const Type  *)(m_action[state]), symbols);
    }
  }
#pragma warning(pop)

  ParserTablesTemplate(const BYTE           *compressedSet
                     , const BYTE           *compressedLasets
                     , const void * const   *action
                     , const Type * const   *successor
                     , const unsigned char  *productionLength
                     , const Type           *leftSide
                     , const Type           *rightSideTable
                     , const TCHAR         **symbolName
                     , unsigned short        terminalCount
                     , unsigned short        symbolCount
                     , unsigned short        productionCount
                     , unsigned short        stateCount)
   :m_compressedSet    ( compressedSet   )
   ,m_compressedLasets ( compressedLasets)
   ,m_action           ( action          )
   ,m_successor        ( successor       )
   ,m_productionLength ( productionLength)
   ,m_leftSide         ( leftSide        )
   ,m_rightSideTable   ( rightSideTable  )
   ,m_symbolName       ( symbolName      )
   ,m_terminalCount    ( terminalCount   )
   ,m_symbolCount      ( symbolCount     )
   ,m_productionCount  ( productionCount )
   ,m_stateCount       ( stateCount      )
  {
    m_rightSides = NULL;
  }
  ~ParserTablesTemplate() {
    delete[] m_rightSides;
  }
};

typedef ParserTablesTemplate<char>  ParserCharTables;
typedef ParserTablesTemplate<short> ParserShortTables;

class ParserStackElement {
public:
  unsigned short m_state;
  unsigned short m_symbol;
  SourcePosition m_pos;
  inline ParserStackElement() 
    : m_state( 0)
    , m_symbol(0)
  {
  }
  inline ParserStackElement(unsigned short state, unsigned short symbol, const SourcePosition &pos)
    : m_state( state)
    , m_symbol(symbol)
    , m_pos(   pos)
  {
  }
};

class LRparser {
private:
  UINT                m_state;              // Current parserstate
  int                 m_input;              // Current inputsymbol
  UINT                m_stackSize;
  ParserStackElement *m_parserStack;
  UINT                m_stackTop;           // Topelement of stack = parserStack[m_stackTop-1]
  UINT                m_suppressError;      // Dont give errormessage when > 0. Decremented on every parsecycle
                                            // and set to m_cascadecount when PAaction return _ParserError
  UINT                m_cascadeCount;       // Suppress the next m_cascadecount parsererrors
  UINT                m_maxErrorCount;      // Maximal number of errors before terminate parse.
  UINT                m_errorCount;         // Count parsererrors
  bool                m_done;               // Have we finished the parse
  TCHAR              *m_text;               // Current lexeme
  int                 m_textLength;         // Length of current lexeme
  SourcePosition      m_pos;                // Current SourcePosition
  UINT                m_productionLength;   // Length of current reduceproduction
  bool                m_debug;              // If true call debug on each parsecycle
  const ParserTables &m_tables;             // Generated by parsegen.exe
  Scanner            *m_scanner;            // Lexical scanner. generated by lexgen.exe

  void parserStackCreate(UINT stackSize);
  void parserStackDestroy();
  void stackOverflow();

  inline void parserStackInit() {
    m_stackTop = 0;
  }

  inline void parserStackShift(unsigned short state, unsigned short symbol, const SourcePosition &pos) {
    if(m_stackTop < m_stackSize - 1) {
      m_parserStack[m_stackTop++] = ParserStackElement(state, symbol, pos);
    } else {
      stackOverflow();
    }
  }

  inline const ParserStackElement &getParserStackTop(int fromTop = 0) const {
    return m_parserStack[m_stackTop-1-fromTop];
  }

  inline void parserStackPop(UINT count) {
    m_stackTop -= count;
  }

  inline void parserStackRestore(UINT newTop) {
    m_stackTop = newTop;
  }

  bool recover();
  void initialize();
  void dumpState();
protected:
  virtual int reduceAction(UINT prod) {  // Called for each reduction in the parse
    return 0;
  }
  virtual void userStackInit()                   = 0; // Called before the first parsecycle
  virtual void userStackShiftSymbol(UINT symbol) = 0; // Called when LRparser shift in inputtoken
  virtual void userStackPopSymbols( UINT count ) = 0; // Pop count symbols from userstack
  virtual void userStackShiftDollarDollar()      = 0; // Push($$) to userstack. called at the end of each reduction
  virtual void defaultReduce(       UINT prod  ) = 0; // $$ = $1
public:
  LRparser(const ParserTables &tables, Scanner *lex = NULL, UINT stackSize = 256);
  LRparser(const LRparser &src);                     // Not defined
  LRparser &operator=(const LRparser &rhs);          // Not defined
  ~LRparser();

  inline int input() const {
    return m_input;
  }

  inline UINT state() const {
    return m_state;
  }

  inline bool stackEmpty() const {
    return m_stackTop == 0;
  }

  inline UINT getStackHeight() const {
    return m_stackTop;
  }

  inline UINT getStackSize() const {
    return m_stackSize;
  }

  void setStackSize(UINT newSize);

  inline const ParserStackElement &getStackElement(UINT index) const {
    return m_parserStack[index];
  }

  inline void setCascadeCount(UINT value) {
    m_cascadeCount = value;
  }

  inline UINT getCascadeCount() const {
    return m_cascadeCount;
  }

  inline void setMaxErrorCount(UINT value) {
    m_maxErrorCount = value;
  }

  inline UINT getMaxErrorCount() const {
    return m_maxErrorCount;
  }

  inline UINT getProductionLength(UINT prod) const {
    return m_tables.getProductionLength(prod);
  }

  inline const TCHAR *getSymbolName(UINT symbol) const {
    return m_tables.getSymbolName(symbol);
  }

  inline const ParserTables &getParserTables() const {
    return m_tables;
  }

  int getNextAction() const;

  Scanner *setScanner(Scanner *lex);           // Set new scanner, return old scanner.

  inline Scanner *getScanner() {
    return m_scanner;
  }
  int parseBegin();                            // Return 0 on ok. < 0 on error.
  int parseStep();                             // Return 0 on continue, != 0 terminate parse.
  int parse();                                 // Return 0 on accept.   != 0 on error

  inline void setDebug(bool newValue) {
    m_debug = newValue;
  }

  inline bool done() const {
    return m_done;
  }

  inline bool accept() const {
    return m_errorCount == 0;
  }

  inline const TCHAR *getText() const {
    return m_text;
  }

  inline int getTextLength() const {
    return m_textLength;
  }

  inline int getLineNumber() const {
    return m_pos.getLineNumber();
  }

  SourcePosition getPos() const;                                                       // Return current sourceposition
  const SourcePosition &getPos(int i) const;                                           // Return sourceposition of symbol number i in current production. i = [1..prodlen]
  void error(const SourcePosition &pos, const TCHAR *format, ...);
  void debug(const TCHAR *format, ...);                                                // Called on every step of the parse if m_debug is true
  virtual void verror(const SourcePosition &pos, const TCHAR *format, va_list argptr); // Errors can be caught by usersupplied error-handler
  virtual void vdebug(const TCHAR *format, va_list argptr);                            
};
