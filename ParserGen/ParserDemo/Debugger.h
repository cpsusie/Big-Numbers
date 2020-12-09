#pragma once

#include <InterruptableRunnable.h>
#include <PropertyContainer.h>
#include <FlagTraits.h>
#include "TestParser.h"

#define FL_BREAKONERROR   0x01
#define FL_BREAKONPROD    0x02
#define FL_BREAKONSTATE   0x04
#define FL_BREAKONSYMBOL  0x08
#define FL_BEFOREBREAKPOS 0x10
#define FL_BREAKONRESET   0x20
#define FL_BREAKONSHIFT   0x40
#define FL_BREAKSTEP      0x80
#define FL_ALLBREAKFLAGS  0x1F // all but FL_BREAKONRESET,FL_BREAKSTEP,FL_BREAKONSHIFT

typedef enum {
  DEBUGGER_STATE           // DebuggerState
 ,DEBUGGER_RUNTIME         // double*
 ,PARSER_PROPERTY          // *TestParserPropertyData (oldValue = nullptr)
} DebuggerProperty;

typedef enum {
  DEBUGGER_CREATED
 ,DEBUGGER_RUNNING
 ,DEBUGGER_PAUSED
 ,DEBUGGER_TERMINATED
} DebuggerState;

typedef enum {
  PPROP_PARSERERROR       // TestParserPropertyData*
 ,PPROP_PARSERDEBUG       // TestParserPropertyData*
} ParserPropertyId;

class TextAndSourcePos {
private:
  SourcePosition m_pos;
  String         m_text;
public:
  TextAndSourcePos() {
  }
  TextAndSourcePos(const SourcePosition &pos, const String &text)
    : m_pos(pos)
    , m_text(text)
  {
  }
  inline const SourcePosition &getPos() const {
    return m_pos;
  }
  const String &getText() const {
    return m_text;
  }
};

class ParserProperty {
public:
  TestParser      *m_parser;
  ParserPropertyId m_id;
  TextAndSourcePos m_textAndPos;   // use when m_id = PPROP_PARSERERROR or PPROP_PARSERDEBUG

  ParserProperty()
    : m_parser(nullptr)
    , m_id(ParserPropertyId(-1))
  {
  }
  ParserProperty(TestParser &parser, bool error, const SourcePosition &pos, const TCHAR *form, va_list argptr);
  inline bool operator==(const ParserProperty &p) const {
    return false;
  }
  inline bool operator!=(const ParserProperty &p) const {
    return !(*this == p);
  }
};

class CParserDemoDlg;

class DebuggerJob : public InterruptableRunnable, public PropertyContainer, public ParserHandler {
private:
  DebuggerState              m_state;
  CParserDemoDlg            &m_dlg;
  TestParser                &m_parser;
  FLAGTRAITS(DebuggerJob, BYTE, m_breakFlags);
  SourcePosition             m_startPos;
  UINT                       m_errorCount;
  double                     m_runTime;
  ParserProperty             m_parserProperty;
  inline DebuggerJob &checkTerminated() {
    if(getState() == DEBUGGER_TERMINATED) throwException(_T("Debugger is terminated"));
    return *this;
  }
  void suspend();
  void updateBreakFlags();

public:
  DebuggerJob(CParserDemoDlg *dlg, BYTE breakFlags);
  ~DebuggerJob();
  void singleStep(BYTE breakFlags);
  inline void go() {
    singleStep(0);
  }
  void breakASAP();
  void kill();
  UINT safeRun();
  inline DebuggerState getState() const {
    return m_state;
  }
  inline String getStateName() const {
    return getStateName(getState());
  }
  static String getStateName(DebuggerState state);
  void  handleError(const SourcePosition &pos, const TCHAR *form, va_list argptr) final;
  void  handleDebug(const SourcePosition &pos, const TCHAR *form, va_list argptr) final;
};

class Debugger { // monitor
private:
  DebuggerJob      *m_job;
  CParserDemoDlg   *m_dialog;
  mutable Semaphore m_lock;

  // no lock
  void createJob(BYTE breakFlags);
  // no lock
  void deleteJob();
  // no lock
  bool isJobActive() const;
public:
  Debugger() 
    : m_job(    nullptr)
    , m_dialog( nullptr)
  {
  }
  // with lock
  ~Debugger();
  // with lock
  void setDialog(CParserDemoDlg *dialog);
  // with lock
  bool isRunning() const;
  // no lock
  bool hasJob()    const;
  // with lock
  void restart(BYTE breakFlags);
  // with lock
  void step(BYTE breakFlags);
  // with lock
  void stop();
};
