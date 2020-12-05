#pragma once

#include "CompressNodeBase.h"

class SymbolNodeBase : public CompressNodeBase {
private:
  const UINT m_symbol;
public:
  SymbolNodeBase(const Grammar &grammar, const MatrixOptimizeParameters &opt, UINT symbol)
    : CompressNodeBase(grammar,opt)
    , m_symbol(        symbol   )
  {
  }
  inline UINT getSymbol()       const {
    return m_symbol;
  }
  UINT          getNTIndex()    const;
  UINT          getTerm()       const;
  bool          isTerminal()    const;
  const String &getSymbolName() const;
};
