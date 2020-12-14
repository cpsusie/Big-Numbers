#include "stdafx.h"
#include <ThreadPool.h>
#include <Thread.h>
#include "Debugger.h"
#include "ParserDemoDlg.h"

DebuggerJob::DebuggerJob(CParserDemoDlg *dlg, BYTE breakFlags)
: m_dlg(      *dlg              )
, m_parser(    dlg->m_parser    )
, m_breakFlags(breakFlags       )
, m_state(     DEBUGGER_CREATED )
, m_errorCount(0                )
, m_runTime(   0                )
{
  addPropertyChangeListener(dlg);
  m_parser.setHandler(this);
  ThreadPool::executeNoWait(*this);
}

DebuggerJob::~DebuggerJob() {
  kill();
  m_parser.setHandler(nullptr);
}

void DebuggerJob::updateBreakFlags() {
  m_breakFlags.clr(FL_ALLBREAKFLAGS);
  if(m_dlg.m_breakOnError      ) m_breakFlags.set(FL_BREAKONERROR );
  if(m_dlg.m_breakOnProduction ) m_breakFlags.set(FL_BREAKONPROD  );
  if(m_dlg.m_breakOnState      ) m_breakFlags.set(FL_BREAKONSTATE );
  if(m_dlg.m_breakOnSymbol     ) m_breakFlags.set(FL_BREAKONSYMBOL);
  if(m_dlg.m_textBox.isMarked() && (m_parser.getScanner()->getPos() < m_dlg.m_textBox.getMarkedPos())) {
    m_breakFlags.set(FL_BEFOREBREAKPOS);
  }
}

void DebuggerJob::singleStep(BYTE breakFlags) {
  checkTerminated().clrFlag(0xff).setFlag(breakFlags).resume();
}

void DebuggerJob::kill() {
  setInterrupted();
  waitUntilJobDone();
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_TERMINATED);
}

UINT DebuggerJob::safeRun() {
  SETTHREADDESCRIPTION("DebuggerJob");
  const double startTime = getThreadTime();
  m_parser.setNewInput(m_dlg.getInputString().cstr(), m_dlg.makeDerivationTree());
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_RUNNING);
  m_parser.parseBegin();
  if(m_breakFlags & FL_BREAKONRESET) {
    suspend();
  }
  updateBreakFlags();

  m_startPos = m_parser.getScanner()->getPos();
  while(!m_parser.done()) {
    m_parser.parseStep();
    checkInterruptAndSuspendFlags();
    if(m_breakFlags & FL_BREAKSTEP) {
      suspend();
      continue;
    }
    if(m_breakFlags) {
      if((m_breakFlags & FL_BREAKONSHIFT) && (m_parser.getScanner()->getPos() != m_startPos)) {
        suspend();
        continue;
      }
      const Action action = m_parser.getNextAction();
      if((m_breakFlags & FL_BREAKONERROR) && ((m_dlg.m_errorPos.size() > m_errorCount) || action.isParserError())) {
        suspend();
        continue;
      }
      if((m_breakFlags & FL_BREAKONPROD) && (action.isReduceAction() && m_dlg.m_breakProductions->contains(action.getReduceProduction()))) {
        suspend();
        continue;
      }
      if((m_breakFlags & FL_BREAKONSTATE) && m_dlg.m_breakStates->contains(m_parser.state())) {
        suspend();
        continue;
      }
      if((m_breakFlags & FL_BREAKONSYMBOL) && m_dlg.m_breakSymbols->contains(m_parser.input())) {
        suspend();
        continue;
      }
      if((m_breakFlags & FL_BEFOREBREAKPOS) && m_dlg.m_textBox.isMarked()) {
        if(m_parser.getScanner()->getPos() >= m_dlg.m_textBox.getMarkedPos()) {
          m_breakFlags.clr(FL_BEFOREBREAKPOS);
          suspend();
        }
      }
    }
  }
  const double t = getThreadTime() - startTime;
  setProperty(DEBUGGER_RUNTIME, m_runTime, t);
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_TERMINATED);
  return 0;
}

void DebuggerJob::suspend() {
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_PAUSED);
  __super::suspend();
  updateBreakFlags();
  m_startPos = m_parser.getScanner()->getPos();
  m_errorCount = (UINT)m_dlg.m_errorPos.size();

  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_RUNNING);
}

void DebuggerJob::breakASAP() {
  if(getState() == DEBUGGER_RUNNING) {
    setSuspended();
  }
}

String DebuggerJob::getStateName(DebuggerState state) { // static
#define CASESTR(s) case DEBUGGER_##s: return _T(#s)
  switch(state) {
  CASESTR(CREATED   );
  CASESTR(RUNNING   );
  CASESTR(PAUSED    );
  CASESTR(TERMINATED);
  default: return format(_T("Unknown debuggerState:%d"), state);
  }
#undef CASESTR
}

ParserProperty::ParserProperty(TestParser &parser, bool error, const SourcePosition &pos, const TCHAR *form, va_list argptr) {
  m_parser = &parser;
  if(error) {
    m_id         = PPROP_PARSERERROR;
    m_textAndPos = TextAndSourcePos(pos, format(_T("error in (%d,%d):%s"), pos.getLineNumber(), pos.getColumn(), vformat(form, argptr).cstr()));
  } else {
    m_id         = PPROP_PARSERDEBUG;
    String line  = format(_T("(%d,%d):%s"), pos.getLineNumber(), pos.getColumn(), vformat(form, argptr).cstr());
    line.replace('\n', _T("\\n")).replace('\r', _T("\\r")).replace('\t', _T("\\t"));
    m_textAndPos = TextAndSourcePos(pos, line);
  }
}

void DebuggerJob::handleError(const SourcePosition &pos, const TCHAR *form, va_list argptr) {
  setProperty(PARSER_PROPERTY, m_parserProperty, ParserProperty(m_parser, true, pos, form, argptr));
}

void DebuggerJob::handleDebug(const SourcePosition &pos, const TCHAR *form, va_list argptr) {
  setProperty(PARSER_PROPERTY, m_parserProperty, ParserProperty(m_parser, false, pos, form, argptr));
}


// with lock
Debugger::~Debugger() {
  m_lock.wait();
  if(hasJob()) {
    deleteJob();
  }
  m_lock.notify();
}

// with lock
void Debugger::setDialog(CParserDemoDlg *dialog) {
  m_lock.wait();
  if(hasJob()) {
    deleteJob();
  }
  m_dialog = dialog;
  m_lock.notify();
}

// with lock
bool Debugger::isRunning() const {
  m_lock.wait();
  const bool result = isJobActive();
  m_lock.notify();
  return result;
}

// with lock
void Debugger::restart(BYTE breakFlags) {
  m_lock.wait();
  try {
    deleteJob();
    createJob(breakFlags);
  } catch(...) {
    m_lock.notify();
    throw;
  }
  m_lock.notify();
}

// with lock
void Debugger::step(BYTE breakFlags) {
  m_lock.wait();
  try {
    if(!hasJob()) {
      createJob(breakFlags);
    } else {
      switch(m_job->getState()) {
      case DEBUGGER_RUNNING   :
        break;
      case DEBUGGER_CREATED   :
      case DEBUGGER_PAUSED    :
        m_job->singleStep(breakFlags);
        break;
      case DEBUGGER_TERMINATED:
        deleteJob();
        createJob(breakFlags);
        break;
      }
    }
  } catch(...) {
    m_lock.notify();
    throw;
  }
  m_lock.notify();
}

// with lock
void Debugger::stop() {
  m_lock.wait();
  deleteJob();
  m_lock.notify();
}

// no lock
void Debugger::createJob(BYTE breakFlags) {
  if(m_dialog == nullptr) {
    throwException(_T("%s:m_dialog = null"), __TFUNCTION__);
  }
  if(hasJob()) {
    throwException(_T("%s:job not null"), __TFUNCTION__);
  }
  m_job = new DebuggerJob(m_dialog, breakFlags); TRACE_NEW(m_job);
}

// no lock
void Debugger::deleteJob() {
  if(hasJob()) {
    m_job->setInterrupted();
  }
  SAFEDELETE(m_job);
}

// no lock
bool Debugger::isJobActive() const {
  return hasJob() && (m_job->getState() == DEBUGGER_RUNNING);
}

// no lock
bool Debugger::hasJob() const {
  return m_job != nullptr;
}
