#include "stdafx.h"
#include <Math/Double80.h>
#include "TestToString.h"

using namespace std;

String D80ToDbgString(const Double80 &d80) {
  const int expo10 = Double80::getExpo10(d80);
  if(expo10 < -4 || (expo10 >= 18)) {
    TCHAR str[50];
    return d80tot(str, d80);
  } else {
    return toString(d80, 19-expo10,21,ios::fixed);
  }
}


int main(int argc, char **argv) {
  try {
    const UINT64   ui64 = -1;
    const Double80 x    = ui64;
    const String   s1   = D80ToDbgString(x);
    testToString();
  } catch(Exception e) {
    tcout << _T("Exception:") << e.what() << endl;
    return -1;
  }
  return 0;
}
