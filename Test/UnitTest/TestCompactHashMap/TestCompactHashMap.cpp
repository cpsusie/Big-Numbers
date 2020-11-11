#include "stdafx.h"
#include <BitSet.h>
#include <ProcessTools.h>
#include <CompactHashMap.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestCompactHashMap {

#include <UnitTestTraits.h>

  template<class K, class V> void testMapStream(const CompactHashMap<K,V> &m) {
    INFO(_T("Testing save/load"));
    const String fileName = getTestFileName(__TFUNCTION__);
    m.save(ByteOutputFile(fileName));
    CompactHashMap<K,V> tmp;
    tmp.load(ByteInputFile(fileName));
    verify(tmp == m);
  }

  template<class K, class V> void compareMapList(const CompactHashMap<K,V> &map, CompactArray<MapEntry<K,V> > &list) {
    verify(map.size() == list.size());
    for(size_t i = 0; i < list.size(); i++) {
      const MapEntry<K,V> &listElement = list[i];
      const V *e = map.get(listElement.m_key);
      verify(*e == listElement.m_value);
    }
  }

  template<class T> class ValueGenerator {
  public:
    virtual T getRandom() = 0;
  };

  class Int32Generator : public ValueGenerator<CompactIntKeyType> {
  private:
    JavaRandom m_rnd;
  public:
    Int32Generator() {
      m_rnd.randomize();
    }
    CompactIntKeyType getRandom() {
      return m_rnd.nextInt();
    }
  };

  template<class K, class V> void mapTestSuite(const TCHAR *name, ValueGenerator<K> &keyGenerator, ValueGenerator<V> &valueGenerator) {
    INFO(_T("Testing %s"), name);
    const double startTime = getProcessTime();

    CompactHashMap<K,V>          map;
    CompactArray<MapEntry<K,V> > list;
    size_t                       count;

    for(int k = 0; k < 10; k++) {
      INFO(_T("Iteration %d/10"), k);
      for(int i = 0; i < 35000; i++) {
        const K key   = keyGenerator.getRandom();
        const V value = valueGenerator.getRandom();
        if(map.put(key, value)) {
          list.add(MapEntry<K,V>(key, value));
        }
      }
      testMapStream(map);
      compareMapList(map, list);

      for(int i = 0; i < 250 && list.size() > 5; i++) {
        const size_t   index = randSizet(list.size());
        MapEntry<K,V> &a     = list[index];
        const V       *e     = map.get(a.m_key);
        verify( e != nullptr);
        verify(*e == a.m_value);
        verify( map.remove(a.m_key));
        list.remove(index);
      }
      compareMapList(map, list);
    } // for

    map.clear();
    list.clear();
    verify(map.isEmpty());
    INFO(_T("Testing entrySet.Iterator"));
    for(int i = 0; i < 30; i++) {
      const K key   = keyGenerator.getRandom();
      const V value = valueGenerator.getRandom();
      if(map.put(key, value)) {
        list.add(MapEntry<K,V>(key, value));
      }
    } // for

    BitSet foundSet(map.size());
    for(auto it = map.getIterator(); it.hasNext();) {
      const Entry<K,V> &entry = it.next();
      bool found = false;
      for(size_t i = 0; i < list.size(); i++) {
        const MapEntry<K,V> &e = list[i];
        if(e.m_key == entry.getKey()) {
          verify(e.m_value == entry.getValue());
          verify(!foundSet.contains(i));
          foundSet.add(i);
          found = true;
          break;
        }
      }
      verify(found);
    } // for
    verify(foundSet.size() == list.size());

    for(size_t i = 0; i < list.size(); i++) {
      const MapEntry<K,V> &e = list[i];
      const V *v = map.get(e.m_key);
      verify(v != nullptr);
      verify(*v == e.m_value);
    }

    INFO(_T("Testing map.addAll"));
    CompactHashMap<K,V> map1;
    map1.addAll(map);
    verify(map1 == map);

    INFO(_T("Testing map.removeAll"));
    CompactArray<K> keyList;
    for(auto it = map1.getKeyIterator(); it.hasNext();) {
      keyList.add(it.next());
    }
    verify(keyList.size() == map1.size());
    map1.removeAll(keyList);
    verify(map1.size() == 0);

    INFO(_T("Testing copyConstructor"));
    CompactHashMap<K,V> map2(map);
    verify(map2 == map);

    INFO(_T("Testing assignment"));
    map2.clear();
    verify(map2.isEmpty());
    map2 = map;
    verify(map2 == map);

    INFO(_T("Testing operator=="));
    const size_t h = map2.size() / 2;
    auto itHalf = map2.getIterator();
    for(size_t i = 0; i < h; i++) {
      itHalf.next();
    }
    Entry<K,V> &e = itHalf.next();
    const V oldValue = e.getValue();
    do {
      e.getValue() = valueGenerator.getRandom();
    } while(e.getValue() == oldValue);
    verify(map2 != map);
    e.getValue() = oldValue;
    verify(map2 == map);

    verify(map2.remove(e.getKey()));
    verify(map2 != map);

    map2.clear();
    verify(map2.isEmpty());
    auto it2 = map2.getIterator();
    verify(!it2.hasNext());
    auto keyIt = map2.getKeyIterator();
    verify(!keyIt.hasNext());

    map2 = map;
    verify(map2 == map);

    INFO(_T("Testing map.iterator"));
    count = 0;
    for(auto it = map.getIterator(); it.hasNext();) {
      const Entry<K,V> &e     = it.next();
      const K          &key   = e.getKey();
      const V          *value = map2.get(key);
      verify(value != nullptr);
      verify(*value == e.getValue());
      count++;
    }
    verify(count == map.size());

    INFO(_T("Testing iterator.remove"));
    for(auto it = map2.getIterator(); it.hasNext();) {
      it.next();
      it.remove();
    }
    verify(map2.size() == 0);

    INFO(_T("Time(%s):%.2lf sec."), name, (getProcessTime() - startTime) / 1000000);
  }

  TEST_CLASS(TestCompactHashMap) {
  public:
    TEST_METHOD(CompactHashMap) {
      mapTestSuite<CompactIntKeyType,CompactIntKeyType>(_T("CompactIntKeyType"), Int32Generator(), Int32Generator());
    }
  };
}
