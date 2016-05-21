#include "stdafx.h"
#include <MyUtil.h>
#include <Random.h>
#include <ByteFile.h>
#include <Console.h>
#include "TestCollection.h"

class PointerComparator : public Comparator<void*> {
  int compare(void * const &e1, void * const  &e2) {
    return (unsigned long)e1 - (unsigned long)e2;
  }
  AbstractComparator *clone() const {
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
    _tprintf(_T("\nHeap not empty. Dangling pointers:\n"));
    for(Iterator<void*> it = getIterator(); it.hasNext();) {
      void *p = it.next();
      _tprintf(_T("%p "),p);
    }
    _tprintf(_T("\n"));
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
  f = new String(_T(""));
}

Element::Element(int v) {
/*
  if(!pointerSet.add(this)) {
    throwException(_T("Element <%p> already allocated. value=%d"),this,v);
  }
*/
  x = String(v);
  f =  new String(_T("fisk"));
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

Packer &operator>>(Packer &p,       Element &e) {
  p >> e.x >> *e.f;
  return p;
}

class KeyFormatter : public Formatter<Key> {
  String toString(const Key &key) {
    return key.toString();
  }
};

unsigned long keyHash(const Key &key) {
  return key.hashCode();
}

int keyCompare(const Key &key1, const Key &key2) {
  return key1.getValue() - key2.getValue();
}

static int KeyElementCompare(const KeyElement &k1, const KeyElement &k2) {
  return keyCompare(k1.m_key,k2.m_key);
}

typedef Set<Key>   KeySet;
typedef Array<Key> KeyArray;

static void compareSetList(KeySet &set, KeyArray &list) {
  verify(set.size() == list.size());
  if(set.hasOrder()) {
    list.sort(set.getComparator());
    int i = 0;
    for(Iterator<Key> it = set.getIterator(); it.hasNext();i++) {
      Key &k = it.next();
      verify(k == list[i]);
    }
    verify(i == set.size());
  } else {
    for(int i = 0; i < list.size(); i++) {
      const Key &key = list[i];
      verify(set.contains(key));
    }
  }
}

static void testIterator(const KeySet &set) {
  _tprintf(_T("    Testing Iterator\n"));
  int size = set.size();
  KeySet testSet(set);
  KeyArray list;

  testSet.clear();

  verify(set.size() == size);

  for(int k = 0; k < 1000; k++) {
    testSet.add(randInt() % 100000000);
  }

  for(Iterator<Key> it = testSet.getIterator(); it.hasNext(); ) {
    list.add(it.next());
  }

  while(testSet.size() > 10) {
    int i = 0;
    _tprintf(_T("    testSet.size():%6d\r"),testSet.size());
    for(Iterator<Key> it1 = testSet.getIterator(); it1.hasNext();) {
      Key &setKey = it1.next();
      Key &listKey = list[i];
      verify(setKey == listKey);
      if(rand() % 5 == 0) {
        it1.remove();
        list.removeIndex(i);
      } else {
        i++;
      }
    }
  }
  _tprintf(_T("\n"));
}

static void testRandomSample(const Collection<Key> &c) {
  _tprintf(_T("    Testing getRandomSample\n"));
  Collection<Key> S(c);
  IntArray counters;
  randomize();

#define SAMPLE_COUNT  20000
#define SOURCE_SIZE   20
#define SAMPLE_SIZE   5

  S.clear();
  for(int i = 0; i < SOURCE_SIZE; i++) {
    S.add(i);
    counters.add(0);
  }
  for(int e = 0; e < SAMPLE_COUNT; e++) {
    Collection<Key> sample = S.getRandomSample(SAMPLE_SIZE);
    for(Iterator<Key> it = sample.getIterator(); it.hasNext(); ) {
      counters[it.next().getValue()]++;
    }
  }
  for(int i = 0; i < SOURCE_SIZE; i++) {
    _tprintf(_T("%sc[%2d] = %.5lf%s"), ((i%5)==0)?_T("      "):_T(", "), i, (double)counters[i]/SAMPLE_COUNT,((i%5)==4)?_T("\n"):_T(""));
  }
}

static void testCollectionStream(const Collection<Key> &c) {
  _tprintf(_T("  Testing Collection save/load\n"));
  const String fileName = _T("c:\\temp\\testCollection\\Collection.dat");
  c.save(ByteOutputFile(fileName));
  Collection<Key> tmp(c);
  tmp.load(ByteInputFile(fileName));
  verify(tmp == c);
}

static void setTestSuite(const TCHAR *name, KeySet &set) {
  _tprintf(_T("  Testing %s\n"), name);

  const double startTime = getProcessTime();

  testIterator(set);
  testRandomSample(set);

  KeySet bigset(set);
  KeyArray list;
  int count;

  for(int k = 0; k < 10; k++) {
    _tprintf(_T("    Iteration %d/10  \r"), k);
    for(int i = 0; i < 1500; i++) {
      const int key = randInt() % 100000000;
      if(set.add(key)) {
        list.add(Key(key));
      }
    }
    compareSetList(set,list);
    for(int i = 0; i < 250 && list.size() > 5; i++) {
      int index = rand() % list.size();
      Key &key = list[index];
      verify(set.remove(key));
      list.removeIndex(index);
    }
    compareSetList(set,list);
  }

  if(set.hasOrder()) {
    _tprintf(_T("    Testing order       \r"));
    int counter = 0;
    Iterator<Key> it = set.getIterator();
    Comparator<Key> &comparator = set.getComparator();
    const Key firstKey = set.getMin();
    const Key lastKey  = set.getMax();
    const Key *last = NULL;
    while(it.hasNext()) {
      Key &key = it.next();
      if(last != NULL) {
        verify(comparator.compare(*last,key) < 0);
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
    CompactIntArray a = ((HashSet<Key>&)set).getLength();
    for(int i = 0; i < a.size(); i++) {
      _tprintf(_T("    Count(%d):%d\n"),i,a[i]);
    }
    try {
      const Key key = set.getMin();
      verify(false);
    } catch(Exception e) {
      // ignore
    }
    try {
      const Key key = set.getMax();
      verify(false);
    } catch(Exception e) {
      // ignore
    }
  }

  _tprintf(_T("    Testing set.contains     \r"));
  for(int i = 0; i < list.size(); i++) {
    const Key &key = list[i];
    verify(set.contains(key));
  }

  bigset.addAll(set);
  count = 0;
  _tprintf(_T("    Testing set.iterator1      \r"));
  for(Iterator<Key> it1 = set.getIterator(); it1.hasNext();) {
    const Key &k = it1.next();
    count++;
    verify(bigset.contains(k));
  }

  verify(count == set.size());

  _tprintf(_T("    Testing set.iterator2      \r"));
  for(Iterator<Key> it2 = bigset.getIterator(); it2.hasNext();) {
    const Key &k = it2.next();
    verify(set.contains(k));
  }

  _tprintf(_T("    Testing set.iterator3      \r"));
  for(Iterator<Key> it3 = set.getIterator(); it3.hasNext();) {
    const Key &k = it3.next();
    verify(!bigset.add(k));
  }

  _tprintf(_T("    Testing select             \r"));
  for(int i = list.size() - 1; i >= 0; i--) {
    const Key &key = list[i];
    verify(set.remove(key));
    verify(set.size() == i);
    if(set.size() != 0) {
      const Key &e = set.select();
      verify(set.contains(e));
    } else {
      try {
        const Key &e = set.select();
        verify(false);
      } catch(Exception e) {
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

  for(int i = 0; i < list.size()/2; i++) {
    set.remove(list[i]);
  }

  verify(!(set == bigset));
  verify(set != bigset);
  verify(set <= bigset);
  verify(!(set >= bigset));
  verify(set < bigset);
  verify(!(set > bigset));

  set = bigset;

  _tprintf(_T("    Testing set.removeAll         \r"));
  set.removeAll(bigset);
  verify(set.size() == 0);

  _tprintf(_T("    Testing set.clear             \r"));
  bigset.clear();
  verify(bigset.size() == 0);

  set.clear();
  _tprintf(_T("    Testing set.copyConstructor   \r"));
  KeySet set1(set);
  set.add(1);
  set.add(2);

  set1.add(1);
  set1.add(4);

  _tprintf(_T("    Testing set.intersection   \r"));
  KeySet intersectionSet = set * set1;
//  _tprintf(_T("intersect:%s\n"),intersectionSet.toString().cstr());
  verify(intersectionSet.size() == 1);

  _tprintf(_T("    Testing set.union          \r"));
  KeySet unionSet = set + set1;
//  _tprintf(_T("union:%s\n"),unionSet.toString().cstr());
  verify(unionSet.size() == 3);

  _tprintf(_T("    Testing set.difference     \r"));
  KeySet diffSet1 = set - set1;
//  _tprintf(_T("diff1:%s\n"),diffSet1.toString().cstr());
  verify(diffSet1.size() == 1);

  KeySet diffSet2 = set1 - set;
//  _tprintf(_T("diff2:%s\n"),diffSet2.toString().cstr());
  verify(diffSet2.size() == 1);

  KeySet xorSet = set ^ set1;
  verify(xorSet.size() == diffSet1.size() + diffSet2.size());
  verify(xorSet == diffSet1 + diffSet2);

  _tprintf(_T("    Testing set.copyConstructor \r"));
  KeySet set2(set);
  
  _tprintf(_T("    Testing set.retainAll       \r"));
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
  _tprintf(_T("Time(%s):%.2lf sec.    \n"), name, (getProcessTime() - startTime)/1000000);
}

typedef Map<Key, Element> KeyElementMap;

static void compareMapList(KeyElementMap &map, Array<KeyElement> &list) {
  verify(map.size() == list.size());
  if(map.hasOrder()) {
    list.sort(KeyElementCompare);
    int i = 0;
    for(Iterator<Entry<Key, Element> > it = map.entrySet().getIterator(); it.hasNext();i++) {
      Entry<Key, Element> &e = it.next();
      verify(e.getKey() == list[i].m_key && e.getValue() == list[i].m_elem);
    }
    verify(i == map.size());
  } else {
    for(int i = 0; i < list.size(); i++) {
      const KeyElement &listElement = list[i];
      const Element &e = *map.get(listElement.m_key);
      verify(e == listElement.m_elem);
    }
  }
}

static void testMapStream(const KeyElementMap &m) {
  _tprintf(_T("  Testing Map save/load\n"));
  const String fileName = _T("c:\\temp\\testCollection\\Map.dat");
  m.save(ByteOutputFile(fileName));
  KeyElementMap tmp(m);
  tmp.load(ByteInputFile(fileName));
  verify(tmp == m);
}

static void mapTestSuite(const TCHAR *name, KeyElementMap &map) {
  _tprintf(_T("  Testing %s\n"),name);

  const double startTime = getProcessTime();

  KeyElementMap bigmap(map);
  Array<KeyElement> list;
  int count;

  for(int k = 0; k < 10; k++) {
    _tprintf(_T("    Iteration %d/10  \r"), k);
    for(int i = 0; i < 1500; i++) {
      const int key = randInt() % 100000000;
      const int elem = rand() % 10000;
      if(map.put(key,elem)) {
        list.add(KeyElement(key,elem));
      }
    }
    compareMapList(map,list);
    for(int i = 0; i < 250 && list.size() > 5; i++) {
      int index = rand() % list.size();
      KeyElement &a = list[index];
      Element *e = map.get(a.m_key);
      verify(e != NULL);
      verify(*e == a.m_elem);
      verify(map.remove(a.m_key));
      list.removeIndex(index);
    }
    compareMapList(map,list);
  }

  _tprintf(_T("    Testing entrySet.Iterator      \r"));
  for(Iterator<Entry<Key, Element> > entryIterator = map.entrySet().getIterator(); entryIterator.hasNext();) {
    Entry<Key, Element> &entry = entryIterator.next();
    String dd = format(_T("(%s,%s)\n"),entry.getKey().toString().cstr(),entry.getValue().toString().cstr());
  }

  if(map.hasOrder()) {
    _tprintf(_T("    Testing map order         \r"));

    testMapStream(map);
    int counter = 0;
    Iterator<Entry<Key, Element> > it = map.entrySet().getIterator();
    Comparator<Key> &comparator = map.getComparator();
    const Key firstKey1 = map.keySet().getMin();
    const Key lastKey1  = map.keySet().getMax();
    const Key firstKey2 = map.entrySet().getMin().getKey();
    const Key lastKey2  = map.entrySet().getMax().getKey();

    const Key *last = NULL;
    while(it.hasNext()) {
      Entry<Key, Element> *e = &it.next();
      if(last != NULL) {
        verify(comparator.compare(*last,e->getKey()) < 0);
      } else {
        verify(comparator.compare(firstKey1, e->getKey()) == 0);
        verify(comparator.compare(firstKey2, e->getKey()) == 0);
      }
      last = &e->getKey();
      counter++;
    }
    verify(comparator.compare(lastKey1, *last) == 0);
    verify(comparator.compare(lastKey2, *last) == 0);
    Iterator<Entry<Key, Element> > it1 = it;
    verify(counter == list.size());
  } else {
    try {
      const Key firstKey = map.keySet().getMin();
      verify(false);
    } catch(Exception e) {
      // ignore
    }
    try {
      const Key firstKey = map.keySet().getMax();
      verify(false);
    } catch(Exception e) {
      // ignore
    }
    try {
      const Key firstKey = map.entrySet().getMin().getKey();
      verify(false);
    } catch(Exception e) {
      // ignore
    }
    try {
      const Key firstKey = map.entrySet().getMax().getKey();
      verify(false);
    } catch(Exception e) {
      // ignore
    }
  }

  for(int i = 0; i < list.size(); i++) {
    const KeyElement &e = list[i];
    verify(map.get(e.m_key) != NULL);
  }

  _tprintf(_T("    Testing map.addAll           \r"));
  bigmap.addAll(map);
  count = 0;

  _tprintf(_T("    Testing map.iterator1        \r"));
  Iterator<Entry<Key, Element> > it1 = map.entrySet().getIterator();
  while(it1.hasNext()) {
    const Key &k = it1.next().getKey();
    count++;
    verify(bigmap.get(k) != NULL);
  }
  verify(count == map.size());

  Iterator<Entry<Key, Element> > itcopy = it1;
  verify(!itcopy.hasNext());

  _tprintf(_T("    Testing map.iterator2        \r"));
  for(Iterator<Entry<Key, Element> > it2 = bigmap.entrySet().getIterator(); it2.hasNext();) {
    const Key &k = it2.next().getKey();
    verify(map.get(k) != NULL);
  }
  _tprintf(_T("    Testing map.remove           \r"));
  for(int i = list.size() - 1; i >= 0; i--) {
    const KeyElement &e = list[i];
    verify(map.remove(e.m_key));
    verify(map.size() == i);
  }
  verify(map.size() == 0);
  const KeyElement &e = list[0];
  verify(!map.remove(e.m_key));

  _tprintf(_T("    Testing map.assignment           \r"));
  map = bigmap;

  verify(map.size() == bigmap.size());

  _tprintf(_T("    Testing map.removeAll         \r"));
  map.removeAll(bigmap.keySet());
  verify(map.size() == 0);

  bigmap.clear();
  verify(bigmap.size() == 0);

  map.clear();
  KeyElementMap map1(map);
  map.put(1,-1);
  map.put(2,-2);

  map1.put(1,-1);
  map1.put(4,-4);

  _tprintf(_T("    Testing map.intersection        \r"));
  KeyElementMap Intersect = map * map1;
//  _tprintf(_T("intersect:%s\n"),Intersect.toString().cstr());
  verify(Intersect.size() == 1);

  _tprintf(_T("    Testing map.union               \r"));
  KeyElementMap Union = map + map1;
//  _tprintf(_T("union:%s\n"),Union.toString().cstr());
  verify(Union.size() == 3);

  _tprintf(_T("    Testing map.difference          \r"));
  KeyElementMap Diff1 = map - map1;
//  _tprintf(_T("diff1:%s\n"),Diff1.toString().cstr());
  verify(Diff1.size() == 1);

  KeyElementMap Diff2 = map1 - map;
//  _tprintf(_T("diff2:%s\n"),Diff2.toString().cstr());
  verify(Diff2.size() == 1);

  _tprintf(_T("    Testing map.copyConstructor    \r"));
  KeyElementMap map2(map);
  
  _tprintf(_T("    Testing map.assignment         \r"));
  map2 = map;
  map2.addAll(map1);
  verify(map2.size() == Union.size());

  map2 = map;
  const Key                 &selectedKey     = map.keySet().select();
  const Entry<Key, Element> &selectedEntry   = map.entrySet().select();
  const Key                 &SEK             = selectedEntry.getKey();
  const Element             &SEE             = selectedEntry.getValue();
  const Element             &selectedElement = map.values().select();

  _tprintf(_T("    Testing map.removeAll        \r"));
  map2.removeAll(map1.keySet());
  verify(map2.size() == Diff1.size());

  _tprintf(_T("    Testing map.valueSet         \r"));
  Collection<Element> mapValues = map.values();

  verify(mapValues.contains(-1));
  verify(mapValues.contains(-2));
  verify(!mapValues.contains(-3));
  for(Iterator<Element> eit = mapValues.getIterator(); eit.hasNext();) {
    Element &e = eit.next();
//    _tprintf(_T("%s\n"),eit.next().toString().cstr());
    eit.remove();
    Iterator<Element> itCopy(mapValues.getIterator());
    itCopy = eit;
    if(mapValues.size() != 0) {
      const Element &e = mapValues.select();
    } else {
      try {
        const Element &e = mapValues.select();
        verify(false);
      } catch(Exception e) {
        // ok !
      }
    }
  }
  verify(map.size() == 0);
  _tprintf(_T("Time(%s):%.2lf sec.  \n"), name, (getProcessTime() - startTime)/1000000);
}

void testCollection() {
  _tprintf(_T("Testing Collection\n"));

  KeyHashSet s1;
  setTestSuite(_T("HashSet"),  s1);
  KeyHashSet hs(s1);

  Array<Key> keyArray;
  testRandomSample(keyArray);

  KeyTreeSet s2;
  setTestSuite(_T("TreeSet"),  s2);
  KeyTreeSet ts(s2);

  KeyBTreeSet s3;
  setTestSuite(_T("BTreeSet"), s3);
  KeyBTreeSet bts(s3);

  KeyElementHashMap m1;
  mapTestSuite(_T("HashMap"),  m1);
  KeyElementHashMap hm(m1);

  KeyElementTreeMap m2;
  mapTestSuite(_T("TreeMap"),  m2);
  KeyElementTreeMap tm(m2);

  KeyElementBTreeMap m3;
  mapTestSuite(_T("BTreeMap"), m3);
  KeyElementBTreeMap btm(m3);

  _tprintf(_T("Collection ok!\n"));
}

