#include "pch.h"
#include <StreamParameters.h>

using namespace std;


TCHAR *StreamParameters::flagsToStr(TCHAR *dst, FormatFlags flags) { // static
#define BITCH(f,chset,chclr) (flags & ios::f) ? #@chset : chclr
#define BIT(f)                BITCH(f,1,'0')
#define BITCH1(f,ch)          BITCH(f,ch,' ')

  _stprintf(dst, _T("%c%c,skw%c,uc%c,shb%c,adj(%c%c%c),b(%c%c%c),fl(%c%c),u%c,ba%c,std%c")
               , BITCH1(showpos   ,+)
               , BITCH1(showpoint ,.)
               , BIT(skipws         )
               , BIT(uppercase      )
               , BIT(showbase       )
               , BITCH1(left      ,l)
               , BITCH1(right     ,r)
               , BITCH1(internal  ,i)
               , BITCH1(dec       ,d)
               , BITCH1(hex       ,x)
               , BITCH1(oct       ,o)
               , BITCH1(scientific,e)
               , BITCH1(fixed     ,f)
               , BIT(   unitbuf     )
               , BIT(   boolalpha   )
               , BIT(   _Stdio      )
           );
  return dst;
}

String StreamParameters::flagsToString(FormatFlags flags) { // static
  TCHAR result[200];
  return flagsToStr(result, flags);
}

String StreamParameters::toString() const {
  TCHAR tmp[200];
  return format(_T("Flags[%s] fill:'%c' %3lld.%-3lld"), flagsToStr(tmp, flags()), fill(), width(), precision());
}
