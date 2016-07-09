#include "stdafx.h"
#include <Tokenizer.h>

Function2DSurfaceParameters::Function2DSurfaceParameters() {
  m_expr          = "";
  m_xInterval     = DoubleInterval(-10,10);
  m_yInterval     = DoubleInterval(-10,10);
  m_tInterval     = DoubleInterval(0,10);
  m_pointCount    = 10;
  m_timeCount     = 20;
  m_includeTime   = false;
  m_machineCode   = true;
  m_doubleSided   = true;
}

void Function2DSurfaceParameters::write(FILE *f) {
  fprintf(f,"%d %lf %lf %lf %lf %d %d"
           ,m_pointCount
           ,m_xInterval.getFrom()
           ,m_xInterval.getTo()
           ,m_yInterval.getFrom()
           ,m_yInterval.getTo()
           ,m_machineCode
           ,m_includeTime
         );
  if(m_includeTime) {
    fprintf(f, " %lf %lf %d", m_tInterval.getFrom(), m_tInterval.getTo(), m_timeCount);
  }
  fprintf(f, " %d", m_doubleSided);
  fprintf(f, "\n");
  writeString(f, m_expr);
}

void Function2DSurfaceParameters::read(FILE *f) {
  m_machineCode = false;
  m_includeTime = false;
  m_tInterval   = DoubleInterval(0,20);
  String line = readLine(f);
  Tokenizer tok(line, " ");
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
        m_tInterval.setFrom(tok.getDouble());
        m_tInterval.setTo(tok.getDouble());
        m_timeCount = tok.getInt();
      }
      if(tok.hasNext()) {
        m_doubleSided = tok.getInt() ? true : false;
      }
    }
  }
  m_expr = readString(f);
}
