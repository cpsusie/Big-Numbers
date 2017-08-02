#pragma once

#include <Date.h>
#include "GridParameters.h"

class PixRectWithTimestamp {
private:
  inline void updateTimeStamp() {
    m_ts = Timestamp();
  }

public:
  const PixRect *m_pr;
  Timestamp      m_ts;
  PixRectWithTimestamp() : m_pr(NULL) {
  }
  PixRectWithTimestamp(const PixRect *pr, Timestamp ts)
    : m_pr(pr)
    , m_ts(ts) {
  }
  // NB... No destructor, copy constructor or operator=
  void clear() {
    if(m_pr != NULL) {
      SAFEDELETE(m_pr);
      updateTimeStamp();
    }
  }
  void set(const PixRect *pr) {
    clear();
    m_pr = pr; // NB. no clone
    updateTimeStamp();
  }
  void set(const PixRectWithTimestamp &src) {
    clear();
    m_pr = src.m_pr->clone(true);
    m_ts = src.m_ts;
  }
  inline bool operator==(const PixRectWithTimestamp &rhs) const {
    return  (m_ts == rhs.m_ts) && (*m_pr == *rhs.m_pr);
  }
  inline bool operator!=(const PixRectWithTimestamp &rhs) const {
    return !(*this == rhs);
  }
  inline bool hasImage() const {
    return m_pr != NULL;
  }
  inline CSize getSize() const {
    return hasImage() ? m_pr->getSize() : CSize(0,0);
  }
};

class CPearlImageDoc : public CDocument {
private:
  static const CString               s_defaultName;
  PixRectWithTimestamp               m_image;
  GridParameters                     m_gridParam;
  Timestamp                          m_lastImageSave;
  CompactArray<PixRectWithTimestamp> m_history;
  int                                m_index; // Invariant: -1 <= m_index < m_history.size()

  int getHistorySize() const {
    return (int)m_history.size();
  }
  void resetHistory();
  void removeLast();
  void addImage();
  bool canAddImage() const;
  void setSaveTime();
  bool isModified() const;
  void init();

#ifdef _DEBUG
  void checkInvariant(int line) const;
#define CHECKINVARIANT checkInvariant(__LINE__)
#else
#define CHECKINVARIANT
#endif

protected:
  CPearlImageDoc();
  DECLARE_DYNCREATE(CPearlImageDoc)
  DECLARE_MESSAGE_MAP()

public:
  virtual BOOL OnOpenDocument(LPCTSTR name);
  virtual BOOL OnSaveDocument(LPCTSTR name);
  virtual void Serialize(CArchive& ar);

  bool hasDefaultName() const;
  BOOL IsModified();

  String getInfo() const;

  inline bool hasImage() const {
    return m_image.hasImage();
  }
  inline CSize getSize() const {
    return m_image.getSize();
  }

  void setSize(const CSize &newSize);
  void clear();

  const PixRect *getImage() {
    return m_image.m_pr;
  }
  void setImage(PixRect *image);

  const GridParameters &getGridParameters() const {
    return m_gridParam;
  }
  void saveState();
  bool undo();
  bool redo();
  bool canUndo() const;
  bool canRedo() const;

  virtual ~CPearlImageDoc();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif
};
