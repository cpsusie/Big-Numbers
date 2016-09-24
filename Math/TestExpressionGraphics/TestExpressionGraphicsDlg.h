#pragma once

#include <MFCUtil/StaticBottomAligned.h>
#include <TinyBitSet.h>
#include "DebugThread.h"

typedef enum {
  ISDIRTY
 ,ISCOMPILED
 ,ISDERIVED
 ,ISEXPRREDUCED
 ,ISDERIVEDREDUCED
 ,HASSAVEDVARIABLES
 ,HASFVALUE
 ,HASDERIVEDVALUE1
 ,HASDERIVEDVALUE2
} StatusFlags;

class CTestExpressionGraphicsDlg : public CDialog, public PropertyChangeListener {
private:
  HICON                        m_hIcon;
  HACCEL                       m_accelTabel;
  PixRectDevice                m_device;
  SimpleLayoutManager          m_layoutManager;
  CStaticBottomAligned         m_reductionStackWindow;
  BitSet16                     m_flags;
  Expression                   m_expr     , m_derivedExpr;
  ExpressionImage              m_exprImage, m_derivedImage;
  int                          m_numberFormat;
  ExpressionVariableArray      m_savedVariables;
  DebugThread                 *m_debugThread;
  PropertyContainer           *m_currentChildDlg;
  int                          m_contextWinId;
  const ExpressionRectangle   *m_contextRect;
  int                          m_debugWinId;
  Expression                  *m_debugExpr;
  String                       m_debugError;

#ifdef TRACE_REDUCTION_CALLSTACK
  const ReductionStackElement *m_selectedStackElement;
  const ReductionStackElement *getSelectedStackElement(CPoint p); // p in screen coordinates
#endif

  double getX(bool &ok);
  CComboBox *getFontSizeCombo() {
    return ((CComboBox*)GetDlgItem(IDC_COMBOFONTSIZE));
  }
  CComboBox *getNumberFormatCombo() {
    return ((CComboBox*)GetDlgItem(IDC_COMBONUMBERFORMAT));
  }
  int          getFontSize();
  NumberFormat getNumberFormat();
  bool loadMenu(CMenu &menu, int id);
  void compileExpr();
  void deriveExpr();
  void makeExprImage();
  void makeExprImage(const Expression &expr);
  void makeDerivedImage();
  void makeDerivedImage(const Expression &expr);

  void clearExprImage();
  void clearDerivedExpr();
  void clearDerivedImage();
  bool hasExprImage()    const { return !m_exprImage.isEmpty();    }
  bool hasDerivedImage() const { return !m_derivedImage.isEmpty(); }
  void paintExprImage();
  void paintDerivedImage();
  void paintImage(int id, const ExpressionImage &image);
  void onSelChangeCombo();
  void clearResultValue();
  void clearDerivedValue1();
  void clearDerivedValue2();
  void showDebugInfo(const TCHAR *format,...);
  void clearDebugInfo();
  void saveExprVariables();
  void clearSavedVariables();
  bool restoreExprVariables();
  void startThread(int debugWinId, bool singleStep);
  void createThread(Expression &expr);
  void destroyThread();
  void showError(const Exception &e);
  void showError(const TCHAR *format, ...);
  void paintDebugExpr();
  void enableFieldList(const int *ids, int n, bool enabled);
  void ajourDialogItems();
  void callNotepad(const String &s);
  inline bool hasDebugThread() const {
    return m_debugThread != NULL;
  }
  inline bool isThreadPaused() const {
    return hasDebugThread() && !m_debugThread->isRunning() && !m_debugThread->isTerminated();
  }
  inline bool isThreadRunning() const {
    return hasDebugThread() && m_debugThread->isRunning();
  }

  int                                getWindowIdFromPoint(CPoint &p); // return childwindow and adjust p to be relative to this window (if selectable)
  void                               setContextWindow(int winId, const ExpressionRectangle *rect = NULL);
  void                               clearContextWindow();
  void                               clearContextRect();
  inline bool                        hasContextWindow()          const { return m_contextWinId != -1;      }
  inline bool                        hasContextImage()                 { return getContextImage() != NULL; }
  inline bool                        hasContextRect()            const { return m_contextRect  != NULL;    }
  inline int                         getContextWindow()          const { return m_contextWinId;            }
  inline bool                        hasContextNode()            const { return getContextNode() != NULL;  }
               Expression           *getContextExpression();
               ExpressionImage      *getContextImage();
  inline const ExpressionRectangle  *getContextRect()            const { return m_contextRect;             }
  inline       ExpressionNode       *getContextNode()            const { return hasContextRect() ? m_contextRect->getNode() : NULL; }
  inline       ExpressionInputSymbol getContextNodeSymbol()      const { return hasContextNode() ? getContextNode()->getSymbol() : EOI; }
  inline       bool                  isContextNodeExpandable()   const { return hasContextNode() ? getContextNode()->isExpandable() : false; }
  inline       bool                  isContextNodeMultiplyable() const { return hasContextNode() ? getContextNode()->isMultiplyable() : false; }
  void                               updateContextWinImage();
  Expression                        *getExprFromWinId( int winId);
  ExpressionImage                   &getImageFromWinId(int winId);
  bool                               hasImageInWindow( int winId) const;

public:
  CTestExpressionGraphicsDlg(CWnd *pParent = NULL);
 ~CTestExpressionGraphicsDlg();
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
  enum { IDD = IDD_TESTEXPRESSIONGRAPHICS_DIALOG };
  CString	m_exprText;
  double	m_x;

public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
protected:
  virtual void DoDataExchange(CDataExchange *pDX);
protected:

  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg HCURSOR OnQueryDragIcon();
  virtual BOOL OnInitDialog();
  afx_msg void OnPaint();
  virtual void OnOK();
  virtual void OnCancel();
  afx_msg void OnClose();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnContextMenu(CWnd *pWnd, CPoint point);
  afx_msg void OnContextMenuShowExprTree();
  afx_msg void OnContextMenuToStandardForm();
  afx_msg void OnContextMenuToCanoncalForm();
  afx_msg void OnContextMenuToNumericForm();
  afx_msg void OnContextMenuShowNodeTree();
  afx_msg void OnContextMenuExpand();
  afx_msg void OnContextMenuMultiply();
  afx_msg void OnGotoX();
  afx_msg void OnGotoFontSize();
  afx_msg void OnGotoNumberFormat();
  afx_msg void OnChangeEditExpr();
  afx_msg void OnChangeEditX();
  afx_msg void OnSelchangeComboNumberFormat();
  afx_msg void OnSelChangeComboFontSize();
  afx_msg void OnFileExit();
  afx_msg void OnEditFindMatchingParentesis();
  afx_msg void OnEditGotoEditFx();
  afx_msg void OnEditEnterParameters();
  afx_msg void OnViewShowReductionStack();
  afx_msg void OnViewShowRectangles();
  afx_msg void OnDebugReduceExpr();
  afx_msg void OnDebugReduceDerived();
  afx_msg void OnDebugRun();
  afx_msg void OnDebugStop();
  afx_msg void OnDebugClearAllBreakPoints();
  afx_msg void OnDebugTraceReductionStep();
  afx_msg void OnDebugStep1ReduceIteration();
  afx_msg void OnDebugTestTreesEqual();
  afx_msg void OnFunctionsCompileFx();
  afx_msg void OnFunctionsDeriveFx();
  afx_msg void OnFunctionsEvaluateFx();
  afx_msg void OnFunctionsEvaluateDerived();
  afx_msg void OnFunctionsReduceFx();
  afx_msg void OnFunctionsReduceDerived();
  afx_msg void OnFunctionsEvaluateAll();
  afx_msg LRESULT OnMsgRunStateChanged(      WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgShowDebugError(       WPARAM wp, LPARAM lp);
  DECLARE_MESSAGE_MAP()
};

