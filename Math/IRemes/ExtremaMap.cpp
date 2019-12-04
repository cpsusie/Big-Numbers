#include "stdafx.h"
#include "BigRealRemes2.h"

static unsigned long ExtremaKeyHash(const ExtremaKey &key) {
  return key.hashCode();
}

static int ExtremaKeyCompare(const ExtremaKey &k1, const ExtremaKey &k2) {
  return (k1 == k2) ? 0 : 1;
}


ExtremaMap::ExtremaMap() : HashMap<ExtremaKey,Array<ExtremaVector> >(ExtremaKeyHash,ExtremaKeyCompare) {
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
  out << (FullFormatBigReal&)v.getE()      << endl;
  out << (FullFormatBigReal&)v.getMMQuot() << endl;
  out << v.getDimension()                  << endl;
  for(size_t i = 0; i < v.getDimension(); i++) {
    out << (FullFormatBigReal&)v[i] << endl;
  }
  return out;
}

tistream &operator>>(tistream &in, ExtremaVector &v) {
  BigReal E, mmQuot;
  size_t dimension;
  in >> E >> mmQuot;
  in >> dimension;
  BigRealVector tmp;
  tmp.setDimension(dimension);
  for(size_t i = 0; i < dimension; i++) {
    in >> tmp[i];
  }
  v = ExtremaVector(tmp, E, mmQuot);
  return in;
}

static int extremaVectorCmp(const ExtremaVector &v1, const ExtremaVector &v2) {
  return sign(v1.getMMQuot() - v2.getMMQuot());
}

typedef Entry<ExtremaKey, Array<ExtremaVector> > ExtremaMapEntry;

void ExtremaMap::save(tostream &out) {
  for(Iterator<ExtremaMapEntry> it = entrySet().getIterator(); it.hasNext();) {
    ExtremaMapEntry &e = it.next();
    Array<ExtremaVector> &a = e.getValue();
    if(a.size() > 1) {
      a.sort(extremaVectorCmp);
      while(a.size() > 1) {
        a.removeLast();
      }
    }
  }
  out << *this;
}

void ExtremaMap::load(tistream &in) {
  clear();
  in >> *this;
}

void ExtremaMap::setName(const String &name) {
  if (name != m_name) {
    clear();
    m_saveCount = m_updateCount = 0;
    m_name      = name;
    if (fileExist()) {
      load();
    }
  }
}

void ExtremaMap::save() {
  if (m_updateCount == m_saveCount) { // no updates
    return;
  }
  tofstream out(m_name.cstr());
  save(out);
  out.close();
  m_saveCount = m_updateCount;
}

void ExtremaMap::load() {
  clear();
  m_saveCount = m_updateCount = 0;

  if(!fileExist()) {
    return;
  }

  tifstream in(m_name.cstr());
  load(in);
  in.close();
}

bool ExtremaMap::fileExist() const {
  return ACCESS(m_name,0) >= 0;
}

const Array<ExtremaVector> *ExtremaMap::get(UINT M, UINT K) const {
  return __super::get(ExtremaKey(M, K));
}

void ExtremaMap::put(UINT M, UINT K, const ExtremaVector &v) {
  const ExtremaKey key(M, K);
  Array<ExtremaVector> *a = __super::get(key);
  if(a != NULL) {
    a->add(v);
  } else {
    Array<ExtremaVector> a;
    a.add(v);
    __super::put(key, a);
  }
  m_updateCount++;
}
