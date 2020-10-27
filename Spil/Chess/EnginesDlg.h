#pragma once

#include <MFCUTil/OBMButton.h>

class CEnginesDlg : public CDialog, OptionsAccessor {
private:
  OBMButton      m_moveUpButton, m_moveDownButton;
  EngineRegister m_engineList, m_origList;
  bool isChanged() const;
  void updateListCtrl();
  int  getSelectedIndex();
  int  getListSize();
  void ajourButtons(int selected = -1);
  void addNewEngine();

public:
    CEnginesDlg(CWnd *pParent = nullptr);

    enum { IDD = IDD_ENGINES_DIALOG };
    CListCtrl   m_listCtrl;

    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnButtonAdd();
    afx_msg void OnButtonDelete();
    afx_msg void OnButtonCheck();
    afx_msg void OnButtonMoveUp();
    afx_msg void OnButtonMoveDown();
    afx_msg void OnItemChangedList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    DECLARE_MESSAGE_MAP()
};

