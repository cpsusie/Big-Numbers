#include "pch.h"
#include <Thread.h>
#include <Date.h>
#include <MFCUtil/ProgressWindow.h>
#include <MFCUtil/resource.h>
#include <MFCUtil/ProgressDlg.h>

ProgressWindow::ProgressWindow(CWnd *parent, InteractiveRunnable &jobToDo, UINT delay, UINT updateRate) {
  Thread jobExecutor(jobToDo);

  jobToDo.setStartTime();
  jobExecutor.start();

  while(delay) {
    const int sleepTime = min(delay, 150);
    Sleep(sleepTime);
    if(!jobExecutor.stillActive()) {
      return;
    }
    delay -= sleepTime;
  }

  CProgressDlg dlg(parent, jobExecutor, jobToDo, updateRate);
  INT_PTR ret;
  switch(ret = dlg.DoModal()) {
  case IDOK:
  case IDCANCEL:
    break;
  default:
    { const String programName = FileNameSplitter(getModuleFileName()).getFileName();
      showError(_T("Cannot open CProgressDlg. Add \"#include <MFCUtil/MFCUtil.rc>\" to \"%s\\res\\%s.rc2\"")
               ,programName.cstr()
               ,programName.cstr());

      while(jobExecutor.stillActive()) {
        Sleep(200);
      }
    }
  }
}
