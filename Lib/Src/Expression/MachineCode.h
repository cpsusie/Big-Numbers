#pragma once

#include <IntelCPU/ExecutableByteArray.h>
#include <Math/Real.h>

namespace Expr {

typedef void *ExpressionEntryPoint;

#ifdef IS64BIT
extern "C" {
  void callRealResultExpression(ExpressionEntryPoint ep, const void *rsiValue, Real &result);
  int  callIntResultExpression( ExpressionEntryPoint ep, const void *rsiValue);
};
#endif // IS64BIT

class MachineCode : public ExecutableByteArray {
private:
  ExpressionEntryPoint              m_entryPoint;
  void                             *m_esi;
  MachineCode(const MachineCode &src);             // not implemented
  MachineCode &operator=(const MachineCode &src);  // not implemented
public:
  MachineCode() : m_entryPoint(NULL), m_esi(NULL) {
  }
  int  addBytes(const void *bytes, int count);
  void finalize(void *esi);

  inline Real evaluateReal() const {
    Real result;
#ifdef IS32BIT
    const ExpressionEntryPoint ep    = m_entryPoint;
    const void                *daddr = m_esi;
    __asm {
      push esi
      mov  esi, daddr
      call ep
      pop  esi
      fstp result;
    }
#else // IS64BIT
    callRealResultExpression(m_entryPoint, m_esi, result);
#endif // IS64BIT
    return result;
  }

  inline bool evaluateBool() const {
#ifdef IS32BIT
    const ExpressionEntryPoint ep    = m_entryPoint;
    const void                *daddr = m_esi;
    int result;
    __asm {
      push esi
      mov  esi, daddr
      call ep
      pop  esi
      mov result, eax
    }
    return result ? true : false;
#else  // IS64BIT
    return callIntResultExpression(m_entryPoint, m_esi) ? true : false;
#endif // IS64BIT
  }
};

}; // namespace Expr
