#include "stdafx.h"
#include <CompactArray.h>
#include <CommonHashFunctions.h>
#include <ProcessTools.h>
#include <Math/Statistic.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestCompactArray {		

#include <UnitTestTraits.h>

  class CompactArrayElement {
  private:
    char s[12];
    void setn(int v) {
      n = v; sprintf_s(s, "%d", n);
    }
  public:
    int n;
    CompactArrayElement(const CompactArrayElement &a) {
      setn(a.n);
    }
    CompactArrayElement(int n = 0) {
      setn(n);
    }
    ~CompactArrayElement() {
    }
    CompactArrayElement &operator=(const CompactArrayElement &e) {
      setn(e.n);
      return *this;
    }
    CompactArrayElement &operator=(int n) {
      setn(n);
      return *this;
    }
    bool operator==(const CompactArrayElement &e) const {
      return n == e.n;
    }
    String toString() const {
      return s;
    }
    friend Packer &operator<<(Packer &p, const CompactArrayElement &e) {
      p << e.s << e.n;
      return p;
    }
    friend Packer &operator >> (Packer &p, CompactArrayElement &e) {
      p >> e.s >> e.n;
      return p;
    }
  };

  class CompactArrayElemComparator : public Comparator<CompactArrayElement> {
  public:
    int compare(const CompactArrayElement &e1, const CompactArrayElement &e2) {
      return e1.n - e2.n;
    }
    AbstractComparator *clone() const {
      return new CompactArrayElemComparator;
    }
  };

  static CompactArrayElemComparator comparator;

  static int compare1(const CompactArrayElement &e1, const CompactArrayElement &e2) {
    return e1.n - e2.n;
  }

  static int compare3(const CompactArrayElement *e1, const CompactArrayElement *e2) {
    return e1->n - e2->n;
  }

  class CompactArrayType : public CompactArray<CompactArrayElement> {
  };

  class PrintCompactIntArray : public CompactIntArray::PermutationHandler {
  private:
    int m_counter;
  public:
    PrintCompactIntArray() : m_counter(0) {
    }
    int getPermutationCount() const {
      return m_counter;
    }
    bool handlePermutation(const CompactIntArray &a);
  };

  bool PrintCompactIntArray::handlePermutation(const CompactIntArray &a) {
    m_counter++;
#if defined(_DEBUG)
    String line = format(_T("%2d:"), m_counter);
    for(size_t i = 0; i < a.size(); i++) {
      line += format(_T("%d "), a[i]);
    }
    INFO(_T("%s"), line.cstr());
#endif
    return true;
  }


	TEST_CLASS(TestCompactArray) {
    public:

    void listCompactArray(const TCHAR *name, CompactArrayType &a) {
      INFO(_T("%s:%s"), name, a.toString().cstr());

      /*
      for(int i = 0; i < a.size(); i++) {
      _tprintf(_T("%2d "),a[i].n);
      }
      _tprintf(_T("\n"));
      */
    }

    static void testSaveLoad(const String &fileName) {
      JavaRandom rnd;
      rnd.randomize();
      CompactArrayType a;
      int es = sizeof(CompactArrayElement);
      INFO(_T("sizeof(CompactArrayElement):%d"), es);
      for(int i = 0; i < 2000; i++) {
        a.add(CompactArrayElement(rnd.nextInt(100000)));
      }
      a.save(ByteOutputFile(fileName));

      CompactArrayType loaded;
      loaded.load(ByteInputFile(fileName));


      verify(loaded == a);
      loaded.load(ByteInputFile(fileName));
      verify(loaded == a);

      CompactFileArray<CompactArrayElement> cfArray(fileName, 0);
      verify(cfArray.size() == loaded.size());

      for(int i = 0; i < 1000; i++) {
        const size_t index = randSizet(cfArray.size(),rnd);
        const CompactArrayElement &cfElement = cfArray[index];
        const CompactArrayElement &element = loaded[index];
        verify(cfElement == element);
      }
    }

    TEST_METHOD(CompactArrayRandomSample) {
      JavaRandom rnd;
      rnd.randomize();

#define SAMPLE_COUNT  20000
#define SOURCE_SIZE   20
#define SAMPLE_SIZE   5

      CompactIntArray S(SOURCE_SIZE); // each element in range [0..SOURCE_SIZE-1]
      CompactDoubleArray counters(SOURCE_SIZE), frequencies(SOURCE_SIZE);
      for(int i = 0; i < SOURCE_SIZE; i++) {
        S.add(i);
      }
      counters.add(0,0.0, SOURCE_SIZE);
      for(int e = 0; e < SAMPLE_COUNT; e++) {
        const CompactIntArray sample = S.getRandomSample(SAMPLE_SIZE, rnd);
        for(const int *ep = &sample.first(), *endp = &sample.last(); ep <= endp;) {
          counters[*(ep++)]++;
        }
      }
      frequencies.add(0, 1.0 / SOURCE_SIZE, SOURCE_SIZE);
      const double pvalue = chiSquareGoodnessOfFitTest(counters, frequencies);
      if(pvalue < 0.1) {
        OUTPUT(_T("Randomsample differs from expected with pvalue = %.6lf"), pvalue);
        OUTPUT(_T("Counters   :%s"), counters.toStringBasicType().cstr());
        OUTPUT(_T("Frequencies:%s"), frequencies.toStringBasicType().cstr());
      }
      INFO(_T("Random samples"));
      for(int i = 0; i < SOURCE_SIZE; i++) {
        INFO(_T("    Counters[%2d] = %.5lf"), i, (double)counters[i] / SAMPLE_COUNT);
      }
      /*
      for(int k = 0; k < 10; k++) {
      IntArray sample = S.getRandomSample(k, &rnd);
      _tprintf(_T("sample(%d):%s\n"), k, sample.toStringBasicType().cstr());
      }
      */
    }

    TEST_METHOD(CompactArraySort) {
      JavaRandom rnd;
      rnd.randomize();
      const int size = 50000;

      CompactArrayType a, b;
      double start;
      for(int i = 0; i < size; i++) {
        a.add(rnd.nextInt() % (10 * size));
      }

      verify(a.size() == size);

      b = a;
      verify(b == a);

      CompactArrayType c = a;
      verify(c == a);

      if(a.size() < 30) {
        listCompactArray(_T("a"), a);
      }

      start = getProcessTime();
      a.sort(comparator);
      INFO(_T("  sort(comparator):%.3lf"), (getProcessTime() - start) / 1000000);
      start = getProcessTime();
      b.sort(compare1);
      INFO(_T("  sort(compare1)  :%.3lf"), (getProcessTime() - start) / 1000000);
      start = getProcessTime();

      for(size_t i = 1; i < a.size(); i++) {
        verify(comparator.compare(a[i - 1], a[i]) <= 0);
      }

      if(a != b) {
        OUTPUT(_T("Not same ordering!!"));
        if(a.size() < 30) {
          listCompactArray(_T("a"), a);
          listCompactArray(_T("b"), b);
        }
        verify(false);
      }

      /*
      CompactArrayElement tmp = a[55];
      verify(a.search(      tmp,comparator) >= 0);
      verify(a.search(      tmp,compare1  ) >= 0);
      verify(a.search(      tmp,compare2  ) >= 0);
      verify(a.binarySearch(tmp,comparator) >= 0);
      verify(a.binarySearch(tmp,compare1  ) >= 0);
      verify(a.binarySearch(tmp,compare2  ) >= 0);
      verify(a.binaryInsert(tmp,comparator) >= 0);
      verify(a.binaryInsert(tmp,compare1  ) >= 0);
      verify(a.binaryInsert(tmp,compare2  ) >= 0);
      */

      CompactArrayElement *ca = new CompactArrayElement[size];
      for(int i = 0; i < size; i++) {
        ca[i] = rnd.nextInt(2 * size);
      }

      start = getProcessTime();

      quickSort(ca, size, sizeof(ca[0]), (int(__cdecl *)(const void*, const void*))compare3);

      INFO(_T("  quickSort(compare3):%.3lf"), (getProcessTime() - start) / 1000000);

      for(int i = 1; i < size; i++) {
        verify(compare3(&ca[i - 1], &ca[i]) <= 0);
      }
      delete[] ca;
    }

    static int intReverseCompare(const int &key1, const int &key2) {
      return key2 - key1;
    }

    static intptr_t linearSearchLE(const CompactIntArray &a, int e) {
      for(size_t i = a.size(); i--;) {
        if(a[i] <= e) {
          return i;
        }
      }
      return -1;
    }

    static intptr_t linearSearchGE(const CompactIntArray &a, int e) {
      size_t i;
      for(i = 0; i < a.size(); i++) {
        if(a[i] >= e) {
          return i;
        }
      }
      return i;
    }

    TEST_METHOD(CompactArrayBinarySearch) {
      JavaRandom rnd;
      rnd.randomize();
      CompactIntArray a;
      for(int i = 0; i < 40; i++) {
        a.add(i / 3);
      }
      //  _tprintf(_T("a:%s\n"), a.toStringBasicType().cstr());
      const int maxValue = a.last();
      for(int e = -1; e <= maxValue + 1; e++) {
        intptr_t indexB = a.binarySearchLE(e, intHashCmp);
        intptr_t indexL = linearSearchLE(a, e);
        verify(indexB == indexL);

        indexB = a.binarySearchGE(e, intHashCmp);
        indexL = linearSearchGE(a, e);
        verify(indexB == indexL);
      }

      a.clear();
      for(int i = 0; i < 20; i++) {
        a.add(rnd.nextInt(10000000));
      }
      a.sort(intHashCmp);
      for(size_t i = a.size() - 1; i--;) {
        if(a[i] == a[i + 1]) {
          a.remove(i);
        }
      }
      /*
      for(i = 0; i < a.size(); i++) {
      _tprintf(_T("%2d:%d\n"), i,a[i]);
      }
      */
      for(size_t i = 0; i < a.size(); i++) {
        const int v = a[i];
        const intptr_t index = a.binarySearch(v, intHashCmp);
        verify(index == i);
      }
      int v = a[0] - 1;
      verify(a.binarySearch(v, intHashCmp) < 0);
      v = a.last() + 1;
      verify(a.binarySearch(v, intHashCmp) < 0);
    }

    TEST_METHOD(CompactArrayPacking) {
      JavaRandom rnd;
      rnd.randomize();
      CompactArrayType a, b;
      const String fileName = getTestFileName(__TFUNCTION__);
      for(int i = 0; i < 1000; i++) {
        a.add(rnd.nextInt(1000000));
      }
      b = a;
      Packer packer;
      packer << a;
      packer.write(ByteOutputFile(fileName));
      packer.clear();
      packer.read(ByteInputFile(fileName));
      packer >> a;
      verify(a == b);
    }

    TEST_METHOD(CompactArrayPermutations) {
      CompactIntArray a;
      for(int i = 0; i < 4; i++) {
        a.add(i);
      }
      PrintCompactIntArray permArray;
      a.generateAllPermuations(permArray);
      verify(permArray.getPermutationCount() == 24);
    }

    TEST_METHOD(CompactArrayReverse) {
      JavaRandom rnd;
      rnd.randomize();
      CompactIntArray a;
      for(int i = 0; i < 100; i++) {
        a.add(rnd.nextInt());
      }
      CompactIntArray b = a;
      b.reverse();
      verify(b.size() == a.size());
      for(size_t i = 0, j = a.size()-1; i < a.size(); i++, j--) {
        verify(a[i] == b[j]);
      }
    }

    TEST_METHOD(CompactArrayTestEnhancedForLoop) {
      CompactIntArray a, b;
      for(int i = 0; i < 20; i++) {
        a.add(i);
      }
      const CompactIntArray ca = a;
      for(int v : a) {
        b.add(v);
      }
      verify(b == a);
      b.clear();
      for(int v : ca) {
        b.add(v);
      }
      verify(b == ca);
      a.clear();
      b.clear();
      for(int v : a) {
        b.add(v);
      }
      verify(b == a);

    }

    TEST_METHOD(CompactArrayIterator) {
      CompactIntArray a;

      for(int i = 0; i < 20; i++) {
        a.add(i);
      }
      Iterator<int> it;
      for(it = a.getIterator(); it.hasNext();) {
        int v = it.next();
        if(v & 1) it.remove();
      }
      String line = _T("Iterator:");
      int count = 0;
      int first, last;
      for(it = a.getIterator(); it.hasNext();) {
        int v = it.next();
        if(count++ == 0) {
          first = v;
        }
        last = v;
        line += format(_T(" %d"), v);
      }
      INFO(_T("%s"), line.cstr());
      verify(count == 10);
      verify(first == 0 );
      verify(last  == 18);
      it = a.getIterator();
      int v = it.next();
      a.remove(0);
      try {
        it.remove();
        it.next();
        verify(false);
      } catch (Exception e) {
        // ignore. ok
      }
    }

    TEST_METHOD(CompactArraySaveLoad) {
      const String fileName = getTestFileName(__TFUNCTION__);
      testSaveLoad(fileName);
      unlink(fileName);
    }

    TEST_METHOD(CompactArrayPrimitiveOperations) {

      CompactIntArray a;

      for(int i = 0; i < 100; i++) {
        a.add(i);
        verify(a.last() == i);
        verify(a.size() == i + 1);
        try {
          a[a.size()] = 1;
          verify(false);
        } catch (Exception e) {
          // ok
        }
        try {
          a.swap(a.size() - 1, a.size());
          verify(false);
        } catch (Exception e) {
          // ok
        }
        try {
          a.remove(a.size());
          verify(false);
        } catch (Exception e) {
          // ok
        }
        try {
          a.add(a.size() + 1, 1);
          verify(false);
        } catch (Exception e) {
          // ok
        }
      }
      verify(a.contains(99));
      verify(!a.contains(100));
      CompactIntArray b(a);
      verify(a == b);
      b.clear();
      verify(b.size() == 0);
      verify(a != b);
      b = a;
      verify(a == b);
      b.add(1, -1);
      verify(b[0] == 0);
      verify(b[1] == -1);
      verify(b[2] == 1);
      verify(b.size() == 101);
      verify(b.last() == 99);


      b.remove(1);
      verify(b[1] == 1);
      verify(b.size() == 100);
      verify(b.last() == 99);
      verify(a == b);

      b.swap(1, 98);
      verify(a != b);
      verify(b[1] == 98);
      verify(b[98] == 1);

      b.swap(1, 98);
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
      for(size_t i = 0; i < a.size(); i++) {
        verify(a[i] == b[b.size() - i - 1]);
      }
      verify(a != b);

      /*
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
      for(Iterator<int> it = a.getIterator(); it.hasNext() && index < 10; index++)
      verify(a.contains(it.next()));
      */

    }

#pragma warning(disable : 4244)

    TEST_METHOD(CompactArrayMeasureSort) {
      JavaRandom rnd;
      rnd.randomize();

      OUTPUT(_T("%7s SortTime (compare1)"), _T("Size"));
      for(int size = 100; size < 300000; size *= 1.07) {
        CompactArrayType a;
        for(int i = 0; i < size; i++) {
          a.add(CompactArrayElement(rnd.nextInt(2 * size)));
        }

        const double start = getProcessTime();
        a.sort(compare1);
        OUTPUT(_T("%7d %.3lf"), size, (getProcessTime() - start) / 1000000);

        for(size_t i = 1; i < a.size(); i++) {
          if(comparator.compare(a[i-1], a[i]) > 0) {
            OUTPUT(_T("Array not sorted. a[%d] = %d, a[%d] = %d"), i - 1, a[i - 1].n, i, a[i].n);
            verify(false);
          }
        }
      }

      OUTPUT(_T("%7s SortTime (quickSort(compare3)"), _T("Size"));
      for(int size = 100; size < 300000; size *= 1.07) {
        CompactArrayElement *a = new CompactArrayElement[size];
        for(int i = 0; i < size; i++) {
          a[i] = rnd.nextInt(2 * size);
        }

        const double start = getProcessTime();
        quickSort(a, size, sizeof(a[0]), (int(__cdecl *)(const void*, const void*))compare3);
        OUTPUT(_T("%7d %.3lf"), size, (getProcessTime() - start) / 1000000);

        for(int i = 1; i < size; i++) {
          if(comparator.compare(a[i-1], a[i]) > 0) {
            OUTPUT(_T("Array not sorted. a[%d] = %d, a[%d] = %d"), i - 1, a[i - 1].n, i, a[i].n);
            verify(false);
          }
        }
        delete[] a;
      }
    }
  };
}
