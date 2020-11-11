#include "stdafx.h"
#include "ModificationMap.h"

bool ModificationMap::addModification(ByteContainer &bc, unsigned __int64 addr, BYTE to, BYTE &old) {
  BytePair *bpp = get(addr);
  if(bpp) {
    if(to == bpp->getFrom()) {
      old = to;
      remove(addr);
      return true;
    } else if(to != bpp->getTo()) {
      BytePair nbp(bpp->getFrom(), to);
      old = bpp->getTo();
      remove(addr);
      put(addr, nbp);
      return true;
    }
  } else { // addr not found in map
    const BYTE from = bc.getByte(addr);
    if(to != from) {
      old = from;
      put(addr, BytePair(from, to));
      return true;
    }
  }
  return false;
}

void ModificationMap::applyModifications(unsigned __int64 offset, ByteArray &content) {
  const AddrRange modifiedRange = getAddrRange();
  const __int64 lastOffset = offset + content.size()-1;
  const AddrRange dataRange(offset, lastOffset);
  if(modifiedRange.overlap(dataRange)) {
    for(auto it = getIterator(); it.hasNext();) {
      const Entry<__int64, BytePair> &e = it.next();
      const __int64 &key = e.getKey();
      if(key >= (__int64)offset) {
        if(key <= lastOffset) {
          content[(UINT)(key - (__int64)offset)] = e.getValue().getTo();
        } else {
          break; // we're done
        }
      }
    }
  }
}

void ModificationMap::applyModifications(UpdatableByteContainer &ubc) {
  if(isEmpty()) {
    return;
  }

  Iterator<Entry<__int64, BytePair> > it = getIterator();
  Entry<__int64, BytePair> &e = it.next();

  __int64 seqStartAddr = e.getKey();
  __int64 lastAddr     = seqStartAddr;
  ByteArray byteSequence;
  byteSequence.add(e.getValue().getTo());
  while(it.hasNext()) {
    Entry<__int64, BytePair> &e1 = it.next();
    const __int64 addr = e1.getKey();
    if(addr != lastAddr+1) {
      ubc.putBytes(seqStartAddr, byteSequence);
      byteSequence.clear();
      seqStartAddr = addr;
    }
    byteSequence.add(e1.getValue().getTo());
    lastAddr = addr;
  }
  ubc.putBytes(seqStartAddr, byteSequence); // byteSequence is NOT empty
}

AddrRange ModificationMap::getAddrRange() {
  if(isEmpty()) {
    return AddrRange();
  } else {
    const Set<__int64> set = keySet();
    return AddrRange(set.getMin(), set.getMax());
  }
}
