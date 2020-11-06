#pragma once

#include <Stack.h>
#include "MBCalculator.h"
#include "ImageListJob.h"
#include "EditColorMapDlgThread.h"

typedef enum {
  STATE_IDLE
 ,STATE_DRAGGING
 ,STATE_MOVING
} DlgState;

class ImageStackEntry {
public:
  const BigRealRectangle2D m_scale;
  CellCountMatrix         *m_ccm;
  const ColorMapData       m_cmd;
  ImageStackEntry(const BigRealRectangle2D &scale, CellCountMatrix *ccm, const ColorMapData &cmd)
  : m_scale(scale)
  , m_ccm(  ccm  )
  , m_cmd(  cmd  )
  {
  }
};

typedef enum {
  NW = 0
 ,SW = 1
 ,NE = 2
 ,SE = 3
} MoveDirection;

class CMandelbrotDlg;

class ExpTransformation {
private:
  BigReal         m_a, m_b;
  BigRealInterval m_toInterval;
public:
  ExpTransformation(const BigRealInterval &from, const BigRealInterval &to, DigitPool *digitPool=nullptr);
  inline BigReal transform(const BigReal &x) const {
    return m_b*rPow(m_a,x,20);
  }
  inline const BigRealInterval &getToInterval() const {
    return m_toInterval;
  }
  inline DigitPool *getDigitPool() const {
    return m_a.getDigitPool();
  }
};

class MBFrameGenerator : public FrameGenerator {
private:
  CMandelbrotDlg              &m_dlg;
  const String                 m_dirName;
  // in complex plane
  const BigRealRectangle2D     m_finalRect;
  BigRealRectangle2D           m_startRect;
  ExpTransformation           *m_expTransform;
  BigRealLinearTransformation *m_linearTransform;
  // in pixels
  const CSize                  m_frameSize;
  ImageListJob                *m_imageListJob;
  Semaphore                    m_frameReady;
  int                          m_totalFrameCount;
  int                          m_frameIndex;
  HBITMAP                      m_bm;
  HDC                          m_dc;

  static int findTotalFrameCount(const BigRealRectangle2D &startRect, const BigRealRectangle2D &finalRect);
  void postMovieDone();
  // Return false if done
  bool requestNextFrame();
  BigRealRectangle2D getInterpolatedRectangle() const;
  inline DigitPool *getDigitPool() const {
    return m_finalRect.getDigitPool();
  }
public:
  MBFrameGenerator(CMandelbrotDlg *dlg, const String &dirName);
  ~MBFrameGenerator();
  inline String getDirName() {
    return m_dirName;
  }
  inline CSize getFrameSize() {
    return m_frameSize;
  }
  void notifyFrameReady() {
    DEBUGLOG(_T("notify FrameReady\n"));
    m_frameReady.notify();
  }
  inline int getTotalFrameCount() const {
    return m_totalFrameCount;
  }
  inline int getFrameIndex() const {
    return m_frameIndex;
  }
  // should return nullptr when no more frames.
  HBITMAP nextBitmap();
};

class CMandelbrotDlg;

class DialogMBContainer : public MBContainer {
private:
  CMandelbrotDlg      *m_dlg;
  const ColorMapEntry *m_colorMap;
public:
  DialogMBContainer(CMandelbrotDlg *dlg, CellCountMatrix *m);
  const RealRectangleTransformation    &getRealTransformation()    const;
  const BigRealRectangleTransformation &getBigRealTransformation() const;
  UINT                                  getMaxCount()              const;
  size_t                                getDigits()                const;
  bool                                  useFPUCalculators()        const;
  CellCountAccessor                    *getCCA();
  bool                                  calculateWithOrbit()       const;
  bool                                  useEdgeDetection()         const;
  bool                                  getJobToDo(CRect &rect);
  CSize                                 getWindowSize()            const;
  void                                  paintMark(       const CPoint &p);
  COLORREF                              setOrbitPixel(   const CPoint &p, COLORREF color);
  void                                  resetOrbitPixels(const OrbitPoint *op, size_t count);
  // overrides DWordpixelAccessor::setPixel. color actually count
  void                                  setPixel(UINT x, UINT y, D3DCOLOR color);
  // overrides DWordPixelAccessor::getPixel. return count
  D3DCOLOR                              getPixel(UINT x, UINT y) const;
  void                                  handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
};

typedef enum {
  TIMER_CALCULATION = 1
 ,TIMER_MOVIEMAKER  = 2
} TimerId;

#define MAXCOUNT_CHANGED     0x0001
#define COLORDATA_CHANGED    0x0002
#define IMAGE_CHANGED        0x0004
#define WORKSIZE_CHANGED     0x0008
#define SCALE_CHANGED        0x0010

class CMandelbrotDlg : public CDialog, public PropertyChangeListener {
private:
  static const TCHAR            *s_stateName[];
  HICON                          m_hIcon;
  HACCEL                         m_accelTable;
  HICON                          m_crossIcon;
  SimpleLayoutManager            m_layoutManager;
  MBContainer                   *m_mbContainer;
  ColorMap                       m_colorMap;
  DigitPool                     *m_digitPool;
  size_t                         m_digits;
  BigRealRectangle2D             m_rect0, m_zoom1Rect;
  int                            m_lastChangeFlags;

  bool                           m_animateCalculation;
  bool                           m_calculateWithOrbit;
  bool                           m_useOnly1CPU;
  bool                           m_useEdgeDetection;
  bool                           m_retainAspectRatio;
  bool                           m_showZoomFactor;
  bool                           m_suspendingMenuTextIsSuspending;
  Size2D                         m_zoomFactor;
  int                            m_precisionMode; // one of { ID_OPTIONS_AUTOPRECISION, ID_OPTIONS_FORCEFPU, ID_OPTIONS_FORCEBIGREAL }
  DlgState                       m_state;
  CalculatorPool                *m_calculatorPool;
  MBFrameGenerator              *m_frameGenerator;
  CEditColorMapDlgThread        *m_colorMapEditThread;
  RealRectangleTransformation    m_realTransform;
  BigRealRectangleTransformation m_bigRealTransform;
  Semaphore                      m_gate;
  bool                           m_hasResized;
  BitSet8                        m_runningTimerSet;
  SynchronizedQueue<CRect>       m_jobQueue;
  size_t                         m_totalPixelsInJob;
  CWnd                          *m_imageWindow;
  HRGN                           m_imageRGN;
  HDC                            m_imageDC;
  PixRect                       *m_imageCopy;
  CellCountMatrix               *m_ccMatrix, *m_dummyMatrix; // matrix of UINT, 1 for each pixel in m_pixRect
  CellCountAccessor             *m_cca;
  CRect                          m_dragRect;
  CPoint                         m_mouseDownPoint;
  Stack<ImageStackEntry>         m_imageStack;

  void                    updateWindowStateInternal();
  void                    showWindowState();
  void                    showCalculationState();
  void                    showMousePoint(const CPoint &p);
  void                    saveRectangle(const String &fileName);
  int                     loadRectangle(const String &fileName);
  int                     createColorMap();
  int                     loadColorMap(const String &fileName);
  void                    saveColorMap(const String &fileName);
  int                     setWorkSize();
  int                     setWorkSize( const CSize  &size);
  inline CSize            getWorkSize() const {
    return hasCCM() ? m_ccMatrix->getSize() : CSize(0,0);
  }
  void                    handleChangeCode(int flags);
  void                    createImageDC(const CSize &size);
  void                    destroyImageDC();
  void                    createCCM(    const CSize &size);
  void                    destroyCCM();
  int                     copyCCM(const CellCountMatrix *ccm); // copy ccm to m_ccMatrix
  void                    createImageCopy(); // take a screenshot of m_imageDC, for dragging
  void                    destroyImageCopy();
  PixRect                *windowToPixRect();
  void                    pixRectToWindow(PixRect *pr, HDC hdc=nullptr);

  static CellCountMatrix *newCCM(const CSize &size, UINT maxCount);
  static void             deleteCCM(CellCountMatrix *ccm);
  static PixRect         *newPixRect(const CSize &size);
  static void             deletePixRect(PixRect *pr);

  // Return total number of uncalculated pixels
  size_t                  setUncalculatedRectsToEmpty();
  void                    clearUncalculatedWindowArea();
  UINT                    getCPUCountToUse() const;
  int                     setMaxCount(UINT maxCount);
  int                     setColorMapData(const ColorMapData &cdm);
  int                     setScale(const BigReal &minX, const BigReal &maxX, const BigReal &minY, const BigReal &maxY, bool allowAdjustAspectRatio);
  int                     handleTransformChanged(bool adjustAspectRatio);
  void                    setPrecision(int id);
  void                    setDigits();
  void                    updateZoomFactor();
  void                    startTimer(TimerId id, int msec);
  void                    stopTimer(TimerId id);
  void                    startColorMapEditor();
  void                    stopColorMapEditor();
  inline bool             hasColorMapEditor() const {
    return m_colorMapEditThread != nullptr;
  }
  inline DigitPool       *getDigitPool() {
    return m_digitPool;
  }
  inline BigRealRectangle2D getScale() const {
    return m_bigRealTransform.getFromRectangle();
  }
  CPoint                  setDragRect(   const CPoint &topLeft, const CPoint &bottomRight); // return the point diagonal to topLeft
  CRect                   createDragRect(const CPoint &topLeft, const CPoint &bottomRight);
  void                    removeDragRect();
  CPoint                  getImagePointFromMousePoint(const CPoint &p) const;
  void                    copyVisiblePart(PixRect *dst, const PixRect *src, const CSize &dp);
  void                    paintMovedImage(       const CSize &dp);
  CellCountMatrix        *getCalculatedPart(     const CSize &dp);
  void                    calculateMovedImage(   const CSize &dp);
  static    MoveDirection getMoveDirection(      const CSize &dp);
  void                    dumpHistogram();
  void                    flushCCM();
  void                    paintZoomFactor(CDC &dc);
  void                    pushImage();
  void                    popImage();
  void                    resetImageStack();
  void                    paintPointSet(const PointSet &ps, COLORREF color);
  void                    startCalculation();
  void                    setRectanglesToCalculate(const CompactArray<CRect> &rectangles);
  void                    setRectangleToCalculate(const CRect &rectangle, bool splitLast=true);
  void                    setSuspendingMenuText(bool isSuspendingText);
  inline bool             hasCCM() const {
    return m_ccMatrix != nullptr;
  }
  inline bool             hasCCA() const {
    return m_cca != nullptr;
  }
  void                    clearCCA();
  void                    setState(DlgState newState);
  inline DlgState         getState()  const {
    return m_state;
  }
  inline const TCHAR     *getStateName() const {
    return s_stateName[m_state];
  }
  inline double           getPercentDone() const {
    return PERCENT(m_calculatorPool->getDoneCount(),m_totalPixelsInJob);
  }
public:
  CMandelbrotDlg(DigitPool *digitPool, CWnd *pParent = nullptr);
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);

  inline CWnd *getImageWindow() {
    return m_imageWindow;
  }
  int      setScale(const BigRealRectangle2D &scale, bool allowAdjustAspectRatio = true);
  COLORREF setOrbitPixel(const CPoint &p, COLORREF color);
  void     resetOrbitPixels(const OrbitPoint *op, size_t count);
  inline void setCount(UINT x, UINT y , UINT count) {
    return m_cca->setCount(x,y,count);
  }
  inline UINT getCount(UINT x, UINT y) const {
    return m_cca->getCount(x,y);
  }
  inline const RealRectangleTransformation &getRealTransformation() const {
    return m_realTransform;
  }
  inline RealRectangleTransformation &getRealTransformation() {
    return m_realTransform;
  }
  inline const BigRealRectangleTransformation &getBigRealTransformation() const {
    return m_bigRealTransform;
  }
  inline BigRealRectangleTransformation &getBigRealTransformation() {
    return m_bigRealTransform;
  }
  UINT                    getMaxCount() const {
    return m_colorMap.getMaxCount();
  }
  inline const ColorMap &getColorMap() const {
    return m_colorMap;
  }
  CellCountAccessor *getCCA();
  inline size_t getDigits() const {
    return m_digits;
  }
  bool useFPUCalculators() const;
  inline bool calculateWithOrbit() const {
    return m_calculateWithOrbit;
  }
  inline bool useEdgeDetection() const {
    return m_useEdgeDetection;
  }
  inline bool isRetainAspectRatio() const {
    return m_retainAspectRatio;
  }
  inline bool showZoomFactor() const {
    return m_showZoomFactor;
  }
  inline const Size2D &getZoomFactor() const {
    return m_zoomFactor;
  }
  void paintMark(const CPoint &p);
  bool getJobToDo(CRect &rect);
  CSize getWindowSize() const {
    return getWorkSize();
  }
  inline HDC &getImageDC() {
    return m_imageDC;
  }
//-----------------------------------------------

  void initScale();
  inline bool isCalculationActive(bool checkMovie = true) const {
    return (m_calculatorPool && m_calculatorPool->isCalculationActive()) || (checkMovie && isMakingMovie());
  }
  inline bool isCalculationSuspended() const {
    return isCalculationActive() && m_calculatorPool->getCalculatorsInState(CALC_RUNNING).isEmpty();
  }
  inline bool animateCalculation() const {
    return m_animateCalculation;
  }
  inline bool isMakingMovie() const {
    return m_frameGenerator != nullptr;
  }
  void updateMovieMenuItem();
  void remoteStartCalculation();
  void remoteUpdateWindowState();

  enum { IDD = IDD_MANDELBROT_DIALOG };

public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
  afx_msg void OnSize(  UINT nType, int cx, int cy);
  afx_msg void OnClose();
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnCancel();
  afx_msg void OnLButtonDown(  UINT nFlags, CPoint point);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(    UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(  UINT nFlags, CPoint point);
  afx_msg void OnRButtonUp(    UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(    UINT nFlags, CPoint point);
  afx_msg BOOL OnMouseWheel(   UINT nFlags, short zDelta, CPoint pt);
  afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
  afx_msg void OnNcMouseMove(  UINT nHitTest, CPoint point);
  afx_msg void OnFileSaveRectangle();
  afx_msg void OnFileSaveColorMap();
  afx_msg void OnFileSaveImage();
  afx_msg void OnFileLoadRectangle();
  afx_msg void OnFileLoadColorMap();
  afx_msg void OnFileMakeMovie();
  afx_msg void OnFileStopMovie();
  afx_msg void OnFileExit();
  afx_msg void OnEditCalculateImage();
  afx_msg void OnEditSuspendCalculation();
  afx_msg void OnEditAbortCalculation();
  afx_msg void OnEditBack();
  afx_msg void OnOptionsMaxIterations();
  afx_msg void OnOptionsEditColorMap();
  afx_msg void OnOptionsShowColorMap();
  afx_msg void OnOptionsAutoPrecision();
  afx_msg void OnOptionsForceFPU();
  afx_msg void OnOptionsForceBigReal();
  afx_msg void OnOptionsAnimateCalculation();
  afx_msg void OnOptionsPaintOrbit();
  afx_msg void OnOptionsUseEdgeDetection();
  afx_msg void OnOptionsUseOnly1CPU();
  afx_msg void OnOptionsRetainAspectRatio();
  afx_msg void OnOptionsShowZoomFactor();
  afx_msg void OnOptionsShowCalculationTime();
  afx_msg void OnOptionsShowChangeFlags();
  afx_msg void OnHelpAboutMandelbrot();
  afx_msg LRESULT OnMsgStartCalculation( WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgUpdateWindowState(WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgMovieDone(        WPARAM wp, LPARAM lp);
  DECLARE_MESSAGE_MAP()
};
