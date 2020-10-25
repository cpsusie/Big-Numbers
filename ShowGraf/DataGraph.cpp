#include "stdafx.h"
#include "DataGraph.h"

DataGraph::DataGraph(CCoordinateSystem &system, DataGraphParameters &param)
: PointGraph(system , param.clone())
{
  m_lastReadTime = 0;
  readData();
}

void DataGraph::refreshData() {
  readData();
}

bool DataGraph::needRefresh() const {
  const DataGraphParameters &param = (DataGraphParameters&)getParam();
  if(!param.inputIsDiscFile()) {
    return false;
  }
  try {
    return STAT(param.getFileName()).st_mtime > m_lastReadTime;
  } catch (...) { // file might be removed
    return false;
  }
}


void DataGraph::readData() {
  const DataGraphParameters &param = (DataGraphParameters&)getParam();
  FILE *f = !param.inputIsDiscFile() ? stdin : fopen(param.getFileName(),_T("r"));
  if(f != NULL) {
    try {
      readData(f);
      if(f != stdin) {
        fclose(f);
        f = NULL;
      }
    } catch(...) {
      if(f && (f != stdin)) fclose(f);
      clear();
      throw;
    }
  }
}

void DataGraph::readData(FILE *f) {
  const DataGraphParameters &param = (DataGraphParameters&)getParam();
  const bool onePerLine   = param.getOnePerLine();
  const bool ignoreErrors = param.getIgnoreErrors();
  UINT       lineCount    = 0;
  double     currentX     = 1;
  TCHAR      line[4000];
  clear();
  m_hasFirstDataPoint = false;
  while(FGETS(line,ARRAYSIZE(line),f)) {
    lineCount++;
    TCHAR sx[1024],sy[1024];
    if(onePerLine) {
      if(_stscanf(line, _T("%s"), sy) != 1) {
        if(ignoreErrors) {
          continue;
        }
        throwException(_T("Invalid input in %s line %u:%s"), param.getFileName().cstr(), lineCount, line);
      }
      _stprintf(sx,_T("%lg"), currentX++);
    } else if(_stscanf(line,_T("%s %s"), sx, sy) != 2) {
      if(ignoreErrors) {
        continue;
      }
      throwException(_T("Invalid input in %s line %u:%s"), param.getFileName().cstr(), lineCount, line);
    }
    try {
      addPoint(strToPoint(sx, sy, param));
    } catch(Exception e) {
      if(ignoreErrors) {
        continue;
      }
      throwException(_T("Invalid input in %s line %u:<%s>:\n\r%s\n"), param.getFileName().cstr(),lineCount,line,e.what());
    }
  }
  updateDataRange();
  try {
    m_lastReadTime = FSTAT(f).st_mtime;
  } catch (...) {
    m_lastReadTime = 0;
  }
}

Point2D DataGraph::strToPoint(const TCHAR *sx, const TCHAR *sy, const DataGraphParameters &param) {
  const BYTE flags = param.getFlags();
  Point2D result;
  result.x() = param.convertX(sx);
  result.y() = param.convertY(sy);
  if(flags & (DGP_RELATIVETOFIRSTX | DGP_RELATIVETOFIRSTY)) {
    if(m_hasFirstDataPoint) {
      if(flags & DGP_RELATIVETOFIRSTX) {
        result.x() -= m_firstPoint.x();
      }
      if(flags & DGP_RELATIVETOFIRSTY) {
        result.x() -= m_firstPoint.y();
      }
    } else {
      m_firstPoint        = result;
      m_hasFirstDataPoint = true;
      if(flags & DGP_RELATIVETOFIRSTX) {
        result.x() = 0;
      }
      if(flags & DGP_RELATIVETOFIRSTY) {
        result.y() = 0;
      }
    }
  }
  return result;
}
