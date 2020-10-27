#pragma once

class RulesDialog : public CDialog {
public:
    RulesDialog(CWnd *pParent = nullptr);

    enum { IDD = IDD_RULES_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
};

