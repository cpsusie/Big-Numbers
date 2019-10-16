#include "pch.h"
#include <tcp.h>
#include <MathUtil.h>
#include <Packer.h>

Packer::Packer() {
  m_first = 0;
}

void Packer::write(ByteOutputStream &s) const {
  const int   len = (int)getDataSize();
  const ULONG nl  = htonl(len);
  s.putBytes((BYTE*)&nl, sizeof(nl));
  s.putBytes(m_buffer.getData(),len);
}

bool Packer::read(ByteInputStream &s) {
  ULONG nl;
  s.getBytesForced((BYTE*)&nl,sizeof(nl));
  const int len = ntohl(nl);
  if(len == -1) {
    return false;
  }

  BYTE buffer[4096];
  for(int total = 0; total < len;) {
    const int n = min(sizeof(buffer), len - total);
    s.getBytesForced(buffer,n);
    m_buffer.append(buffer, n);
    total += n;
  }
  m_first = 0; // get ready to unpack with >>
  return true;
}

void Packer::writeEos(ByteOutputStream &s) const {
  const int len = htonl(-1);
  s.putBytes((BYTE*)&len,sizeof(len));
}

void Packer::clear() {
  m_first = 0;
  m_buffer.clear();
}

Packer &Packer::addElement(ElementType t, const void *e, size_t size) {
  const BYTE ct = (BYTE)t;
  m_buffer.append(&ct            ,1   );
  m_buffer.append((const BYTE *)e,size);
  return *this;
}

Packer &Packer::getElement(ElementType et, void *e, size_t size) {
  const ElementType t = (ElementType)(m_buffer[m_first++]);
  if(t != et) {
    m_first--;
    throwInvalidArgumentException(__TFUNCTION__, _T("Received invalid datetype:%d. Expected %d"), t, et);
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

Packer &Packer::operator<<(SHORT n) {
  if(isChar(n)) {
    *this << (CHAR)n;
  } else {
    const USHORT ns = htons(n);
    addElement(E_SHORT, &ns, sizeof(ns));
  }
  return *this;
}

Packer &Packer::operator>>(SHORT &n) {
  switch(peekType()) {
  case E_CHAR     :
    { CHAR c;
      *this >> c; n = c;
    }
    break;
  case E_SHORT    :
    { USHORT ns;
      getElement(E_SHORT, &ns, sizeof(ns));
      n = ntohs(ns);
    }
    break;
  default:
    throwException(_T("%s:Invalid type:%d. Expected E_CHAR/SHORT"), __TFUNCTION__, peekType());
  }
  return *this;
}

// --------------------------------- USHORT --------------------------

Packer &Packer::operator<<(USHORT n) {
  if(isUchar(n)) {
    *this << (UCHAR)n;
  } else {
    const USHORT ns = htons(n);
    addElement(E_SHORT, &ns, sizeof(ns));
  }
  return *this;
}

Packer &Packer::operator>>(USHORT &n) {
  switch(peekType()) {
  case E_CHAR     :
    { UCHAR uc;
      *this >> uc; n = uc;
    }
    break;
  case E_SHORT    :
    { USHORT ns;
      getElement(E_SHORT, &ns, sizeof(ns));
      n = ntohs(ns);
    }
    break;
  default:
    throwException(_T("%s:Invalid type:%d. Expected E_CHAR/SHORT"), __TFUNCTION__, peekType());
  }
  return *this;
}

// --------------------------------- LONG --------------------------

Packer &Packer::operator<<(LONG n) {
  if(isShort(n)) {
    *this << (SHORT)n;
  } else {
    const ULONG nl = htonl(n);
    addElement(E_LONG, &nl, sizeof(nl));
  }
  return *this;
}

Packer &Packer::operator>>(LONG &n) {
  switch(peekType()) {
  case E_CHAR     :
  case E_SHORT    :
    { SHORT s;
      *this >> s; n = s;
    }
    break;
  case E_RESERVED :
    { ULONG nl;
      getElement(E_RESERVED, &nl, sizeof(nl));
      n = ntohl(nl);
    }
    break;
  case E_LONG     :
    { ULONG nl;
      getElement(E_LONG, &nl, sizeof(nl));
      n = ntohl(nl);
    }
    break;
  default:
    throwException(_T("%s:Invalid type:%d. Expected E_CHAR/SHORT/LONG"), __TFUNCTION__, peekType());
  }
  return *this;
}

// --------------------------------- ULONG --------------------------

Packer &Packer::operator<<(ULONG n) {
  if(isUshort(n)) {
    *this << (USHORT)n;
  } else {
    const ULONG nl = htonl(n);
    addElement(E_LONG, &nl, sizeof(nl));
  }
  return *this;
}

Packer &Packer::operator>>(ULONG &n) {
  switch(peekType()) {
  case E_CHAR     :
  case E_SHORT    :
    { USHORT us;
      *this >> us; n = us;
    }
    break;
  case E_RESERVED :
    { ULONG nl;
      getElement(E_RESERVED, &nl, sizeof(nl));
      n = ntohl(nl);
    }
    break;
  case E_LONG     :
    { ULONG nl;
      getElement(E_LONG, &nl, sizeof(nl));
      n = ntohl(nl);
    }
    break;
  default:
    throwException(_T("%s:Invalid type:%d. Expected E_CHAR/SHORT/LONG"), __TFUNCTION__, peekType());
  }
  return *this;
}

// --------------------------------- INT64 --------------------------
Packer &Packer::operator<<(INT64 n) {
  if(isInt(n)) {
    *this << (INT)n;
  } else {
    const UINT64 nl = htonll(n);
    addElement(E_LONG_LONG, &nl, sizeof(nl));
  }
  return *this;
}

Packer &Packer::operator>>(INT64 &n) {
  switch(peekType()) {
  case E_CHAR     :
  case E_SHORT    :
  case E_RESERVED :
  case E_LONG     :
    { LONG l;
      *this >> l; n = l;
    }
    break;
  case E_LONG_LONG:
    { UINT64 nl;
      getElement(E_LONG_LONG, &nl, sizeof(nl));
      n = ntohll(nl);
    }
    break;
  default:
    throwException(_T("%s:Invalid type:%d. Expected E_CHAR/SHORT/LONG/LONG_LONG"), __TFUNCTION__, peekType());
  }
  return *this;
}

// --------------------------------- UINT64 --------------------------
Packer &Packer::operator<<(UINT64 n) {
  if(isUint(n)) {
    *this << (UINT)n;
  } else {
    const UINT64 nl = htonll(n);
    addElement(E_LONG_LONG, &nl, sizeof(nl));
  }
  return *this;
}

Packer &Packer::operator>>(UINT64 &n) {
  switch(peekType()) {
  case E_CHAR     :
  case E_SHORT    :
  case E_RESERVED :
  case E_LONG     :
    { ULONG ul;
      *this >> ul; n = ul;
    }
    break;
  case E_LONG_LONG:
    { UINT64 nl;
      getElement(E_LONG_LONG, &nl, sizeof(nl));
      n = ntohll(nl);
    }
    break;
  default:
    throwException(_T("%s:Invalid type:%d. Expected E_CHAR/SHORT/LONG/LONG_LONG"), __TFUNCTION__, peekType());
  }
  return *this;
}

// ----------------------------------Float  ---------------------------

Packer &Packer::operator<<(float n) {
  if(isInt(n)) {
    *this << getInt(n);
  } else {
    addElement(E_FLOAT , &n, sizeof(n));
  }
  return *this;
}

Packer &Packer::operator>>(float &n) {
  switch(peekType()) {
  case E_CHAR     :
  case E_SHORT    :
  case E_RESERVED :
  case E_LONG     :
    { int i;
      *this >> i;
      n = (float)i;
    }
    break;
  case E_FLOAT    :
    getElement(E_FLOAT, &n, sizeof(float));
    break;
  default         :
    throwException(_T("%s:Invalid type:%d. Expected E_CHAR/SHORT/LONG/FLOAT"), __TFUNCTION__, peekType());
  }
  return *this;
}

// ----------------------------------Double ---------------------------

Packer &Packer::operator<<(double n) {
  if(isFloat(n)) {
    *this << getFloat(n);
  } else if(isInt64(n)) {
    *this << getInt64(n);
  } else {
    addElement(E_DOUBLE, &n, sizeof(double));
  }
  return *this;
}

Packer &Packer::operator>>(double &n) {
  switch(peekType()) {
  case E_CHAR     :
  case E_SHORT    :
  case E_LONG     :
  case E_RESERVED :
  case E_LONG_LONG:
    { INT64 i;
      *this >> i;
      n = (double)i;
    }
    break;
  case E_FLOAT    :
    { float f;
      *this >> f;
      n = f;
    }
    break;
  case E_DOUBLE   :
    getElement(E_DOUBLE, &n, sizeof(double));
    break;
  default:
    throwException(_T("%s:Invalid type:%d. Expected E_CHAR/SHORT/LONG/LONG_LONG/FLOAT/DOUBLE"), __TFUNCTION__, peekType());
  }
  return *this;
}
