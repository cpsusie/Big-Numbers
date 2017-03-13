#include "pch.h"
#include <tcp.h>
#include "Packer.h"
#include <MyAssert.h>

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
    throwInvalidArgumentException(__TFUNCTION__,_T("Received invalid datetype:%d. Expected %d"), t, et);
  }
  assert(m_buffer.size() >= m_first + size);
  memcpy(e, m_buffer.getData() + m_first, size);
  m_first += size;
  if(m_first == m_buffer.size()) {
    clear();
  }
  return *this;
}

// --------------------------------- USHORT --------------------------

Packer &Packer::operator<<(USHORT n) {
  const USHORT ns = htons(n);
  addElement(E_SHORT, &ns, sizeof(ns));
  return *this;
}

Packer &Packer::operator>>(USHORT &n) {
  switch (peekType()) {
  case E_CHAR     :
    { BYTE b;
      *this >> b; n = b;
    }
    break;
  case E_SHORT    :
    { USHORT ns;
      getElement(E_SHORT, &ns, sizeof(ns));
      n = ntohs(ns);
    }
    break;
  default:
    throwException(_T("%s:Invalid type:%d. Expected E_CHAR/E_SHORT"), __TFUNCTION__, peekType());
  }
  return *this;
}

// --------------------------------- UINT --------------------------

Packer &Packer::operator<<(UINT n) {
  const UINT ni = htonl(n);
  return addElement(E_INT, &ni, sizeof(ni));
}

Packer &Packer::operator>>(UINT &n) {
  switch (peekType()) {
  case E_CHAR     :
    { BYTE b;
      *this >> b; n = b;
    }
    break;
  case E_SHORT    :
    { USHORT us;
      *this >> us; n = us;
    }
    break;
  case E_INT      :
    { UINT ni;
      getElement(E_INT, &ni, sizeof(ni));
      n = ntohl(ni);
    }
    break;
  case E_LONG     :
    { ULONG ul;
      *this >> ul; n = ul;
    }
    break;
  default: 
    throwException(_T("%s:Invalid type:%d. Expected E_CHAR/E_SHORT/E_INT/E_LONG"), __TFUNCTION__, peekType());
  }
  return *this;
}

// --------------------------------- ULONG --------------------------

Packer &Packer::operator<<(ULONG n) {
  const ULONG nl = htonl(n);
  return addElement(E_LONG, &nl, sizeof(nl));
}

Packer &Packer::operator>>(ULONG &n) {
  switch (peekType()) {
  case E_CHAR     :
    { BYTE b;
      *this >> b; n = b;
    }
    break;
  case E_SHORT    :
    { USHORT us;
      *this >> us; n = us;
    }
    break;
  case E_INT      :
    { UINT ui;
      *this >> ui; n = ui;
    }
    break;
  case E_LONG     :
    { ULONG nl;
      getElement(E_LONG, &nl, sizeof(nl));
      n = ntohl(nl);
    }
    break;
  default: 
    throwException(_T("%s:Invalid type:%d. Expected E_CHAR/E_SHORT/E_INT/E_LONG"), __TFUNCTION__, peekType());
  }
  return *this;
}

// --------------------------------- UINT64 --------------------------

Packer &Packer::operator<<(UINT64 n) {
  const UINT64 nl = htonll(n);
  return addElement(E_LONG_LONG, &nl, sizeof(nl));
}

Packer &Packer::operator>>(UINT64 &n) {
  switch (peekType()) {
  case E_CHAR     :
    { BYTE b;
      *this >> b; n = b;
    }
    break;
  case E_SHORT    :
    { USHORT us;
      *this >> us; n = us;
    }
    break;
  case E_INT      :
    { UINT ui;
      *this >> ui; n = ui;
    }
    break;
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
    throwException(_T("%s:Invalid type:%d. Expected E_CHAR/E_SHORT/E_INT/E_LONG/E_LONG_LONG"), __TFUNCTION__, peekType());
  }
  return *this;
}

// ----------------------------------Double ---------------------------

Packer &Packer::operator>>(double &n) {
  switch (peekType()) {
  case E_FLOAT:
    { float f;
      *this >> f;
      n = f;
    }
    break;
  case E_DOUBLE:
    getElement(E_DOUBLE, &n, sizeof(n));
    break;
  default:
    throwException(_T("%s:Invalid type:%d. Expected E_FLOAT/E_DOUBLE"), __TFUNCTION__, peekType());
  }
  return *this;
}
