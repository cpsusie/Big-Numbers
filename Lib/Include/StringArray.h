#pragma once

#include "Array.h"
#include "Tokenizer.h"

class StringArray : public Array<String> {
public:
  StringArray() {}
  explicit StringArray(size_t capacity) : Array(capacity) {
  }
  StringArray(const char    **strArray); // terminate with NULL-pointer
  StringArray(const wchar_t **strArray); // terminate with NULL-pointer
  StringArray(Tokenizer &tok);
  size_t maxLength() const;
  size_t minLength() const;
  String getAsDoubleNullTerminatedString() const; // return a double 0-terminated string containing all strings of the array, each 0-terminated
};

Packer &operator<<(Packer &p, const StringArray &strings);
Packer &operator>>(Packer &p, StringArray &strings);
