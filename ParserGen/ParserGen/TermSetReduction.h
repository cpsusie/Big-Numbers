#pragma once

class TermSetReduction {
private:
  const Grammar &m_grammar;
  SymbolSet      m_termSet;     // set of terminals which should give reduce by production m_prod
  UINT           m_setSize;
  const UINT     m_prod;
public:
  TermSetReduction(const Grammar &grammar, UINT prod, UINT term0)
    : m_grammar(grammar                )
    , m_termSet(grammar.getTermCount() )
    , m_prod(   prod                   )
    , m_setSize(0                      )
  {
    addTerm(term0);
  }
  inline const SymbolSet &getTermSet() const {
    return m_termSet;
  }
  inline UINT getLegalTermCount() const {
    return m_setSize;
  }
  inline UINT getProduction() const {
    return m_prod;
  }
  inline void addTerm(UINT term) {
    m_termSet.add(term);
    m_setSize++;
  }
  operator ParserActionArray() const;
  String toString() const;
};

class TermSetReductionArray : public Array<TermSetReduction> {
public:
  TermSetReductionArray() {
  }
  TermSetReductionArray(size_t capacity) : Array(capacity) {
  }
  // sort by setSize, decreasing, ie. largest set first
  void sortByLegalTermCount();
  // Return sum(((*this)[i].getLegalTermCount()...i=0..size-1)
  UINT getLegalTermCount() const;
  String toString() const;
};
