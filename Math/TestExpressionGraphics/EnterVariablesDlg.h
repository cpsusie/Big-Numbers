#pragma once

#include "VariableEditField.h"

class CEnterVariablesDlg : public CDialog {
private:
  Expression            &m_expr;
  VariableEditFieldArray m_fieldArray;
public:
    CEnterVariablesDlg(Expression &expr, CWnd* pParent = NULL);

    enum { IDD = IDD_ENTER_VARIABLES_DIALOG };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:

    virtual BOOL OnInitDialog();
    virtual void OnOK();
    DECLARE_MESSAGE_MAP()
};

