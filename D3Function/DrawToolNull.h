#pragma once

#include "DrawTool.h"

class DrawToolNull : public DrawTool {
public:
  DrawToolNull()
    : DrawTool(nullptr)
  {
  }
};
