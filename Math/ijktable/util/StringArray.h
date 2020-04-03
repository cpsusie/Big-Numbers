#pragma once

#include "vector"
#include "Tokenizer.h"

class StringArray : public vector<string> {
public:
  StringArray() {
  }
  StringArray(Tokenizer &tok);
  StringArray(const char  **strArray); // terminate with NULL-pointer
  size_t maxLength() const;
  size_t minLength() const;
  string getAsDoubleNullTerminatedString() const; // return a double 0-terminated string containing all strings of the array, each 0-terminated
};
