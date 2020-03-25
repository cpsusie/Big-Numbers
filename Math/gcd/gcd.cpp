#include "stdafx.h"
#include <MyUtil.h>
#include <InputValue.h>

bool even(ULONG n) {
  return (n & 0x1) == 0;
}

ULONG gcd(ULONG u, ULONG v) {
  bool gprinted = false;

  const ULONG u0=u, v0=v;
  ULONG g =1;

  while(even(u) && even(v)) {          // Invariant: g==2^k && u0 == g*u && v0 == g*v
    if(!gprinted) {
      printf("g:");
      gprinted=true;
    }
    printf("%d ", g);
    u /= 2; // (right shift)
    v /= 2;
    g *= 2; // (left shift)
  }

  // g == max(2^k), u0 == 0 mod g && v0 == 0 mod g

  if(gprinted) {
    printf("\ng=%d\n", g);
  }
  printf("(u,v):(%8d,%8d)\n", u, v);
                                       // Now u or v (or both) are odd

  while(u > 0) {                       // Invariant: g==2^k && u0 == g*v*n && v0 == g*v*m && (odd(u) || odd(v))
    if(even(u))      u /= 2;
    else if(even(v)) v /= 2;
    else if(u < v)   v = (v-u)/2;
    else             u = (u-v)/2;
    printf("(u,v):(%8d,%8d)\n", u, v);
  }
  return g*v;
}

int main(int argc, char **argv) {
  for(;;) {
    String line = inputValue<String>(_T("Angiv u v:"));
    ULONG u,v;
    if(_stscanf(line.cstr(),_T("%d %d"),&u,&v) != 2) continue;
    ULONG gc = gcd(u, v);
    _tprintf(_T("gcd(%d,%d):%d\n"), u, v, gc);
  }
  return 0;
}
