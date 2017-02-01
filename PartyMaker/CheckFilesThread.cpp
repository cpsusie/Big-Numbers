#include "stdafx.h"
#include "PartyMaker.h"
#include "CheckFilesThread.h"
#include "CheckFilesDlg.h"
#include <sys\stat.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CCheckFilesThread, CWinThread)

CCheckFilesThread::CCheckFilesThread()
: m_mediaArray(((CPartyMakerDlg*)AfxGetApp()->GetMainWnd())->getMediaArray())
, m_player(_T("MP3Check"))
{
}

CCheckFilesThread::~CCheckFilesThread() {
}

BOOL CCheckFilesThread::InitInstance() {
  m_index      = 0;
  m_errorCount = 0;
  m_suspend    = false;
  CWinThread::InitInstance();
  return TRUE;
}

int CCheckFilesThread::ExitInstance() {
  return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CCheckFilesThread, CWinThread)
END_MESSAGE_MAP()

void CCheckFilesThread::checkNummer() {
  const TCHAR *fileName = m_mediaArray[m_index].getFileName();
//  char ext[100];
//  getExtension(ext,fname);
//  if(stricmp(ext,_T(".wma")) != 0) return;
  try {
    m_player.open(fileName);
//    m_player.start();
//    FILE *f = fopen(_T("c:\\temp\\wma.dat"),_T("a"));
//    struct stat st;
//    stat(fname,&st);
//    fprintf(f,_T("%d %lf\n"),st.st_size,m_player.LengthInSec());
//    fclose(f);
//    m_player.setMute(CHANNEL_BOTH,false);
  } catch(MP3Exception e) {
    logError(_T("Fejl ved <%s>:%s"),fileName,e.what());
    m_errorCount++;
  } catch(Exception e) {
    logError(_T("Fejl ved <%s>:%s"),fileName,e.what());
    m_errorCount++;
  }
}

BOOL CCheckFilesThread::OnIdle(LONG lCount) {
  bool ret;
  if(m_index < m_mediaArray.size()) {
    checkNummer();
    m_index++;;
    ret = true;
  } else {
    ret = false;
  }
  CWinThread::OnIdle(lCount);
  if(m_suspend) {
    CWinThread::SuspendThread();
  }
  return ret;
}
