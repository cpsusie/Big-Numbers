#pragma once

class COpeningDlgThread : public CWinThread, OptionsAccessor {
	DECLARE_DYNCREATE(COpeningDlgThread)
protected:
	COpeningDlgThread();

public:

	public:
	virtual BOOL InitInstance();

protected:
	virtual ~COpeningDlgThread();


	DECLARE_MESSAGE_MAP()
};

void startNewOpeningDialogThread();

