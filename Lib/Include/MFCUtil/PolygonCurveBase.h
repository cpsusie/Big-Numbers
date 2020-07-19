#pragma once

class _PolygonCurveTypeName {
public:
  // throws exception if type not in {0,TT_PRIM_LINE, TT_PRIM_QSPLINE, TT_PRIM_CSPLINE}. type==0 returns ""
  static const TCHAR *typeToStr(short        type);
  // throws exception if str not in {"","line","qspline", "cspline"} (comparing ignore case) str=="" return 0
  static short        strToType(const TCHAR *str );
};
