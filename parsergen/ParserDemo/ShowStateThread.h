#pragma once

class CShowStateThread : public CWinThread {
  DECLARE_DYNCREATE(CShowStateThread)
protected:
  CShowStateThread();

public:
  CDialog *m_maindialog;
public:

    //{{AFX_VIRTUAL(CShowStateThread)
    public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    //}}AFX_VIRTUAL

protected:
    virtual ~CShowStateThread();

    //{{AFX_MSG(CShowStateThread)
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
