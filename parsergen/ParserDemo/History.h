#pragma once

class History : public Array<String> {
public:
  History();
  ~History();
  bool add(const String &s);
};

