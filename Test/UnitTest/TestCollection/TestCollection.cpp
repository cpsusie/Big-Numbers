#include "stdafx.h"
#include <ByteMemoryStream.h>
#include <HashMap.h>
#include <TreeMap.h>
#include <Math/Statistic.h>
#include <ProcessTools.h>
#include "MemBtree.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestCollection {

#include <UnitTestTraits.h>

  class Key {
  private:
    String x;
  public:
    Key() {
    }
    Key(int v);
    Key(const Key &src);
    ~Key();

    int getValue() const {
      return _ttoi(x.cstr());
    }

    ULONG hashCode() const {
      return x.hashCode();
    }

    String toString() const {
      return x;
    }

    friend bool operator==(const Key &k1, const Key &k2) {
      return k1.x == k2.x;
    }
    friend Packer &operator<<(Packer &p, const Key &k);
    friend Packer &operator>>(Packer &p,       Key &k);
  };

  ULONG keyHash(const Key &key) {
    return key.hashCode();
  }

  int keyCompare(const Key &key1, const Key &key2) {
    return intHashCmp(key1.getValue(), key2.getValue());
  }

  class Element {
    String x;
    String *f;
  public:
    Element();
    Element(int v);
    Element(const Element &src);
    ~Element();
    String toString() const {
      return x; /*format("%d",x);*/
    }
    friend bool operator==(const Element &e1, const Element &e2) {
      return e1.x == e2.x && *e1.f == *e2.f;
    }
    friend Packer &operator<<(Packer &p, const Element &e);
    friend Packer &operator>>(Packer &p, Element &e);
  };

  class KeyElement {
  public:
    Key     m_key;
    Element m_elem;
    KeyElement(int key, int elem) : m_key(key), m_elem(elem) {
    }
  };

  class KeyHashSet : public HashSet<Key> {
  public:
    KeyHashSet(int capacity = 31) : HashSet<Key>(keyHash, keyCompare, capacity) {
    }
  };

  class KeyTreeSet : public TreeSet<Key> {
  public:
    KeyTreeSet() : TreeSet<Key>(keyCompare) {
    }
  };

  class KeyBTreeSet : public BTreeSet<Key> {
  public:
    KeyBTreeSet() : BTreeSet<Key>(keyCompare) {
    }
  };

  class KeyElementHashMap : public HashMap<Key, Element> {
  public:
    KeyElementHashMap(int capacity = 31) : HashMap<Key, Element>(keyHash, keyCompare, capacity) {
    }
  };

  class KeyElementTreeMap : public TreeMap<Key, Element> {
  public:
    KeyElementTreeMap() : TreeMap<Key, Element>(keyCompare) {
    }
  };

  class KeyElementBTreeMap : public BTreeMap<Key, Element> {
  public:
    KeyElementBTreeMap() : BTreeMap<Key, Element>(keyCompare) {
    }
  };

  class PointerComparator : public Comparator<void*> {
    int compare(void * const &e1, void * const  &e2) override {
      return sign((BYTE*)e1 - (BYTE*)e2);
    }
    AbstractComparator *clone() const override {
      return new PointerComparator();
    }
  };

  class PointerSet : public TreeSet<void *> {
  public:
    PointerSet() : TreeSet<void*>(PointerComparator()) {
    }
    ~PointerSet();
  };

  PointerSet::~PointerSet() {
    if(size() != 0) {
      OUTPUT(_T("Heap not empty. Dangling pointers:"));
      for(auto it = getIterator(); it.hasNext();) {
        void *p = it.next();
        OUTPUT(_T("%p "), p);
      }
      verify(size() == 0);
    }
  }

  static PointerSet pointerSet;

  Key::Key(int v) {
    /*
    if(!pointerSet.add(this)) {
    throwException(_T("Key <%p> already allocated. value=%d"),this,v);
    }
    */
    x = String(v);
  }

  Key::Key(const Key &src) {
    /*
    if(!pointerSet.add(this)) {
    throwException(_T("Key <%p> already allocated."),this);
    }
    */
    x = src.x;
  }

  Key::~Key() {
    /*
    if(!pointerSet.remove(this)) {
    throwException(_T("Key <%p> not allocated."),this);
    }
    */
  }

  Packer &operator<<(Packer &p, const Key &k) {
    p << k.x;
    return p;
  }

  Packer &operator>>(Packer &p, Key &k) {
    p >> k.x;
    return p;
  }


  Element::Element() {
    f = new String(EMPTYSTRING);
  }

  Element::Element(int v) {
    /*
    if(!pointerSet.add(this)) {
    throwException(_T("Element <%p> already allocated. value=%d"),this,v);
    }
    */
    x = String(v);
    f = new String(_T("fisk"));
  }

  Element::Element(const Element &src) {
    /*
    if(!pointerSet.add(this)) {
    throwException(_T("Element <%p> already allocated."),this);
    }
    */
    x = src.x;
    f = new String(*src.f);
  }

  Element::~Element() {
    /*
    if(!pointerSet.remove(this)) {
    throwException(_T("Key <%p> not allocated."),this);
    }
    */
    delete f;
  }

  Packer &operator<<(Packer &p, const Element &e) {
    p << e.x << *e.f;
    return p;
  }

  Packer &operator >> (Packer &p, Element &e) {
    p >> e.x >> *e.f;
    return p;
  }

  class KeyFormatter : public Formatter<Key> {
    String toString(const Key &key) {
      return key.toString();
    }
  };

  static int KeyElementCompare(const KeyElement &k1, const KeyElement &k2) {
    return keyCompare(k1.m_key, k2.m_key);
  }

  typedef Set<Key>   KeySet;
  typedef Array<Key> KeyArray;

  static void compareSetList(KeySet &set, KeyArray &list) {
    verify(set.size() == list.size());
    if(set.hasOrder()) {
      list.sort(set.getComparator());
      int i = 0;
      for(auto it = set.getIterator(); it.hasNext();i++) {
        Key &k = it.next();
        verify(k == list[i]);
      }
      verify(i == set.size());
    } else {
      for(size_t i = 0; i < list.size(); i++) {
        const Key &key = list[i];
        verify(set.contains(key));
      }
    }
  }

  static void testIterator(const KeySet &set, RandomGenerator &rnd) {
    INFO(_T("Testing Iterator"));
    size_t size = set.size();
    KeySet testSet(set);
    KeyArray list;

    testSet.clear();
    verify(set.size() == size);

    for(int k = 0; k < 1000; k++) {
      testSet.add(rnd.nextInt(100000000));
    }

    for(auto it = testSet.getIterator(); it.hasNext(); ) {
      list.add(it.next());
    }

    while(testSet.size() > 10) {
      int i = 0;
      INFO(_T("TestSet.size():%6d"), testSet.size());
      for(auto it1 = testSet.getIterator(); it1.hasNext();) {
        Key &setKey = it1.next();
        Key &listKey = list[i];
        verify(setKey == listKey);
        if(rnd.nextInt(5) == 0) {
          it1.remove();
          list.removeIndex(i);
        } else {
          i++;
        }
      }
    }
  }


  template<class ValueArray> static Real sum(const ValueArray &a) {
    Real result = 0;
    for(size_t i = 0; i < a.size();) result += a[i++];
    return result;
  }

  static void testRandomSample(const Collection<Key> &c, RandomGenerator &rnd) {
    INFO(_T("Testing getRandomSample"));

#define SAMPLE_COUNT  20000
#define SOURCE_SIZE   20
#define SAMPLE_SIZE   5

    Collection<Key> S(c); // each element in range [0..SOURCE_SIZE-1]
    CompactDoubleArray counters(SOURCE_SIZE), frequencies(SOURCE_SIZE);

    S.clear();
    for(int i = 0; i < SOURCE_SIZE; i++) {
      S.add(i);
    }
    counters.insert(0,0.0,SOURCE_SIZE);
    for(size_t e = 0; e < SAMPLE_COUNT; e++) {
      Collection<Key> sample = S.getRandomSample(SAMPLE_SIZE, rnd); // sample = SAMPLE_SIZE elements in range [0..SOURCE_SIZE-1]
      for(auto it = sample.getIterator(); it.hasNext(); ) {
        counters[it.next().getValue()]++;
      }
    }
    frequencies.insert(0, 1.0 / SOURCE_SIZE, SOURCE_SIZE);
//      double sum1 = sum(counters);
//      for(int i = 0; i < SOURCE_SIZE; i++) {
//        counters[i] += (Real)(i - ((Real)SOURCE_SIZE-1)/2.0) * 10;
//      }
//      double sum2 = sum(counters);
    const double pvalue = chiSquareGoodnessOfFitTest(counters, frequencies);
    if(pvalue < 0.1) {
      OUTPUT(_T("Randomsample differs from expected with pvalue = %.6lf"), pvalue);
      OUTPUT(_T("Counters   :%s"), counters.toString().cstr());
      OUTPUT(_T("Frequencies:%s"), frequencies.toString().cstr());
    }
  }

  static void sendReceive(Packer &dst, const Packer &src) {
    ByteArray a;
    src.write(ByteMemoryOutputStream(a));
    dst.read( ByteMemoryInputStream(a));
  }

  static void testCollectionStream(const Collection<Key> &c) {
    INFO(_T("Testing Collection save/load"));
    const String fileName = getTestFileName(__TFUNCTION__);
    c.save(ByteOutputFile(fileName));
    Collection<Key> tmp(c);
    tmp.load(ByteInputFile(fileName));
    verify(tmp == c);

    INFO(_T("Testing Collection Packer"));
    Packer psrc, pdst;
    psrc << c;
    sendReceive(pdst, psrc);
    pdst >> tmp;
    verify(tmp == c);
  }

  static void setTestSuite(const TCHAR *name, KeySet &set) {
    INFO(_T("Testing %s"), name);

    JavaRandom rnd;
    rnd.randomize();

    const double startTime = getProcessTime();

    testIterator(    set, rnd);
    testRandomSample(set, rnd);

    KeySet bigset(set);
    KeyArray list;
    int count;

    for(int k = 0; k < 10; k++) {
      INFO(_T("  Iteration %d/10"), k);
      for(int i = 0; i < 1500; i++) {
        const int key = randInt() % 100000000;
        if(set.add(key)) {
          list.add(Key(key));
        }
      }
      compareSetList(set, list);
      for(int i = 0; i < 250 && list.size() > 5; i++) {
        int index = rand() % list.size();
        Key &key = list[index];
        verify(set.remove(key));
        list.removeIndex(index);
      }
      compareSetList(set, list);
    }

    if(set.hasOrder()) {
      INFO(_T("Testing order"));
      int counter = 0;
      auto it = set.getIterator();
      Comparator<Key> &comparator = set.getComparator();
      const Key firstKey = set.getMin();
      const Key lastKey = set.getMax();
      const Key *last = nullptr;
      while (it.hasNext()) {
        Key &key = it.next();
        if(last != nullptr) {
          verify(comparator.compare(*last, key) < 0);
        } else {
          verify(comparator.compare(firstKey, key) == 0);
        }
        last = &key;
        counter++;
      }
      verify(counter == list.size());
      verify(comparator.compare(lastKey, *last) == 0);
      testCollectionStream(set);
    } else {
      const CompactUIntArray a = ((HashSet<Key>&)set).getLength();
      String line;
      for(size_t i = 0; i < a.size(); i++) {
        line += format(_T("Count(%zu):%u "), i, a[i]);
      }
      INFO(_T("%s"), line.cstr());
      try {
        const Key key = set.getMin();
        verify(false);
      } catch (Exception e) {
        // ignore
      }
      try {
        const Key key = set.getMax();
        verify(false);
      } catch (Exception e) {
        // ignore
      }
    }

    INFO(_T("Testing set.contains"));
    for(size_t i = 0; i < list.size(); i++) {
      const Key &key = list[i];
      verify(set.contains(key));
    }

    bigset.addAll(set);
    count = 0;
    INFO(_T("Testing set.iterator1"));
    for(auto it1 = set.getIterator(); it1.hasNext();) {
      const Key &k = it1.next();
      count++;
      verify(bigset.contains(k));
    }
    verify(count == set.size());

    INFO(_T("Testing set.iterator2"));
    for(auto it2 = bigset.getIterator(); it2.hasNext();) {
      const Key &k = it2.next();
      verify(set.contains(k));
    }

    INFO(_T("Testing set.iterator3"));
    for(auto it3 = set.getIterator(); it3.hasNext();) {
      const Key &k = it3.next();
      verify(!bigset.add(k));
    }

    INFO(_T("Testing select"));
    for(intptr_t i = list.size() - 1; i >= 0; i--) {
      const Key &key = list[i];
      verify(set.remove(key));
      verify(set.size() == i);
      if(set.size() != 0) {
        const Key &e = set.select(rnd);
        verify(set.contains(e));
      } else {
        try {
          const Key &e = set.select(rnd);
          verify(false);
        } catch (Exception e) {
          // ok
        }
      }
    }
    verify(set.size() == 0);

    const Key &key = list[0];
    verify(!set.remove(key));

    set = bigset;

    verify(set.size() == bigset.size());
    verify(set == bigset);
    verify(!(set != bigset));
    verify(set <= bigset);
    verify(set >= bigset);
    verify(!(set < bigset));
    verify(!(set > bigset));

    for(size_t i = 0; i < list.size() / 2; i++) {
      set.remove(list[i]);
    }

    verify(!(set == bigset));
    verify(  set != bigset);
    verify(  set <= bigset);
    verify(!(set >= bigset));
    verify(  set <  bigset);
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
    KeySet set1(set);
    set.add(1);
    set.add(2);

    set1.add(1);
    set1.add(4);

    INFO(_T("Testing set.intersection"));
    KeySet intersectionSet = set * set1;
    INFO(_T("intersect:%s\n"),intersectionSet.toString().cstr());
    verify(intersectionSet.size() == 1);

    INFO(_T("Testing set.union"));
    KeySet unionSet = set + set1;
    INFO(_T("union:%s\n"),unionSet.toString().cstr());
    verify(unionSet.size() == 3);

    INFO(_T("Testing set.difference"));
    KeySet diffSet1 = set - set1;
    INFO(_T("diff1:%s\n"),diffSet1.toString().cstr());
    verify(diffSet1.size() == 1);

    KeySet diffSet2 = set1 - set;
    INFO(_T("diff2:%s\n"),diffSet2.toString().cstr());
    verify(diffSet2.size() == 1);

    KeySet xorSet = set ^ set1;
    verify(xorSet.size() == diffSet1.size() + diffSet2.size());
    verify(xorSet == diffSet1 + diffSet2);

    INFO(_T("Testing set.copyConstructor"));
    KeySet set2(set);

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

    Collection<Key> &col = set;
    INFO(_T("Time(%s):%.2lf sec."), name, (getProcessTime() - startTime) / 1000000);
  }

  typedef Map<Key, Element> KeyElementMap;

  static void compareMapList(KeyElementMap &map, Array<KeyElement> &list) {
    verify(map.size() == list.size());
    if(map.hasOrder()) {
      list.sort(KeyElementCompare);
      int i = 0;
      for(auto it = map.getIterator(); it.hasNext();i++) {
        Entry<Key, Element> &e = it.next();
        verify(e.getKey() == list[i].m_key && e.getValue() == list[i].m_elem);
      }
      verify(i == map.size());
    } else {
      for(size_t i = 0; i < list.size(); i++) {
        const KeyElement &listElement = list[i];
        const Element &e = *map.get(listElement.m_key);
        verify(e == listElement.m_elem);
      }
    }
  }

  static void testMapStream(const KeyElementMap &m) {
    INFO(_T("Testing Map save/load"));
    const String fileName = getTestFileName(__TFUNCTION__);
    m.save(ByteOutputFile(fileName));
    KeyElementMap tmp(m);
    tmp.load(ByteInputFile(fileName));
    verify(tmp == m);

    INFO(_T("Testing Map Packer"));
    Packer psrc, pdst;
    psrc << m;
    sendReceive(pdst, psrc);
    pdst >> tmp;
    verify(tmp == m);
  }

  static void mapTestSuite(const TCHAR *name, KeyElementMap &map) {
    INFO(_T("Testing %s"), name);

    const double startTime = getProcessTime();

    KeyElementMap bigmap(map);
    Array<KeyElement> list;
    int count;

    for(int k = 0; k < 10; k++) {
      INFO(_T("Iteration %d/10"), k);
      for(int i = 0; i < 1500; i++) {
        const int key = randInt() % 100000000;
        const int elem = rand() % 10000;
        if(map.put(key, elem)) {
          list.add(KeyElement(key, elem));
        }
      }
      compareMapList(map, list);
      for(int i = 0; i < 250 && list.size() > 5; i++) {
        int index = rand() % list.size();
        KeyElement &a = list[index];
        Element *e = map.get(a.m_key);
        verify(e != nullptr);
        verify(*e == a.m_elem);
        verify(map.remove(a.m_key));
        list.removeIndex(index);
      }
      compareMapList(map, list);
    }

    INFO(_T("Testing entrySet.Iterator"));
    for(auto entryIterator = map.getIterator(); entryIterator.hasNext();) {
      Entry<Key, Element> &entry = entryIterator.next();
      String dd = format(_T("(%s,%s)\n"), entry.getKey().toString().cstr(), entry.getValue().toString().cstr());
    }

    if(map.hasOrder()) {
      INFO(_T("Testing map order"));
      testMapStream(map);
      int counter = 0;
      auto it = map.getIterator();
      Comparator<Key> &comparator = map.getComparator();
      const Key firstKey1 = map.keySet().getMin();
      const Key lastKey1 = map.keySet().getMax();
      const Key firstKey2 = map.entrySet().getMin().getKey();
      const Key lastKey2 = map.entrySet().getMax().getKey();

      const Key *last = nullptr;
      while(it.hasNext()) {
        Entry<Key, Element> *e = &it.next();
        if(last != nullptr) {
          verify(comparator.compare(*last, e->getKey()) < 0);
        } else {
          verify(comparator.compare(firstKey1, e->getKey()) == 0);
          verify(comparator.compare(firstKey2, e->getKey()) == 0);
        }
        last = &e->getKey();
        counter++;
      }
      verify(comparator.compare(lastKey1, *last) == 0);
      verify(comparator.compare(lastKey2, *last) == 0);
      auto it1 = it;
      verify(counter == list.size());
    } else {
      try {
        const Key firstKey = map.keySet().getMin();
        verify(false);
      } catch (Exception e) {
        // ignore
      }
      try {
        const Key firstKey = map.keySet().getMax();
        verify(false);
      } catch (Exception e) {
        // ignore
      }
      try {
        const Key firstKey = map.entrySet().getMin().getKey();
        verify(false);
      } catch (Exception e) {
        // ignore
      }
      try {
        const Key firstKey = map.entrySet().getMax().getKey();
        verify(false);
      } catch (Exception e) {
        // ignore
      }
    }

    for(size_t i = 0; i < list.size(); i++) {
      const KeyElement &e = list[i];
      verify(map.get(e.m_key) != nullptr);
    }

    INFO(_T("Testing map.addAll"));
    bigmap.addAll(map);
    count = 0;

    INFO(_T("Testing map.iterator1"));
    auto it1 = map.getIterator();
    while(it1.hasNext()) {
      const Key &k = it1.next().getKey();
      count++;
      verify(bigmap.get(k) != nullptr);
    }
    verify(count == map.size());

    auto itcopy = it1;
    verify(!itcopy.hasNext());

    INFO(_T("Testing map.iterator2"));
    for(auto it2 = bigmap.getIterator(); it2.hasNext();) {
      const Key &k = it2.next().getKey();
      verify(map.get(k) != nullptr);
    }
    INFO(_T("Testing map.remove"));
    for(intptr_t i = list.size() - 1; i >= 0; i--) {
      const KeyElement &e = list[i];
      verify(map.remove(e.m_key));
      verify(map.size() == i);
    }
    verify(map.size() == 0);
    const KeyElement &e = list[0];
    verify(!map.remove(e.m_key));

    INFO(_T("Testing map.assignment"));
    map = bigmap;

    verify(map.size() == bigmap.size());

    INFO(_T("Testing map.removeAll"));
    map.removeAll(bigmap.keySet());
    verify(map.size() == 0);

    bigmap.clear();
    verify(bigmap.size() == 0);

    map.clear();
    KeyElementMap map1(map);
    map.put(1, -1);
    map.put(2, -2);

    map1.put(1, -1);
    map1.put(4, -4);

    INFO(_T("Testing map.intersection"));
    KeyElementMap Intersect = map * map1;
    INFO(_T("intersect:%s\n"),Intersect.toString().cstr());
    verify(Intersect.size() == 1);

    INFO(_T("Testing map.union"));
    KeyElementMap Union = map + map1;
    INFO(_T("union:%s\n"),Union.toString().cstr());
    verify(Union.size() == 3);

    INFO(_T("Testing map.difference"));
    KeyElementMap Diff1 = map - map1;
    INFO(_T("diff1:%s\n"),Diff1.toString().cstr());
    verify(Diff1.size() == 1);

    KeyElementMap Diff2 = map1 - map;
    INFO(_T("diff2:%s\n"),Diff2.toString().cstr());
    verify(Diff2.size() == 1);

    INFO(_T("Testing map.copyConstructor"));
    KeyElementMap map2(map);

    INFO(_T("Testing map.assignment"));
    map2 = map;
    map2.addAll(map1);
    verify(map2.size() == Union.size());

    map2 = map;
    const Key                 &selectedKey   = map.keySet().select();
    const Entry<Key, Element> &selectedEntry = map.entrySet().select();
    const Key                 &SEK = selectedEntry.getKey();
    const Element             &SEE = selectedEntry.getValue();
    const Element             &selectedElement = map.values().select();

    INFO(_T("Testing map.removeAll"));
    map2.removeAll(map1.keySet());
    verify(map2.size() == Diff1.size());

    INFO(_T("Testing map.valueSet"));
    Collection<Element> mapValues = map.values();

    verify(mapValues.contains(-1));
    verify(mapValues.contains(-2));
    verify(!mapValues.contains(-3));
    for(auto eit = mapValues.getIterator(); eit.hasNext();) {
      Element &e = eit.next();
      //    OUTPUT(_T("%s\n"),eit.next().toString().cstr());
      eit.remove();
      Iterator<Element> itCopy(mapValues.getIterator());
      itCopy = eit;
      if(mapValues.size() != 0) {
        const Element &e = mapValues.select();
      } else {
        try {
          const Element &e = mapValues.select();
          verify(false);
        } catch (Exception e) {
          // ok !
        }
      }
    }
    verify(map.size() == 0);
    INFO(_T("Time(%s):%.2lf sec."), name, (getProcessTime() - startTime) / 1000000);
  }

	TEST_CLASS(TestCollection) {
    public:

      TEST_METHOD(CollectionArray) {
        JavaRandom rnd;
        rnd.randomize();
        Array<Key> keyArray;
        testRandomSample(keyArray, rnd);
      }

      TEST_METHOD(CollectionHashSet) {
        KeyHashSet s1;
        setTestSuite(_T("HashSet"), s1);
        KeyHashSet hs(s1);
      }

      TEST_METHOD(CollectionTreeSet) {
        KeyTreeSet s2;
        setTestSuite(_T("TreeSet"), s2);
        KeyTreeSet ts(s2);
      }

      TEST_METHOD(CollectionBTreeSet) {
        KeyBTreeSet s3;
        setTestSuite(_T("BTreeSet"), s3);
        KeyBTreeSet bts(s3);
      }

      TEST_METHOD(CollectionHashMap) {
        KeyElementHashMap m1;
        mapTestSuite(_T("HashMap"), m1);
        KeyElementHashMap hm(m1);
      }

      TEST_METHOD(CollectionTreeMap) {
        KeyElementTreeMap m2;
        mapTestSuite(_T("TreeMap"), m2);
        KeyElementTreeMap tm(m2);
      }

      TEST_METHOD(CollectionBTreeMap) {
        KeyElementBTreeMap m3;
        mapTestSuite(_T("BTreeMap"), m3);
        KeyElementBTreeMap btm(m3);
      }
  };
}
