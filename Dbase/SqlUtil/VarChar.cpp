#include "stdafx.h"
#include "SqlAPI.h"

void varchar::clear() {
  if(m_data != NULL) { 
    delete[] m_data; 
    m_data = NULL;
  }
}

#define ZEROTERMINATE() *((wchar_t*)(m_data+m_len)) = 0
#define COPYDATA(src)   { if(m_len > 0) memcpy(m_data,src,m_len); \
                          ZEROTERMINATE();                        \
                        }

void varchar::init(ULONG len) {
  m_len  = len;
  m_data = new BYTE[m_len+2]; // always have 2 extract zero bytes at the end, to make faster conversion to C-String
}

varchar::varchar() { init(0); }

varchar::varchar(ULONG len) {
  init(len);
  memset(m_data,0,m_len+sizeof(TCHAR));
}

varchar::varchar(const varchar &src) {
  init(src.len());
  COPYDATA(src.m_data);
}

varchar::~varchar() {
  clear();
}

varchar::varchar(const char *src) {
  init((ULONG)strlen(src)); // NB not + 1
  COPYDATA(src);
}

varchar::varchar(const wchar_t *src) {
  init((ULONG)wcslen(src)*sizeof(wchar_t)); // NB not + 1
  COPYDATA(src);
}

varchar::varchar(const String &src) {
  init((ULONG)src.length()*sizeof(TCHAR)); // NB not + 1
  COPYDATA(src.cstr());
}

varchar::varchar(ULONG len, const void *data) {
  init(len);
  COPYDATA(data);
}

varchar &varchar::operator=(const varchar &rhs) {
  clear();
  init(rhs.len());
  COPYDATA(rhs.m_data);
  return *this;
}

varchar &varchar::operator=(const char *rhs) {
  clear();
  init((ULONG)strlen(rhs)); // NB not + 1
  COPYDATA(rhs);
  return *this;
}

varchar &varchar::operator=(const wchar_t *rhs) {
  clear();
  init((ULONG)wcslen(rhs)*sizeof(wchar_t)); // NB not + 1
  COPYDATA(rhs);
  return *this;
}

varchar &varchar::operator=(const String &rhs) {
  clear();
  init((ULONG)rhs.length()*sizeof(TCHAR)); // NB not + 1
  COPYDATA(rhs.cstr());
  return *this;
}

void varchar::setdata(ULONG len, const void *data) {
  clear();
  init(len);
  COPYDATA(data);
}

int compare(const varchar &vc1, const varchar &vc2) {
  int c;
  if(vc1.len() == vc2.len()) {
    return memcmp(vc1.data(),vc2.data(),vc1.len());
  }
  if(vc1.len() < vc2.len()) {
    c = memcmp(vc1.data(),vc2.data(),vc1.len());
    return c ? c : -1;
  } // else vc1.len > vc2.len
  c = memcmp(vc1.data(),vc2.data(),vc2.len());
  return c ? c : 1;
}

bool operator==(const varchar &vc1, const varchar &vc2) {
  if(vc1.len() != vc2.len()) return false;
  return memcmp(vc1.data(),vc2.data(),vc1.len()) == 0;
}

bool operator!=(const varchar &vc1, const varchar &vc2) {
  return !(vc1 == vc2);
}

bool operator< (const varchar &vc1, const varchar &vc2) {
  return compare(vc1, vc2) < 0;
}

bool operator<=(const varchar &vc1, const varchar &vc2) {
  return compare(vc1, vc2) <= 0;
}

bool operator> (const varchar &vc1, const varchar &vc2) {
  return compare(vc1, vc2) > 0;
}

bool operator>=(const varchar &vc1, const varchar &vc2) {
  return compare(vc1, vc2) >= 0;
}

void varchar::dump(FILE *f) const {
  hexdump(m_data, m_len, f);
}
