#pragma once

#include "SortJob.h"

class CSortDemoDlg;

typedef enum {
  JOBSTATE
} SortPanelProperty;

class SortPanelWnd : public CStatic, PropertyContainer {
  friend class SortJob;
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
  SortJob                    *m_sortJob;
  SortJobState                m_jobState;
  FastSemaphore               m_resume, m_lock;
  std::atomic<char>           m_jobFlags;
  intptr_t                    m_oldIndex1;
  intptr_t                    m_oldIndex2;
  CRect                       m_rect;
  bool invalidStateTransition(SortJobState newState);
  void saveArray();
  void drawElement(      CDC &dc, size_t index);
  void markElement(      CDC &dc, size_t index, CPen &pen);
  void printCompareCount(CDC &dc, bool showTime);
  static const TCHAR *s_stateStringTable[];

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
    m_jobFlags = stopCode;
  }
  void resumeSort();
  inline const TCHAR *getStateStr() const {
    return getStateString(m_jobState);
  }
  static inline const TCHAR *getStateString(SortJobState state) {
    return s_stateStringTable[state];
  }

  inline SortJobState getJobState() const {
    return m_jobState;
  }
  bool setJobState(SortJobState newState);
  void waitForResume(SortJobState newState);
  inline DataArray &getDataArray() {
    return m_dataArray;
  }
  int &getCompareCount() {
    return m_compareCount;
  }
  bool &getFast() {
    return m_fast;
  }
  std::atomic<char> &getJobFlags() {
    return m_jobFlags;
  }
  SortJob &getJob() {
    return *m_sortJob;
  }
  const InitializeParameters &getInitParameters() const;

  void updateMarks( CDC &dc, size_t index1, size_t index2);
  void updateScreen(CDC &dc, bool showTime);
};
