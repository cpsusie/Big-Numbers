#include "pch.h"
#include <Math/fft.h>

/*
public static long reverse(long i) {
    // HD, Figure 7-1
i = (i & 0x5555555555555555L) << 1 | (i >>> 1) & 0x5555555555555555L;
i = (i & 0x3333333333333333L) << 2 | (i >>> 2) & 0x3333333333333333L;
i = (i & 0x0f0f0f0f0f0f0f0fL) << 4 | (i >>> 4) & 0x0f0f0f0f0f0f0f0fL;
i = (i & 0x00ff00ff00ff00ffL) << 8 | (i >>> 8) & 0x00ff00ff00ff00ffL;
i = (i << 48) | ((i & 0xffff0000L) << 16) |
	((i >>> 16) & 0xffff0000L) | (i >>> 48);
return i;
}
*/


// This computes an in-place Complex-to-Complex Direct fourier transform
// forward = true  => forward transform. ie. time-domain -> frequens-domain.
// forward = false => reverse transform  ie. frequens-domain -> time-domain.
void dft(CompactArray<Complex> &data, bool forward) {
  const int n = (int)data.size();
  Real *x2 = new Real[n];
  Real *y2 = new Real[n];

  memset(x2,0,sizeof(Real)*n);
  memset(y2,0,sizeof(Real)*n);

  Real factor = 2.0 * REAL_PI / n;
  if (!forward) {
    factor = -factor;
  }
  for(int j = 0; j < n; j++) {
    Real arg = factor * j;
    for(int k = 0; k < n; k++) {
      Real cosArg = cos(k * arg);
      Real sinArg = sin(k * arg);
      const Complex &c = data[k];
      x2[j] += c.re*cosArg - c.im*sinArg;
      y2[j] += c.re*sinArg + c.im*cosArg;
    }
  }

  // Copy the data back
  if(forward) {
    for(int i = 0; i < n; i++) {
      Complex &c = data[i];
      c.re = x2[i];
      c.im = y2[i];
    }
  } else {
    for(int i = 0; i < n; i++) {
      Complex &c = data[i];
      c.re = x2[i]/n;
      c.im = y2[i]/n;
    }
  }

  delete[] x2;
  delete[] y2;
}

// return p, assuming n = 2^p
static UINT log2(UINT n) {
  switch(n) {
  case 0x00000001: return 0;
  case 0x00000002: return 1;
  case 0x00000004: return 2;
  case 0x00000008: return 3;
  case 0x00000010: return 4;
  case 0x00000020: return 5;
  case 0x00000040: return 6;
  case 0x00000080: return 7;
  case 0x00000100: return 8;
  case 0x00000200: return 9;
  case 0x00000400: return 10;
  case 0x00000800: return 11;
  case 0x00001000: return 12;
  case 0x00002000: return 13;
  case 0x00004000: return 14;
  case 0x00008000: return 15;
  case 0x00010000: return 16;
  case 0x00020000: return 17;
  case 0x00040000: return 18;
  case 0x00080000: return 19;
  case 0x00100000: return 20;
  case 0x00200000: return 21;
  case 0x00400000: return 22;
  case 0x00800000: return 23;
  case 0x01000000: return 24;
  case 0x02000000: return 25;
  case 0x04000000: return 26;
  case 0x08000000: return 27;
  case 0x10000000: return 28;
  case 0x20000000: return 29;
  case 0x40000000: return 30;
  case 0x80000000: return 31;
  default        : throwException(_T("Size of data array must be a power of 2 (=%lx)."),n);
                   return 32;
  }
}


// This computes an in-place Complex-to-Complex Fast fourier transform
// data containing 2^t Complex numbers.
// forward = true  => forward transform. ie. time-domain -> frequens-domain.
// forward = false => reverse transform  ie. frequens-domain -> time-domain.
void fft(CompactArray<Complex> &data, bool forward) {
  const UINT n = (UINT)data.size();
  const UINT t = log2((UINT)n);

  // Do the bit reversal
  UINT i2 = n >> 1;
  UINT j  = 0;
  for(UINT i = 0; i < n-1; i++) {
    if(i < j) {
      data.swap(i, j);
//      cout << "swap(" << i << "," << j << ")" << endl;
    }
    UINT k;
    for(k = i2; k <= j; k >>= 1) j -= k;
    j += k;
  }

  // Compute the FFT
  Real c1 = -1;
  Real c2 =  0;
  UINT l2 =  1;
  for(UINT l = 0; l < t; l++) {

/*
    cout << "l:" << l << " f:";
    for(UINT dd = 0; dd < n; dd++) {
      if(dd > 0)
        tcout << _T(",");
      tcout << _T("\"") << data[dd] << _T("\"");
    }
    tcout << endl;
*/

    const UINT l1 = l2;
    l2 <<= 1;
    Real u1 = 1;
    Real u2 = 0;
    for(UINT j = 0; j < l1; j++) {
      for(UINT p = j; p < n; p += l2) {
        const UINT q = p + l1;
        Complex &dp = data[p];
        Complex &dq = data[q];
        Real t1 = u1*dq.re - u2*dq.im;
        Real t2 = u1*dq.im + u2*dq.re;
        dq.re = dp.re - t1;
        dq.im = dp.im - t2;
        dp.re += t1;
        dp.im += t2;
      }
      Real z = u1*c1 - u2*c2;
      u2 = u1*c2 + u2*c1;
      u1 = z;
    }
    c2 = sqrt((1.0 - c1) / 2.0);
    if(!forward) {
      c2 = -c2;
    }
    c1 = sqrt((1.0 + c1) / 2.0);
  }

  // Scaling for forward transform
  if(!forward) {
    for(UINT i = 0; i < n; i++) {
      data[i] /= n;
    }
  }
}
