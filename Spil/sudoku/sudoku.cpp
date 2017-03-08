#include "stdafx.h"
#include "sudoku.h"
#include "sudokuDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CSudokuApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CSudokuApp::CSudokuApp() {
}

CSudokuApp theApp;

BOOL CSudokuApp::InitInstance() {
    AfxEnableControlContainer();

    CSudokuDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
    } else if (nResponse == IDCANCEL) {
    }

    return FALSE;
}
