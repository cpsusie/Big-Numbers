#pragma once

#include <MFCUtil/PropertyDialog.h>
#include "GridParameters.h"

typedef enum {
  PROP_GRIDPARAM
} DialogProperties;

class CGridDlg : public CPropertyDialog<GridParameters> {
private:
	DECLARE_DYNAMIC(CGridDlg)
  HACCEL         m_accelTable;
  const PixRect *m_image;
  bool           m_changeHandlerActive;
  bool validate();
  void windowToValue();
  void valueToWindow(const GridParameters &param);
  void setCellSize(     double value);
  void setHorizontalCount(UINT value);
  void setVerticalCount(  UINT value);
  void setColorCount(     UINT value);
  void resetControls();
  void cellCountFromSize();
  void updateTotalCellCount();
  void releaseImage();
  bool getUintValue(int id, UINT &value);
public:
	CGridDlg(CWnd *pParent = NULL);
	virtual ~CGridDlg();

	enum { IDD = IDR_PEARLGRID };
  const GridParameters &getParam() const {
    return getCurrentValue();
  }
  void setImage(PixRect *image);
  const PixRect *getImage() const {
    return m_image;
  }
protected:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
private:
  double       m_cellSize;
  unsigned int m_horizontalCount;
  unsigned int m_verticalCount;
  unsigned int m_colorCount;
  unsigned int m_totalCellCount;
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  virtual void OnCancel();
  afx_msg void OnClose();
  afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
  afx_msg LRESULT OnMsgResetControls(WPARAM wp, LPARAM lp);
  afx_msg void OnHideWindow();
  afx_msg void OnClickedCalculate();
  afx_msg void OnEnChangeEditCellSize();
  afx_msg void OnEnChangeEditHorizontalCellCount();
  afx_msg void OnEnChangeEditVerticalCellCount();
  afx_msg void OnEnChangeEditColorCount();
  afx_msg void OnDeltaposSpinHorizontalCount(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnDeltaposSpinVerticalCount(  NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnDeltaposSpinColorCount(     NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnGotoHorizontalCount();
  afx_msg void OnGotoVerticalCount();
  afx_msg void OnGotoCellSize();
  afx_msg void OnGotoColorCount();
};
