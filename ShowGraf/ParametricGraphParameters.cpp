#include "stdafx.h"
#include "ParametricGraph.h"

ParametricGraphParameters::ParametricGraphParameters(const String &name, COLORREF color, int rollSize, GraphStyle style, TrigonometricMode trigonomtetricMode) 
: GraphParameters(name,color,rollSize,style) {

  m_trigonometricMode = trigonomtetricMode;
  m_exprX             = EMPTYSTRING;
  m_exprY             = EMPTYSTRING;
  m_interval          = DoubleInterval(0,1);
  m_steps             = 500;
}

void ParametricGraphParameters::writeTextFile(FILE *f) {
  USES_CONVERSION;
  const TCHAR *tstyle = graphStyleToString(m_style);
  const TCHAR *ttrigo = trigonometricModeToString(m_trigonometricMode);
  const char  *astyle = T2A(tstyle);
  const char  *atrigo = T2A(ttrigo);

  fprintf(f, "%lf %lf %d %s %s %08x %d\n"
   ,m_interval.getFrom()
   ,m_interval.getTo()
   ,m_steps
   ,astyle
   ,atrigo
   ,m_color
   ,m_rollSize
  );
  writeString(f, m_exprX);
  writeString(f, m_exprY);
}

void ParametricGraphParameters::readTextFile(FILE *f) {
  char styleStr[100], trigoStr[100];
  double from, to;
  int steps;
  int color, rollSize;
  if(fscanf(f, "%lf %lf %d %s %s %x %d\n", &from, &to, &steps, &styleStr, &trigoStr, &color, &rollSize) != 7) {
    throwException(_T("Invalid input in line 1"));
  }

  m_interval.setFrom(from);
  m_interval.setTo(to);
  m_steps             = steps;
  m_style             = graphStyleFromString(styleStr);
  m_trigonometricMode = trigonometricModeFromString(trigoStr);
  m_color             = color;
  m_rollSize          = rollSize;
  m_exprX             = readString(f);
  m_exprY             = readString(f);
}

