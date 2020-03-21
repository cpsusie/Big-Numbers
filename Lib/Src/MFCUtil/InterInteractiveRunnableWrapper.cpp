#include "pch.h"
#ifdef _DEBUG
#include <Thread.h>
#endif
#include <MFCUTil/InteractiveRunnableWrapper.h>

UINT InteractiveRunnableWrapper::run() {
#ifdef _DEBUG
  setThreadDescription(getJob().getTitle());
#endif // _DEBUG
  return __super::run();
}
