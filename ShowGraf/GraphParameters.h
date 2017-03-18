#pragma once

#include <MFCUtil/Viewport2D.h>
#include <Math/Expression/Expression.h>

typedef enum {
  GSCURVE,
  GSPOINT,
  GSCROSS
} GraphStyle;

typedef double(*StringConverter)(const TCHAR *);

#define HAS_LOWER_LIMIT       0x1
#define LOWER_LIMIT_INCLUSIVE 0x2
#define HAS_UPPER_LIMIT       0x4
#define UPPER_LIMIT_INCLUSIVE 0x8

class DataReader {
private:
  StringConverter m_convert;
  DoubleInterval  m_legalInterval;
  int             m_flags; // any combination of above attributes
public:
  DataReader(StringConverter convert, double from, double to, int flags);
  double convertString(const TCHAR *s) const;
  static DataReader LinearDataReader, LogarithmicDataReader, NormalDistributionDataReader, DateTimeDataReader;
};

class GraphParameters {
private:
  String     m_name;
protected:
  static String readString(   FILE *f);
  static void   writeString(  FILE *f, const String &str);
  static String readLine(     FILE *f);
  virtual void  readTextFile( FILE *f);
  virtual void  writeTextFile(FILE *f);
public:
  COLORREF   m_color;
  int        m_rollSize;
  GraphStyle m_style;
  GraphParameters(const String &name, COLORREF color, int rollSize, GraphStyle style);
  
  void setName(const String &name);
  const String &getFullName() const {
    return m_name;
  }
  String getPartialName() const;
  bool hasName() const {
    return m_name.length() > 0;
  }
  bool hasDefaultName() const {
    return m_name == _T("Untitled");
  }

  void setDefaultName() {
    m_name = _T("Untitled");
  }

  void load(const String &fileName);
  void save(const String &fileName);

  static const TCHAR      *graphStyleToString(GraphStyle style);
  static GraphStyle        graphStyleFromString(const String &s);
  static const TCHAR      *trigonometricModeToString(TrigonometricMode mode);
  static TrigonometricMode trigonometricModeFromString(const String &str);
};
