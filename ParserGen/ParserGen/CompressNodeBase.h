#pragma once

class Grammar;

class CompressNodeBase {
private:
  const Grammar                 &m_grammar;
  const MatrixOptimizeParameters m_opt;
public:
  CompressNodeBase(const Grammar &grammar, const MatrixOptimizeParameters &opt)
    : m_grammar(  grammar  )
    , m_opt(      opt      )
  {
  }
  const Grammar &getGrammar()   const {
    return m_grammar;
  }
  inline const MatrixOptimizeParameters &getOptimizeParam() const {
    return m_opt;
  }
};
