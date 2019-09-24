#include "pch.h"
#include <Math/Number.h>
#include "D80ToDbgString.h"

class Numberx86 {
public:
  DWORD        m_vfptr;               // pointer to vtable
  NumberType   m_type;
  DWORD        m_datap;
};

class Numberx64 {
public:
  QWORD        m_vfptr;               // pointer to vtable
  NumberType   m_type;
  QWORD        m_datap;
};

template<class Number> char *toDbgString(char *dst, Number &n, DEBUGHELPER *helper) {
  switch(n.m_type) {
  case NUMBERTYPE_UNDEFINED:
    return strcpy(dst, "Undefined");
  case NUMBERTYPE_FLOAT    :
    { float v;
      helper->getObjectx64(&v, n.m_datap, sizeof(v));
      sprintf(dst, "%g", v);
      return dst;
    }
  case NUMBERTYPE_DOUBLE   :
    { double v;
      helper->getObjectx64(&v, n.m_datap, sizeof(v));
      sprintf(dst, "%lg", v);
      return dst;
    }
  case NUMBERTYPE_DOUBLE80 :
    { Double80 v;
      helper->getObjectx64(&v, n.m_datap, sizeof(v));
      return strcpy(dst, D80ToDbgString(v).c_str());
    }
  case NUMBERTYPE_RATIONAL :
    { Rational v;
      helper->getObjectx64(&v, n.m_datap, sizeof(v));
      return rattoa(dst, v, 10);
    }
  default:
    sprintf(dst, "unknown type:%d", n.m_type);
    return dst;
  }
}

ADDIN_API HRESULT WINAPI AddIn_Number(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  char tmpstr[300];
  try {
    switch(pHelper->getProcessorType()) {
    case PRTYPE_X86:
      { Numberx86 n;
        pHelper->getRealObject(&n, sizeof(n));
        toDbgString(tmpstr, n, pHelper);
      }
      break;
    case PRTYPE_X64:
      { Numberx64 n;
        pHelper->getRealObject(&n, sizeof(n));
        toDbgString(tmpstr, n, pHelper);
      }
      break;
    }
  } catch (...) {
    tmpstr[0] = 0;
  }

  strncpy(pResult, tmpstr, maxResult);
  return S_OK;
}

