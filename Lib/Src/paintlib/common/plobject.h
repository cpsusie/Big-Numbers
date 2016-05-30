#pragma once

class PLObject {
public:
  PLObject();
  virtual ~PLObject();
  virtual void AssertValid() const;
};

