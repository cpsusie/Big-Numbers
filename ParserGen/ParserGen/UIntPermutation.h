#pragma once

class UIntPermutation : public CompactUIntArray {
protected:
  UIntPermutation(size_t capacity);
  UIntPermutation();
  // Make an array, with size and capacity = capacity, all elements set to -1
  void init(UINT capacity);
public:
  inline UINT getOldCapacity() const {
    return (UINT)size();
  }
  // Find index of the specified element. throw exception if invalid argument or not found
  UINT findIndex(UINT v) const;
  // Check that all elements are distinct, and in range [0..size-1]
  // throw exception, if not
  virtual void validate() const;
};
