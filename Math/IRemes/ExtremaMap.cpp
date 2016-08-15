#include "stdafx.h"
#include "BigRealRemes2.h"

static unsigned long ExtremaKeyHash(const ExtremaKey &key) {
  return key.hashCode();
}

static int ExtremaKeyCompare(const ExtremaKey &k1, const ExtremaKey &k2) {
  return k1 == k2 ? 0 : 1;
}

ExtremaHashMap::ExtremaHashMap() : HashMap<ExtremaKey,Array<ExtremaVector> >(ExtremaKeyHash,ExtremaKeyCompare) {
}

ExtremaHashMap Remes::s_extremaMap;

void ExtremaHashMap::save(const String &name) {
  tofstream out(name.cstr());
  save(out);
  out.close();
}

void ExtremaHashMap::load(const String &name) {
  if(ACCESS(name,0) < 0) {
    clear();
    return;
  }

  tifstream in(name.cstr());
  load(in);
  in.close();
}

static int extremaVectorCmp(const ExtremaVector &v1, const ExtremaVector &v2) {
  return sign(v1.getMMQuot() - v2.getMMQuot());
}

typedef Entry<ExtremaKey, Array<ExtremaVector> > ExtremaMapEntry;

void ExtremaHashMap::save(tostream &out) {
  Iterator<ExtremaMapEntry> it = entrySet().getIterator();
  out << size() << endl;
  while(it.hasNext()) {
    ExtremaMapEntry &e = it.next();
    out << e.getKey() << endl;
    Array<ExtremaVector> &a = e.getValue();
    if(a.size() > 1) {
      a.sort(extremaVectorCmp);
      while(a.size() > 1) {
        a.removeLast();
      }
    }
    out << a.size() << endl;
    for(size_t i = 0; i < a.size(); i++) {
      out << a[i];
    }
  }
}

void ExtremaHashMap::load(tistream &in) {
  clear();
  int count;
  in >> count;
  for(int k = 0; k < count; k++) {
    ExtremaKey key;
    in >> key;
    Array<ExtremaVector> a;
    int arraySize;
    in >> arraySize;
    for(int i = 0; i < arraySize; i++) {
      ExtremaVector v;
      in >> v;
      a.add(v);
    }
    put(key,a);
  }
}

tostream &operator<<(tostream &out, const ExtremaKey &key) {
  out << key.m_M << _T(" ") << key.m_K;
  return out;
}

tistream &operator>>(tistream &in, ExtremaKey &key) {
  int m, k;
  in >> m >> k;
  key = ExtremaKey(m, k);
  return in;
}

tostream &operator<<(tostream &out, const ExtremaVector &v) {
  out << FullFormatBigReal(v.getE())      << endl;
  out << FullFormatBigReal(v.getMMQuot()) << endl;
  out << v.getDimension() << endl;
  for(size_t i = 0; i < v.getDimension(); i++) {
    out << FullFormatBigReal(v[i]) << endl;
  }
  return out;
}

tistream &operator>>(tistream &in, ExtremaVector &v) {
  BigReal E, mmQuot;
  int dimension;
  in >> E >> mmQuot;
  in >> dimension;
  BigRealVector tmp;
  tmp.setDimension(dimension);
  for(int i = 0; i < dimension; i++) {
    in >> tmp[i];
  }
  v = ExtremaVector(tmp,E,mmQuot);
  return in;
}
