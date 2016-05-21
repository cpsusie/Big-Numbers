#include "pch.h"

StringArray::StringArray(const char **strArray) { // terminate with NULL-pointer
  while(*strArray) {
    add(*(strArray++));
  }
}

StringArray::StringArray(Tokenizer &tok) {
  while(tok.hasNext()) {
    add(tok.next());
  }
}

int StringArray::maxLength() const {
  if(size() == 0) {
    return 0;
  }
  int result = (*this)[0].length();
  for(int i = 1; i < size(); i++) {
    const int l = (*this)[i].length();
    if(l > result) {
      result = l;
    }
  }
  return result;
}

int StringArray::minLength() const {
  if(size() == 0) {
    return 0;
  }
  int result = (*this)[0].length();
  for(int i = 1; i < size(); i++) {
    const int l = (*this)[i].length();
    if(l < result) {
      result = l;
    }
  }
  return result;
}

String StringArray::getAsDoubleNullTerminatedString() const { // return a double 0-terminated string containing all strings of the array, each 0-terminated
  const String zeroString = spaceString(1, _T('\0'));
  String result;
  for(int i = 0; i < size(); i++) {
    result = result + (*this)[i] + zeroString;
  }
  return result + zeroString;
}

