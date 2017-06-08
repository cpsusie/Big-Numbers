#pragma once

template<class Ctype> class StringSearchAlgorithm {
protected:
  const Ctype *m_translateTable;
  inline bool charsEqual(Ctype c1, Ctype c2) const {
    return m_translateTable[c1] == m_translateTable[c2];
  }
  inline Ctype translate(Ctype c) const {
    return m_translateTable[c];
  }
  inline bool charsEqualC(Ctype c1, Ctype c2) const {
    return m_translateTable ? charsEqual(c1,c2) : (c1==c2);
  }
  inline Ctype translateC(Ctype c) const {
    return m_translateTable ? translate(c) : c;
  }
  Ctype *reversebuf(Ctype *s, size_t size) {
    Ctype tmp,*l = s, *r = s + size - 1;
    while(l < r) {
      tmp = *l; *(l++) = *r; *(r--) = tmp;
    }
    return s;
  }
public:
  virtual void     compilePattern(const Ctype *pattern, size_t patternLength, bool forwardSearch = true, const Ctype *translateTable = NULL) = 0;
  virtual intptr_t search(        const Ctype *text   , size_t textLength) const = 0;
  virtual StringSearchAlgorithm<Ctype> *clone() const = 0;
  virtual ~StringSearchAlgorithm() {
  }
};
