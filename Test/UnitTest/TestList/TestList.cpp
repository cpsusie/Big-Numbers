#include "stdafx.h"
#include <LinkedList.h>
#include <TreeSet.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestList {

#include <UnitTestTraits.h>

  class ListElement {
  private:
    String m_s;
  public:
    ListElement(int i) {
      m_s = format(_T("List element %d"), i);
    }
    String toString() const {
      return m_s;
    }
  };

  bool operator==(const ListElement &e1, const ListElement &e2) {
    return e1.toString() == e2.toString();
  }

  template<typename S> S &operator<<(S &out, const ListElement &e) {
    out << e.toString();
    return out;
  }

  class ElementComparator : public Comparator<ListElement> {
  public:
    int compare(const ListElement &e1, const ListElement &e2) override {
      return _tcscmp(e1.toString().cstr(), e2.toString().cstr());
    }
    AbstractComparator *clone() const override {
      return new ElementComparator();
    }
  };


  TEST_CLASS(TestList) {
    public:

    TEST_METHOD(LinkedListPrimitiveOperations) {

      Array<ListElement>      array;
      LinkedList<ListElement> list;
      ElementComparator       elementComparator;
      TreeSet<ListElement>    set(elementComparator);

      for(int i = 0; i < 20; i++) {
        list.add(i);
        array.add(i);
      }


      verify(list.size() == array.size());
      for(auto it = list.getIterator(), it1 = array.getIterator(); it.hasNext();) {
        verify(it.next() == it1.next());
      }

      for(size_t i = 0; i < array.size(); i++) {
        verify(array[i] == list[i]);
      }

      list.removeIndex(4);
      array.removeIndex(4);
      list.removeIndex(13);
      array.removeIndex(13);
      list.remove(6);
      array.remove(6);
      verify(list.size() == array.size());

      for(auto it = list.getIterator(), it1 = array.getIterator(); it.hasNext();) {
        verify(it.next() == it1.next());
      }

      int i = 0;
      for(auto it = list.getIterator(), it1 = array.getIterator(); it.hasNext(); it.next(), it1.next()) {
        if (++i == 5) {
          it.remove();
          it1.remove();
          break;
        }
      }

      verify(list.size() == array.size());
      for(auto it = list.getIterator(), it1 = array.getIterator(); it.hasNext();) {
        verify(it.next() == it1.next());
      }

      array = list;
      verify(list.size() == array.size());
      array.removeIndex(0);
      list.removeFirst();

      verify(list.size() == array.size());
      for(auto it = list.getIterator(), it1 = array.getIterator(); it.hasNext();) {
        verify(it.next() == it1.next());
      }

      array.removeLast();
      list.removeLast();

      verify(list.size() == array.size());
      for(auto it = list.getIterator(), it1 = array.getIterator(); it.hasNext();) {
        verify(it.next() == it1.next());
      }

      LinkedList<ListElement> list1(array);

      verify(list1 == list);

      for(auto it = list.getIterator(); it.hasNext();) {
        verify(list1.contains(it.next()));
      }

      verify(list1.toString() == list.toString());

      set = array;

      verify(set.containsAll(array));

      array.sort(elementComparator);

      verify(set.toString() == array.toString());

      list.clear();
      verify(list.size() == 0);
    }
  };
}