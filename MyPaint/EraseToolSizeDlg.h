#pragma once

class CEraseToolSizeDlg : public CDialog {
public:
  CEraseToolSizeDlg(const CSize &size, CWnd *pParent = NULL);

  CSize getToolSize() const {
    return CSize(m_width,m_height);
  }

private:
  enum { IDD = IDD_ERASETOOLSIZEDIALOG };
  UINT  m_height;
  UINT  m_width;

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()
};

