#include "pch.h"
#include <MyUtil.h>
#include "NumberAddIn.h"

void *DEBUGHELPER::getRealObject(void *pobj, size_t size) const {
  DWORD got;
  HRESULT hr = ReadDebuggeeMemoryEx(this, GetRealAddress(this), size, pobj, &got);
  if (got != size) {
    throwException(_T("Tried to read %d bytes, at addr %llu. Got only %d"), size, GetRealAddress(this), got);
  }
  return pobj;
}

ProcessorType DEBUGHELPER::getProcessorType() const {
  return (ProcessorType)GetProcessorType(this);
}
