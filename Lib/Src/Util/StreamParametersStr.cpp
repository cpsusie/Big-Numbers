#include "pch.h"
#include <StreamParameters.h>

using namespace std;

String StreamParameters::flagsToString(FormatFlags flags) {
  String result;
  result =  (flags & ios::showpos   ) ? _T("+") : _T(" ");
  result += (flags & ios::showpoint ) ? _T(".") : _T(" ");
  result += (flags & ios::left      ) ? _T("l") : _T(" ");
  result += (flags & ios::right     ) ? _T("r") : _T(" ");
  result += (flags & ios::scientific) ? (flags & ios::uppercase) ? _T("E") : _T("e") : _T(" ");
  result += (flags & ios::fixed     ) ? _T("f") : _T(" ");
  return result;
}

String StreamParameters::toString() const {
  return format(_T("Flags:[%6s] %3lld.%-3lld"), flagsToString(getFlags()).cstr(), getWidth(), getPrecision());
}
