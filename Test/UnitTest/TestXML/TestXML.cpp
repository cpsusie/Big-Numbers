#include "stdafx.h"
#include <Array.h>
#include <HashSet.h>
#include <TreeSet.h>
#include <HashMap.h>
#include <TreeMap.h>
#include <CompactHashSet.h>
#include <CompactHashMap.h>
#include <Random.h>
#include <limits>
#include <float.h>
#include <XMLUtil.h>
#include <Math/Rectangle2D.h>
#include <Math/MathXML.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestXML {

#include <UnitTestTraits.h>

  class KeyType {
  private:
    UINT m_v;
  public:
    inline KeyType() : m_v(0) {
    }
    inline KeyType(UINT v) : m_v(v) {
    }
    inline ULONG hashCode() const {
      return m_v;
    }
    inline bool operator==(const KeyType &k) const {
      return m_v == k.m_v;
    }
    inline operator UINT() const {
      return m_v;
    }
  };

  ULONG keyHash(const KeyType &key) {
    return key.hashCode();
  }
  int keyCmp(const KeyType &e1, const KeyType &e2) {
    return uintHashCmp(e1, e2);
  }

  void getValue(  XMLDoc &doc, const XMLNodePtr &n, KeyType &k) {
    UINT v;
    getValue(doc, n, v);
    k = v;
  }

  TEST_CLASS(TestXML)	{
    public:

    TEST_METHOD(TestXMLDocBasic) {
      SHORT  i16_1   = SHRT_MIN , i16_2   = SHRT_MAX;
      USHORT ui16_1  = 0        , ui16_2  = USHRT_MAX;
      INT    i32_1   = INT_MIN  , i32_2   = INT_MAX;
      UINT   ui32_1  = 0        , ui32_2  = UINT_MAX;
      INT64  i64_1   = LLONG_MIN, i64_2   = LLONG_MAX;
      UINT64 ui64_1  = 0        , ui64_2  = ULLONG_MAX;
      float  f32_1   = FLT_MIN  , f32_2   = FLT_MAX   , f32_3 = FLT_EPSILON;
      double d64_1   = DBL_MIN  , d64_2   = DBL_MAX   , d64_3 = DBL_EPSILON;
      char    *cstr  =  "abcdefghijklmnopqrst0123456789\"<>!#$¤%&/()=?´´´¨^*'-.,;:\\";
      wchar_t *wstr  = L"abcdefghijklmnopqrst0123456789\"<>!#$¤%&/()=?´´´¨^*'-.,;:\\";
      String   str   = wstr;

      XMLDoc doc;
      verify(doc.isEmpty());
      XMLNodePtr root       = doc.createRoot(      _T("root"      ));
      XMLNodePtr intNode    = doc.createNode(root, _T("intdata"   ));
      XMLNodePtr floatNode  = doc.createNode(root, _T("floatdata" ));
      XMLNodePtr strNode    = doc.createNode(root, _T("stringdata"));
      XMLNodePtr boolNode   = doc.createNode(root, _T("booldata"  ));
      XMLNodePtr collNode   = doc.createNode(root, _T("colldata"  ));
      verify(!doc.isEmpty());

      setValue(doc, intNode  , _T("SHRT_MIN"   ), i16_1 );
      setValue(doc, intNode  , _T("SHRT_MAX"   ), i16_2 );
      setValue(doc, intNode  , _T("USHRT_MIN"  ), ui16_1);
      setValue(doc, intNode  , _T("USHRT_MAX"  ), ui16_2);

      setValue(doc, intNode  , _T("INT_MIN"    ), i32_1 );
      setValue(doc, intNode  , _T("INT_MAX"    ), i32_2 );
      setValue(doc, intNode  , _T("UINT_MIN"   ), ui32_1);
      setValue(doc, intNode  , _T("UINT_MAX"   ), ui32_2);

      setValue(doc, intNode  , _T("LLONG_MIN"  ), i64_1 );
      setValue(doc, intNode  , _T("LLONG_MAX"  ), i64_2 );
      setValue(doc, intNode  , _T("ULLONG_MIN" ), ui64_1);
      setValue(doc, intNode  , _T("ULLONG_MAX" ), ui64_2);

      setValue(doc, floatNode, _T("FLT_MIN"    ), f32_1 ,-1);
      setValue(doc, floatNode, _T("FLT_MAX"    ), f32_2 ,-1);
      setValue(doc, floatNode, _T("FLT_EPSILON"), f32_3 ,-1);

      setValue(doc, floatNode, _T("DBL_MIN"    ), d64_1 ,-1);
      setValue(doc, floatNode, _T("DBL_MAX"    ), d64_2 ,-1);
      setValue(doc, floatNode, _T("DBL_EPSILON"), d64_3 ,-1);

      setValue(doc, strNode  , _T("CSTR"       ), cstr  );
      setValue(doc, strNode  , _T("WSTR"       ), wstr  );
      setValue(doc, strNode  , _T("STR"        ) ,str   );

      setValue(doc, boolNode  , _T("F"         ), false );
      setValue(doc, boolNode  , _T("T"         ), true  );

      CompactIntArray i32carray;
      for(int i = 0; i < 100; i++) {
        i32carray.add(randInt());
      }
      setValue(doc, collNode  , _T("i32carray"   ), i32carray );

      IntArray i32array;
      i32array.addAll(i32carray);
      setValue(doc, collNode  , _T("i32array"   ), i32array );

      IntTreeSet i32set;
      i32set.addAll(i32carray);
      setValue(doc, collNode  , _T("i32set"   ), i32set );

      IntTreeMap<int> i32map;
      for(auto it = i32array.getIterator(); it.hasNext();) {
        i32map.put(it.next(), randInt());
      }
      setValue(doc, collNode  , _T("i32map"   ), i32map );

      const String fileName = getTestFileName(__TFUNCTION__);
      doc.saveToFile(fileName);

      doc.clear();
      verify(doc.isEmpty());
      doc.loadFromFile(fileName);
      verify(!doc.isEmpty());

      XMLNodePtr root1 = doc.getRoot();
      verify(_tcsicmp(root1->nodeName, _T("root")) == 0);

      XMLNodePtr intNode1   = doc.getChild(root1, _T("intdata"   ));
      XMLNodePtr floatNode1 = doc.getChild(root1, _T("floatdata" ));
      XMLNodePtr strNode1   = doc.getChild(root1, _T("stringdata"));
      XMLNodePtr boolNode1  = doc.getChild(root1, _T("booldata"  ));
      XMLNodePtr collNode1  = doc.getChild(root1, _T("colldata"  ));

      SHORT  I16_1              , I16_2   ;
      USHORT UI16_1             , UI16_2  ;
      INT    I32_1              , I32_2   ;
      UINT   UI32_1             , UI32_2  ;
      INT64  I64_1              , I64_2   ;
      UINT64 UI64_1             , UI64_2  ;
      FLOAT  F32_1              , F32_2   , F32_3 ;
      DOUBLE D64_1              , D64_2   , D64_3 ;
      String STR1,STR2,STR3;
      bool   B1, B2;

      getValue(doc, intNode1 , _T("SHRT_MIN"  ), I16_1  );
      getValue(doc, intNode1 , _T("SHRT_MAX"  ), I16_2  );
      getValue(doc, intNode1 , _T("USHRT_MIN" ), UI16_1 );
      getValue(doc, intNode1 , _T("USHRT_MAX" ), UI16_2 );

      getValue(doc, intNode1 , _T("INT_MIN"   ), I32_1  );
      getValue(doc, intNode1 , _T("INT_MAX"   ), I32_2  );
      getValue(doc, intNode1 , _T("UINT_MIN"  ), UI32_1 );
      getValue(doc, intNode1 , _T("UINT_MAX"  ), UI32_2 );

      getValue(doc, intNode1 , _T("LLONG_MIN" ), I64_1  );
      getValue(doc, intNode1 , _T("LLONG_MAX" ), I64_2  );
      getValue(doc, intNode1 , _T("ULLONG_MIN"), UI64_1 );
      getValue(doc, intNode1 , _T("ULLONG_MAX"), UI64_2 );

      verify(I16_1  == i16_1 );
      verify(I16_2  == i16_2 );
      verify(UI16_1 == ui16_1);
      verify(UI16_2 == ui16_2);

      verify(I32_1  == i32_1 );
      verify(I32_2  == i32_2 );
      verify(UI32_1 == ui32_1);
      verify(UI32_2 == ui32_2);

      verify(I64_1  == i64_1 );
      verify(I64_2  == i64_2 );
      verify(UI64_1 == ui64_1);
      verify(UI64_2 == ui64_2);

      getValue(doc, floatNode1, _T("FLT_MIN"    ), F32_1 );
      getValue(doc, floatNode1, _T("FLT_MAX"    ), F32_2 );
      getValue(doc, floatNode1, _T("FLT_EPSILON"), F32_3 );

      verify(F32_1  == f32_1);
      verify(F32_2  == f32_2);
      verify(F32_3  == f32_3);

      getValue(doc, floatNode1, _T("DBL_MIN"    ), D64_1 );
      getValue(doc, floatNode1, _T("DBL_MAX"    ), D64_2 );
      getValue(doc, floatNode1, _T("DBL_EPSILON"), D64_3 );

      verify(D64_1  == d64_1);
      verify(D64_2  == d64_2);
      verify(D64_3  == d64_3);

      getValue(doc, strNode1  , _T("CSTR"       ), STR1  );
      getValue(doc, strNode1  , _T("WSTR"       ), STR2  );
      getValue(doc, strNode1  , _T("STR"        ) ,STR3  );

      verify(STR1 == cstr);
      verify(STR2 == wstr);
      verify(STR3 == str );

      getValue(doc, boolNode1  , _T("F"         ), B1    );
      getValue(doc, boolNode1  , _T("T"         ), B2    );

      verify(!B1);
      verify( B2);

      CompactIntArray I32CARRAY;
      getValue(doc, collNode1 , _T("i32carray"   ), I32CARRAY );

      verify(I32CARRAY == i32carray);

      IntArray I32ARRAY;
      getValue(doc, collNode1 , _T("i32array"   ), I32ARRAY   );

      verify(I32ARRAY == i32array);

      IntTreeSet I32SET;
      getValue(doc, collNode1 , _T("i32set"     ), I32SET     );
      verify(I32SET == i32set);

      IntTreeMap<int> I32MAP;
      getValue(doc, collNode1 , _T("i32map"     ), (TreeMap<int,int>&)I32MAP );
      verify(I32MAP == i32map);
    }

    TEST_METHOD(TestMathXML) {
      TreeSet<KeyType>                keytreeset(   keyCmp);
      TreeMap<KeyType,Point2D>        keyp2Dtreemap(keyCmp);
      HashSet<KeyType>                keyhashset(   keyHash,keyCmp);
      HashMap<KeyType,Point2D>        keyp2Dhashmap(keyHash,keyCmp);
      IntTreeMap<Point2D>             intp2Dtreemap;
      IntHashMap<Point2D>             intp2Dhashmap;
      Array<Point2D>                  p2Darray;
      Point2DArray                    p2Dcarray;
      CompactIntHashMap<Point2D>      intp2Dchashmap;
      CompactHashSet<KeyType>         keychashset;
      CompactHashMap<KeyType,Point2D> keyp2Dchashmap;

      for(int i = 0; i < 10; i++) {
        const KeyType key(i);
        const Point2D p(randDouble(), randDouble());
        keytreeset.add(key);
        keyp2Dtreemap.put(key, p);
        keyhashset.add(key);
        keyp2Dhashmap.put(key, p);
        intp2Dtreemap.put(i,p);
        intp2Dhashmap.put(i,p);
        p2Darray.add(p);
        p2Dcarray.add(p);
        intp2Dchashmap.put(i,p);
        keychashset.add(key);
        keyp2Dchashmap.put(key,p);
      }
      XMLDoc doc;
      verify(doc.isEmpty());
      XMLNodePtr root       = doc.createRoot(_T("root"));

      setValue(doc, root, _T("keytreeset"    ), keytreeset       );
      setValue(doc, root, _T("keyp2Dtreemap" ), keyp2Dtreemap ,-1);
      setValue(doc, root, _T("keyhashset"    ), keyhashset       );
      setValue(doc, root, _T("keyp2Dhashmap" ), keyp2Dhashmap ,-1);
      setValue(doc, root, _T("intp2Dtreemap" ), intp2Dtreemap ,-1);
      setValue(doc, root, _T("intp2Dhashmap" ), intp2Dhashmap ,-1);
      setValue(doc, root, _T("p2Darray"      ), p2Darray      ,-1);
      setValue(doc, root, _T("p2Dcarray"     ), p2Dcarray     ,-1);
      setValue(doc, root, _T("intp2Dchashmap"), intp2Dchashmap,-1);
      setValue(doc, root, _T("keychashset"   ), keychashset      );
      setValue(doc, root, _T("keyp2Dchashmap"), keyp2Dchashmap,-1);

      const String fileName = getTestFileName(__TFUNCTION__);
      doc.saveToFile(fileName);

      doc.clear();
      verify(doc.isEmpty());

      doc.loadFromFile(fileName);
      verify(!doc.isEmpty());

      XMLNodePtr root1 = doc.getRoot();
      verify(_tcsicmp(root1->nodeName, _T("root")) == 0);

      TreeSet<KeyType>                keytreeset1(   keyCmp);
      TreeMap<KeyType,Point2D>        keyp2Dtreemap1(keyCmp);
      HashSet<KeyType>                keyhashset1(   keyHash,keyCmp);
      HashMap<KeyType,Point2D>        keyp2Dhashmap1(keyHash,keyCmp);
      IntTreeMap<Point2D>             intp2Dtreemap1;
      IntHashMap<Point2D>             intp2Dhashmap1;
      Array<Point2D>                  p2Darray1;
      Point2DArray                    p2Dcarray1;
      CompactIntHashMap<Point2D>      intp2Dchashmap1;
      CompactHashSet<KeyType>         keychashset1;
      CompactHashMap<KeyType,Point2D> keyp2Dchashmap1;

      getValue(doc, root1, _T("keytreeset"    ), keytreeset1       );
      getValue(doc, root1, _T("keyp2Dtreemap" ), keyp2Dtreemap1    );
      getValue(doc, root1, _T("keyhashset"    ), keyhashset1       );
      getValue(doc, root1, _T("keyp2Dhashmap" ), keyp2Dhashmap1    );
      getValue(doc, root1, _T("intp2Dtreemap" ), intp2Dtreemap1    );
      getValue(doc, root1, _T("intp2Dhashmap" ), intp2Dhashmap1    );
      getValue(doc, root1, _T("p2Darray"      ), p2Darray1         );
      getValue(doc, root1, _T("p2Dcarray"     ), p2Dcarray1        );
      getValue(doc, root1, _T("intp2Dchashmap"), intp2Dchashmap1   );
      getValue(doc, root1, _T("keychashset"   ), keychashset1      );
      getValue(doc, root1, _T("keyp2Dchashmap"), keyp2Dchashmap1   );

      verify(keytreeset1       == keytreeset       );
      verify(keyp2Dtreemap1    == keyp2Dtreemap    );
      verify(keyhashset1       == keyhashset       );
      verify(keyp2Dhashmap1    == keyp2Dhashmap    );
      verify(intp2Dtreemap1    == intp2Dtreemap    );
      verify(intp2Dhashmap1    == intp2Dhashmap    );
      verify(p2Darray1         == p2Darray         );
      verify(p2Dcarray1        == p2Dcarray        );
      verify(intp2Dchashmap1   == intp2Dchashmap   );
      verify(keychashset1      == keychashset      );
      verify(keyp2Dchashmap1   == keyp2Dchashmap   );
    }
  };
}
