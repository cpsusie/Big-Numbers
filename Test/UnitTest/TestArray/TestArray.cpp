#include "stdafx.h"
#include <CompressFilter.h>
#include "TestArray.h"
#include <MyAssert.h>

static int compare1(const ArrayElement &e1, const ArrayElement &e2) {
  return e1.n - e2.n;
}

static int compare2(const ArrayElement **e1, const ArrayElement **e2) {
  return (*e1)->n - (*e2)->n;
}

static int compare3(const ArrayElement *e1, const ArrayElement *e2) {
  return e1->n - e2->n;
}

Packer &operator<<(Packer &p, const ArrayElement &e) {
  p << e.n << e.s;
  return p;
}

Packer &operator>>(Packer &p, ArrayElement &e) {
  p >> e.n >> e.s;
  return p;
}

static ElemComparator comparator;

void listArray(const TCHAR *name, ArrayType &a) {
  _tprintf(_T("%s:"), name);
  for(int i = 0; i < a.size(); i++) {
    _tprintf(_T("%2d "),a[i].n);
  }
  _tprintf(_T("\n"));
}

static void testArrayStream() {
  _tprintf(_T("  Testing Array save/load\n"));
  ArrayType a;
  for(int i = 0; i < 10000; i++) {
    a.add(randInt(1000000));
  }
  const String fileName = "c:\\temp\\testCollection\\Array.dat";
  a.save(CompressFilter(ByteOutputFile(fileName)));
  ArrayType tmp;
  tmp.load(DecompressFilter(ByteInputFile(fileName)));
  verify(tmp == a);
}

static void testArraySort() {
  _tprintf(_T("Testing sort\n"));

  const int size = 70000;

  ArrayType a,b;
  double start;
  for(int i = 0; i < size; i++) {
    a.add(randInt(10*size));
  }

  verify(a.size() == size);

  b = a;
  verify(b == a);

  ArrayType c = a;
  verify(c == a);

  ArrayType d = a;
  verify(d == c);

  if(a.size() < 30) {
    listArray(_T("a"),a);
  }

  start = getProcessTime();
  a.sort(comparator);
  _tprintf(_T("  sort(comparator):%.3lf\n"), (getProcessTime() - start)/1000000);
  start = getProcessTime();
  b.sort(compare1);
  _tprintf(_T("  sort(compare1)  :%.3lf\n"),(getProcessTime() - start)/1000000);
  start = getProcessTime();
  c.sort(compare2);
  _tprintf(_T("  sort(compare2)  :%.3lf\n"),(getProcessTime() - start)/1000000);

  for(int i = 1; i < a.size(); i++) {
    verify(comparator.compare(a[i-1],a[i]) <= 0);
  }

  if(a != b || a != c) {
    _tprintf(_T("Not same ordering!!\n"));
    if(a.size() < 30) {
      listArray(_T("a"),a);
      listArray(_T("b"),b);
      listArray(_T("c"),c);
    }
    abort();
  }

  ArrayElement tmp = a[55];
  verify(a.search(      tmp,comparator) >= 0);
  verify(a.search(      tmp,compare1  ) >= 0);
  verify(a.search(      tmp,compare2  ) >= 0);
  verify(a.binarySearch(tmp,comparator) >= 0);
  verify(a.binarySearch(tmp,compare1  ) >= 0);
  verify(a.binarySearch(tmp,compare2  ) >= 0);
  verify(a.binaryInsert(tmp,comparator) >= 0);
  verify(a.binaryInsert(tmp,compare1  ) >= 0);
  verify(a.binaryInsert(tmp,compare2  ) >= 0);

  ArrayElement *ca = new ArrayElement[size];
  for(int i = 0; i < size; i++) {
    ca[i] = randInt(2*size);
  }

  start = getProcessTime();

  quickSort(ca,size,sizeof(ca[0]),(int (__cdecl *)(const void*, const void*))compare3);

  _tprintf(_T("  quickSort(compare3):%.3lf\n"),(getProcessTime() - start)/1000000);

  for(int i = 1; i < size; i++) {
    verify(compare3(&ca[i-1],&ca[i]) <= 0);
  }
  delete[] ca;
}

static int intReverseCompare(const int &key1, const int &key2) {
  return key2 - key1;
}

class PrintIntArray : public IntArray::PermutationHandler {
private:
  int m_counter;
public:
  PrintIntArray() : m_counter(0) {
  }
  int getPermutationCount() const {
    return m_counter;
  }
  bool handlePermutation(const IntArray &a);
};

bool PrintIntArray::handlePermutation(const IntArray &a) {
  m_counter++;
  _tprintf(_T("%2d:"), m_counter);
  for(int i = 0; i < a.size(); i++) {
    _tprintf(_T("%d "), a[i]);
  }
  _tprintf(_T("\n"));
  return true;
}

static void testPermuations() {
  IntArray a;
  for(int i = 0; i < 4; i++) {
    a.add(i);
  }
  PrintIntArray permArray;
  a.generateAllPermuations(permArray);
  verify(permArray.getPermutationCount() == 24);
}

void testArray() {
  _tprintf(_T("Testing Array\n"));

  testPermuations();
  IntArray a;

  for(int i = 0; i < 100; i++) {
    a.add(i);
    verify(a.last() == i);
    verify(a.size() == i+1);
    try {
      a[a.size()] = 1;
      verify(false);
    } catch(Exception e) {
      // ok
    }
    try {
      a.swap(a.size()-1,a.size());
      verify(false);
    } catch(Exception e) {
      // ok
    }
    try {
      a.removeIndex(a.size());
      verify(false);
    } catch(Exception e) {
      // ok
    }
    try {
      a.add(a.size()+1,1);
      verify(false);
    } catch(Exception e) {
      // ok
    }
  }
  verify(a.contains(99));
  verify(!a.contains(100));
  IntArray b(a);
  verify(a == b);
  b.clear();
  verify(b.size() == 0);
  verify(a != b);
  b = a;
  verify(a == b);
  b.add(1,-1);
  verify(b[0] == 0);
  verify(b[1] == -1);
  verify(b[2] == 1);
  verify(b.size() == 101);
  verify(b.last() == 99);
  

  b.removeIndex(1);
  verify(b[1] == 1);
  verify(b.size() == 100);
  verify(b.last() == 99);
  verify(a == b);

  b.swap(1,98);
  verify(a != b);
  verify(b[1] == 98);
  verify(b[98] == 1);

  b.swap(1,98);
  verify(a == b);

  a.addAll(b);
  verify(a.size() == 200);
  verify(a != b);

  a = b;
  for(int i = 99; i >= 0; i--) {
    verify(a.last() == i);
    if(a.size() > 0) {
      verify(a[0] == 0);
    }
    a.removeLast();
  }
  verify(a.size() == 0);
  a = b;
  verify(a.size() == b.size());
  a.sort(intReverseCompare);
  for(int i = 0; i < a.size(); i++) {
    verify(a[i] == b[b.size() - i - 1]);
  }
  verify(a != b);

  int key = 55;
  int index = a.binarySearch(key,intReverseCompare);
  verify(index == 44);

  index = a.search(key,intReverseCompare);
  verify(index == 44);

  key = 200;
  index = a.search(key,intReverseCompare);
  verify(index < 0);

  verify(!a.remove(1010));
  verify(a.remove(55));

  index = 0;
  for(Iterator<int> it = a.getIterator(); it.hasNext() && index < 10; index++) {
    verify(a.contains(it.next()));
  }

  testArraySort();
  testArrayStream();


  _tprintf(_T("Array ok!\n"));
}

#pragma warning(disable : 4244)

void measureArraySort() {
  FILE *f = FOPEN(_T("sortTime.dat"),_T("w"));
  for(int size = 100; size < 3000000; size *= 1.07) {
    ArrayType a;
    for(int i = 0; i < size; i++) {
      a.add(ArrayElement(randInt(2*size)));
    }

    double start = getProcessTime();
    a.sort(compare1);
    _tprintf(_T("%7d %.3lf\n"),size,(getProcessTime() - start)/1000000);
    _ftprintf(f,_T("%7d %.3lf\n"),size,(getProcessTime() - start)/1000000);

    for(int i = 1; i < a.size(); i++) {
      if(comparator.compare(a[i-1],a[i]) > 0) {
        _tprintf(_T("Array not sorted. a[%d] = %d, a[%d] = %d\n"),i-1,a[i-1].n,i,a[i].n);
        abort();
      }
    }
  }
  fclose(f);

  f = FOPEN(_T("sortCArray.dat"),_T("w"));
  for(int size = 100; size < 3000000; size *= 1.07) {
    ArrayElement *a = new ArrayElement[size];
    for(int i = 0; i < size; i++)
      a[i] = randInt(2*size);

    double start = getProcessTime();
    quickSort(a,size,sizeof(a[0]),(int (__cdecl *)(const void*, const void*))compare3);
    _tprintf(_T("%7d %.3lf\n"),size,(getProcessTime() - start)/1000000);
    _ftprintf(f,_T("%7d %.3lf\n"),size,(getProcessTime() - start)/1000000);

    for(int i = 1; i < size; i++) {
      if(comparator.compare(a[i-1],a[i]) > 0) {
        _tprintf(_T("Array not sorted. a[%d] = %d, a[%d] = %d\n"),i-1,a[i-1].n,i,a[i].n);
        abort();
      }
    }
    delete[] a;
  }
  fclose(f);
}
