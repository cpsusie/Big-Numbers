#pragma once

#define VALIDATETOLERANCE(f)                \
  DEFINEMETHODNAME;                         \
  if(!f.isPositive()) {                     \
    throwInvalidToleranceException(method); \
  }

inline BigReal binop_inf(const BigReal &x, const BigReal &y, DigitPool *pool) {
  if(isfinite(x)) return BigReal(y,pool);
  if(isfinite(y)) return BigReal(x,pool);
  return isunordered(x,y) ? pool->getnan() : x;
}

#define HANDLE_INFBINARYOP(x, y, pool)      \
  if(!isfinite(x) || !isfinite(y)) {        \
    return binop_inf(x, y, pool);           \
  }
