#include "stdafx.h"
#include "DiffEquationGraph.h"

DiffEquationGraphParameters::DiffEquationGraphParameters(const String &name, GraphStyle style, TrigonometricMode trigonomtetricMode)
  : GraphParameters(name, 0, 1, style) {

  m_trigonometricMode = trigonomtetricMode;
  m_interval          = DoubleInterval(0, 1);
  m_eps               = 0.1;
}

void DiffEquationGraphParameters::writeFile(FILE *f) {
  USES_CONVERSION;
  const TCHAR *tstyle = graphStyleToString(m_style);
  const TCHAR *ttrigo = trigonometricModeToString(m_trigonometricMode);
  const char  *astyle = T2A(tstyle);
  const char  *atrigo = T2A(ttrigo);
  const DiffEquationSystemDescription &eqDescArray = m_equationsDescription;
  const UINT   dim    = (UINT)eqDescArray.size();

  assert(dim == m_attrArray.size());
  fprintf(f, "%lf %lf %le %s %s %d\n"
    , m_interval.getFrom()
    , m_interval.getTo()
    , m_eps
    , astyle
    , atrigo
    , dim
  );
  for (UINT i = 0; i < dim; i++) {
    const DiffEquationDescription &eq   = eqDescArray[i];
    const EquationAttributes      &attr = m_attrArray[i];
    writeString(f, eq.m_name);
    writeString(f, eq.m_expr);
    writeString(f, attr.toString());
  }
}

void DiffEquationGraphParameters::readFile(FILE *f) {
  char   styleStr[100], trigoStr[100];
  double from, to, eps;
  UINT   dim;
  if (fscanf(f, "%lf %lf %le %s %s %d\n", &from, &to, &eps, &styleStr, &trigoStr, &dim) != 6) {
    throwException(_T("Invalid input in line 1"));
  }

  m_interval.setFrom(from);
  m_interval.setTo(to);
  m_eps               = eps;
  m_style             = graphStyleFromString(styleStr);
  m_trigonometricMode = trigonometricModeFromString(trigoStr);

  DiffEquationSystemDescription    eqDescArray(dim);
  CompactArray<EquationAttributes> attrArray(  dim);

  for (UINT i = 0; i < dim; i++) {
    const String             name = readString(f);
    const String             expr = readString(f);
    const EquationAttributes attr(readString(f));
    eqDescArray.add(DiffEquationDescription(name, expr));
    attrArray.add(attr);
  }
  m_equationsDescription = eqDescArray;
  m_attrArray            = attrArray;
}

DiffEquationSet DiffEquationGraphParameters::getVisibleEquationSet() const {
  DiffEquationSet result;
  for (size_t i = 0; i < m_attrArray.size(); i++) {
    const EquationAttributes &attr = m_attrArray[i];
    if(attr.m_visible) result.add((UINT)i);
  }
  return result;
}

Vector DiffEquationGraphParameters::getStartVector() const {
  const int vectorDim = getEquationCount() + 1;
  Vector result(vectorDim);
  result[0] = m_interval.getFrom();
  for (size_t i = 1; i < result.getDimension(); i++) {
    result[i] = m_attrArray[i-1].m_startValue;
  }
  return result;
}

EquationAttributes::EquationAttributes(const String &str) {
  Tokenizer tok(str, _T(" "));
  m_startValue = tok.getDouble();
  m_visible    = tok.getBool();
  m_color      = tok.getUint(true);
}

String EquationAttributes::toString() const {
  return format(_T("%s %s %08x"), ::toString(m_startValue).cstr(), boolToStr(m_visible), m_color);
}
