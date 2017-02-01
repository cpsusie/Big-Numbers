#pragma once

class CCheckFilesDlgThread : public CWinThread {
  DECLARE_DYNCREATE(CCheckFilesDlgThread)
protected:
  CCheckFilesDlgThread();

public:
public:
  virtual BOOL InitInstance();
  virtual int ExitInstance();

protected:
  virtual ~CCheckFilesDlgThread();


  DECLARE_MESSAGE_MAP()
};

