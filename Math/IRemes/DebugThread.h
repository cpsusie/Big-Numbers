#pragma once

#include <Thread.h>
#include <PropertyChangeListener.h>
#include <TinyBitSet.h>
#include "BigRealRemes2.h"

typedef enum {
  THREAD_RUNNING           // bool*
 ,THREAD_TERMINATED        // bool*
 ,THREAD_ERROR             // TCHAR*
 ,REMES_STATE              // RemesState*
 ,REMES_MAIN_ITERATION     // int*
 ,REMES_SEARCHE_ITERATION  // int*
 ,REMES_EXTREMA_COUNT      // int*
} DebugThreadProperty;

class DebugThread : public Thread, public RemesHandler, public PropertyContainer {
private:
  DECLARECLASSNAME;
  bool                m_running, m_killed, m_terminated;
  String              m_errorMsg;
  BitSet8             m_breakPoints;
  int                 m_M, m_K;
  Remes              &m_r;
  RemesState          m_oldState;
  int                 m_oldMainIteration, m_oldSearchEIteration, m_oldExtremaCount;
  const Remes        *m_rp;

  void throwInvalidStateException(const TCHAR *method, RemesState state) const;
  void stop();
  void setBoolProperty(DebugThreadProperty id, bool &v, bool newValue);
public:
  DebugThread(int M, int K, Remes &r);
  ~DebugThread();
  void handleData(const Remes &r);
  void singleStep();
  void singleSubStep();
  void stopASAP();
  void kill();
  void go();
  unsigned int run();
  const Remes &getRemes() const;
  inline const String &getErrorMsg() const {
    return m_errorMsg;
  }
  inline bool isRunning() const {
    return m_running;
  }
  inline bool isTerminated() const {
    return m_terminated;
  }
};
