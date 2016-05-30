#pragma once

#include "resource.h"
#include "WinTools.h"

class CSelectDirDlg : public CDialog {
private:
  HACCEL     m_accelTable;
  CImageList m_images;
  void       fillTree(const TCHAR *path);
  CString    getSelectedPath() const;
public:
  CSelectDirDlg(const String &startDir, CWnd *pParent = NULL);
  String getSelectedDir() const {
    return (LPCTSTR)m_dir;
  }
    //{{AFX_DATA(CSelectDirDlg)
    enum { IDD = _IDD_SELECTDIR_DIALOG };
    CString	m_dir;
    CString	m_drive;
    //}}AFX_DATA

    //{{AFX_VIRTUAL(CSelectDirDlg)
    public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    //}}AFX_VIRTUAL

protected:

    //{{AFX_MSG(CSelectDirDlg)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnEditChangeDriveCombo();
    afx_msg void OnGotoDir();
    afx_msg void OnGotoDrive();
    afx_msg void OnSelChangeDriveCombo();
    afx_msg void OnDblclkDirTree(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
