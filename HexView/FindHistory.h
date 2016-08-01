#pragma once

class FindHistory : public StringArray {
public:
  FindHistory();
  ~FindHistory();
  bool add(const String &s);
};

