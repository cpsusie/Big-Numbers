#include "stdafx.h"
#include "IsoCurve.h"

IsoCurveParameters::IsoCurveParameters() {
  m_expr             = "";
  m_size             = 0.25;
  m_boundingBox      = Rectangle2D(-5,-5,10,10);
  m_machineCode      = true;
  m_includeTime      = false;
  m_tInterval        = DoubleInterval(0,10);
  m_timeCount        = 20;
}

void IsoCurveParameters::write(FILE *f) {
  const DoubleInterval xInterval = m_boundingBox.getXInterval();
  const DoubleInterval yInterval = m_boundingBox.getXInterval();

  fprintf(f,"%lf %le %le %le %le %d %d",
            m_size
           ,xInterval.getFrom(), xInterval.getTo()
           ,yInterval.getFrom(), yInterval.getTo()
           ,m_machineCode
           ,m_includeTime
         );
  if(m_includeTime) {
    fprintf(f, " %lf %lf %d", m_tInterval.getFrom(), m_tInterval.getTo(), m_timeCount);
  }
  fprintf(f, "\n");
  writeString(f, m_expr);
}

#define GETBOOL(tok) tok.getInt() ? true : false

void IsoCurveParameters::read(FILE *f) {
  String line = readLine(f);
  Tokenizer tok(line, " ");

  m_machineCode = true;
  m_includeTime = false;
  m_tInterval   = DoubleInterval(0,20);
  m_size        = tok.getDouble();

  double xFrom, xTo, yFrom, yTo;

  xFrom = tok.getDouble(), xTo = tok.getDouble();
  yFrom = tok.getDouble(); yTo = tok.getDouble();

  m_boundingBox = Rectangle2D(xFrom, yFrom, xTo-xFrom,yTo-yFrom);
  m_machineCode = GETBOOL(tok);
  m_includeTime = GETBOOL(tok);
  if(m_includeTime) {
    m_tInterval.setFrom(tok.getDouble());
    m_tInterval.setTo(tok.getDouble());
    m_timeCount = tok.getInt();
  }
  m_expr = readString(f);
}
