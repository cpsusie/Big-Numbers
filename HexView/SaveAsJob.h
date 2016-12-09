#pragma once

#include "SearchMachine.h"

class SaveAsJob : public InteractiveRunnable {
private:
  bool           m_interrupted;
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

  void interrupt() {
    m_interrupted = true;
  };

  USHORT getProgress();

  USHORT getMaxProgress() {
    return 1000;
  }

  String getTitle() {       // Title of progress-window
    return format(_T("Save file as %s"), m_newName.cstr());
  }

  int getSupportedFeatures() { // Should return any combination of SUPPORT-constants
    return IR_INTERRUPTABLE | IR_PROGRESSBAR | IR_SHOWTIMEESTIMATE;
  }

  bool isInterrupted() const {
    return m_interrupted;
  }

  bool isOk() const {
    return m_ok;
  }

  const String &getErrorMessage() const {
    return m_errorMessage;
  }
};
