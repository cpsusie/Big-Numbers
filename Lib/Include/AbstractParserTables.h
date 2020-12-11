#pragma once

#include "AbstractSymbolNameContainer.h"

namespace LRParsing {

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

typedef enum {
  PA_SHIFT
 ,PA_NEWSTATE
 ,PA_REDUCE
 ,PA_ERROR
} ActionType;

inline bool _typeHasNewState(      ActionType type) { return type <= PA_NEWSTATE; }
inline bool _typeIsValidActionType(ActionType type) { return (type & 0x1) == 0;   }

#define ACTIONRESULTTRAITS()                                                                       \
  inline ActionType getType()             const { return m_type;                                }  \
  inline bool       isShiftAction()       const { return getType() == PA_SHIFT;                 }  \
  inline bool       isReduceAction()      const { return getType() == PA_REDUCE;                }  \
  inline bool       isParserError()       const { return getType() == PA_ERROR;                 }  \
  inline bool       isValidActionType()   const { return _typeIsValidActionType(getType());     }  \
  inline bool       hasNewState()         const { return _typeHasNewState(getType());           }  \
  inline UINT       getIndex()            const { return m_index;                               }  \
  inline UINT       getReduceProduction() const { assert(isReduceAction()); return getIndex();  }  \
  inline UINT       getNewState()         const { assert(hasNewState()   ); return getIndex();  }  \
  inline bool       isAcceptAction()      const { return isReduceAction()                          \
                                                      && (getReduceProduction() == 0);          }

class Action {
private:
  ActionType m_type  : 3;
  UINT       m_index : 29;
private:
public:
  inline Action(int dummy=0) : m_type(PA_ERROR), m_index(0x0fffffff) {
  }
  inline Action(ActionType type, UINT index) : m_type(type), m_index(index) {
    assert(m_type == type);
  }
  inline bool operator==(const Action &a) const {
    return (m_type == a.m_type) && (m_index == a.m_index);
  }
  inline bool operator!=(const Action &a) const {
    return !(*this == a);
  }
  ULONG hashCode() const {
    return *(int *)this;
  }
  ACTIONRESULTTRAITS()
  String toString() const;
};

inline std::wostream &operator<<(std::wostream &out, const Action &a) {
  out << a.toString();
  return out;
}

class AbstractParserTables : public AbstractSymbolNameContainer {
public:
  virtual Action        getAction(           UINT state, UINT term    ) const = 0;
  // Return >= 0 if new state exist, or -1 if no next state with the given combination of state,nterm exist
  virtual int           getSuccessor(        UINT state, UINT nterm   ) const = 0;
  virtual UINT          getProductionLength( UINT prod                ) const = 0;
  virtual UINT          getLeftSymbol(       UINT prod                ) const = 0;
  const   String       &getLeftSymbolName(   UINT prod                ) const {
    return getSymbolName(getLeftSymbol(prod));
  }
          String        getRightString(      UINT prod                ) const;
  virtual void          getRightSide(        UINT prod , UINT *dst    ) const = 0;
  virtual UINT          getProductionCount()                            const = 0;
  virtual UINT          getStateCount()                                 const = 0;
  virtual UINT          getStartState()                                 const = 0;

  // Assume state  < getStateCount() && symbol < getSymbolCount()
  // if symbol is a terminal     and (action=getAction(   state,symbol)).isShiftAction() then return action.nextState()
  // if symbol is a terminal     and        !getAction(   state,symbol).isShiftAction()  then return -1
  // if symbol is a nonterminal then return getSuccessor(state,symbol)
  int                   getNewState(         UINT state, UINT symbol  ) const;

  virtual UINT          getTableByteCount(   Platform platform        ) const = 0;
  virtual ~AbstractParserTables() {
  }
};

}; // namespace LRParsing
