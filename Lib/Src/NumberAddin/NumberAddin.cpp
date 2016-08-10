#include "pch.h"
#include <Math/Expression/Number.h>

class Numberx86 {
public:
  DWORD        m_vfptr;               // pointer to vtable
  NumberType   m_type;
  union {
    DWORD     m_real;
    DWORD     m_rational;
  };
};

class Numberx64 {
public:
  QWORD        m_vfptr;               // pointer to vtable
  NumberType   m_type;
  union {
    QWORD     m_real;
    QWORD     m_rational;
  };
};

template<class Number> class NumberAddIn {
private:
  DEBUGHELPER *m_helper;
public:
  NumberAddIn(DEBUGHELPER *pHelper) : m_helper(pHelper) {
  }
  String toString(Number &n, size_t maxResult) const;
};

template<class Number> String NumberAddIn<Number>::toString(Number &n, size_t maxResult) const {
  switch(n.m_type) {
  case NUMBERTYPE_UNDEFINED:
    return _T("undefined");
  case NUMBERTYPE_REAL     :
    { double v;
      m_helper->getObjectx64(&v, n.m_real, sizeof(v));
      return format(_T("%lg"), v);
    }
  case NUMBERTYPE_RATIONAL :
    { Rational r;
      m_helper->getObjectx64(&r, n.m_rational, sizeof(r));
      return r.toString();
    }
  default: return _T("unknown type");
  }
}

ADDIN_API HRESULT WINAPI AddIn_Number(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  String tmpStr;
  try {
    switch (pHelper->getProcessorType()) {
    case PRTYPE_X86:
      { Numberx86 n;
        pHelper->getRealObject(&n, sizeof(n));
        tmpStr = NumberAddIn<Numberx86>(pHelper).toString(n, maxResult - 1);
      }
      break;
    case PRTYPE_X64:
      { Numberx64 n;
        pHelper->getRealObject(&n, sizeof(n));
        tmpStr = NumberAddIn<Numberx64>(pHelper).toString(n, maxResult - 1);
      }
      break;
    }
  }
  catch (...) {
    tmpStr = _T("Invalid address");
  }

  USES_CONVERSION;
  const char *cp = T2A(tmpStr.cstr());
  strncpy(pResult, cp, maxResult);
  return S_OK;
}
