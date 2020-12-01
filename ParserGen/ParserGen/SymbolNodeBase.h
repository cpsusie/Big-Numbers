#pragma once

class SymbolNodeBase {
private:
  const Grammar                 &m_grammar;
  const UINT                     m_symbol;
  const MatrixOptimizeParameters m_opt;
public:
  SymbolNodeBase(const Grammar &grammar, UINT symbol, const MatrixOptimizeParameters &opt)
    : m_grammar(  grammar  )
    , m_symbol(   symbol   )
    , m_opt(      opt      )
  {
  }
  const Grammar &getGrammar() const {
    return m_grammar;
  }
  inline UINT getSymbol() const {
    return m_symbol;
  }
  inline UINT getNTIndex() const {
    assert(!isTerminal());
    return m_symbol - m_grammar.getTermCount();
  }
  inline UINT getTerm() const {
    assert(isTerminal());
    return m_symbol;
  }
  inline bool isTerminal() const {
    return m_grammar.isTerminal(m_symbol);
  }
  inline const String &getSymbolName() const {
    return m_grammar.getSymbolName(m_symbol);
  }
  inline const MatrixOptimizeParameters &getOptimizeParam() const {
    return m_opt;
  }
};
