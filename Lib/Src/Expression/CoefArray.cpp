#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

bool CoefArray::equalMinus(const CoefArray &a) const { // recursive compare all nodes ( deep compare)
  const size_t n = size();
  if(a.size() != n) {
    return false;
  }
  for(size_t i = 0; i < n; i++) {
    if(!(*this)[i].equalMinus(a[i])) {
      return false;
    }
  }
  return true;
}

}; // namespace Expr
