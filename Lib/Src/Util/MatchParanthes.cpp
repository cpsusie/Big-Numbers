#include "pch.h"

int findMatchingpParanthes(const TCHAR *str,int pos) {
  const int length = _tcsclen(str);
  if(pos < 0 || pos >= length) {
    return -1;
  }
  int dir = 0;
  TCHAR ch, partner;
  bool firsttime = true;
  for(;;) {
    ch = str[pos];
    switch(ch) {
    case _T('('):
      partner = _T(')'); dir = 1;
      break;
    case _T(')'):
      partner = _T('('); dir = -1;
      break;
    case _T('['):
      partner = _T(']'); dir = 1;
      break;
    case _T(']'):
      partner = _T('['); dir = -1;
      break;
    case _T('{'):
      partner = _T('}'); dir = 1;
      break;
    case _T('}'):
      partner = _T('{'); dir = -1;
      break;
    default:
      if(!firsttime) {
        return -1;
      } else {
        firsttime = false;
      }
      break;
    }
    if(dir != 0) {
      break;
    } else if(pos > 0) {
      pos--;
    } else {
      return -1;
    }
  }
  int t = 1, k;
  for(k = pos+dir; (k < length) && (k >= 0) && (t > 0); k+=dir) {
    if(str[k] == partner) {
      t--;
    } else if(str[k] == ch) {
      t++;
    }
  }
  if(t == 0) {
    return k-dir;
  } else {
    return -1;
  }
}
