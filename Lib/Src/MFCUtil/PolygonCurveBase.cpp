#include "pch.h"
#include <MFCUtil/PolygonCurveBase.h>

typedef struct {
  const short  m_type;
  const TCHAR *m_name;
} PolygonCurveTypeName;

static const PolygonCurveTypeName s_typeNames[4] = {
  TT_PRIM_LINE   , _T("line"   )
 ,TT_PRIM_QSPLINE, _T("qspline")
 ,TT_PRIM_CSPLINE, _T("cspline")
 ,0              , EMPTYSTRING
};

const TCHAR *_PolygonCurveTypeName::typeToStr(short type)  { // static
  for(PolygonCurveTypeName pct : s_typeNames) {
    if(pct.m_type == type) {
      return pct.m_name;
    }
  }
  throwInvalidArgumentException(__TFUNCTION__, _T("type=%d"), type);
  return NULL;
}

short _PolygonCurveTypeName::strToType(const TCHAR *str) { // static
  for(PolygonCurveTypeName ct : s_typeNames) {
    if(_tcsicmp(ct.m_name,str) == 0) {
      return ct.m_type;
    }
  }
  throwInvalidArgumentException(__TFUNCTION__, _T("str=%s"), str);
  return 0;
}
