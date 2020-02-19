#pragma once

#include <MFCUtil/PropertyDialog.h>
#include "GridParameters.h"

typedef enum {
  PROP_GRIDPARAM
} DialogProperties;

class CGridDlg : public CPropertyDialog<GridParameters> {
private:
  HACCEL         m_accelTable;
  const PixRect *m_image;
  bool           m_changeHandlerActive;
  double         m_cellSize;
  unsigned int   m_horizontalCount;
  unsigned int   m_verticalCount;
  unsigned int   m_colorCount;
  double         m_cellSizeMM;

  DECLARE_DYNAMIC(CGridDlg)

  bool validate();
  void windowToValue();
  void calculate();
  void valueToWindow(const GridParameters &param);
  bool getData();
  void flushData();
  void setCellSize(     double value);
  void setHorizontalCount(UINT value);
  void setVerticalCount(  UINT value);
  void setColorCount(     int  value);
  void setCellSizeMM(   double value);
  void resetControls();
  void cellCountFromSize();
  void releaseImage();
  bool getUintValue(    int id, UINT   &value);
  bool getUintEmptyZero(int id, UINT   &value);
  void setUintEmptyZero(int id, UINT    value);
  bool getDoubleValue(  int id, double &value);
  CSize getImageSize() const;
public:
  CGridDlg(CWnd *pParent = NULL);
  virtual ~CGridDlg();

  enum { IDD = IDR_PEARLGRID };
  const GridParameters &getParam() const {
    return getCurrentValue();
  }
  void setImage(const PixRect *image);
  const PixRect *getImage() const {
    return m_image;
  }
  String getTypeName() const {
    return _T("GridParameters");
  }
protected:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  virtual void OnCancel();
  afx_msg void OnClose();
  afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
  afx_msg LRESULT OnMsgResetControls(WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgNewImage(     WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgWindowToTop(  WPARAM wp, LPARAM lp);
  afx_msg void OnHideWindow();
  afx_msg void OnClickedCalculate();
  afx_msg void OnBnClickedCheckAutoCalculate();
  afx_msg void OnBnClickedButtonDiagram();
  afx_msg void OnEnChangeEditCellSize();
  afx_msg void OnEnChangeEditHorizontalCount();
  afx_msg void OnEnChangeEditVerticalCount();
  afx_msg void OnEnChangeEditColorCount();
  afx_msg void OnEnChangeEditCellSizeMM();
  afx_msg void OnDeltaposSpinHorizontalCount(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnDeltaposSpinVerticalCount(  NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnDeltaposSpinColorCount(     NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnGotoHorizontalCount();
  afx_msg void OnGotoVerticalCount();
  afx_msg void OnGotoCellSize();
  afx_msg void OnGotoColorCount();
  DECLARE_MESSAGE_MAP()
};
