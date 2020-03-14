#pragma once

namespace Expr {

class ParserTree;

class ParserTreeComplexity {
private:
  UINT m_nodeCount;
  UINT m_nameCount;
  UINT m_treeDepth;
  int compare(const ParserTreeComplexity &tc) const;
public:
  ParserTreeComplexity(const ParserTree &tree);
  inline bool operator==(const ParserTreeComplexity &tc) const {
    return compare(tc) == 0;
  }
  inline bool operator!=(const ParserTreeComplexity &tc) const {
    return compare(tc) != 0;
  }
  inline bool operator< (const ParserTreeComplexity &tc) const {
    return compare(tc) < 0;
  }
  inline bool operator<=(const ParserTreeComplexity &tc) const {
    return compare(tc) <= 0;
  }
  inline bool operator> (const ParserTreeComplexity &tc) const {
    return compare(tc) > 0;
  }
  inline bool operator>=(const ParserTreeComplexity &tc) const {
    return compare(tc) >= 0;
  }
  inline UINT getNodeCount() const {
    return m_nodeCount;
  }
  String toString() const;
};

}; // namespace Expr
