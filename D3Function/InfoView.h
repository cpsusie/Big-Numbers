#pragma once

#include "MainFrm.h"

class CInfoView : public CFormView, public OptionsAccessor {
private:
  int  m_topLine; // 1st visible line in edit-box
  bool m_scrollEnabled;

  inline CEdit *getEditBox() {
    return (CEdit*)GetDlgItem(IDC_EDIT_INFOBOX);
  }
protected: // create from serialization only
  CInfoView();
  DECLARE_DYNCREATE(CInfoView)

public:
  enum { IDD = IDD_INFOVIEW };

  CD3FunctionDoc *GetDocument();
  void vprintf(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
  void printf( _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  inline void enableScroll(bool enabled) {
    m_scrollEnabled = enabled;
  }
  virtual ~CInfoView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

public:
  DECLARE_MESSAGE_MAP()
  virtual void OnDraw(           CDC *pDC);  // overridden to draw this view
  virtual BOOL OnPreparePrinting(CPrintInfo *pInfo);
  virtual void OnBeginPrinting(  CDC *pDC, CPrintInfo *pInfo);
  virtual void OnEndPrinting(    CDC *pDC, CPrintInfo *pInfo);
  afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnEnVscrollEditInfoBox();
};

#ifndef _DEBUG  // debug version in CInfoView.cpp
inline CD3FunctionDoc *CInfoView::GetDocument() {
  return (CWinDiffDoc*)m_pDocument;
}
#endif
