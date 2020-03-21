#include "pch.h"
#include <MyUtil.h>
#include <FileNameSplitter.h>
#include <ThreadPool.h>
#include <RunnableWrapper.h>
#include "ProgressDlg.h"
#include <MFCUtil/resource.h>
#include <MFCUtil/ProgressWindow.h>

ProgressWindow::ProgressWindow(CWnd *parent, InteractiveRunnable &jobToDo, UINT delay, UINT updateRate) {
  InteractiveRunnableWrapper rm(jobToDo);
  const Timestamp startTime = jobToDo.setStartTime();
  ThreadPool::executeNoWait(rm);

  for(;;) {
    const double timeElapsed = Timestamp::diff(startTime, Timestamp(), TMILLISECOND);
    const double sleepTime   = min((double)delay-timeElapsed, 150.0);
    if(sleepTime <= 0) {
      break;
    }
    Sleep((DWORD)sleepTime);
    if(rm.isJobDone()) {
      if(!rm.isOk()) {
        showError(rm.getErrorMsg());
      }
      return;
    }
  }

  CProgressDlg dlg(parent, rm, updateRate);
  const INT_PTR ret = dlg.DoModal();
  switch(ret) {
  case IDOK:
  case IDCANCEL:
    if(!rm.isOk()) {
      showError(rm.getErrorMsg());
    }
    break;
  default:
    { const String programName = FileNameSplitter(getModuleFileName()).getFileName();
      showError(_T("Cannot open CProgressDlg. Add \"#include <MFCUtil/MFCUtil.rc>\" to \"%s\\res\\%s.rc2\"")
               ,programName.cstr()
               ,programName.cstr());

    }
    break;
  }
}
