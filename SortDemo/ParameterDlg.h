#pragma once

class CParameterDlg : public CDialog {
private:
    HACCEL                m_accelTable;
    InitializeParameters  m_parameters;
    void radioChecked(int radioChecked);
    bool readTextFile(const String &fileName);
    void enableSeed();
    int getElementSize() const;
    void setElementSize(int v);
    void setRandomizationMethod(RandomizationMethod method);
    RandomizationMethod getRandomizationMethod();
    CComboBox *getComboElementSize() const;
    CComboBox *getComboRandomize()   const;
public:
    CParameterDlg(const InitializeParameters &parameters, CWnd *pParent = NULL);
    const InitializeParameters &getParameters() const {
      return m_parameters;
    }

    enum { IDD = IDD_PARAMETER_DIALOG };
    int     m_initMethod;
    UINT    m_elementCount;
    UINT    m_seed;
    UINT    m_periodCount;
    CString m_fileName;

    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnRadioRandom();
    afx_msg void OnRadioSorted();
    afx_msg void OnRadioInverseSorted();
    afx_msg void OnRadioSinus();
    afx_msg void OnRadioFileData();
    afx_msg void OnButtonBrowse();
    afx_msg void OnGotoElementCount();
    afx_msg void OnGotoElementSize();
    afx_msg void OnGotoPeriodCount();
    afx_msg void OnGotoFileName();
    afx_msg void OnGotoSeed();
    virtual void OnOK();
    afx_msg void OnSelchangeComboRandomize();
    DECLARE_MESSAGE_MAP()
};

