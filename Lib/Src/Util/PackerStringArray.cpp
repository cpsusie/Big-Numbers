#include "pch.h"
#include "Packer.h"

Packer &operator<<(Packer &p, const StringArray &strings) {
  p << (UINT64)strings.size();
  for(size_t i = 0; i < strings.size(); i++) {
    p << strings[i];
  }
  return p;
}

Packer &operator>>(Packer &p, StringArray &strings) {
  strings.clear();
  UINT64 n;
  p >> n;
  for(UINT64 i = 0; i < n; i++) {
    String tmp;
    p >> tmp;
    strings.add(tmp);
  }
  return p;
}
