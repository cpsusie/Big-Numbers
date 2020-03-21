#pragma once

#include <RunnableWrapper.h>
#include "InteractiveRunnable.h"

class InteractiveRunnableWrapper : public RunnableWrapperTemplate<InteractiveRunnable> {
public:
  InteractiveRunnableWrapper(InteractiveRunnable &r) : RunnableWrapperTemplate<InteractiveRunnable>(r) {
  }

  UINT run();
};
