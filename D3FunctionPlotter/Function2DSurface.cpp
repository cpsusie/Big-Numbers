#include "stdafx.h"
#include <Tokenizer.h>

Function2DSurfaceParameters::Function2DSurfaceParameters() {
  m_expr          = "";
  m_xInterval     = DoubleInterval(-10,10);
  m_yInterval     = DoubleInterval(-10,10);
  m_timeInterval  = DoubleInterval(0,10);
  m_pointCount    = 10;
  m_frameCount    = 20;
  m_includeTime   = false;
  m_machineCode   = true;
  m_doubleSided   = true;
}

void Function2DSurfaceParameters::write(FILE *f) {
  fprintf(f,"%u %lf %lf %lf %lf %d %d"
           ,m_pointCount
           ,m_xInterval.getFrom()
           ,m_xInterval.getTo()
           ,m_yInterval.getFrom()
           ,m_yInterval.getTo()
           ,m_machineCode
           ,m_includeTime
         );
  if(m_includeTime) {
    fprintf(f, " %lf %lf %u", m_timeInterval.getFrom(), m_timeInterval.getTo(), m_frameCount);
  }
  fprintf(f, " %d", m_doubleSided);
  fprintf(f, "\n");
  writeString(f, m_expr);
}

void Function2DSurfaceParameters::read(FILE *f) {
  m_machineCode  = false;
  m_includeTime  = false;
  m_timeInterval = DoubleInterval(0,20);
  String line = readLine(f);
  Tokenizer tok(line, _T(" "));
  m_pointCount = tok.getInt();
  m_xInterval.setFrom(tok.getDouble());
  m_xInterval.setTo(tok.getDouble());
  m_yInterval.setFrom(tok.getDouble());
  m_yInterval.setTo(tok.getDouble());
  if(tok.hasNext()) {
    m_machineCode = tok.getInt() ? true : false;
    if(tok.hasNext()) {
      m_includeTime = tok.getInt() ? true : false;
      if(m_includeTime) {
        m_timeInterval.setFrom(tok.getDouble());
        m_timeInterval.setTo(tok.getDouble());
        m_frameCount = tok.getInt();
      }
      if(tok.hasNext()) {
        m_doubleSided = tok.getInt() ? true : false;
      }
    }
  }
  m_expr = readString(f);
}
