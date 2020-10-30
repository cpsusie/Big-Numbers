#pragma once

#include "Real.h"

#pragma pack(push,1)
class FunctionDomainRangeDimension {
private:
  union {
    struct Dimensions {
      USHORT m_domainDim;
      USHORT m_rangeDim;
      inline Dimensions(USHORT domainDim, USHORT rangeDim) : m_domainDim(domainDim), m_rangeDim(rangeDim) {
      }
    } m_dim;
    UINT m_data;
  };
public:
  FunctionDomainRangeDimension(USHORT domainDim, USHORT rangeDim)
    : m_dim(domainDim,rangeDim)
  {
  }
  inline UINT getDomainDim() const {
    return m_dim.m_domainDim;
  }
  inline UINT getRangeDim() const {
    return m_dim.m_rangeDim;
  }
  inline ULONG hashCode() const {
    return m_data;
  }
  inline operator UINT() const {
    return m_data;
  }
};
#pragma pack(pop)

template<typename Domain, typename Range> class FunctionTemplate {
public:
  virtual Range operator()(const Domain &x) = 0;
  virtual ~FunctionTemplate() {
  }
};

template<typename ValueType> class ValueOperatorTemplate {
public:
  virtual void apply(const ValueType &x) = 0;
};

template<typename ValueType> class ValueRefArrayTemplate : public CompactArray<ValueType*> {
public:
  inline ValueRefArrayTemplate() {
  }
  inline ValueRefArrayTemplate(size_t capacity) : CompactArray<ValueType*>(capacity) {
  }
  inline ValueRefArrayTemplate(CompactArray<ValueType> &src)
    : CompactArray<VauleType*>(src.size())
  {
    addAll(src);
  }
  bool addAll(CompactArray<ValueType> &a) {
    if(a.isEmpty()) {
      return false;
    }
    for(ValueType *p = a.begin(), *endp = a.end(); p < endp;) {
      add(p++);
    }
    return true;
  }
  bool addAll(const ValueRefArrayTemplate &a) {
    return __super::addAll(a);
  }

  // set x = f(x) for all x in the array
  ValueRefArrayTemplate &apply(FunctionTemplate<ValueType, ValueType> &f) {
    for(ValueType **x = begin(), **endp = end(); x < endp; x++) {
      **x = f(**x);
    }
    return *this;
  }
  void apply(ValueOperatorTemplate<ValueType> &op) const {
    for(const ValueType **x = begin(), **endp = end(); x < endp;) {
      op.apply(**(x++));
    }
  }
};

typedef FunctionTemplate<Real,Real> FunctionR1R1;

typedef FunctionR1R1 Function;

CompactRealArray findZeroes(Function &f, const RealInterval &interval);

template<typename F> class FunctionWithTimeTemplate : public F {
public:
  virtual void        setTime(const Real &time)   = 0;
  virtual const Real &getTime() const             = 0;
  virtual FunctionWithTimeTemplate *clone() const = 0;
};
