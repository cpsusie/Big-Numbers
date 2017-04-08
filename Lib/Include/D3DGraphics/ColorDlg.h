#pragma once

#include "ColormapDialog.h"
#include "colormap.h"

class CColorDlg : public CColormapDialog<D3DCOLOR> {
private:
  const String m_caption;
public:
  CColorDlg(const String &caption, int propertyId, D3DCOLOR color, CWnd *pParent = NULL);

	enum { IDD = IDD_COLOR_DIALOG };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()
private:
  CColormap m_color;
  DECLARE_EVENTSINK_MAP()
  void OnColorchangedColormapColor();
};

