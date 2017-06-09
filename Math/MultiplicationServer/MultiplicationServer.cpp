#include "stdafx.h"
#include <MyUtil.h>
#include <Math/BigReal.h>

int main(int argc, char **argv) {
  for(;;) {
    if(tcin.eof()) break;
    BigReal x, y, f;
    tcin >> x >> y >> f;
    
    FullFormatBigReal z = prod(x, y, f);
    tcout << z << endl;
  }
  return 0;
}
