#include "stdafx.h"

static const SortMethodId sortMethodTable[] = {
  SortMethodId(ID_SORT_QUICKSORTMS         , quickSortMS          )
 ,SortMethodId(ID_SORT_QUICKSORTTEMPLATES  , quickSort            )
 ,SortMethodId(ID_SORT_QUICKSORT1          , quickSort1           )
 ,SortMethodId(ID_SORT_QUICKSORT3          , quickSort3           )
 ,SortMethodId(ID_SORT_QSORT3NORECURSION   , quickSort3NoRecursion)
 ,SortMethodId(ID_SORT_QUICKSORT4          , quickSort4           )
 ,SortMethodId(ID_SORT_QUICKSORTPIVOT5     , quickSortPivot5      )
 ,SortMethodId(ID_SORT_QUICKSORT5_OPTIMAL  , quickSort5Optimal    )
 ,SortMethodId(ID_SORT_QUICKSORT_EX        , quickSortEx          )
 ,SortMethodId(ID_SORT_MULTITHREADQUICKSORT, MTQuickSort          )
 ,SortMethodId(ID_SORT_COMBSORT            , combSort             )
 ,SortMethodId(ID_SORT_HEAPSORT            , heapSort             )
 ,SortMethodId(ID_SORT_INSERTIONSORT       , insertionSort        )
 ,SortMethodId(ID_SORT_SHELLSORTSLOW       , shellSortSlow        )
 ,SortMethodId(ID_SORT_SHELLSORTFAST       , shellSortFast        )
 ,SortMethodId(ID_SORT_SHAKERSORT          , shakerSort           )
 ,SortMethodId(ID_SORT_BUBBLESORT          , bubbleSort           )
 ,SortMethodId(ID_SORT_SHORTSORTMS         , shortSortMS          )
 ,SortMethodId(ID_SORT_MERGESORT           , mergeSort            )
};

SortMethodId::SortMethodId(int methodId, SortMethod sortMethod) 
: m_methodId(methodId)
, m_sortMethod(sortMethod)
{
  m_menuText = loadString(m_methodId);
  m_name     = m_menuText;
  m_name.replace('&',_T(""));
}

const SortMethodId &SortMethodId::getMethodById(int methodId) { // static
  for(int i = 0; i < ARRAYSIZE(sortMethodTable); i++) {
    const SortMethodId &m = sortMethodTable[i];
    if(m.m_methodId == methodId) {
      return m;
    }
  }
  throwException(_T("SortMethod %d not defined"), methodId);
  return sortMethodTable[0];
}


const SortMethodId &SortMethodId::getMethodByIndex(int index) { //  static
  return sortMethodTable[index];
}

int SortMethodId::getMethodCount() { // static
  return ARRAYSIZE(sortMethodTable);
}

#ifdef _DEBUG
bool debugCheckOrder(const TCHAR *fileName, int line, void *base, int n, char *p1, char *p2) {
  const int e1 = *(int*)p1;
  const int e2 = *(int*)p2;
  if(e1 <= e2) {
    return true;
  } else {
    const size_t index1 = (int*)p1 - (int*)base;
    const size_t index2 = (int*)p2 - (int*)base;
    errorMessage(_T("Compare error in %s line %d. a[%u] (=%d) > a[%u] (=%d). nelem=%u")
                ,fileName, line
                ,(UINT)index1, e1
                ,(UINT)index2, e2
                ,n
                );
    return false;
  }
}
#endif
