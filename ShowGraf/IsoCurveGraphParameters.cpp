#include "stdafx.h"
#include "IsoCurveGraphParameters.h"

IsoCurveGraphParameters::IsoCurveGraphParameters(const String &name, COLORREF color, GraphStyle style, TrigonometricMode trigonomtetricMode) 
: GraphParameters(name,color,0,style) {

  m_trigonometricMode = trigonomtetricMode;
  m_boundingBox       = Rectangle2D(-10,-10,20,20);
  m_cellSize          = 0.1;
}
   
void IsoCurveGraphParameters::writeFile(FILE *f) {
  USES_CONVERSION;
  const TCHAR *tstyle = graphStyleToString(m_style);
  const TCHAR *ttrigo = trigonometricModeToString(m_trigonometricMode);
  const char  *astyle = T2A(tstyle);
  const char  *atrigo = T2A(ttrigo);

  fprintf(f,"%lf %lf %lf %lf %lf %lf %lf %s %s %08x %d\n"
   ,m_boundingBox.getMinX()
   ,m_boundingBox.getMinY()
   ,m_boundingBox.getWidth()
   ,m_boundingBox.getHeight()
   ,m_cellSize
   ,0.0
   ,0.0
   ,astyle
   ,atrigo
   ,m_color
   ,m_rollSize
  );
  writeString(f, m_expr);
}

void IsoCurveGraphParameters::readFile(FILE *f) {
  char styleStr[100], trigoStr[100];
  double bx,by,bw,bh,csz,x0,y0;
  int color, rollSize;
  if(fscanf(f, "%lf %lf %lf %lf %lf %lf %lf %s %s %x %d\n"
             , &bx,&by,&bw,&bh, &csz, &x0,&y0, &styleStr, &trigoStr, &color, &rollSize) != 11) {
    throwException(_T("Invalid input in line 1"));
  }

  m_boundingBox       = Rectangle2D(bx,by,bw,bh);
  m_cellSize          = csz;
  m_style             = graphStyleFromString(styleStr);
  m_trigonometricMode = trigonometricModeFromString(trigoStr);
  m_color             = color;
  m_rollSize          = rollSize;
  m_expr              = readString(f);
}

