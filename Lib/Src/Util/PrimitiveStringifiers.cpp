#include "pch.h"

String IntStringifier::toString(const int &e) {
  TCHAR tmp[60];
  return _itot(e, tmp, 10);
}

String UIntStringifier::toString(const UINT &e) {
  TCHAR tmp[60];
  return _ui64tot(e, tmp, 10);
}

String Int64Stringifier::toString(const INT64 &e) {
  TCHAR tmp[60];
  return _i64tot(e, tmp, 10);
}

String UInt64Stringifier::toString(const UINT64 &e) {
  TCHAR tmp[60];
  return _ui64tot(e, tmp, 10);
}

String SizeTStringifier::toString(const size_t &e) {
  TCHAR tmp[60];
  return _ui64tot(e, tmp, 10);
}
