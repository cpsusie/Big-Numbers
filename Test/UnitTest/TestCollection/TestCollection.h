#pragma once

#include <HashMap.h>
#include <TreeMap.h>
#include "MemBtree.h"

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

  unsigned long hashCode() const {
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
  friend Packer &operator>>(Packer &p,       Element &e);
};

class KeyElement {
public:
  Key     m_key;
  Element m_elem;
  KeyElement(int key, int elem) : m_key(key), m_elem(elem) {
  }
};

unsigned long keyHash(const Key &key);
int keyCompare(const Key &key1, const Key &key2);

class KeyHashSet : public HashSet<Key> {
public:
  KeyHashSet(int capacity = 31) : HashSet<Key>(keyHash,keyCompare,capacity) {
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

class KeyElementHashMap : public HashMap<Key,Element> {
public:
  KeyElementHashMap(int capacity = 31) : HashMap<Key,Element>(keyHash,keyCompare,capacity) {
  }
};

class KeyElementTreeMap : public TreeMap<Key,Element> {
public:
  KeyElementTreeMap() : TreeMap<Key,Element>(keyCompare) {
  }
};

class KeyElementBTreeMap : public BTreeMap<Key,Element> {
public:
  KeyElementBTreeMap() : BTreeMap<Key,Element>(keyCompare) {
  }
};


void testCollection();
