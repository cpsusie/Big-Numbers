#include "pch.h"
#include <tcp.h>
#include "Packer.h"
#include <MyAssert.h>

Packer::Packer() {
  m_first = 0;
}

void Packer::write(ByteOutputStream &s) const {
  int len = (int)getDataSize();
  unsigned long nl = htonl(len);
  s.putBytes((BYTE*)&nl, sizeof(nl));
  s.putBytes(m_buffer.getData(),len);
}

bool Packer::read(ByteInputStream &s) {
  unsigned long nl;
  s.getBytesForced((BYTE*)&nl,sizeof(nl));
  int len = ntohl(nl);
  if(len == -1) {
    return false;
  }

  BYTE buffer[4096];
  for(int total = 0; total < len;) {
    int n = min(sizeof(buffer), len - total);
    s.getBytesForced(buffer,n);
    m_buffer.append(buffer, n);
    total += n;
  }
  m_first = 0; // get ready to unpack with >>
  return true;
}

void Packer::writeEos(ByteOutputStream &s) const {
  int len = htonl(-1);
  s.putBytes((BYTE*)&len,sizeof(len));
}

void Packer::clear() {
  m_first = 0;
  m_buffer.clear();
}

Packer &Packer::addElement(ElementType t, const void *e, size_t size) {
  unsigned char ct = (unsigned char)t;
  m_buffer.append((const BYTE *)&ct,1   );
  m_buffer.append((const BYTE *)e,  size);
  return *this;
}

Packer &Packer::getElement(ElementType et, void *e, size_t size) {
  unsigned char ct = m_buffer[m_first++];
  ElementType t = (ElementType)ct;
  if(t != et) {
    throwException(_T("Packer::getElement:Received invalid datetype:%d. Expected %d"),t,et);
  }
  assert(m_buffer.size() >= m_first + size);
  memcpy(e, m_buffer.getData() + m_first, size);
  m_first += size;
  if(m_first == m_buffer.size()) {
    clear();
  }
  return *this;
}

// --------------------------------- SHORT --------------------------
Packer &Packer::operator<<(short n) {
  unsigned short ns = htons(n);
  return addElement(E_SHORT, &ns, sizeof(ns));
}

Packer &Packer::operator>>(short &n) {
  unsigned short ns;
  getElement(E_SHORT, &ns, sizeof(ns));
  n = ntohs(ns);
  return *this;
}

Packer &Packer::operator<<(unsigned short n) {
  unsigned short ns = htons(n);
  addElement(E_SHORT, &ns, sizeof(ns));
  return *this;
}

Packer &Packer::operator>>(unsigned short &n) {
  unsigned short ns;
  getElement(E_SHORT, &ns, sizeof(ns));
  n = ntohs(ns);
  return *this;
}

// --------------------------------- INT --------------------------

Packer &Packer::operator<<(int n) {
  unsigned int ni = htonl(n);
  return addElement(E_INT, &ni, sizeof(ni));
}

Packer &Packer::operator>>(int &n) {
  unsigned int ni;
  getElement(E_INT, &ni, sizeof(ni));
  n = ntohl(ni);
  return *this;
}

Packer &Packer::operator<<(unsigned int n) {
  unsigned int ni = htonl(n);
  return addElement(E_INT, &ni, sizeof(ni));
}

Packer &Packer::operator>>(unsigned int &n) {
  unsigned int ni;
  getElement(E_INT, &ni, sizeof(ni));
  n = ntohl(ni);
  return *this;
}

// --------------------------------- LONG --------------------------

Packer &Packer::operator<<(long n) {
  unsigned long nl = htonl(n);
  return addElement(E_LONG, &nl, sizeof(nl));
}

Packer &Packer::operator>>(long &n) {
  unsigned long nl;
  getElement(E_LONG, &nl, sizeof(nl));
  n = ntohl(nl);
  return *this;
}

Packer &Packer::operator<<(unsigned long n) {
  unsigned long nl = htonl(n);
  return addElement(E_LONG, &nl, sizeof(nl));
}

Packer &Packer::operator>>(unsigned long &n) {
  unsigned long nl;
  getElement(E_LONG, &nl, sizeof(nl));
  n = ntohl(nl);
  return *this;
}

// --------------------------------- __INT64 --------------------------

Packer &Packer::operator<<(__int64 n) {
  unsigned __int64 nl = htonll(n);
  return addElement(E_LONG_LONG, &nl, sizeof(nl));
}

Packer &Packer::operator>>(__int64 &n) {
  unsigned __int64 nl;
  getElement(E_LONG_LONG, &nl, sizeof(nl));
  n = ntohll(nl);
  return *this;
}

Packer &Packer::operator<<(unsigned __int64 n) {
  unsigned __int64 nl = htonll(n);
  return addElement(E_LONG_LONG, &nl, sizeof(nl));
}

Packer &Packer::operator>>(unsigned __int64 &n) {
  unsigned __int64 nl;
  getElement(E_LONG_LONG, &nl, sizeof(nl));
  n = ntohll(nl);
  return *this;
}

// --------------------------------- String --------------------------

Packer &Packer::operator<<(const char * const s) {
  const size_t l = strlen(s);
  *this << l;
  addElement(E_STR,s,l);
  return *this;
}

Packer &Packer::operator>>(char *s) {
  size_t l;
  *this >> l;
  getElement(E_STR,s,l);
  s[l] = 0;
  return *this;
}

Packer &Packer::operator<<( const wchar_t * const s) {
  const size_t l = wcslen(s);
  *this << l;
  addElement(E_WSTR,s,l*sizeof(wchar_t));
  return *this;
}

Packer &Packer::operator<<(wchar_t *s) {
  size_t l;
  *this >> l;
  getElement(E_WSTR,s,l*sizeof(wchar_t));
  s[l] = 0;
  return *this;
}

#ifdef UNICODE
#define E_STRING E_WSTR
#else
#define E_STRING E_STR
#endif

Packer &Packer::operator<<(const String &s) {
  const size_t l = s.length();
  *this << l;
  addElement(E_STRING,s.cstr(),l*sizeof(TCHAR));
  return *this;
}

Packer &Packer::operator>>(String &s) {
  size_t l;
  *this >> l;
  TCHAR *tmp = new TCHAR[l+1];
  getElement(E_STRING, tmp, l*sizeof(TCHAR));
  tmp[l] = 0;
  s = tmp;
  delete[] tmp;
  return *this;
}

Packer &Packer::operator<<(const ByteArray &a) {
  const size_t l = a.size();
  *this << l;
  if(l) {
    addElement(E_CHAR, a.getData(), l);
  }
  return *this;
}

Packer &Packer::operator>>(ByteArray &a) {
  size_t l;
  *this >> l;
  a.clear();
  if(l) {
    BYTE *tmp = new BYTE[l];
    getElement(E_CHAR, tmp, l);
    a.setData(tmp, l);
    delete[] tmp;
  }
  return *this;
}
