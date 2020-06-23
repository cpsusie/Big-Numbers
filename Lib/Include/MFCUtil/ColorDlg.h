#pragma once

#include "ColormapDialog.h"
#include "resource.h"
#include "colormap.h"

class CColorDlg : public CColormapDialog<D3DCOLOR> {
private:
  DECLARE_EVENTSINK_MAP()
  const String m_caption;
  CColormap    m_color;
  void OnColorchangedColormapColor();
public:
  CColorDlg(const String &caption, int propertyId, D3DCOLOR color, CWnd *pParent = NULL);
  String getTypeName() const override {
    return _T("D3DCOLOR");
  }
  enum { IDD = _IDD_COLOR_DIALOG };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()
};

