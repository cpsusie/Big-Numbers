#pragma once

#include "SortThread.h"

class CSortDemoDlg;

class SortPanelWnd : public CStatic {
private:
  CSortDemoDlg               *m_parent;
  const SortMethodId         &m_sortMethod;
  bool                       &m_fast;
  CFont                       m_font;

  CPen                        m_whitePen, m_blackPen, m_redPen;
  DataArray                   m_dataArray, m_savedArray;
  int                         m_compareCount;
  BitSet                      m_modifiedSet;
  double                      m_startTime;
  size_t                      m_elementCount;
  UINT                        m_maxElementSize;
  SortThread                 *m_sortThread;
  SortThreadState             m_threadState;
  Semaphore                   m_resume;
  char                        m_threadSignal;
  intptr_t                    m_oldIndex1;
  intptr_t                    m_oldIndex2;
  CRect                       m_rect;
  bool invalidStateTransition(SortThreadState newState);
  void notifyStateChange(SortThreadState oldState, SortThreadState newState);
  void saveArray();
  void drawElement(      CDC &dc, size_t index);
  void markElement(      CDC &dc, size_t index, CPen &pen);
  void printCompareCount(CDC &dc, bool showTime);
  static const TCHAR *stateStringTable[];
protected:
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

public:
  SortPanelWnd(CSortDemoDlg *parent, int methodId);
  void doSort();
  bool isAnimatedSort() const;
  void setRect(const CRect &r);
  inline const SortMethodId &getMethodId() const {
    return m_sortMethod;
  }
  void initArray();
  void stopSort(int stopCode) { // any combination of PAUSE_SORT TERMINATE_SORT
    m_threadSignal = stopCode;
  }
  void resumeSort();
  inline const TCHAR *getStateStr() const {
    return stateStringTable[m_threadState];
  }
  static const TCHAR *getStateString(SortThreadState state) {
    return stateStringTable[state];
  }

  inline SortThreadState getThreadState() const {
    return m_threadState;
  }
  bool setThreadState(SortThreadState newState);
  void waitForResume();
  friend class SortThread;
  inline DataArray &getDataArray() {
    return m_dataArray;
  }
  int &getCompareCount() {
    return m_compareCount;
  }
  bool &getFast() {
    return m_fast;
  }
  char &getThreadSignal() {
    return m_threadSignal;
  }
  SortThread &getThread() {
    return *m_sortThread;
  }
  const InitializeParameters &getInitParameters() const;

  void updateMarks( CDC &dc, size_t index1, size_t index2);
  void updateScreen(CDC &dc, bool showTime);
};

