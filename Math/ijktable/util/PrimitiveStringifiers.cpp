#include "stdafx.h"

string IntStringifier::toString(const int &e) {
  char tmp[60];
  return _itoa(e, tmp, 10);
}

string UIntStringifier::toString(const uint &e) {
  char tmp[60];
  return _ui64toa(e, tmp, 10);
}

string Int64Stringifier::toString(const int64 &e) {
  char tmp[60];
  return _i64toa(e, tmp, 10);
}

string UInt64Stringifier::toString(const uint64 &e) {
  char tmp[60];
  return _ui64toa(e, tmp, 10);
}

string SizeTStringifier::toString(const size_t &e) {
  char tmp[60];
  return _ui64toa(e, tmp, 10);
}
