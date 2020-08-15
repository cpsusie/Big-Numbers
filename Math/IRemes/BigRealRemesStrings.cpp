#include "stdafx.h"

const TCHAR *Remes::s_stateName[] = {
  _T("INITIALIZED")
 ,_T("SOLVE STARTED")
 ,_T("SEARCH E")
 ,_T("SEARCH EXTREMA")
 ,_T("SUCCEEDED")
};

String Remes::getHeaderString() const {
  return format(_T("// (%d,%d)-Minimax-approximation of %s in [%le,%le] with max %serror = %21.15le\n")
               ,m_M,m_K
               ,m_targetFunction.getName().cstr()
               ,(double)m_domain.getFrom()
               ,(double)m_domain.getTo()
               ,m_useRelativeError ? _T("relative ") : EMPTYSTRING
               ,(double)m_maxError
               );
}

String Remes::getCFunctionString(bool useDouble80) const {
  const String     header   = getHeaderString();
  const StreamSize prec     = useDouble80 ? std::numeric_limits<Double80>::max_digits10 : std::numeric_limits<double>::max_digits10;
  const TCHAR     *typeStr  = useDouble80 ? _T("Double80") : _T("double");
  const int        typeSize = useDouble80 ? sizeof(Double80) : sizeof(double);

  String str;
  if(useDouble80) {
    str = format(_T("#include <Double80.h>\n\n"));
  }

  str += format(_T("static const BYTE coefdata[] = {\n"));
  const TCHAR *delimStr = _T(" ");
  for(UINT i = 0; i <= m_N; i++) {
    const Double80 d80  = (Double80)m_coefficientVector[i];
    const double   d64  = (double  )m_coefficientVector[i];
    const BYTE    *byte = useDouble80 ? (BYTE*)&d80 : (BYTE*)&d64;
    str += _T(" ");
    for(int j = 0; j < typeSize; j++, delimStr = _T(",")) {
      str += format(_T("%sx%02x"), delimStr,*(byte++));
    }
    str += format(_T(" // %s\n"), toString(d80,prec,27,ios::scientific).cstr());
  }
  str += _T("};\n\n");

  str += format(_T("static const %s *coef = (const %s*)coefdata;\n"), typeStr, typeStr);
  str += format(_T("%s approximation%02d%02d(%s x) {\n"), typeStr, m_M, m_K, typeStr);

  if(m_K) {
    str += format(_T("  %s sum1 = coef[%d];\n"), typeStr, m_M);
    str += format(_T("  %s sum2 = coef[%d];\n"), typeStr, m_N);
    str += format(_T("  int i;\n"));
    if(m_M - 1 >= 0) {
      str += format(_T("  for(i = %d; i >= 0; i--) sum1 = sum1 * x + coef[i];\n"), m_M - 1);
    }
    if(m_N - 1 > m_M) {
      str += format(_T("  for(i = %d; i > %d; i--) sum2 = sum2 * x + coef[i];\n"), m_N - 1, m_M);
    }
    str += _T("  return sum1 / (sum2 * x + 1.0)\n");
  } else {
    str += format(_T("  %s sum = coef[%d];\n"), typeStr, m_N);
    if(m_N - 1 >= 0) {
      str += format(_T("  for(int i = %d; i >= 0; i--) sum = sum * x + coef[i];\n"), m_N - 1);
    }
    str += _T("  return sum\n");
  }
  str += _T("}\n");
  return header + str;
}

String Remes::getJavaFunctionString() const {
  const String header = getHeaderString();

  String str = format(_T("    private static final double coef[] = {\n"));
  for(UINT i = 0; i <= m_N; i++) {
    const double            coef = (double)m_coefficientVector[i];
    const unsigned __int64 *c    = (unsigned __int64*)&coef;
    str += format(_T("        Double.longBitsToDouble(0x%I64xL)"), *c);
    str += format(_T("%s // %20.16le\n"), i == m_N ? _T(" ") : _T(","), coef);
  }
  str += format(_T("    };\n\n"));

  str += format(_T("    public static double approximation%02d%02d(double x) {\n"), m_M, m_K);
  if(m_K) {
    str += format(_T("        double sum1 = coef[%d];\n"), m_M);
    str += format(_T("        double sum2 = coef[%d];\n"), m_N);
    if(m_M - 1 >= 0) {
      str += format(_T("        for(int i = %d; i >= 0; i--) sum1 = sum1 * x + coef[i];\n"), m_M - 1);
    }
    if(m_N - 1 > m_M) {
      str += format(_T("        for(int i = %d; i > %d; i--) sum2 = sum2 * x + coef[i];\n"), m_N - 1, m_M);
    }
    str += format(_T("        return sum1 / (sum2 * x + 1.0);\n"));
  } else {
    str += format(_T("        double sum = coef[%d];\n"), m_N);
    if(m_N - 1 >= 0) {
      str += format(_T("        for(int i = %d; i >= 0; i--) sum = sum * x + coef[i];\n"), m_N - 1);
    }
    str += _T("        return sum;\n");
  }
  str += _T("    }\n");
  return header + str;
}

String Remes::getExtremumString(UINT index) const {
  return format(_T("Extr[%2u]:%s %s")
               ,index
               ,FormatBigReal(m_extrema[index]   , m_visiblePrecisions.m_extremaDigits).cstr()
               ,FormatBigReal(m_errorValue[index], m_visiblePrecisions.m_errorDigits  ).cstr());
}

String Remes::getMMQuotString() const {
  const BigReal mmQuot = BigReal::_1 - fabs(rQuot(getMinAbsExtremumValue(),getMaxAbsExtremumValue(), m_digits)); // minExtr -> maxExtr => mmQuot -> 0
  return format(_T("MinMaxQuot = 1-|MinExtr/MaxExtr|:%s"), FormatBigReal(mmQuot).cstr());
}

StringArray Remes::getCoefficientStringArray() const {
  StringArray result;
  for(UINT i = 0; i <= m_M; i++) {
    result.add(format(_T("a[%2d] = %s     %s"), i, FormatBigReal(m_coefficientVector[i], 25, 35).cstr(), FormatBigReal((Real)m_coefficientVector[i]).cstr()));
  }
  result.add(format(_T("b[ 0] = +1")));
  for(UINT i = m_M + 1; i <= m_N; i++) {
    result.add(format(_T("b[%2d] = %s     %s"), i-m_M, FormatBigReal(m_coefficientVector[i], 25, 35).cstr(), FormatBigReal((Real)m_coefficientVector[i]).cstr()));
  }
  result.add(format(_T("Max.error:%s"), FormatBigReal(m_maxError).cstr()));
  return result;
}

String Remes::getSearchEString() const {
  if(m_K == 0) {
    return format(_T("New E:%s"), FormatBigReal(m_E, 25).cstr());
  } else {
    String str;
    str  = format(_T("Search E. Iteration %3d. MMQuot:%s\n")
                 ,m_searchEIteration
                 ,FormatBigReal(m_MMQuot,5,9).cstr());
    str += format(_T("Last E:%s\n"), FormatBigReal(m_E    , 38).cstr());
    str += format(_T("New  E:%s\n"), FormatBigReal(m_nextE, 38).cstr());
    str += format(_T("Q=1-min(|Last E|, |New E|)/max(|Last E|, |New E|)\n"));
    str += format(_T("Q=%s. Stop when Q<%s")
                 ,FormatBigReal(m_Q,5,9).cstr()
                 ,toString(m_QEpsilon).cstr());
    return str;
  }
}

String Remes::getTotalStateString() const {
  return format(_T("It:%2d State:%s\nSolve(%s,%s,%s)")
               , m_mainIteration
               , getStateName()
               , m_solveStateDecrM             ? _T("DecM")    : _T("DecK")
               , m_solveStateHighPrecision     ? _T("Hi")      : _T("Lo")
               , m_solveStateInterpolationDone ? _T("Ip done") : _T("Not IP")
               );
}

