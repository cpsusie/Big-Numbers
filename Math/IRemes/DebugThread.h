#pragma once

#include <Thread.h>
#include <PropertyChangeListener.h>
#include <TinyBitSet.h>
#include "BigRealRemes2.h"

typedef enum {
  THREAD_RUNNING           // bool*
 ,THREAD_TERMINATED        // bool*
 ,THREAD_ERROR             // TCHAR*
 ,REMES_PROPERTY           // *RemesPropertyData (oldValue = NULL)
} DebugThreadProperty;

class RemesPropertyData {
public:
  const Remes  &m_src;
  RemesProperty m_id;
  const void   *m_oldValue;
  const void   *m_newValue;
  RemesPropertyData(const Remes &src, int id, const void *oldValue, const void *newValue) 
    : m_src(src)
    , m_id((RemesProperty)id)
    , m_oldValue(oldValue)
    , m_newValue(newValue)
  {}
};

class DebugThread : public Thread, public PropertyContainer, public PropertyChangeListener {
private:
  DECLARECLASSNAME;
  bool                m_running, m_killed, m_terminated;
  String              m_errorMsg;
  BitSet8             m_breakPoints;
  int                 m_M, m_K;
  Remes              &m_r;

  void throwInvalidStateException(const TCHAR *method, RemesState state) const;
  void stop();
public:
  DebugThread(int M, int K, Remes &r);
  ~DebugThread();
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
  void singleStep();
  void singleSubStep();
  void stopASAP();
  void kill();
  void go();
  unsigned int run();
  inline const String &getErrorMsg() const {
    return m_errorMsg;
  }
  inline bool isRunning() const {
    return m_running;
  }
  inline bool isTerminated() const {
    return m_terminated;
  }
  const TCHAR *getStateName() const;
};
