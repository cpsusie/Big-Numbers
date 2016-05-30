#include "stdafx.h"

void plot(DigitPool *pool) {
  FILE *ff = fopen("fisk","w");
  double x,y;
  BigReal Y(pool),F(pool),D(pool);
  F = e(BIGREAL_1,-40,pool);
  BigReal tmp("132487615234.1234",pool);
  BigInt itmp(tmp,pool);
  tcout << "itmp:" << itmp << _T("\n"); tcout.flush();

  BigInt i("123948761.12347986",pool);

  tcout << _T("i:") << i << _T("\n"); tcout.flush();
  BigInt itmp2(itmp,pool);

  tcout << _T("itmp2:") << itmp2 << _T("\n"); tcout.flush();

  i = itmp;

  tcout << _T("i:") << i << _T("\n"); tcout.flush();

  i = (BigInt)tmp;

  tcout << _T("tmp:") << tmp << _T("\n"); tcout.flush();
  tcout << _T("i:") << i << _T("\n"); tcout.flush();

  for(x = -1; x <= 1; x += 0.01) {
    Y = acos(BigReal(x,pool),F);
    y = acos(x);
    if(y == 0) {
      D = Y - BigReal(y);
    } else {
      D = quot(Y - BigReal(y),BigReal(y),F,pool);
    }
    _tprintf(_T("%le "),x); D.print(); _tprintf(_T("\n"));
    _ftprintf(ff, _T("%le "),x); D.print(ff,false); _ftprintf(ff, _T("\n"));
  }

  fclose(ff);
}
