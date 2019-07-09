#include "stdafx.h"
#include <float.h>
#include <Date.h>
#include "DataGraphParameters.h"

static String lastComponent(const String &fname) {
  return FileNameSplitter(fname).getFileName();
}

DataGraphParameters::DataGraphParameters(const String &name
                                        ,COLORREF      color
                                        ,bool          onePerLine
                                        ,bool          ignoreErrors
                                        ,bool          xRelativeToFirst
                                        ,bool          yRelativeToFirst
                                        ,DataReader   &xReader
                                        ,DataReader   &yReader
                                        ,UINT          rollAvgSize
                                        ,GraphStyle    style)
: GraphParameters(lastComponent(name),color,rollAvgSize,style)
, m_xReader(xReader)
, m_yReader(yReader)
{
  m_fileName         = name;
  m_flags            = 0;
  if(onePerLine      ) m_flags |= DGP_ONEPERLINE;
  if(ignoreErrors    ) m_flags |= DGP_IGNOREERRORS;
  if(xRelativeToFirst) m_flags |= DGP_RELATIVETOFIRSTX;
  if(yRelativeToFirst) m_flags |= DGP_RELATIVETOFIRSTY;
}

DataReader::DataReader(StringConverter convert, double from, double to, int flags) : m_legalInterval(from,to) {
  m_convert = convert;
  m_flags   = flags;
}

double DataReader::convertString(const TCHAR *s) const {
  double x = m_convert(s);
  if(m_flags & HAS_LOWER_LIMIT) {
    if(m_flags & LOWER_LIMIT_INCLUSIVE) {
      if(x < m_legalInterval.getMin()) {
        throwException(_T("Read value=%s is below lower limit (=%lg)"), s, m_legalInterval.getMin());
      }
    } else if(x <= m_legalInterval.getMin()) {
      throwException(_T("Read value=%s must be greater than %lg"), s, m_legalInterval.getMin());
    }
  }

  if(m_flags & HAS_UPPER_LIMIT) {
    if(m_flags & UPPER_LIMIT_INCLUSIVE) {
      if(x > m_legalInterval.getMax()) {
        throwException(_T("Read value=%s is above upper limit (=%lg)"), s, m_legalInterval.getMax());
      }
    } else if(x >= m_legalInterval.getMax()) {
      throwException(_T("Read value=%s must be less than %lg"), s, m_legalInterval.getMax());
    }
  }

  return x;
}

static double readNumber(const TCHAR *s) {
  double x;
  if(_stscanf(s,_T("%le"), &x) != 1) {
    throwException(_T("String not a valid number:<%s>"), s);
  }
  return x;
}

static double readTimestamp(const TCHAR *s) {
  try {
    double x = readNumber(s);
    unsigned long date = (unsigned long)floor(x);
    int year  = (int)date / 10000;
    int month = (int)(date / 100) % 100;
    int day   = (int)date % 100;
    year = Date::adjustYear100(year);

    if(!Date::dateValidate(day,month,year)) {
      throwException(_T("Invalid Date:%02d.%02d.%d"), day, month, year);
    }
    unsigned long time = (unsigned long)round(fraction(x) * 1000000);
    if(time == 0) {
      return Date(day,month,year).getDATE();
    } else {
      int hour,minute,second;
      hour   = time / 10000;
      minute = (time / 100) % 100;
      second = time % 100;
      if(!Time::timeValidate(hour,minute,second)) {
        throwException(_T("Invalid Time:%02d.%02d:%02d"), hour, minute, second);
      }
      return Timestamp(day,month,year,hour,minute,second).getDATE();
    }
  } catch(Exception e) {
    Date date(s);
    return date.getDATE();
  }
}

DataReader DataReader::LinearDataReader(readNumber,0,0,0);
DataReader DataReader::LogarithmicDataReader(readNumber,0,1,HAS_LOWER_LIMIT);
DataReader DataReader::NormalDistributionDataReader(readNumber,0,1,HAS_LOWER_LIMIT|HAS_UPPER_LIMIT);
DataReader DataReader::DateTimeDataReader(readTimestamp,0,0,0);
