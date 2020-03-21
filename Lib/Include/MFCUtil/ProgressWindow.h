#pragma once

#include "InteractiveRunnable.h"

class ProgressWindow {
public:
  // if delay > 0, jobToDo will run this amount of milliseconds before ProgressWindow gets visible
  // updateRate is in milliseconds
  ProgressWindow(CWnd *parent, InteractiveRunnable &jobToDo, UINT delay = 0, UINT updateRate = 400);
};
