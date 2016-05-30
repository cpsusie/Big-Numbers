#pragma once

#include "InteractiveRunnable.h"

class ProgressWindow {
public:
  ProgressWindow(CWnd *parent, InteractiveRunnable &jobToDo, unsigned int delay = 0, unsigned int updateRate = 400);
  // if delay > 0, jobToDo will run this amount of milliseconds before ProgressWindow gets visible
  // updateRate is in milliseconds
};
