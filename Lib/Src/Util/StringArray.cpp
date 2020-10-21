#include "pch.h"
#include <StringArray.h>

StringArray::StringArray(const char **strArray) { // terminate with nullptr-pointer
  while(*strArray) {
    add(*(strArray++));
  }
}

StringArray::StringArray(const wchar_t **strArray) { // terminate with nullptr-pointer
  while(*strArray) {
    add(*(strArray++));
  }
}

StringArray::StringArray(Tokenizer &tok) {
  while(tok.hasNext()) {
    add(tok.next());
  }
}

size_t StringArray::maxLength() const {
  if(size() == 0) {
    return 0;
  }
  size_t result = (*this)[0].length();
  for(size_t i = 1; i < size(); i++) {
    const size_t l = (*this)[i].length();
    if(l > result) {
      result = l;
    }
  }
  return result;
}

size_t StringArray::minLength() const {
  const size_t n = size();
  if(n == 0) {
    return 0;
  }
  size_t result = -1;
  for(size_t i = 0; i < n; i++) {
    const size_t l = (*this)[i].length();
    if(l < result) {
      if((result = l) == 0) {
        break;
      }
    }
  }
  return result;
}

String StringArray::getAsDoubleNullTerminatedString() const { // return a double 0-terminated string containing all strings of the array, each 0-terminated
  const String zeroString = spaceString(1, _T('\0'));
  String result;
  for(size_t i = 0; i < size(); i++) {
    result = result + (*this)[i] + zeroString;
  }
  return result + zeroString;
}
