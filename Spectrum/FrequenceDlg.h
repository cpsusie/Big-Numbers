#pragma once

class CFrequenceDlg : public CDialog {
public:
    CFrequenceDlg(unsigned int frequence, CWnd *pParent = nullptr);
    unsigned int getFrequence() const {
      return m_frequence;
    }

private:
    enum { IDD = IDD_FREQUENCEDIALOG };
    UINT    m_frequence;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
};

