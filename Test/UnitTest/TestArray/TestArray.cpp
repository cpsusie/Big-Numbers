#include "stdafx.h"
#include <Array.h>
#include <ByteMemoryStream.h>
#include <CompressFilter.h>
#include <ProcessTools.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestArray {

#include <UnitTestTraits.h>

  class ArrayElement {
  private:
    char s[12];
    void setn(int v) {
      n = v; sprintf_s(s, "%d", n);
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
    friend Packer &operator >> (Packer &p, ArrayElement &e);
  };

  class ElemComparator : public Comparator<ArrayElement> {
  public:
    int compare(const ArrayElement &e1, const ArrayElement &e2) override {
      return e1.n - e2.n;
    }
    AbstractComparator *clone() const override {
      return new ElemComparator();
    }
  };

  class ArrayType : public Array<ArrayElement> {
  };

  static int compare1(const ArrayElement &e1, const ArrayElement &e2) {
    return e1.n - e2.n;
  }

  static int compare2(const ArrayElement **e1, const ArrayElement **e2) {
    return (*e1)->n - (*e2)->n;
  }

  static int compare3(const ArrayElement *e1, const ArrayElement *e2) {
    return e1->n - e2->n;
  }

  static int intReverseCompare(const int &key1, const int &key2) {
    return key2 - key1;
  }

  Packer &operator<<(Packer &p, const ArrayElement &e) {
    p << e.n << e.s;
    return p;
  }

  Packer &operator >> (Packer &p, ArrayElement &e) {
    p >> e.n >> e.s;
    return p;
  }

  static ElemComparator comparator;

  void listArray(const TCHAR *name, ArrayType &a) {
    OUTPUT(_T("%s:"), name);
    for(size_t i = 0; i < a.size(); i++) {
      OUTPUT(_T("%2d "), a[i].n);
    }
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
#if defined(_DEBUG)
    String line;
    line += format(_T("%2d:"), m_counter);
    for(size_t i = 0; i < a.size(); i++) {
      line += format(_T("%d "), a[i]);
    }
    INFO(_T("%s"), line.cstr());
#endif
    return true;
  }

	TEST_CLASS(TestArray)
	{
	  public:
		
    TEST_METHOD(ArrayPrimitiveOperations) {
      IntArray a;

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
          a.removeIndex(a.size());
          verify(false);
        } catch (Exception e) {
          // ok
        }
        try {
          a.insert(a.size() + 1, 1);
          verify(false);
        } catch (Exception e) {
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
      b.insert(1, -1);
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

      int key = 55;
      intptr_t index = a.binarySearch(key, intReverseCompare);
      verify(index == 44);

      index = a.search(key, intReverseCompare);
      verify(index == 44);

      key = 200;
      index = a.search(key, intReverseCompare);
      verify(index < 0);

      verify(!a.remove(1010));
      verify(a.remove(55));

      index = 0;
      for(Iterator<int> it = a.getIterator(); it.hasNext() && index < 10; index++) {
        verify(a.contains(it.next()));
      }
    }

    static void sendReceive(Packer &dst, const Packer &src) {
      ByteArray a;
      src.write(ByteMemoryOutputStream(a));
      dst.read( ByteMemoryInputStream(a));
    }

    TEST_METHOD(ArrayStream) {
      JavaRandom rnd;
      rnd.randomize();
      INFO(_T("Testing Array save/load"));
      ArrayType a;
      for(int i = 0; i < 10000; i++) {
        a.add(rnd.nextInt(1000000));
      }
      const String fileName = getTestFileName(__TFUNCTION__);
      a.save(CompressFilter(ByteOutputFile(fileName)));
      ArrayType tmp;
      tmp.load(DecompressFilter(ByteInputFile(fileName)));
      verify(tmp == a);
      INFO(_T("Testing Array Packer"));
      Packer psrc, pdst;
      psrc << a;
      sendReceive(pdst, psrc);
      pdst >> tmp;
      verify(tmp == a);
    }

    TEST_METHOD(ArraySort) {
      JavaRandom rnd;
      rnd.randomize();

      const int size = 70000;

      ArrayType a, b;
      double start;
      for(int i = 0; i < size; i++) {
        a.add(rnd.nextInt(10 * size));
      }

      verify(a.size() == size);

      b = a;
      verify(b == a);

      ArrayType c = a;
      verify(c == a);

      ArrayType d = a;
      verify(d == c);

      if(a.size() < 30) {
        listArray(_T("a"), a);
      }

      start = getProcessTime();
      a.sort(comparator);
      INFO(_T("  sort(comparator):%.3lf"), (getProcessTime() - start) / 1000000);
      start = getProcessTime();
      b.sort(compare1);
      INFO(_T("  sort(compare1)  :%.3lf"), (getProcessTime() - start) / 1000000);
      start = getProcessTime();
      c.sort(compare2);
      INFO(_T("  sort(compare2)  :%.3lf"), (getProcessTime() - start) / 1000000);

      for(size_t i = 1; i < a.size(); i++) {
        verify(comparator.compare(a[i - 1], a[i]) <= 0);
      }

      if(a != b || a != c) {
        OUTPUT(_T("Not same ordering!!"));
        if(a.size() < 30) {
          listArray(_T("a"), a);
          listArray(_T("b"), b);
          listArray(_T("c"), c);
        }
        abort();
      }

      ArrayElement tmp = a[55];
      verify(a.search(tmp, comparator) >= 0);
      verify(a.search(tmp, compare1) >= 0);
      verify(a.search(tmp, compare2) >= 0);
      verify(a.binarySearch(tmp, comparator) >= 0);
      verify(a.binarySearch(tmp, compare1) >= 0);
      verify(a.binarySearch(tmp, compare2) >= 0);
      verify(a.binaryInsert(tmp, comparator) >= 0);
      verify(a.binaryInsert(tmp, compare1) >= 0);
      verify(a.binaryInsert(tmp, compare2) >= 0);

      ArrayElement *ca = new ArrayElement[size];
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

    TEST_METHOD(ArrayPermuations) {
      IntArray a;
      for(int i = 0; i < 4; i++) {
        a.add(i);
      }
      PrintIntArray permArray;
      INFO(_T("Array permutations (%d elements):"), a.size());
      a.generateAllPermuations(permArray);
      verify(permArray.getPermutationCount() == 24);
    }

    TEST_METHOD(ArrayReverse) {
      JavaRandom rnd;
      rnd.randomize();

      IntArray a;
      for(int i = 0; i < 100; i++) {
        a.add(rnd.nextInt());
      }
      IntArray b = a;
      b.reverse();
      verify(b.size() == a.size());
      for(size_t i = 0, j = a.size()-1; i < a.size(); i++, j--) {
        verify(a[i] == b[j]);
      }
    }
  };

}
