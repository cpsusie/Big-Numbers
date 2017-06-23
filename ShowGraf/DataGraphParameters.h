#pragma once

#include <MFCUtil/Viewport2D.h>
#include "GraphParameters.h"

class DataGraphParameters : public GraphParameters {
private:
  String     m_fileName;
public:
  DataGraphParameters(const String &name, COLORREF color, bool onePerLine, bool ignoreErrors, bool xRelativeToFirst, bool yRelativeToFirst, DataReader &xReader, DataReader &yReader, int rollAvgSize, GraphStyle style);
  const String &getFileName() const {
    return m_fileName;
  }
  void putDataToDoc(XMLDoc &doc) {
    throwUnsupportedOperationException(__TFUNCTION__);
  }
  void getDataFromDoc(XMLDoc &doc) {
    throwUnsupportedOperationException(__TFUNCTION__);
  }

  bool        m_onePerLine;
  bool        m_ignoreErrors;
  bool        m_xRelativeToFirst, m_yRelativeToFirst;
  DataReader &m_xReader;
  DataReader &m_yReader;
  int getType() const {
    return DATAGRAPH;
  }
};

