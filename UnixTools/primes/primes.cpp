#include "stdafx.h"
#include <MyUtil.h>
#include <limits.h>
#include <Math/Double80.h>

#define BITSINATOM (sizeof(BitSet64::Atom)*8)
#define ATOMCOUNT(size) (((size)-1) / BITSINATOM + 1)
#define ATOMINDEX(i) ((i)/BITSINATOM)
#define ATOM(p, i) p[ATOMINDEX(i)]
#define ATOMBIT(i) ((BitSet64::Atom)1<< ((i)%BITSINATOM))

class BitSet64 {
public:
  typedef ULONG  Atom;

protected:
  Atom  *m_p;
  size_t m_capacity;
public:
  BitSet64(size_t capacity);
  ~BitSet64();
  bool contains(size_t i) const;
  void remove(size_t i);
};

BitSet64::BitSet64(size_t capacity) {
  if(capacity == 0) {
    throwException(_T("BitSet64:Illegal capacity (=0)"));
  }
  m_capacity = capacity;
  const size_t atomCount = ATOMCOUNT(m_capacity);
  m_p = new Atom[atomCount];
  memset(m_p,0xff,atomCount * sizeof(Atom));
  remove(0);
  remove(1);
}

BitSet64::~BitSet64() {
  delete[] m_p;
}

bool BitSet64::contains(size_t i) const {
  if(i > m_capacity) {
    return false;
  }

  return (ATOM(m_p,i) & ATOMBIT(i)) != 0;
}

void BitSet64::remove(size_t i) {
  if(i < m_capacity) {
    ATOM(m_p,i) &= ~ATOMBIT(i);
  }
}

static void printPrimes(size_t lower, size_t upper) {
  BitSet64 primes(upper+1);
  size_t count = 0;
  for(size_t i = 2; i <= upper; i++) {
    if(primes.contains(i)) {
      if(i >= lower) {
        _tprintf(_T("%12I64u%c"),(UINT64)i,((count%8)==7)?_T('\n'):_T(' '));
        count++;
      }
      for(size_t j = i+i; j <= upper; j+=i) {
        primes.remove(j);
      }
    }
  }
  _tprintf(_T("\n"));
}

static void printFactors(size_t number) {
  ULONG upper = getLong(sqrt(Double80(number))) + 1;
  BitSet64 primes(upper+1);
  size_t count = 0;
  for(size_t p = 2; p <= upper; p++) {
    if(primes.contains(p)) {
      for(size_t j = 2*p; j <= upper; j+=p) {
        primes.remove(j);
      }
      while(number % p == 0) {
        _tprintf(_T("%I64u "), (UINT64)p);
        number /= p;
        upper = getLong(sqrt(Double80(number))) + 1;
      }
      if(number == 1) {
        break;
      }
    }
  }
  if(number != 1) {
    _tprintf(_T("%I64u\n"),(UINT64)number);
  } else {
    _tprintf(_T("\n"));
  }
}

static void usage() {
  _ftprintf(stderr,_T("Usage:primes [-fnumber] [-slower-upper]\n"));
  exit(-1);
}

#if defined(IS32BIT)
#define SIZETMAX _UI32_MAX
#else
#define SIZETMAX _UI64_MAX
#endif

int main(int argc, char **argv) {
  unsigned __int64 lower = 2;
  unsigned __int64 upper = 1000;
  unsigned __int64 number;
  char *cp;
  UseSafeNew = 1;
  try {
    for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
      for(cp++;*cp;cp++) {
        switch(*cp) {
        case 'f':
          if(sscanf(cp+1,"%I64u", &number) != 1) {
            usage();
          }
          if((format(_T("%I64u"), number) != cp+1) || (number > SIZETMAX)) {
            _ftprintf(stderr, _T("Number too big. Max=%I64u\n"), (UINT64)SIZETMAX);
            exit(-1);
          }
          printFactors((size_t)number);
          exit(0);

        case 's':
          if(sscanf(cp+1,"%I64u-%I64u", &lower, &upper) != 2) {
            usage();
          }
          if(format(_T("%I64u-%I64u"), lower, upper) != cp+1) {
            _ftprintf(stderr, _T("Number too big. Max=%I64u\n"), (UINT64)SIZETMAX);
            exit(-1);
          }
          if(lower >= upper) {
            _ftprintf(stderr,_T("Lower must be less than upper\n"));
            exit(-1);
          }
          if(upper > SIZETMAX) {
            _ftprintf(stderr, _T("Number too big. Max=%I64u\n"), (UINT64)SIZETMAX);
          }
          printPrimes((size_t)lower, (size_t)upper);
          exit(0);

        default :
          usage();
        }
      }
      break;
    }

    if(*argv) {
      usage();
    }

    printPrimes((size_t)lower,(size_t)upper);
  } catch(Exception e) {
    _ftprintf(stderr,_T("Exception:%s\n"), e.what());
    exit(-1);
  }
  return 0;
}
