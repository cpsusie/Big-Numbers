#include "pch.h"
#include <Tokenizer.h>
#include <D3DGraphics/ParametricSurface.h>

ParametricSurfaceParameters::ParametricSurfaceParameters() {
  m_exprX         = EMPTYSTRING;
  m_exprY         = EMPTYSTRING;
  m_exprZ         = EMPTYSTRING;
  m_tInterval     = DoubleInterval(-10,10);
  m_sInterval     = DoubleInterval(-10,10);
  m_timeInterval  = DoubleInterval(0,10);
  m_tStepCount    = 10;
  m_sStepCount    = 10;
  m_frameCount    = 20;
  m_includeTime   = false;
  m_machineCode   = true;
  m_doubleSided   = true;
}

void ParametricSurfaceParameters::write(FILE *f) {
  fprintf(f,"%u %u %lf %lf %lf %lf %d %d"
           ,m_tStepCount
           ,m_sStepCount
           ,m_tInterval.getFrom(),m_tInterval.getTo()
           ,m_sInterval.getFrom(),m_sInterval.getTo()
           ,m_machineCode
           ,m_includeTime
         );
  if(m_includeTime) {
    fprintf(f, " %lf %lf %u", m_timeInterval.getFrom(), m_timeInterval.getTo(), m_frameCount);
  }
  fprintf(f, " %d", m_doubleSided);
  fprintf(f, "\n");
  writeString(f, m_exprX);
  writeString(f, m_exprY);
  writeString(f, m_exprZ);
}

void ParametricSurfaceParameters::read(FILE *f) {
  m_machineCode  = false;
  m_includeTime  = false;
  m_timeInterval = DoubleInterval(0,20);
  String line    = readLine(f);
  Tokenizer tok(line, _T(" "));
  m_tStepCount = tok.getInt();
  m_sStepCount = tok.getInt();
  m_tInterval.setFrom(tok.getDouble());
  m_tInterval.setTo(tok.getDouble());
  m_sInterval.setFrom(tok.getDouble());
  m_sInterval.setTo(tok.getDouble());
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
  m_exprX = readString(f);
  m_exprY = readString(f);
  m_exprZ = readString(f);
}
