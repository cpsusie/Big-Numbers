#pragma once

#include <FlagTraits.h>
#include "GraphParameters.h"

typedef double(*StringConverter)(const TCHAR *);

#define HAS_LOWER_LIMIT       0x1
#define LOWER_LIMIT_INCLUSIVE 0x2
#define HAS_UPPER_LIMIT       0x4
#define UPPER_LIMIT_INCLUSIVE 0x8

class DataReader {
private:
  StringConverter m_convert;
  DoubleInterval  m_legalInterval;
  // Any combination of above HAS_LOWER_LIMIT,LOWER_LIMIT_INCLUSIVE
  //                         ,HAS_UPPER_LIMIT,UPPER_LIMIT_INCLUSIVE
  int             m_flags;
public:
  DataReader(StringConverter convert, double from, double to, int flags);
  double convertString(const TCHAR *s) const;
  static DataReader LinearDataReader, LogarithmicDataReader, NormalDistributionDataReader, DateTimeDataReader;
};

#define DGP_ONEPERLINE       0x01
#define DGP_IGNOREERRORS     0x02
#define DGP_RELATIVETOFIRSTX 0x04
#define DGP_RELATIVETOFIRSTY 0x08

class DataGraphParameters : public GraphParameters {
private:
  String      m_fileName;
  FLAGTRAITS(DataGraphParameters, BYTE, m_flags);
  DataReader &m_xReader;
  DataReader &m_yReader;
public:
  DataGraphParameters(const String     &name
                     ,COLORREF          color
                     ,bool              onePerLine
                     ,bool              ignoreErrors
                     ,bool              xRelativeToFirst
                     ,bool              yRelativeToFirst
                     ,DataReader       &xReader
                     ,DataReader       &yReader
                     ,const RollingAvg &rollingAvg
                     ,GraphStyle        style);
  const String &getFileName() const {
    return m_fileName;
  }
  inline BYTE getFlags() const {
    return m_flags;
  }
  inline bool getOnePerLine() const {
    return isSet(DGP_ONEPERLINE);
  }
  inline bool getIgnoreErrors() const {
    return isSet(DGP_IGNOREERRORS);
  }
  inline bool getRelativeToFirstX() const {
    return isSet(DGP_RELATIVETOFIRSTX);
  }
  inline bool getRelativeToFirstY() const {
    return isSet(DGP_RELATIVETOFIRSTY);
  }
  inline double convertX(const TCHAR *s) const {
    return m_xReader.convertString(s);
  }
  inline double convertY(const TCHAR *s) const {
    return m_yReader.convertString(s);
  }
  virtual GraphParameters *clone() const {
    return new DataGraphParameters(*this);
  }
  bool inputIsDiscFile() const {
    return m_fileName != _T("stdin");
  }
  void putDataToDoc(XMLDoc &doc) {
    throwUnsupportedOperationException(__TFUNCTION__);
  }
  void getDataFromDoc(XMLDoc &doc) {
    throwUnsupportedOperationException(__TFUNCTION__);
  }

  GraphType getType() const {
    return DATAGRAPH;
  }
};
