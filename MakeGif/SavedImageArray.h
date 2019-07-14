#pragma once

#include "PixRectArray.h"

class SavedImageArray : public CompactArray<SavedImage*> {
private:
  SavedImageArray(const SavedImageArray &src);            // not defined
  SavedImageArray &operator=(const SavedImageArray &src); // not defined
public:
  SavedImageArray(const PixRectArray &prArray, int maxColorCount);
  ~SavedImageArray();
  void clear(intptr_t capacity = 0);
};
