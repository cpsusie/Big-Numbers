#pragma once

#include <TreeMap.h>

class BytePair {
private:
  const BYTE m_from, m_to;

public:
  BytePair(BYTE from, BYTE to) : m_from(from), m_to(to) {
  }
  BYTE getFrom() const {
    return m_from;
  }
  BYTE getTo() const {
    return m_to;
  }
};

class ModificationMap : private Int64TreeMap<BytePair> {
private:
  AddrRange getAddrRange();
public:
  bool addModification(ByteContainer &bc, unsigned __int64 addr, BYTE to, BYTE &old); // if true is returned, old bytevalue is returned in old
  void applyModifications(unsigned __int64 offset, ByteArray &content);
  void applyModifications(UpdatableByteContainer &ubc);
  void clear() {
    Int64TreeMap<BytePair>::clear();
  }

  bool isEmpty() const {
    return Int64TreeMap<BytePair>::isEmpty();
  }
};

class ByteModification : public BytePair {
private:
  const __int64 m_addr;
public:
  ByteModification(__int64 addr, BYTE from, BYTE to) : m_addr(addr), BytePair(from,to) {
  }
  __int64 getAddr() const {
    return m_addr;
  }
};

class EditHistory : public Array<ByteModification> {
};
