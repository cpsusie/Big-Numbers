#include "stdafx.h"
#include "ExpressionGraph.h"

ExpressionGraphParameters::ExpressionGraphParameters(const String &name, COLORREF color, int rollSize, GraphStyle style, TrigonometricMode trigonomtetricMode) 
: GraphParameters(name,color,rollSize,style) {

  m_trigonometricMode = trigonomtetricMode;
  m_expr              = _T("");
  m_interval          = DoubleInterval(0,1);
  m_steps             = 500;
}

   
void ExpressionGraphParameters::write(FILE *f) {
  USES_CONVERSION;
  const TCHAR *tstyle = graphStyleToString(m_style).cstr();
  const TCHAR *ttrigo = trigonometricModeToString(m_trigonometricMode).cstr();
  const TCHAR *texpr  = m_expr.cstr();
  const char  *astyle = T2A(tstyle);
  const char  *atrigo = T2A(ttrigo);
  const char  *aexpr  = T2A(texpr);

  fprintf(f,"%lf %lf %d %s %s %08x %d\n%s\n"
   ,m_interval.getFrom()
   ,m_interval.getTo()
   ,m_steps
   ,astyle
   ,atrigo
   ,m_color
   ,m_rollSize
   ,aexpr
  );
}

void ExpressionGraphParameters::read(const String &fileName) {
  FILE *f = FOPEN(fileName, "r");
  try {
    read(f);
    fclose(f);
    setName(fileName);
  } catch(...) {
    fclose(f);
    throw;
  }
}

void ExpressionGraphParameters::read(FILE *f) {
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

  char line[4096];
  m_expr = _T("");
  while(fgets(line, ARRAYSIZE(line), f)) {
    m_expr += line;
  }
}

