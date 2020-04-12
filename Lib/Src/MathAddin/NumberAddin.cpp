#include "pch.h"
#include <StrStream.h>
#include <Math/Number.h>
#include "D80ToDbgString.h"

template<typename PTYPE> class NumberT {
public:
  PTYPE        m_vfptr;               // pointer to vtable
  NumberType   m_type;
  PTYPE        m_datap;
  inline NumberType getType() const {
    return m_type;
  }
};

template<typename PTYPE> char *toDbgString(char *dst, NumberT<PTYPE> &n, DEBUGHELPER *helper) {
  switch(n.getType()) {
  case NUMBERTYPE_FLOAT    :
    { float v;
      helper->getObjectx64(&v, n.m_datap, sizeof(v));
      if(!isfinite(v)) return StrStream::formatUndefined(dst, _fpclass(v));
      sprintf(dst, "%g", v);
      return dst;
    }
  case NUMBERTYPE_DOUBLE   :
    { double v;
      helper->getObjectx64(&v, n.m_datap, sizeof(v));
      if(!isfinite(v)) return StrStream::formatUndefined(dst, _fpclass(v));
      sprintf(dst, "%lg", v);
      return dst;
    }
  case NUMBERTYPE_DOUBLE80 :
    { Double80 v;
      helper->getObjectx64(&v, n.m_datap, sizeof(v));
      if(!isfinite(v)) return StrStream::formatUndefined(dst, _fpclass(v));
      return strcpy(dst, D80ToDbgString(v).c_str());
    }
  case NUMBERTYPE_RATIONAL :
    { Rational v;
      if(!isfinite(v)) return StrStream::formatUndefined(dst, _fpclass(v));
      helper->getObjectx64(&v, n.m_datap, sizeof(v));
      return rattoa(dst, v, 10);
    }
  default:
    sprintf(dst, "Unknown type:%d", n.getType());
    return dst;
  }
}

ADDIN_API HRESULT WINAPI AddIn_Number(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  char tmpstr[300];
  try {
    switch(pHelper->getProcessorType()) {
    case PRTYPE_X86:
      { NumberT<DWORD> n;
        pHelper->getRealObject(&n, sizeof(n));
        toDbgString(tmpstr, n, pHelper);
      }
      break;
    case PRTYPE_X64:
      { NumberT<QWORD> n;
        pHelper->getRealObject(&n, sizeof(n));
        toDbgString(tmpstr, n, pHelper);
      }
      break;
    }
  } catch(...) {
    tmpstr[0] = 0;
  }

  strncpy(pResult, tmpstr, maxResult);
  return S_OK;
}

