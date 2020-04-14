#pragma once

#include <MFCUtil/ExprDialog.h>
#include <D3DGraphics/IsoSurface.h>

class CIsoSurfaceDlg : public SaveLoadExprDialog<IsoSurfaceParameters> {
private:
  CString m_expr;
  double  m_cellSize;
  double  m_lambda;
  UINT    m_bounds;
  BOOL    m_tetrahedral;
  BOOL    m_tetraOptimize4;
  BOOL    m_adaptiveCellSize;
  BOOL    m_originOutside;
  BOOL    m_machineCode;
  BOOL    m_createListFile;
  BOOL    m_debugPolygonizer;
  BOOL    m_doubleSided;
  BOOL    m_includeTime;
  UINT    m_frameCount;
  double  m_timefrom;
  double  m_timeto;
  double  m_xfrom;
  double  m_xto;
  double  m_yfrom;
  double  m_yto;
  double  m_zfrom;
  double  m_zto;

  String getListFileName() const;
  bool validate();
  void paramToWin(const IsoSurfaceParameters &param);
  bool winToParam(      IsoSurfaceParameters &param);
  void enableCheckBoxOrigin();
  void enableTimeFields();
  void enableCheckBoxTetraOptimize4();
  inline void setXInterval(const DoubleInterval &interval) {
    m_xfrom = interval.getMin(); m_xto = interval.getMax();
  }
  inline void setYInterval(const DoubleInterval &interval) {
    m_yfrom = interval.getMin(); m_yto = interval.getMax();
  }
  inline void setZInterval(const DoubleInterval &interval) {
    m_zfrom = interval.getMin(); m_zto = interval.getMax();
  }
  inline void setTimeInterval(const DoubleInterval &interval) {
    m_timefrom = interval.getMin(); m_timeto = interval.getMax();
  }
  inline DoubleInterval getXInterval() const {
    return DoubleInterval(m_xfrom, m_xto);
  }
  inline DoubleInterval getYInterval() const {
    return DoubleInterval(m_yfrom, m_yto);
  }
  inline DoubleInterval getZInterval() const {
    return DoubleInterval(m_zfrom, m_zto);
  }
  inline DoubleInterval getTimeInterval() const {
    return DoubleInterval(m_timefrom, m_timeto);
  }
public:
  CIsoSurfaceDlg(const IsoSurfaceParameters &param, CWnd *pParent = NULL);

#ifdef DEBUG_POLYGONIZER
  inline bool getDebugPolygonizer() const {
    return m_debugPolygonizer ? true : false;
  }
#endif //  DEBUG_POLYGONIZER

  enum { IDD = IDR_ISOSURFACE };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    afx_msg BOOL OnInitDialog();
    afx_msg void OnEditFindMatchingParentesis();
    afx_msg void OnGotoExpr();
    afx_msg void OnGotoCellSize();
    afx_msg void OnGotoLambda();
    afx_msg void OnGotoXInterval();
    afx_msg void OnGotoYInterval();
    afx_msg void OnGotoZInterval();
    afx_msg void OnGotoTimeInterval();
    afx_msg void OnGotoFrameCount();
    afx_msg void OnCheckDoubleSided();
    afx_msg void OnCheckIncludeTime();
    afx_msg void OnCheckMachineCode();
    afx_msg void OnCheckTetrahedral();
    afx_msg void OnButtonHelp();
    DECLARE_MESSAGE_MAP()
};
