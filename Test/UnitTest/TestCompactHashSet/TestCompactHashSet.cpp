#include "stdafx.h"
#include <CompactHashSet.h>
#include <ProcessTools.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestCompactHashSet {

#include <UnitTestTraits.h>

  template<class KeyType> void testIterator(KeyType maxKey) {
    JavaRandom rnd;
    rnd.randomize();
    INFO(_T("Testing Iterator"));
    CompactHashSet<KeyType> testSet;
    CompactArray<  KeyType> list;

    for(int k = 0; k < 1000; k++) {
      testSet.add(rnd.nextInt(maxKey));
    }

    for(Iterator<KeyType> it = testSet.getIterator(); it.hasNext(); ) {
      list.add(it.next());
    }

    verify(list.size() == testSet.size());

    while(testSet.size() > 10) {
      int i = 0;
      INFO(_T("TestSet.size():%6d"), testSet.size());
      for(Iterator<KeyType> it1 = testSet.getIterator(); it1.hasNext();) {
        KeyType &setKey = it1.next();
        KeyType &listKey = list[i];
        verify(setKey == listKey);
        if(rnd.nextInt(5) == 0) {
          it1.remove();
          list.remove(i);
        } else {
          i++;
        }
      }
    }
  }

  template<class KeyType> void testCollectionStream(const CompactHashSet<KeyType> &set) {
    INFO(_T("Testing save/load"));
    const String fileName = getTestFileName(__TFUNCTION__);
    set.save(ByteOutputFile(fileName));
    CompactHashSet<KeyType> tmp;
    tmp.load(ByteInputFile(fileName));
    verify(tmp == set);
  }

  template<class KeyType> void compareSetList(CompactHashSet<KeyType> &set, CompactArray<KeyType> &list) {
    verify(set.size() == list.size());
    for(size_t i = 0; i < list.size(); i++) {
      const KeyType &key = list[i];
      verify(set.contains(key));
    }
  }

  template<class KeyType> void setTestSuite(const TCHAR *name, KeyType minKey, KeyType maxKey) {
    INFO(_T("Testing %s"), name);

    const double startTime = getProcessTime();

    testIterator<KeyType>(maxKey);

    CompactHashSet<KeyType> set;
    CompactHashSet<KeyType> bigset(set);
    CompactArray<KeyType>   list;
    int count;
    JavaRandom rnd;
    rnd.randomize();
    for(int k = 0; k < 10; k++) {
      INFO(_T("  Iteration %d/10"), k);
      for(int i = 0; i < 31500; i++) {
        const KeyType key = randInt(minKey,maxKey, rnd);
        if(set.add(key)) {
          list.add(key);
        }
      }
      testCollectionStream(set);
      compareSetList(set, list);
      for(int i = 0; i < 250 && list.size() > 5; i++) {
        const size_t index = randSizet(list.size(), rnd);
        KeyType &key = list[index];
        verify(set.remove(key));
        list.remove(index);
      }
      compareSetList(set, list);
    }

    CompactIntArray a = set.getLength();
    String line;
    for(size_t i = 0; i < a.size(); i++) {
      line += format(_T("Count(%zu):%d "), i, a[i]);
    }
    INFO(_T("%s"), line.cstr());

    INFO(_T("Testing set.contains"));
    for(size_t i = 0; i < list.size(); i++) {
      const KeyType &key = list[i];
      verify(set.contains(key));
    }

    bigset.addAll(set);
    count = 0;
    INFO(_T("Testing set.iterator1"));
    for(Iterator<KeyType> it1 = set.getIterator(); it1.hasNext();) {
      const KeyType &k = it1.next();
      count++;
      verify(bigset.contains(k));
    }

    verify(count == set.size());

    INFO(_T("Testing set.iterator2"));
    for(Iterator<KeyType> it2 = bigset.getIterator(); it2.hasNext();) {
      const KeyType &k = it2.next();
      verify(set.contains(k));
    }

    INFO(_T("Testing set.iterator3"));
    for(Iterator<KeyType> it3 = set.getIterator(); it3.hasNext();) {
      const KeyType &k = it3.next();
      verify(!bigset.add(k));
    }

    set = bigset;

    verify(set.size() == bigset.size());
    verify(set        == bigset       );
    verify(!(set      != bigset      ));
    verify(set        <= bigset       );
    verify(set        >= bigset       );
    verify(!(set      <  bigset      ));
    verify(!(set      >  bigset      ));

    for(size_t i = 0; i < list.size() / 2; i++) {
      set.remove(list[i]);
    }

    verify(!(set == bigset));
    verify(  set != bigset );
    verify(  set <= bigset );
    verify(!(set >= bigset));
    verify(  set <  bigset );
    verify(!(set >  bigset));

    set = bigset;

    INFO(_T("Testing set.removeAll"));
    set.removeAll(bigset);
    verify(set.size() == 0);

    INFO(_T("Testing set.clear"));
    bigset.clear();
    verify(bigset.size() == 0);

    set.clear();
    INFO(_T("Testing set.copyConstructor"));
    CompactHashSet<KeyType> set1(set);
    set.add(1);
    set.add(2);

    set1.add(1);
    set1.add(4);


    INFO(_T("Testing set.intersection"));
    CompactHashSet<KeyType> intersectionSet = set * set1;
    INFO(_T("intersect:%s\n"), intersectionSet.getIterator().toString().cstr());
    verify(intersectionSet.size() == 1);

    INFO(_T("Testing set.union"));
    CompactHashSet<KeyType> unionSet = set + set1;
    INFO(_T("union:%s\n"), unionSet.getIterator().toString().cstr());
    verify(unionSet.size() == 3);

    INFO(_T("Testing set.difference"));
    CompactHashSet<KeyType> diffSet1 = set - set1;
    INFO(_T("diff1:%s\n"), diffSet1.getIterator().toString().cstr());
    verify(diffSet1.size() == 1);

    CompactHashSet<KeyType> diffSet2 = set1 - set;
    INFO(_T("diff2:%s\n"), diffSet2.getIterator().toString().cstr());
    verify(diffSet2.size() == 1);

    INFO(_T("Testing set.copyConstructor"));
    CompactHashSet<KeyType> set2(set);

    INFO(_T("Testing set.retainAll"));
    set2 = set;
    set2.retainAll(set1);
    verify(set2.size() == intersectionSet.size());

    set2 = set;
    set2.addAll(set1);
    verify(set2.size() == unionSet.size());

    set2 = set;
    set2.removeAll(set1);
    verify(set2.size() == diffSet1.size());
  }

  TEST_CLASS(TestCompactHashSet) {
  public:

    TEST_METHOD(CompactHashSet) {
      setTestSuite<CompactIntKeyType>(_T("CompactIntKeyType"), 0, INT_MAX);
    }
  };
}
