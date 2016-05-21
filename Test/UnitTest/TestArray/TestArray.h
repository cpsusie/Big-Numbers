#pragma once

#include <MyUtil.h>

class ArrayElement {
private:
  char s[12];
  void setn(int v) {
    n = v; sprintf(s,"%d",n);
  }
public:
  int n;
  ArrayElement(const ArrayElement &a) { 
    setn(a.n);
  }
  ArrayElement(int n = 0) { 
    setn(n);
  }
  ~ArrayElement() {
  }
  ArrayElement &operator=(const ArrayElement &e) {
    setn(e.n);
    return *this;
  }
  ArrayElement &operator=(int n) {
    setn(n);
    return *this;
  }
  bool operator==(const ArrayElement &e) const {
    return n == e.n;
  }
  friend Packer &operator<<(Packer &p, const ArrayElement &e);
  friend Packer &operator>>(Packer &p,       ArrayElement &e);
};

class ElemComparator : public Comparator<ArrayElement> {
public:
  int compare(const ArrayElement &e1, const ArrayElement &e2) {
    return e1.n - e2.n;
  }
  AbstractComparator *clone() const {
    return new ElemComparator();
  }
};

class ArrayType : public Array<ArrayElement> {
};

void listArray(const char *name, ArrayType &a);

void testArray();
void measureArraySort();
