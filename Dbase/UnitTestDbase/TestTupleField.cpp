#include "stdafx.h"
#include <Math.h>
#include <limits.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestTupleFields {

  TEST_CLASS(TestTupleFields) {
    public:

#include "UnitTestTraits.h"

    static void verifyEquals(const TupleField &t1, const TupleField &t2) {
      TupleField cmp = t1 == t2;
      int res;
      cmp.get(res);
      verify(res == 1);
    }

    static void verifyIsUndefined(const TupleField &t) {
      verify(!t.isDefined());
    }

    static void verifyIsTrue(const TupleField &t) {
      int v;
      t.get(v);
      verify(t.isDefined() && v == 1);
    }

    static void verifyIsFalse(const TupleField &t) {
      int v;
      t.get(v);
      verify(t.isDefined() && v == 0);
    }

    static String quotedString(const String &s) {
      return String(_T("\"") + s + _T("\""));
    }


#define verifyThrows(stmt,expectedError)                                                                 \
try {                                                                                                    \
  stmt;                                                                                                  \
  _ftprintf(stderr,_T("%s did not throw sqlca in %s line %d. Expected sqlcode=%d")                       \
                  ,_T(#stmt),_T(__FILE__),__LINE__,expectedError);                                       \
  verify(false);                                                                                         \
} catch(sqlca ca) {                                                                                      \
  if(ca.sqlcode != expectedError) {                                                                      \
    OUTPUT(_T("Wrong sqlcode received from %s in %s line %d. sqlca.sqlcode=%d. Expected=%d")             \
                    ,_T(#stmt),_T(__FILE__),__LINE__,ca.sqlcode,expectedError);                          \
    OUTPUT(_T("sqlca:%s"), ca.toString().cstr());                                                        \
    verify(false);                                                                                       \
  }                                                                                                      \
}

#define verifyCast(value,DbType,CastType,expectedValue) \
{ TupleField _x;                                        \
  _x = value;                                           \
  _x.setType(DbType);                                   \
  CastType _castValue;                                  \
  _x.get(_castValue);                                   \
  verify(_castValue == (expectedValue));                \
}

    static void testField(const TupleField &src, const String &expectedString) {
      verify(src.toString() == expectedString);

      TupleField t1 = src;
      verifyEquals(t1,src);

      TupleField t2;
      t2 = src;
      verifyEquals(t2,src);

      TupleField t3(_T("sdf"));
      t3 = src;
      verifyEquals(t3,src);
    }


    TEST_METHOD(testTupleField) {

      String string(_T("jesper"));

      testField(string,quotedString(string));

      int i = 1;
      testField(i,_T("1"));

      Date date(12,12,2001);
      testField(date,date.toString());

      Time time(12,12,12,324);
      testField(time,time.toString(_T("hh:mm:ss:SSS")));

      Timestamp timestamp(date,time);
      testField(timestamp,timestamp.toString(_T("dd.MM.yyyy hh:mm:ss:SSS")));

      varchar vc(_T("sdfgkhg"));
      testField(vc,quotedString((TCHAR*)vc.data()));

      TupleField x = 2;
      TupleField y = 3;
      verifyEquals(x+y,5);
      verifyEquals(x-y,-1);
      verifyEquals(x * y,6);
      verifyEquals(y / x,1.5);
      verifyEquals(x % y,2);
      verifyEquals(sqlPow(x,y),8);
      x = -2;
      verifyEquals(sqlPow(x,y),-8);

      x = 2;
      y = Date(_T("12.12.1900"));
      TupleField z = Date(_T("14.12.1900"));

      verifyEquals(y + x,z);
      verifyEquals(x + y,z);

      verifyEquals(y - x,Date(_T("10.12.1900")));
      verifyEquals(z - y,x);

      verifyThrows(verifyEquals(x - y,Date(_T("10.12.1900"))),SQL_FATAL_ERROR);

      x = _T("str1");
      y = _T("str2");
      verifyEquals(concat(x,y),_T("str1str2"));

      x = 0;
      y = 0;
      verifyIsTrue( x == y);
      verifyIsFalse(x != y);
      verifyIsTrue( x >= y);
      verifyIsFalse(x >  y);
      verifyIsTrue( x <= y);
      verifyIsFalse(x <  y);
      verifyIsFalse(x || y);
      verifyIsFalse(x && y);
      verifyIsTrue(!y);

      x = 0;
      y = 1;
      verifyIsFalse(x == y);
      verifyIsTrue( x != y);
      verifyIsFalse(x >= y);
      verifyIsTrue( y >= x);
      verifyIsFalse(x >  y);
      verifyIsTrue( y >  x);
      verifyIsTrue( x <= y);
      verifyIsFalse(y <= x);
      verifyIsTrue( x <  y);
      verifyIsFalse(y <  x);
      verifyIsTrue( x || y);
      verifyIsTrue( y || x);
      verifyIsFalse(x && y);
      verifyIsFalse(y && x);
      verifyIsFalse(!y);

      x = 1;
      y = 1;
      verifyIsTrue(y || x);
      verifyIsTrue(y && x);

      x = 0;
      y.setUndefined();
      verifyIsFalse(x && y);
      verifyIsFalse(y && x);
      verifyIsUndefined(x || y);
      verifyIsUndefined(y || x);
      verifyIsUndefined(!y);

      verifyIsUndefined(x == y);
      verifyIsUndefined(x != y);
      verifyIsUndefined(x >= y);
      verifyIsUndefined(y >= x);
      verifyIsUndefined(x >  y);
      verifyIsUndefined(y >  x);
      verifyIsUndefined(x <= y);
      verifyIsUndefined(y <= x);
      verifyIsUndefined(x <  y);
      verifyIsUndefined(y <  x);

      verifyIsUndefined(x + y);
      verifyIsUndefined(y + x);
      verifyIsUndefined(x - y);
      verifyIsUndefined(y - x);
      verifyIsUndefined(x * y);
      verifyIsUndefined(y * x);
      verifyIsUndefined(x / y);
      verifyIsUndefined(y / x);
      verifyIsUndefined(x % y);
      verifyIsUndefined(y % x);
      verifyIsUndefined(sqlPow(x,y));
      verifyIsUndefined(sqlPow(y,x));

      x = 1;
      y.setUndefined();
      verifyIsUndefined(x && y);
      verifyIsUndefined(y && x);
      verifyIsTrue(x || y);
      verifyIsTrue(y || x);

      x.setUndefined();
      y.setUndefined();
      verifyIsUndefined(x && y);
      verifyIsUndefined(x || y);
      verifyIsUndefined(x == y);
      verifyIsUndefined(x != y);
      verifyIsUndefined(x >= y);
      verifyIsUndefined(y >= x);
      verifyIsUndefined(x >  y);
      verifyIsUndefined(y >  x);
      verifyIsUndefined(x <= y);
      verifyIsUndefined(y <= x);
      verifyIsUndefined(x <  y);
      verifyIsUndefined(y <  x);

      verifyCast( 1, DBTYPE_CHAR   , char          , 1   );
      verifyCast( 1, DBTYPE_UCHAR  , char          , 1   );
      verifyCast( 1, DBTYPE_CHAR   , unsigned char , 1   );
      verifyCast(-1, DBTYPE_UCHAR  , unsigned char , 255 );
      verifyCast( 1, DBTYPE_UCHAR  , unsigned char , 1   );
      verifyCast( 1, DBTYPE_FLOAT  , float         , 1.0 );
      verifyCast( 1, DBTYPE_DOUBLE , float         , 1.0 );
      verifyCast( 1, DBTYPE_FLOAT  , double        , 1.0 );
      verifyCast( 1, DBTYPE_DOUBLE , double        , 1.0 );
      verifyCast( 1, DBTYPE_DOUBLE , INT64         , 1   );

    #define MAX_INT64CAST ((1ui64<<53)-1)

      verifyCast((INT64)MAX_INT64CAST,DBTYPE_DOUBLE      ,INT64, MAX_INT64CAST);
      verifyCast(-(INT64)MAX_INT64CAST-1,DBTYPE_DOUBLE   ,INT64, -(INT64)MAX_INT64CAST-1);
      verifyCast(MAX_INT64CAST,DBTYPE_DOUBLE             ,UINT64, MAX_INT64CAST);

      for(INT64 t = INT_MAX; t <= MAX_INT64CAST; t = (t << 1) + 1) {
        verifyCast(t,DBTYPE_DOUBLE   ,           INT64,t);
      }
      for(UINT64 ut = INT_MAX; ut <= MAX_INT64CAST; ut = (ut << 1) + 1) {
        verifyCast(ut,DBTYPE_DOUBLE   ,UINT64,ut);
      }
      verifyCast(1,DBTYPE_CSTRING  ,String        ,_T("1"));
      verifyCast(1,DBTYPE_WSTRING  ,String        ,_T("1"));
      verifyCast(1,DBTYPE_VARCHAR  ,varchar       ,_T("1"));

      verifyThrows(verifyCast(256,DBTYPE_SHORT,BYTE ,256    ),SQL_RANGEERROR );
      verifyThrows(verifyCast( -1,DBTYPE_SHORT,BYTE , -1    ),SQL_RANGEERROR );
      verifyThrows(verifyCast(  1,DBTYPE_DATE ,Date ,Date() ),SQL_INVALIDCAST);
      verifyThrows(verifyCast(MAX_INT64CAST+1,DBTYPE_DOUBLE ,INT64,MAX_INT64CAST+1),SQL_LOOSEPRECISION);
      verifyThrows(verifyCast(-(INT64)MAX_INT64CAST-2,DBTYPE_DOUBLE ,INT64,-(INT64)MAX_INT64CAST-2),SQL_LOOSEPRECISION);

      verifyCast(1,DBTYPE_TIME,Time,Time(_T("0:0:0:1")));

      for(i = 0; i < 100; i++) {
        Timestamp ts,now = Timestamp();
    //    _tprintf(_T("now:%s\n"),now.toString(ddMMyyyyhhmmssSSS).cstr());
        x = now;
        x.setType(DBTYPE_DOUBLE);
        double d;
        x.get(d);
    //    _tprintf(_T("x as double::%.0lf\n"),d);
        x.setType(DBTYPE_TIMESTAMP);
    //    _tprintf(_T("x as timestamp::%s\n"),x.toString().cstr());
        x.get(ts);
    //    _tprintf(_T("ts:%s\n"),ts.toString(ddMMyyyyhhmmssSSS).cstr());
        double difference = diff(ts, now,TMILLISECOND);
    //    _tprintf(_T("difference:%lf\n"),difference);
        verify(fabs(difference) < 3);

        x = now;
        x.setType(DBTYPE_CSTRING);
        x.setType(DBTYPE_WSTRING);
        x.setType(DBTYPE_TIMESTAMP);
        x.get(ts);
        verify(ts == now);

        x.setType(DBTYPE_VARCHAR);
        x.setType(DBTYPE_TIMESTAMP);
        x.get(ts);
        verify(ts == now);

        x.setType(DBTYPE_DATE);
        x.get(date);
        verify(date == now.getDate());

        x = now;
        x.setType(DBTYPE_TIME);
        x.get(time);
        verify(time == now.getTime());

        Sleep(1);
      }
        
      verifyThrows(verifyCast(1,DBTYPE_TIMESTAMP,Timestamp,Timestamp()),SQL_INVALIDCAST);
    }
  };
}
