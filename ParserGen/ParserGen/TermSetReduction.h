#pragma once

class Grammar;
class TermActionPairArray;

class TermSetReduction {
private:
  const Grammar &m_grammar;
  TermSet        m_termSet;          // set of terminals which should give reduce by production m_prod
  UINT           m_setSize;
  const UINT     m_prod;

public:
  TermSetReduction(const Grammar &grammar, UINT prod    , UINT term0);
  inline const SymbolSet &getTermSet()          const {
    return m_termSet;
  }
  inline UINT             getLegalTermCount()   const {
    return m_setSize;
  }
  // = 1
  inline UINT             getProductionCount()  const {
    return 1;
  }
  inline UINT             getProduction()       const {
    return m_prod;
  }
  inline void             addTerm(UINT term) {
    m_termSet.add(term);
    m_setSize++;
  }
  operator                TermActionPairArray() const;
  String                  toString()            const;
};
