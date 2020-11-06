#pragma once

#include <InterruptableRunnable.h>
#include <Semaphore.h>
#include "WordBpn.h"

class TrainerJob : public InterruptableRunnable {
  Semaphore  m_lock;
  String     m_msg;
  int        m_priority;
  WordBpn    m_trainingNetwork;

  void updateMessage(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);

public:
  TrainerJob(const WordBpn &n, int priority);
  void    setPriority(int priority);
  String  getMessage();
  const WordBpn &getWordBpn() const {
    return m_trainingNetwork;
  }
  UINT    safeRun();
};
