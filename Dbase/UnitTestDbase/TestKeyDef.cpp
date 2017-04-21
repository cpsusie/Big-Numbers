#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestKeyDefinition {

#include <UnitTestTraits.h>

  TEST_CLASS(TestKeyDefinition) {
    public:

    TEST_METHOD(testKeyDefAddKeyField) {
      KeyFileDefinition keydef;
      keydef.init();

      for(int field = 0; field < MAXKEYFIELDCOUNT; field++)
        keydef.addKeyField(SORT_ASCENDING,DBTYPE_CHAR);

      try {
        keydef.addKeyField(SORT_ASCENDING,DBTYPE_CHAR);
        verify(false);
      } catch(sqlca ca) {
        verify(ca.sqlcode == SQL_KEY_TOO_BIG);
      }
    #define MAXCSTRINGSIZEINKEY (MAXKEYSIZE/sizeof(char   ))
    #define MAXWSTRINGSIZEINKEY (MAXKEYSIZE/sizeof(wchar_t))

      keydef.init();
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_CSTRING,MAXCSTRINGSIZEINKEY);

      try {
        keydef.addKeyField(SORT_ASCENDING,DBTYPE_CHAR);
        verify(false);
      } catch(sqlca ca) {
        verify(ca.sqlcode == SQL_KEY_TOO_BIG);
      }

      keydef.init();
      try {
        keydef.addKeyField(SORT_ASCENDING,DBTYPE_CSTRINGN,MAXCSTRINGSIZEINKEY);
        verify(false);
      } catch(sqlca ca) {
        verify(ca.sqlcode == SQL_KEY_TOO_BIG);
      }

      keydef.init();
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_CSTRINGN,MAXCSTRINGSIZEINKEY-1);
      try {
        keydef.addKeyField(SORT_ASCENDING,DBTYPE_CHAR);
        verify(false);
      } catch(sqlca ca) {
        verify(ca.sqlcode == SQL_KEY_TOO_BIG);
      }



      keydef.init();
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_WSTRING,MAXWSTRINGSIZEINKEY);

      try {
        keydef.addKeyField(SORT_ASCENDING,DBTYPE_CHAR);
        verify(false);
      } catch(sqlca ca) {
        verify(ca.sqlcode == SQL_KEY_TOO_BIG);
      }

      keydef.init();
      try {
        keydef.addKeyField(SORT_ASCENDING,DBTYPE_WSTRINGN,MAXWSTRINGSIZEINKEY);
        verify(false);
      } catch(sqlca ca) {
        verify(ca.sqlcode == SQL_KEY_TOO_BIG);
      }

      keydef.init();
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_WSTRINGN,MAXWSTRINGSIZEINKEY-1);
      try {
        keydef.addKeyField(SORT_ASCENDING,DBTYPE_SHORT);
        verify(false);
      } catch(sqlca ca) {
        verify(ca.sqlcode == SQL_KEY_TOO_BIG);
      }
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_CHAR);
    }

    static void verifySetUndefinedThrows(const KeyFileDefinition &keydef) {
      KeyType key;
      for(UINT field = 0; field < keydef.getFieldCount(); field++) {
        try {
          keydef.setUndefined(key,field);
          verify(false);
        } catch(sqlca ca) {
          verify(ca.sqlcode == SQL_INVALID_FIELDTYPE);
        }
      }
    }

#ifdef BIG_DBADDR
#define TEST_DBADDR 0xf1e2d3c4b5a6ui64
#else
#define TEST_DBADDR 0xf1e2d3c4
#endif

    TEST_METHOD(testKeyDefSigned) {

      KeyFileDefinition keydef;

      const char      ch     = 'a';
      const short     s      = -1;
      const int       i      = -1;
      const long      l      = -1;
      const __int64   i64    = -1;
      const float     flt    = (float)1.234;
      const double    dbl    = 5.678;
      const String    str    = _T("123456");
      const varchar   vchar(_T("ABCDEF"));
      const Date      dd(_T("01.09.2008"));
      const Time      tm(_T("15:50:35:000"));
      const Timestamp ts(_T("01.09.2008 15:50:35:000"));
      const DbAddr    dbaddr = TEST_DBADDR;

      keydef.init();
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_CHAR     );  // 0
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_SHORT    );  // 1
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_INT      );  // 2
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_LONG     );  // 3
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_INT64    );  // 4
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_FLOAT    );  // 5
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_DOUBLE   );  // 6
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_WSTRING,6);  // 7
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_VARCHAR,12); // 8
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_DATE     );  // 9
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_TIME     );  // 10
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_TIMESTAMP);  // 11
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_DBADDR   );  // 12

      verify(keydef.getFieldCount()    == 13);
      verify(keydef.getKeyFieldCount() == 13);

      for(UINT field = 0; field < keydef.getFieldCount(); field++) {
        verify(keydef.getFieldDef(field).isAscending());
        verify(keydef.getFieldDef(field).getOffset() >= 0);
        verify(keydef.getFieldDef(field).getLen()    >  0);
        verify(!isNullAllowed(keydef.getFieldDef(field).getType()));
      }

      verify(keydef.getFieldDef(0).getType()  == DBTYPE_CHAR  );
      verify(keydef.getFieldDef(12).getType() == DBTYPE_DBADDR);

      KeyType key;
      key.init();
      keydef.put(key, 0,ch     );
      keydef.put(key, 1,s      );
      keydef.put(key, 2,i      );
      keydef.put(key, 3,l      );
      keydef.put(key, 4,i64    );
      keydef.put(key, 5,flt    );
      keydef.put(key, 6,dbl    );
      keydef.put(key, 7,str    );
      keydef.put(key, 8,vchar  );
      keydef.put(key, 9,dd     );
      keydef.put(key,10,tm     );
      keydef.put(key,11,ts     );
      keydef.put(key,12,dbaddr );

      verify(keydef.getRecordAddr(key) == dbaddr);

      String keystr   = keydef.sprintf(key);
      String expected = _T("97,-1,-1,-1,-1,1.234,5.678,\"123456\",\"ABCDEF\",01.09.2008,15:50:35:000,01.09.2008 15:50:35:000,") + toString(dbaddr);

      verify(keystr == expected);
      String keydefstr = keydef.toString();

      char      ch1;
      short     s1;
      int       i1;
      long      l1;
      __int64   i641;
      float     flt1;
      double    dbl1;
      String    str1;
      varchar   vchar1;
      Date      dd1;
      Time      tm1;
      Timestamp ts1;
      DbAddr    dbaddr1;

      keydef.get(key, 0,ch1    );
      keydef.get(key, 1,s1     );
      keydef.get(key, 2,i1     );
      keydef.get(key, 3,l1     );
      keydef.get(key, 4,i641   );
      keydef.get(key, 5,flt1   );
      keydef.get(key, 6,dbl1   );
      keydef.get(key, 7,str1   );
      keydef.get(key, 8,vchar1 );
      keydef.get(key, 9,dd1    );
      keydef.get(key,10,tm1    );
      keydef.get(key,11,ts1    );
      keydef.get(key,12,dbaddr1);

      verify(ch1     == ch     );
      verify(s1      == s      );
      verify(i1      == i      );
      verify(l1      == l      );
      verify(i641    == i64    );
      verify(flt1    == flt    );
      verify(dbl1    == dbl    );
      verify(str1    == str    );
      verify(vchar1  == vchar  );
      verify(dd1     == dd     );
      verify(tm1     == tm     );
      verify(ts1     == ts     );
      verify(dbaddr1 == dbaddr );

      try {
        int dummy;
        keydef.get(key,keydef.getFieldCount(),dummy);
        verify(false);
      } catch(sqlca ca) {
        verify(ca.sqlcode == SQL_INVALID_KEYCOUNT);
      }

      keydef.putRecordAddr(key,dbaddr+1);
      verify(keydef.getRecordAddr(key) == dbaddr + 1);

      keydef.initField(key,12);
      verify(keydef.getRecordAddr(key) == DB_NULLADDR);

      verifySetUndefinedThrows(keydef);

      for(UINT field = 0; field < keydef.getFieldCount(); field++) {
        verify(keydef.isDefined(key,field));
      }
    }

    TEST_METHOD(testKeyDefUnsigned) {
      KeyFileDefinition keydef;

      UCHAR  ch  = 'a';
      USHORT s   = (USHORT)DBMAXUSHORT;
      UINT   i   = (UINT  )DBMAXUINT;
      ULONG  l   = (ULONG )DBMAXULONG;
      UINT64 i64 = (UINT64)DBMAXUINT64;

      keydef.init();
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_UCHAR      ); // 0
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_USHORT     ); // 1
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_UINT       ); // 2
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_ULONG      ); // 3
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_UINT64     ); // 4

      verify(keydef.getFieldCount()    == 5);
      verify(keydef.getKeyFieldCount() == 5);

      for(UINT field = 0; field < keydef.getFieldCount(); field++) {
        verify(keydef.getFieldDef(field).isAscending()   );
        verify(keydef.getFieldDef(field).getOffset() >= 0);
        verify(keydef.getFieldDef(field).getLen()    >  0);
        verify(!isNullAllowed(keydef.getFieldDef(field).getType()));
      }

      verify(keydef.getFieldDef(0).getType() == DBTYPE_UCHAR);
      verify(keydef.getFieldDef(3).getType() == DBTYPE_ULONG);
      verify(keydef.getFieldDef(4).getType() == DBTYPE_UINT64);

      KeyType key;
      key.init();
      keydef.put(key, 0,ch     );
      keydef.put(key, 1,s      );
      keydef.put(key, 2,i      );
      keydef.put(key, 3,l      );
      keydef.put(key, 4,i64    );

      UCHAR    ch1;
      USHORT   s1;
      UINT     i1;
      ULONG    l1;
      UINT64   i641;

      keydef.get(key, 0,ch1    );
      keydef.get(key, 1,s1     );
      keydef.get(key, 2,i1     );
      keydef.get(key, 3,l1     );
      keydef.get(key, 4,i641   );

      verify(ch1     == ch     );
      verify(s1      == s      );
      verify(i1      == i      );
      verify(l1      == l      );
      verify(i641    == i64    );

      verifySetUndefinedThrows(keydef);

      String keystr    = keydef.sprintf(key);
      String keydefstr = keydef.toString();

      for(UINT field = 0; field < keydef.getFieldCount(); field++) {
        verify(keydef.isDefined(key, field));
      }
    }

    TEST_METHOD(testKeyDefSignedNull) {
      KeyFileDefinition keydef;

      char      ch     = 'a';
      short     s      = -1;
      int       i      = -1;
      long      l      = -1;
       __int64  i64    = -1;
      float     flt    = (float)1.234;
      double    dbl    = 5.678;
      String    str    = _T("123456");
      varchar   vchar(_T("ABCDEF"));
      Date      dd(_T("01.09.2008"));
      Time      tm(_T("15:50:35:000"));
      Timestamp ts(_T("01.09.2008 15:50:35:000"));
      DbAddr    dbaddr = TEST_DBADDR;

      keydef.init();
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_CHARN      ); // 0
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_SHORTN     ); // 1
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_INTN       ); // 2
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_LONGN      ); // 3
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_INT64N     ); // 4
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_FLOATN     ); // 5
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_DOUBLEN    ); // 6
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_WSTRINGN, 6); // 7
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_VARCHARN,12); // 8
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_DATEN      ); // 9
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_TIMEN      ); //10
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_TIMESTAMPN ); //11
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_DBADDR     ); //12

      verify(keydef.getFieldCount()    == 13);
      verify(keydef.getKeyFieldCount() == 13);

      for(UINT field = 0; field < keydef.getFieldCount() - 1; field++) {
        verify(keydef.getFieldDef(field).isAscending()   );
        verify(keydef.getFieldDef(field).getOffset() >= 0);
        verify(keydef.getFieldDef(field).getLen()    >  0);
        verify(isNullAllowed(keydef.getFieldDef(field).getType()));
      }

      verify(keydef.getFieldDef(0).getType()  == DBTYPE_CHARN );
      verify(keydef.getFieldDef(12).getType() == DBTYPE_DBADDR);


      KeyType key;
      key.init();
      keydef.put(key, 0,ch     );
      keydef.put(key, 1,s      );
      keydef.put(key, 2,i      );
      keydef.put(key, 3,l      );
      keydef.put(key, 4,i64    );
      keydef.put(key, 5,flt    );
      keydef.put(key, 6,dbl    );
      keydef.put(key, 7,str    );
      keydef.put(key, 8,vchar  );
      keydef.put(key, 9,dd     );
      keydef.put(key,10,tm     );
      keydef.put(key,11,ts     );
      keydef.put(key,12,dbaddr );

      char      ch1;
      short     s1;
      int       i1;
      long      l1;
      __int64   i641;
      float     flt1;
      double    dbl1;
      String    str1;
      varchar   vchar1;
      Date      dd1;
      Time      tm1;
      Timestamp ts1;
      DbAddr    dbaddr1;

      for(UINT field = 0; field < keydef.getFieldCount(); field++) {
        verify(keydef.isDefined(key,field));
      }

      keydef.get(key, 0,ch1    );
      keydef.get(key, 1,s1     );
      keydef.get(key, 2,i1     );
      keydef.get(key, 3,l1     );
      keydef.get(key, 4,i641   );
      keydef.get(key, 5,flt1   );
      keydef.get(key, 6,dbl1   );
      keydef.get(key, 7,str1   );
      keydef.get(key, 8,vchar1 );
      keydef.get(key, 9,dd1    );
      keydef.get(key,10,tm1    );
      keydef.get(key,11,ts1    );
      keydef.get(key,12,dbaddr1);

      verify(ch1     == ch    );
      verify(s1      == s     );
      verify(i1      == i     );
      verify(l1      == l     );
      verify(i641    == i64   );
      verify(flt1    == flt   );
      verify(dbl1    == dbl   );
      verify(str1    == str   );
      verify(vchar1  == vchar );
      verify(dd1     == dd    );
      verify(tm1     == tm    );
      verify(ts1     == ts    );
      verify(dbaddr1 == dbaddr);

      String keystr    = keydef.sprintf(key);
      String keydefstr = keydef.toString();

      keystr    = keydef.sprintf(key);
      keydefstr = keydef.toString();

      for(UINT field = 0; field < keydef.getKeyFieldCount() - 1; field++)
        keydef.setUndefined(key,field);

      for(UINT field = 0; field < keydef.getKeyFieldCount() - 1; field++) {
        verify(!keydef.isDefined(key,field));
      }
    }

    TEST_METHOD(testKeyDefUnsignedNull) {
      KeyFileDefinition keydef;

      UCHAR  ch  = 'a';
      USHORT s   = (USHORT)DBMAXUSHORT;
      UINT   i   = (UINT  )DBMAXUINT;
      ULONG  l   = (ULONG )DBMAXULONG;
      UINT64 i64 = (UINT64)DBMAXUINT64;

      keydef.init();
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_UCHARN     ); // 0
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_USHORTN    ); // 1
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_UINTN      ); // 2
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_ULONGN     ); // 3
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_UINT64N    ); // 4

      verify(keydef.getFieldCount()    == 5);
      verify(keydef.getKeyFieldCount() == 5);

      for(UINT field = 0; field < keydef.getFieldCount(); field++) {
        verify(keydef.getFieldDef(field).isAscending()   );
        verify(keydef.getFieldDef(field).getOffset() >= 0);
        verify(keydef.getFieldDef(field).getLen()    >  0);
        verify(isNullAllowed(keydef.getFieldDef(field).getType()));
      }

      verify(keydef.getFieldDef(0).getType() == DBTYPE_UCHARN );
      verify(keydef.getFieldDef(3).getType() == DBTYPE_ULONGN );
      verify(keydef.getFieldDef(4).getType() == DBTYPE_UINT64N);


      KeyType key;
      key.init();
      keydef.put(key, 0,ch    );
      keydef.put(key, 1,s     );
      keydef.put(key, 2,i     );
      keydef.put(key, 3,l     );
      keydef.put(key, 4,i64   );

      UCHAR    ch1;
      USHORT   s1;
      UINT     i1;
      ULONG    l1;
      UINT64   i641;

      keydef.get(key, 0,ch1   );
      keydef.get(key, 1,s1    );
      keydef.get(key, 2,i1    );
      keydef.get(key, 3,l1    );
      keydef.get(key, 4,i641  );

      verify(ch1     == ch    );
      verify(s1      == s     );
      verify(i1      == i     );
      verify(l1      == l     );
      verify(i641    == i64   );

      String keystr    = keydef.sprintf(key);
      String keydefstr = keydef.toString();

      keystr    = keydef.sprintf(key);
      keydefstr = keydef.toString();

      for(UINT field = 0; field < keydef.getFieldCount() - 1; field++)
        keydef.setUndefined(key,field);

      for(UINT field = 0; field < keydef.getFieldCount() - 1; field++) {
        verify(!keydef.isDefined(key,field));
      }
    }

    TEST_METHOD(testKeyDefCompare) {
      KeyFileDefinition keydef;

      char           ch     = 'a';
      short          s      = -1;
      int            i      = -1;
      long           l      = -1;
      __int64        i64    = -1;
      float          flt    = (float)1.234;
      double         dbl    = 5.678;
      String         str    = _T("123456");
      varchar        vchar(_T("ABCDEF"));
      Date           dd(_T("01.09.2008"));
      Time           tm(_T("15:50:35:000"));
      Timestamp      ts(_T("01.09.2008 15:50:35:000"));
      DbAddr         dbaddr = TEST_DBADDR;

      keydef.init();
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_CHARN      ); // 0
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_SHORTN     ); // 1
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_INTN       ); // 2
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_LONGN      ); // 3
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_INT64N     ); // 4
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_FLOATN     ); // 5
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_DOUBLEN    ); // 6
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_WSTRINGN, 6); // 7
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_VARCHARN,12); // 8
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_DATEN      ); // 9
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_TIMEN      ); //10
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_TIMESTAMPN ); //11
      keydef.addKeyField(SORT_ASCENDING,DBTYPE_DBADDR     ); //12

      KeyType key1;
      key1.init();
      keydef.put(key1, 0,ch    );
      keydef.put(key1, 1,s     );
      keydef.put(key1, 2,i     );
      keydef.put(key1, 3,l     );
      keydef.put(key1, 4,i64   );
      keydef.put(key1, 5,flt   );
      keydef.put(key1, 6,dbl   );
      keydef.put(key1, 7,str   );
      keydef.put(key1, 8,vchar );
      keydef.put(key1, 9,dd    );
      keydef.put(key1,10,tm    );
      keydef.put(key1,11,ts    );
      keydef.put(key1,12,dbaddr);

      char      ch1     = ch  + 1;
      short     s1      = s   + 1;
      int       i1      = i   + 1;
      long      l1      = l   + 1;
      INT64     i641    = i64 + 1;
      float     flt1    = flt + 1;
      double    dbl1    = dbl + 1;
      String    str1    = _T("Jespet");
      varchar   vchar1(_T("mortep"));
      Date      dd1     = dd;
      Time      tm1     = tm;
      Timestamp ts1     = ts;
      DbAddr    dbaddr1 = dbaddr + 1;

      dd1.add(TDAYOFMONTH ,1);
      tm1.add(TMILLISECOND,1);
      ts1.add(TMILLISECOND,1);

      KeyType key2;
      key2.init();
  
      keydef.put(key2, 0,ch1    );
      verify(keydef.keynCmp(key1,key2, 1) <  0);

      keydef.put(key2, 0,ch     );
      verify(keydef.keynCmp(key1,key2, 1) == 0);

      keydef.put(key2, 1,s1     );
      verify(keydef.keynCmp(key1,key2, 2) <  0);

      keydef.put(key2, 1,s      );
      verify(keydef.keynCmp(key1,key2, 2) == 0);

      keydef.put(key2, 2,i1     );
      verify(keydef.keynCmp(key1,key2, 3) <  0);

      keydef.put(key2, 2,i      );
      verify(keydef.keynCmp(key1,key2, 3) == 0);

      keydef.put(key2, 3,l1     );
      verify(keydef.keynCmp(key1,key2, 4) <  0);

      keydef.put(key2, 3,l      );
      verify(keydef.keynCmp(key1,key2, 4) == 0);

      keydef.put(key2, 4,i641   );
      verify(keydef.keynCmp(key1,key2, 5) <  0);

      keydef.put(key2, 4,i64    );
      verify(keydef.keynCmp(key1,key2, 5) == 0);

      keydef.put(key2, 5,flt1   );
      verify(keydef.keynCmp(key1,key2, 6) <  0);

      keydef.put(key2, 5,flt    );
      verify(keydef.keynCmp(key1,key2, 6) == 0);

      keydef.put(key2, 6,dbl1   );
      verify(keydef.keynCmp(key1,key2, 7) <  0);

      keydef.put(key2, 6,dbl    );
      verify(keydef.keynCmp(key1,key2, 7) == 0);

      keydef.put(key2, 7,str1   );
      verify(keydef.keynCmp(key1,key2, 8) <  0);

      keydef.put(key2, 7,str    );
      verify(keydef.keynCmp(key1,key2, 8) == 0);

      keydef.put(key2, 8,vchar1 );
      verify(keydef.keynCmp(key1,key2, 9) <  0);

      keydef.put(key2, 8,vchar  );
      verify(keydef.keynCmp(key1,key2, 9) == 0);

      keydef.put(key2, 9,dd1    );
      verify(keydef.keynCmp(key1,key2,10) <  0);

      keydef.put(key2, 9,dd     );
      verify(keydef.keynCmp(key1,key2,10) == 0);

      keydef.put(key2,10,tm1    );
      verify(keydef.keynCmp(key1,key2,11) <  0);

      keydef.put(key2,10,tm     );
      verify(keydef.keynCmp(key1,key2,11) == 0);

      keydef.put(key2,11,ts1    );
      verify(keydef.keynCmp(key1,key2,12) <  0);

      keydef.put(key2,11,ts     );
      verify(keydef.keynCmp(key1,key2,12) == 0);

      keydef.put(key2,12,dbaddr1);
      verify(keydef.keynCmp(key1,key2,13) <  0);

      verify(!keydef.keynCmpRelOp(RELOP_GE,key1,key2,13));
      verify( keydef.keynCmpRelOp(RELOP_GE,key2,key1,13));
      verify( keydef.keynCmpRelOp(RELOP_GT,key2,key1,13));
      verify( keydef.keynCmpRelOp(RELOP_LE,key1,key2,13));
      verify(!keydef.keynCmpRelOp(RELOP_LE,key2,key1,13));
      verify( keydef.keynCmpRelOp(RELOP_LT,key1,key2,13));
      verify(!keydef.keynCmpRelOp(RELOP_NQ,key1,key2,12));
      verify( keydef.keynCmpRelOp(RELOP_NQ,key1,key2,13));
      verify( keydef.keynCmpRelOp(RELOP_EQ,key1,key2,12));
      verify(!keydef.keynCmpRelOp(RELOP_EQ,key1,key2,13));

      keydef.put(key2,12,dbaddr);
      verify(keydef.keynCmp(key1,key2,13) == 0);
      verify(keydef.keynCmpRelOp(RELOP_EQ,key1,key2,13));

      for(UINT field = 0; field < keydef.getKeyFieldCount(); field++) {
        TupleField t1, t2;
        keydef.get(key1, field, t1);
        keydef.get(key2, field, t2);

        if(keydef.getFieldDef(field).isNullable()) {
          keydef.setUndefined(key2, field);
          verify(keydef.keyCmp(key1, key2) <  0);
          keydef.setUndefined(key1, field);
          verify(keydef.keyCmp(key1, key2) == 0);
          keydef.put(key2, field, t2);
          verify(keydef.keyCmp(key1, key2) >  0);
          keydef.put(key1, field, t1);
          verify(keydef.keyCmp(key1, key2) == 0);
        }
      }
    }
  };
}
