#pragma once

#include <Comparator.h>

typedef void (*SortMethod)(void *, size_t nelem, size_t width, AbstractComparator &comparator);

void quickSortMS(            void *base, size_t nelem, size_t width, AbstractComparator &comparator);
void quickSort1(             void *base, size_t nelem, size_t width, AbstractComparator &comparator);
void quickSort3(             void *base, size_t nelem, size_t width, AbstractComparator &comparator);
void quickSort3NoRecursion(  void *base, size_t nelem, size_t width, AbstractComparator &comparator);
void quickSort4OptimalPivot3(void *base, size_t nelem, size_t width, AbstractComparator &comparator);
void quickSort5OptimalPivot3(void *base, size_t nelem, size_t width, AbstractComparator &comparator);
void quickSort5OptimalPivot5(void *base, size_t nelem, size_t width, AbstractComparator &comparator);
void quickSortRandom(        void *base, size_t nelem, size_t width, AbstractComparator &comparator);
void quickSortEx(            void *base, size_t nelem, size_t width, AbstractComparator &comparator);
void MTQuickSort(            void *base, size_t nelem, size_t width, AbstractComparator &comparator);
void combSort(               void *base, size_t nelem, size_t width, AbstractComparator &comparator);
void combSort11(             void *base, size_t nelem, size_t width, AbstractComparator &comparator);
void heapSort(               void *base, size_t nelem, size_t width, AbstractComparator &comparator);
void insertionSort(          void *base, size_t nelem, size_t width, AbstractComparator &comparator);
void shellSortSlow(          void *base, size_t nelem, size_t width, AbstractComparator &comparator);
void shellSortFast(          void *base, size_t nelem, size_t width, AbstractComparator &comparator);
void insertionSort(          void *base, size_t nelem, size_t width, AbstractComparator &comparator);
void shakerSort(             void *base, size_t nelem, size_t width, AbstractComparator &comparator);
void bubbleSort(             void *base, size_t nelem, size_t width, AbstractComparator &comparator);
void shortSortMS(            void *base, size_t nelem, size_t width, AbstractComparator &comparator);
void mergeSort(              void *base, size_t nelem, size_t width, AbstractComparator &comparator);

void swap(register char *p1, register char *p2, size_t width);

typedef struct {
  void  *m_base;
  size_t m_size;
} QSortStackElement;

#define DECLARE_STACK(name, size) QSortStackElement _##name##Buffer[size], *_##name##Top; _##name##Top = _##name##Buffer;
#define ISEMPTY(name)             (_##name##Top == _##name##Buffer)
#define PUSH(name, base, n)       { _##name##Top->m_base = (void*)base;(_##name##Top++)->m_size = n;    }
#define POP( name, base, n, type) { base = (type*)((--_##name##Top)->m_base); n = _##name##Top->m_size; }

#define PNEEDSWAP( p1, p2)        (comparator.cmp(  p1, p2) > 0)
#define MPNEEDSWAP(p1, p2)        (m_comparator.cmp(p1, p2) > 0)
#define NEEDSWAP(i,j)             PNEEDSWAP( p##i,p##j)
#define MNEEDSWAP(i,j)            MPNEEDSWAP(p##i,p##j)

#define EPTR(n)                  ((char*)base + width*(n))
#define PSWAP( p1, p2)           swap(p1, p2, width)
#define SWAP(i,j)                PSWAP(p##i,p##j)
#define PSORT2(p1, p2)           { if(PNEEDSWAP(p1, p2)) PSWAP(p1, p2); }
#define SORT2( i,  j)            { char *p1 = EPTR(i), *p2 = EPTR(j); PSORT2(p1,p2); }
#define PASSIGN(dst, src)        memcpy(dst, src, width)
#define ASSIGN(a, b)             PASSIGN(p##a, p##b)
#define PERMUTER3(a, b, c)       { PASSIGN(pivot, p##a); ASSIGN(a,b); ASSIGN(b,c);                           PASSIGN(p##c, pivot); }
#define PERMUTER4(a, b, c, d)    { PASSIGN(pivot, p##a); ASSIGN(a,b); ASSIGN(b,c); ASSIGN(c,d);              PASSIGN(p##d, pivot); }
#define PERMUTER5(a, b, c, d, e) { PASSIGN(pivot, p##a); ASSIGN(a,b); ASSIGN(b,c); ASSIGN(c,d); ASSIGN(d,e); PASSIGN(p##e, pivot); }

#define SORT3OPT(a, b, c) {                                   \
  char *p0 = EPTR(a), *p1 = EPTR(b), *p2 = EPTR(c);           \
  if(NEEDSWAP(0, 1)) {                                        \
    if(NEEDSWAP(1, 2)) {                                      \
      SWAP(0, 2);                                             \
    } else if(NEEDSWAP(0, 2)) {                               \
      PERMUTER3(1,2,0);                                       \
    } else {                                                  \
      SWAP(0, 1);                                             \
    }                                                         \
  } else if(NEEDSWAP(1, 2)) {                                 \
    if(NEEDSWAP(0, 2)) {                                      \
      PERMUTER3(2,1,0);                                       \
    } else {                                                  \
      SWAP(1, 2);                                             \
    }                                                         \
  }                                                           \
}

#define SORT4OPT(a, b, c, d, exitStmt) {                      \
  char *p0 = EPTR(a), *p1 = EPTR(b), *p2 = EPTR(c);           \
  char *p3 = EPTR(d);                                         \
  if(NEEDSWAP(0, 1)) {                                        \
    if(NEEDSWAP(2, 3)) {                                      \
      if(NEEDSWAP(0, 2)) {                                    \
        SWAP(0, 3); SWAP(1, 2);                               \
        if(NEEDSWAP(0, 2)) {                                  \
          PERMUTER3(0, 2, 1);                                 \
        } else {                                              \
          PSORT2(p1, p2);                                     \
        }                                                     \
        exitStmt;                                             \
      } else {                                                \
        SWAP(0, 1); SWAP(2, 3);                               \
      }                                                       \
    } else {                                                  \
      if(NEEDSWAP(0, 3)) {                                    \
        PERMUTER4(0, 2, 1, 3);                                \
      } else {                                                \
        SWAP(0, 1);                                           \
      }                                                       \
    }                                                         \
  } else {                                                    \
    if(NEEDSWAP(2, 3)) {                                      \
      if(NEEDSWAP(1, 2)) {                                    \
        PERMUTER4(0, 3, 1, 2);                                \
      } else {                                                \
        SWAP(2, 3);                                           \
      }                                                       \
    } else {                                                  \
      if(NEEDSWAP(1, 3)) {                                    \
        SWAP(0, 2); SWAP(1, 3);                               \
      }                                                       \
    }                                                         \
  }                                                           \
  if(NEEDSWAP(1, 2)) {                                        \
    if(NEEDSWAP(0, 2)) {                                      \
      PERMUTER3(0, 2, 1)                                      \
    } else {                                                  \
      SWAP(1, 2);                                             \
    }                                                         \
  }                                                           \
}

#define SORT5OPT(a, b, c, d, e, exitStmt) {                   \
  char *p0 = EPTR(a), *p1 = EPTR(b), *p2 = EPTR(c);           \
  char *p3 = EPTR(d), *p4 = EPTR(e);                          \
  if(NEEDSWAP(0, 1)) {                                        \
    if(NEEDSWAP(2, 3)) {                                      \
      if(NEEDSWAP(0, 2)) {                                    \
        SWAP(0, 3); SWAP(1, 2);                               \
      } else {                                                \
        SWAP(0, 1); SWAP(2, 3);                               \
      }                                                       \
    } else {                                                  \
      if(NEEDSWAP(0, 3)) {                                    \
        PERMUTER4(0, 2, 1, 3);                                \
      } else {                                                \
        SWAP(0, 1);                                           \
      }                                                       \
    }                                                         \
  } else {                                                    \
    if(NEEDSWAP(2, 3)) {                                      \
      if(NEEDSWAP(1, 2)) {                                    \
        PERMUTER4(0, 3, 1, 2);                                \
      } else {                                                \
        SWAP(2, 3);                                           \
      }                                                       \
    } else {                                                  \
      if(NEEDSWAP(1, 3)) {                                    \
        SWAP(0, 2); SWAP(1, 3);                               \
      }                                                       \
    }                                                         \
  }                                                           \
  if(NEEDSWAP(1, 4)) {                                        \
    if(NEEDSWAP(0, 4)) {                                      \
      PERMUTER5(0, 4, 3, 2, 1);                               \
    } else {                                                  \
      PERMUTER4(1, 4, 3, 2);                                  \
    }                                                         \
  } else {                                                    \
    if(NEEDSWAP(3, 4)) {                                      \
      PERMUTER3(2, 4, 3);                                     \
    } else {                                                  \
      if(NEEDSWAP(1, 2)) {                                    \
        if(NEEDSWAP(0, 2)) {                                  \
          PERMUTER3(0, 2, 1);                                 \
        } else {                                              \
          SWAP(1, 2);                                         \
        }                                                     \
      } else {                                                \
        PSORT2(p2, p3);                                       \
      }                                                       \
      exitStmt;                                               \
    }                                                         \
  }                                                           \
  if(NEEDSWAP(1, 3)) {                                        \
    if(NEEDSWAP(0, 3)) {                                      \
      PERMUTER4(0, 3, 2, 1);                                  \
    } else {                                                  \
      PERMUTER3(1, 3, 2);                                     \
    }                                                         \
  } else {                                                    \
    PSORT2(p2, p3);                                           \
  }                                                           \
}

#define TEPTR(n)                  (base + (n))
#define TPSWAP( p1, p2)           { T tmp = *p1; *p1 = *p2; *p2 = tmp; }
#define TPSORT2(p1, p2)           if(PNEEDSWAP(p1, p2)) { TPSWAP(p1, p2); }
#define TSORT2( i ,  j)           { T *p1 = TEPTR(i); T *p2 = TEPTR(j); TPSORT2(p1, p2); }
#define TSWAP(i, j)               TPSWAP(p##i, p##j)
#define TPASSIGN(dst, src)        *(dst) = *(src)
#define TASSIGN(a, b)             TPASSIGN(p##a, p##b)
#define TPERMUTER3(a, b, c)       { T tmp = *p##a; TASSIGN(a,b); TASSIGN(b,c);                             *p##c = tmp; }
#define TPERMUTER4(a, b, c, d)    { T tmp = *p##a; TASSIGN(a,b); TASSIGN(b,c); TASSIGN(c,d);               *p##d = tmp; }
#define TPERMUTER5(a, b, c, d, e) { T tmp = *p##a; TASSIGN(a,b); TASSIGN(b,c); TASSIGN(c,d); TASSIGN(d,e); *p##e = tmp; }

#define TSORT3OPT(a, b, c) {                                  \
  T *p0 = TEPTR(a), *p1 = TEPTR(b), *p2 = TEPTR(c);           \
  if(NEEDSWAP(0, 1)) {                                        \
    if(NEEDSWAP(1, 2)) {                                      \
      TSWAP(0, 2);                                            \
    } else if(NEEDSWAP(0, 2)) {                               \
      TPERMUTER3(1, 2, 0);                                    \
    } else {                                                  \
      TSWAP(0, 1);                                            \
    }                                                         \
  } else if(NEEDSWAP(1, 2)) {                                 \
    if(NEEDSWAP(0, 2)) {                                      \
      TPERMUTER3(2, 1, 0);                                    \
    } else {                                                  \
      TSWAP(1, 2);                                            \
    }                                                         \
  }                                                           \
}

#define TSORT4OPT(a, b, c, d, exitStmt) {                     \
  T *p0 = TEPTR(a), *p1 = TEPTR(b), *p2 = TEPTR(c);           \
  T *p3 = TEPTR(d);                                           \
  if(NEEDSWAP(0, 1)) {                                        \
    if(NEEDSWAP(2, 3)) {                                      \
      if(NEEDSWAP(0, 2)) {                                    \
        TSWAP(0, 3); TSWAP(1, 2);                             \
        if(NEEDSWAP(0, 2)) {                                  \
          TPERMUTER3(0, 2, 1);                                \
        } else {                                              \
          TPSORT2(p1, p2);                                    \
        }                                                     \
        exitStmt;                                             \
      } else {                                                \
        TSWAP(0, 1); TSWAP(2, 3);                             \
      }                                                       \
    } else {                                                  \
      if(NEEDSWAP(0, 3)) {                                    \
        TPERMUTER4(0, 2, 1, 3);                               \
      } else {                                                \
        TSWAP(0, 1);                                          \
      }                                                       \
    }                                                         \
  } else {                                                    \
    if(NEEDSWAP(2, 3)) {                                      \
      if(NEEDSWAP(1, 2)) {                                    \
        TPERMUTER4(0, 3, 1, 2);                               \
      } else {                                                \
        TSWAP(2, 3);                                          \
      }                                                       \
    } else {                                                  \
      if(NEEDSWAP(1, 3)) {                                    \
        TSWAP(0, 2); TSWAP(1, 3);                             \
      }                                                       \
    }                                                         \
  }                                                           \
  if(NEEDSWAP(1, 2)) {                                        \
    if(NEEDSWAP(0, 2)) {                                      \
      TPERMUTER3(0, 2, 1)                                     \
    } else {                                                  \
      TSWAP(1, 2);                                            \
    }                                                         \
  }                                                           \
}

#define TSORT5OPT(a, b, c, d, e, exitStmt) {                  \
  T *p0 = TEPTR(a), *p1 = TEPTR(b), *p2 = TEPTR(c);           \
  T *p3 = TEPTR(d), *p4 = TEPTR(e);                           \
  if(NEEDSWAP(0, 1)) {                                        \
    if(NEEDSWAP(2, 3)) {                                      \
      if(NEEDSWAP(0, 2)) {                                    \
        TSWAP(0, 3); TSWAP(1, 2);                             \
      } else {                                                \
        TSWAP(0, 1); TSWAP(2, 3);                             \
      }                                                       \
    } else {                                                  \
      if(NEEDSWAP(0, 3)) {                                    \
        TPERMUTER4(0, 2, 1, 3);                               \
      } else {                                                \
        TSWAP(0, 1);                                          \
      }                                                       \
    }                                                         \
  } else {                                                    \
    if(NEEDSWAP(2, 3)) {                                      \
      if(NEEDSWAP(1, 2)) {                                    \
        TPERMUTER4(0, 3, 1, 2);                               \
      } else {                                                \
        TSWAP(2, 3);                                          \
      }                                                       \
    } else {                                                  \
      if(NEEDSWAP(1, 3)) {                                    \
        TSWAP(0, 2); TSWAP(1, 3);                             \
      }                                                       \
    }                                                         \
  }                                                           \
  if(NEEDSWAP(1, 4)) {                                        \
    if(NEEDSWAP(0, 4)) {                                      \
      TPERMUTER5(0, 4, 3, 2, 1);                              \
    } else {                                                  \
      TPERMUTER4(1, 4, 3, 2);                                 \
    }                                                         \
  } else {                                                    \
    if(NEEDSWAP(3, 4)) {                                      \
      TPERMUTER3(2, 4, 3);                                    \
    } else {                                                  \
      if(NEEDSWAP(1, 2)) {                                    \
        if(NEEDSWAP(0, 2)) {                                  \
          TPERMUTER3(0, 2, 1);                                \
        } else {                                              \
          TSWAP(1, 2);                                        \
        }                                                     \
      } else {                                                \
        TPSORT2(p2, p3);                                      \
      }                                                       \
      exitStmt;                                               \
    }                                                         \
  }                                                           \
  if(NEEDSWAP(1, 3)) {                                        \
    if(NEEDSWAP(0, 3)) {                                      \
      TPERMUTER4(0, 3, 2, 1);                                 \
    } else {                                                  \
      TPERMUTER3(1, 3, 2);                                    \
    }                                                         \
  } else {                                                    \
    TPSORT2(p2, p3);                                          \
  }                                                           \
}

#define MTPSORT2(p1, p2)           if(MPNEEDSWAP(p1, p2)) { TPSWAP(p1, p2); }
#define MTSORT2( i ,  j)           { T *p1 = TEPTR(i); T *p2 = TEPTR(j); MTPSORT2(p1, p2); }

#define MTSORT3OPT(a, b, c) {                                 \
  T *p0 = TEPTR(a), *p1 = TEPTR(b), *p2 = TEPTR(c);           \
  if(MNEEDSWAP(0, 1)) {                                       \
    if(MNEEDSWAP(1, 2)) {                                     \
      TSWAP(0, 2);                                            \
    } else {                                                  \
      if(MNEEDSWAP(0, 2)) {                                   \
        TPERMUTER3(1, 2, 0);                                  \
      } else {                                                \
        TSWAP(0, 1);                                          \
      }                                                       \
    }                                                         \
  } else {                                                    \
    if(MNEEDSWAP(1, 2)) {                                     \
      if(MNEEDSWAP(0, 2)) {                                   \
        TPERMUTER3(2, 1, 0);                                  \
      } else {                                                \
        TSWAP(1, 2);                                          \
      }                                                       \
    } else {                                                  \
      /* do nothing */                                        \
    }                                                         \
  }                                                           \
}

class SortMethodId {
private:
  const int        m_methodId;
  const SortMethod m_sortMethod;
  String           m_name;
  String           m_menuText;
public:
  SortMethodId(int methodId, SortMethod sortMethod);
  inline int getId() const {
    return m_methodId;
  }
  inline const SortMethod &getMethod() const {
    return m_sortMethod;
  }
  inline const String &getName() const {
    return m_name;
  }
  inline const String &getMenuText() const {
    return m_menuText;
  }
  static const SortMethodId &getMethodById(int methodId);
  static const SortMethodId &getMethodByIndex(int index);
  static int getMethodCount();
};

#ifdef _DEBUG
bool debugCheckOrder(const TCHAR *fileName, int line, void *base, int n, char *p1, char *p2);
#define CHECKORDER(p1,p2) debugCheckOrder(__FILE__,__LINE__, base, nelem, p1, p2)
#else
#define CHECKORDER(p1,p2) 
#endif
