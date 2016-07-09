#include "stdafx.h"
#include "IsoSurface.h"

IsoSurfaceParameters::IsoSurfaceParameters() {
  m_expr             = "";
  m_size             = 0.25;
  m_boundingBox      = Cube3D(Point3D(-5,-5,-5), Point3D( 5, 5, 5));
  m_tetrahedral      = true;
  m_adaptiveCellSize = false;
  m_originOutside    = false;
  m_machineCode      = true;
  m_doubleSided      = false;
  m_includeTime      = false;
  m_tInterval        = DoubleInterval(0,10);
  m_timeCount        = 20;
}

void IsoSurfaceParameters::write(FILE *f) {
  const Point3D &lbn = m_boundingBox.m_lbn;
  const Point3D &rtf = m_boundingBox.m_rtf;

  fprintf(f,"%lf %le %le %le %le %le %le %d %d %d %d %d %d",
            m_size
           ,lbn.x,lbn.y,lbn.z
           ,rtf.x,rtf.y,rtf.z
           ,m_tetrahedral
           ,m_adaptiveCellSize
           ,m_originOutside
           ,m_machineCode
           ,m_doubleSided
           ,m_includeTime
         );
  if(m_includeTime) {
    fprintf(f, " %lf %lf %d", m_tInterval.getFrom(), m_tInterval.getTo(), m_timeCount);
  }
  fprintf(f, "\n");
  writeString(f, m_expr);
}

#define GETBOOL(tok) tok.getInt() ? true : false

void IsoSurfaceParameters::read(FILE *f) {
  String line = readLine(f);
  Tokenizer tok(line, " ");

  m_machineCode = true;
  m_doubleSided = false;
  m_includeTime = false;
  m_tInterval   = DoubleInterval(0,20);

  m_size             = tok.getDouble();

  Point3D &lbn = m_boundingBox.m_lbn;
  Point3D &rtf = m_boundingBox.m_rtf;

  lbn.x = tok.getDouble(); lbn.y = tok.getDouble(); lbn.z = tok.getDouble();
  rtf.x = tok.getDouble(); rtf.y = tok.getDouble(); rtf.z = tok.getDouble();

  m_tetrahedral      = GETBOOL(tok);
  m_adaptiveCellSize = GETBOOL(tok);
  m_originOutside    = GETBOOL(tok);
  if(tok.hasNext()) {
    m_machineCode   = GETBOOL(tok);
    if(tok.hasNext()) {
      m_doubleSided = GETBOOL(tok);
      if(tok.hasNext()) {
        m_includeTime = GETBOOL(tok);
        if(m_includeTime) {
          m_tInterval.setFrom(tok.getDouble());
          m_tInterval.setTo(tok.getDouble());
          m_timeCount = tok.getInt();
        }
      }
    }
  }
  m_expr = readString(f);
}
