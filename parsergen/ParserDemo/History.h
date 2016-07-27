#pragma once

#include <Array.h>
#include <MyString.h>

class History : public Array<String> {
public:
  History();
  ~History();
  bool add(const String &s);
};

