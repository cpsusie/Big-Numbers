#pragma once

#include "SearchMachine.h"

class SaveAsJob : public InteractiveRunnable {
private:
  const String   m_newName;
  ByteContainer &m_src;
  const __int64  m_size;
  __int64        m_fileIndex;

  void doSave();
public:
  SaveAsJob(const String &newName, ByteContainer &src);

  UINT safeRun();

  double getMaxProgress() const {
    return (double)m_size;
  }
  double getProgress() const {
    return (double)m_fileIndex;
  }

  String getTitle() {       // Title of progress-window
    return format(_T("Save file as %s"), m_newName.cstr());
  }

  int getSupportedFeatures() { // Should return any combination of SUPPORT-constants
    return IR_INTERRUPTABLE | IR_PROGRESSBAR | IR_SHOWTIMEESTIMATE;
  }
};
