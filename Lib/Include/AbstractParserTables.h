#pragma once

#include "AbstractSymbolNameContainer.h"

typedef enum {
  PLATFORM_X86 = 0
 ,PLATFORM_X64 = 1
} Platform;

typedef enum {
  NO_MODIFIER
 ,ZEROORONE     // ?
 ,ZEROORMANY    // *
 ,ONEORMANY     // +
} SymbolModifier;

class AbstractParserTables : public AbstractSymbolNameContainer {
public:
  virtual int           getAction(           UINT state, UINT term    ) const = 0; // > 0:shift, <=0:reduce, _ParserError:Error
  virtual UINT          getSuccessor(        UINT state, UINT nterm   ) const = 0;
  virtual UINT          getProductionLength( UINT prod                ) const = 0;
  virtual UINT          getLeftSymbol(       UINT prod                ) const = 0;
  const   String       &getLeftSymbolName(   UINT prod                ) const {
    return getSymbolName(getLeftSymbol(prod));
  }
          String        getRightString(      UINT prod                ) const;
  virtual void          getRightSide(        UINT prod, UINT *dst     ) const = 0;
  virtual UINT          getProductionCount()                            const = 0;
  virtual UINT          getStateCount()                                 const = 0;
  virtual UINT          getStartState()                                 const = 0;
  // if symbol is a terminal   , return getAction(   state,symbol), if this is positive (shift to state), or else _ParserError
  // if symbol is a nonterminal, return getSuccessor(state,symbol)
  UINT                  getNewState(         UINT symbol, UINT state  ) const;

  virtual UINT          getTableByteCount(   Platform platform        ) const = 0;
  virtual ~AbstractParserTables() {
  }

  typedef BYTE CompressionMethod;

  // 4 different compression-codes. Saved in bit 15-16 in m_actionCode[i] and m_successorCode[i]
  // See generated parsertables for more info of encoding
  static constexpr CompressionMethod CompCodeBinSearch = 0;
  static constexpr CompressionMethod CompCodeSplitNode = 1;
  static constexpr CompressionMethod CompCodeImmediate = 2;
  static constexpr CompressionMethod CompCodeBitSet    = 3;

  // Return value from getAction, on invalid input => parser will not accept
  static constexpr INT               _ParserError      = 0x7fff;
  // special value for fromState, indicating always goto newState, no matter which state we come from
  static constexpr UINT              _NoFromStateCheck = 0x7fff;
};
