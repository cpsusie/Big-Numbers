#include "pch.h"

#include <Math/Double80.h>

// (8,7)-Minimax-approximation of gamma(x+1.5) for x in [-0.5,0.5] with max relative error <= 1.471004339559656e-20
static const BYTE coefdata[] = {
   0x3d,0x55,0x7b,0xa7,0x8d,0xc4,0xdf,0xe2,0xfe,0x3f  //  8.86226925452758014e-01
  ,0x08,0x83,0xc5,0xab,0x17,0x6d,0x36,0xa3,0xfe,0x3f  //  6.37549227026985619e-01
  ,0x53,0x4f,0xa4,0x46,0x83,0xd0,0xd0,0x97,0xfd,0x3f  //  2.96515003228087987e-01
  ,0x3a,0x2c,0x47,0x74,0x76,0xd0,0x1b,0xbc,0xfb,0x3f  //  9.184992658132137178e-02
  ,0xc1,0x6c,0x2e,0x67,0xed,0x66,0xd1,0xb7,0xf9,0x3f  //  2.24387178707042195e-02
  ,0x20,0x76,0xe1,0x9f,0x48,0x9f,0x48,0x86,0xf7,0x3f  //  4.09801270984559600e-03
  ,0x78,0x9d,0x5d,0x0c,0xb2,0xb6,0x93,0x9f,0xf4,0x3f  //  6.08737970207654019e-04
  ,0xd4,0xe6,0x44,0x3a,0xda,0xbb,0x39,0x81,0xf1,0x3f  //  6.161953098818497256e-05
  ,0x48,0x1e,0xff,0x2a,0xff,0xff,0xed,0x9e,0xed,0x3f  //  4.73647378237266156e-06
  ,0x3e,0x03,0x4b,0x77,0x24,0x03,0xd3,0xae,0xfe,0x3f  //  6.82907291796521366e-01
  ,0x82,0x5a,0xa9,0x10,0x97,0xdc,0x34,0xa2,0xfc,0xbf  // -1.58404776306420339e-01
  ,0x25,0x17,0xd9,0xcb,0x64,0x10,0x97,0xb6,0xfb,0xbf  // -8.915531928640211952e-02
  ,0xee,0x78,0xa1,0xe8,0x1a,0xc7,0xb4,0xb5,0xf9,0x3f  //  2.21809281131479211e-02
  ,0xde,0x97,0xeb,0xf4,0xc4,0x8b,0x1f,0x9c,0xf6,0x3f  //  2.38225138032859820e-03
  ,0x32,0xc8,0x61,0x1c,0xda,0xb3,0xb2,0x92,0xf5,0xbf  // -1.11921735247545577e-03
  ,0x12,0xed,0x14,0xb5,0xdb,0x95,0xe3,0xc1,0xf1,0x3f  //  9.245348234422154281e-05
};

static const Double80 *coef = (const Double80*)coefdata;
static Double80 approximation0807(Double80 x) {
  Double80 sum1 = coef[8];
  Double80 sum2 = coef[15];
  int i;
  for (i = 7; i >= 0; i--) sum1 = sum1 * x + coef[i];
  for (i = 14; i > 8; i--) sum2 = sum2 * x + coef[i];
  return sum1 / (sum2 * x + 1.0);
}

// assume 1 < x < 2
static inline Double80 D80gamma1_2(const Double80 &x) {
  return approximation0807(x-1.5);
}

Double80 gamma(const Double80 &x) {
  if(x == floor(x)) {
    if(x < 1) {
      return 1.0/(x-floor(x)); // undefined
    }
    Double80 res = 1;
    for(Double80 p = x; --p > 1;) {
      res *= p;
    }
    return res;
  }

  if(x > 2) {
    Double80 res = 1, p = x;
    while(--p > 2) {
      res *= p;
    }
    return res * p * D80gamma1_2(p);
  } else if(x > 1) {
    return D80gamma1_2(x);
  } else {
    Double80 res = 1, p = x;
    for(; p < 1; ++p) {
      res *= p;
    }
    return D80gamma1_2(p) / res;
  }
}

Double80 lnGamma(const Double80 &x) {
  if(x <= 0) {
    throwInvalidArgumentException(__TFUNCTION__
                                 ,_T("x=%s. Must be > 0")
                                 , toString(x).cstr()); // undefined
  }
  if(x == floor(x)) {
    Double80 res = 0;
    for(Double80 p = x; --p > 1;) {
      res += log(p);
    }
    return res;
  }

  if(x > 2) {
    Double80 res = 0, p = x;
    while(--p > 2) {
      res += log(p);
    }
    // p = ]1; 2[
    return res + log(p) + log(D80gamma1_2(p));
  } else { // x = ]0; 2[
    Double80 res = 0, p = x;
    for(; p < 1; ++p) {
      res += log(p);
    }
    // p = ]1; 2[
    return log(D80gamma1_2(p)) - res;
  }
}

// Calculates x! = x*(x-1)*(x-2)*...*2*1, extended to real numbers by the gamma function x! = gamma(x+1)
Double80 fac(const Double80 &x) {
  return gamma(x+1);
}