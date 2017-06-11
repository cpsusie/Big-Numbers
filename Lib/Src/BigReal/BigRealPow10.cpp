#include "pch.h"

// These Functions should be isolated to this file, så we'll no link DigitPool and a lot
// more unessecary datastructures/threads on NumberAddIn which only call (some) of these functions

#define TENE0  1
#define TENE1  10
#define TENE2  100
#define TENE3  1000
#define TENE4  10000
#define TENE5  100000
#define TENE6  1000000
#define TENE7  10000000
#define TENE8  100000000
#define TENE9  1000000000
#define TENE10 10000000000
#define TENE11 100000000000
#define TENE12 1000000000000
#define TENE13 10000000000000
#define TENE14 100000000000000
#define TENE15 1000000000000000
#define TENE16 10000000000000000
#define TENE17 100000000000000000
#define TENE18 1000000000000000000
#define TENE19 10000000000000000000

// Assume n = [1..1e8[
int BigReal::getDecimalDigitCount32(ULONG n) { // static
  assert(n && (n < TENE8));
  // Binary search
  if(n < TENE4) {                           //        n < 1e4
    if(n < TENE2) {                         //        n < 1e2
      return (n < TENE1) ? 1 : 2;
    } else {                                // 1e2 <= n < 1e4
      return (n < TENE3) ? 3 : 4;
    }
  } else {                                  // 1e4 <= n < 1e9
    if(n < TENE6) {                         // 1e4 <= n < 1e6
      return (n < TENE5) ? 5 : 6;
    } else {                                // 1e6 <= n < 1e9
      return (n < TENE7) ? 7 : 8;
    }
  }
}

// Assume n = [1..1e19[
int BigReal::getDecimalDigitCount64(UINT64 n) { // static
//  assert(n && (n < TENE19));
  // Binary search
  if(n < TENE10) {                          //        n < 1e10
    if(n < TENE5) {                         //        n < 1e5
      if(n < TENE2) {                       //        n < 1e2
        return (n < TENE1) ? 1 : 2;
      } else {                              // 1e2 <= n < 1e5
        if(n < TENE4) {                     // 1e2 <= n < 1e4
          return (n < TENE3) ? 3 : 4;
        } else {                            // 1e4 <= n < 1e5
          return 5;
        }
      }
    } else {                                // 1e5 <= n < 1e10
      if(n < TENE7) {                       // 1e5 <= n < 1e7
        return (n < TENE6) ? 6 : 7;
      } else {                              // 1e7 <= n < 1e10
        if(n < TENE9) {                     // 1e7 <= n < 1e9
          return (n < TENE8) ? 8 : 9;
        } else {                            // 1e9 <= n < 1e10
          return 10;
        }
      }
    }
  } else {                                  // 1e10 <= n < 1e19
    if(n < TENE15) {                        // 1e10 <= n < 1e15
      if(n < TENE12) {                      // 1e10 <= n < 1e12
        return (n < TENE11) ? 11 : 12;
      } else {                              // 1e12 <= n < 1e15
        if(n < TENE14) {                    // 1e12 <= n < 1e14
          return (n < TENE13) ? 13 : 14;    //
        } else {                            // 1e14 <= n < 1e15
          return 15;
        }
      }
    } else {                                // 1e15 <= n < 1e19
      if(n < TENE17) {                      // 1e15 <= n < 1e17
        return (n < TENE16) ? 16 : 17;
      } else {                              // 1e17 <= n <= 1e19
        return (n < TENE18) ? 18 : 19;
      }
    }
  }
}

#ifdef HAS_LOOP_DIGITCOUNT
// Assume n = ]0..1eMAXDIGITS_INT64[
int BigReal::getDecimalDigitCount64Loop(UINT64 n) { // static
  static const UINT64 pow10Table[] = {
     TENE0    ,TENE1    ,TENE2    ,TENE3
    ,TENE4    ,TENE5    ,TENE6    ,TENE7
    ,TENE8    ,TENE9    ,TENE10   ,TENE11
    ,TENE12   ,TENE13   ,TENE14   ,TENE15
    ,TENE16   ,TENE17   ,TENE18   ,TENE19
  };
  int l = 0, r = ARRAYSIZE(pow10Table);
  while(l < r) {
    const int     m = (l+r)/2;
    const UINT64 &p10 = pow10Table[m];
    if(p10 <= n) {
      l = m + 1;
    } else {
      r = m;
    }
  }
  return r;
}
#endif // HAS_LOOP_DIGITCOUNT

const BRDigitType BigReal::s_power10Table[POWER10TABLESIZE] = {
  TENE0    ,TENE1
 ,TENE2    ,TENE3
 ,TENE4    ,TENE5
 ,TENE6    ,TENE7
 ,TENE8    ,TENE9
#ifdef IS64BIT
 ,TENE10   ,TENE11
 ,TENE12   ,TENE13
 ,TENE14   ,TENE15
 ,TENE16   ,TENE17
 ,TENE18   ,TENE19
#endif // IS64BIT
};

// Return p if n = 10^p for p = [0..9]. else return -1.
int BigReal::isPow10(size_t n) { // static
  switch(n) {
  case TENE0:  return  0;
  case TENE1:  return  1;
  case TENE2:  return  2;
  case TENE3:  return  3;
  case TENE4:  return  4;
  case TENE5:  return  5;
  case TENE6:  return  6;
  case TENE7:  return  7;
  case TENE8:  return  8;
  case TENE9:  return  9;
#ifdef IS64BIT
  case TENE10: return  10;
  case TENE11: return  11;
  case TENE12: return  12;
  case TENE13: return  13;
  case TENE14: return  14;
  case TENE15: return  15;
  case TENE16: return  16;
  case TENE17: return  17;
  case TENE18: return  18;
  case TENE19: return  19;
#endif // IS64BIT
  default        : return -1;
  }
}

bool BigReal::isPow10(const BigReal &x) { // static
  return !x.isZero() && (x.getLength() == 1) && (isPow10(x.getFirstDigit()) >= 0);
}

