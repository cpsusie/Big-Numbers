#pragma once

#include "PixRectArray.h"

class SavedImageArray : public CompactArray<SavedImage*> {
private:
  SavedImageArray(const SavedImageArray &src);            // Not defined. Class not cloneable
  SavedImageArray &operator=(const SavedImageArray &src); // Not defined. Class not cloneable
public:
  SavedImageArray(const PixRectArray &prArray, int maxColorCount);
  ~SavedImageArray() override;
  void clear(intptr_t capacity = 0);
};
