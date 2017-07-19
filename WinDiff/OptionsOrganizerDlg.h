#pragma once

class COptionsOrganizerDlg : public CDialog {
private:
    OptionList m_nameList;
    void updateListCtrl();
    int  getSelectedIndex();
    int  getListSize();
    void ajourButtons(int selected = -1);

public:
    COptionsOrganizerDlg(CWnd *pParent = NULL);

    enum { IDD = IDD_DIALOGORGANIZEOPTIONS };
    CListCtrl   m_nameListCtrl;

    virtual void DoDataExchange(CDataExchange *pDX);
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnButtonRename();
    afx_msg void OnButtonDelete();
    afx_msg void OnButtonMoveUp();
    afx_msg void OnButtonMoveDown();
    virtual BOOL OnInitDialog();
    afx_msg void OnItemChangedList( NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnEndLabelEditList(NMHDR *pNMHDR, LRESULT *pResult);
    DECLARE_MESSAGE_MAP()
};

