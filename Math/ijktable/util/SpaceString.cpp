#include "stdafx.h"

string spaceString(uint length, char ch) {
  if (length <= 0) {
    return "";
  }
  string result;
  char *buf = new char[length + 1];
  memset(buf, ch, length);
  buf[length] = 0;
  result = buf;
  delete[] buf;
  return result;
}
