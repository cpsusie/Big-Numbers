#pragma once

#include "SearchMachine.h"

class SaveAsJob : public InteractiveRunnable {
private:
  bool           m_ok;
  const String   m_newName;
  ByteContainer &m_src;
  const __int64  m_size;
  __int64        m_fileIndex;
  String         m_errorMessage;
  
  void doSave();
public:
  SaveAsJob(const String &newName, ByteContainer &src);

  UINT run();

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

  bool isOk() const {
    return m_ok;
  }

  const String &getErrorMessage() const {
    return m_errorMessage;
  }
};
