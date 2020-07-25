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

template <typename Domain, typename Range> class FunctionTemplate {
public:
  virtual Range operator()(const Domain &x) = 0;
  virtual ~FunctionTemplate() {
  }
};

typedef FunctionTemplate<Real,Real> FunctionR1R1;

typedef FunctionR1R1 Function;

CompactRealArray findZeroes(  Function &f, const RealInterval &interval);

template <typename F> class FunctionWithTimeTemplate : public F {
public:
  virtual void        setTime(const Real &time)   = 0;
  virtual const Real &getTime() const             = 0;
  virtual FunctionWithTimeTemplate *clone() const = 0;
};
