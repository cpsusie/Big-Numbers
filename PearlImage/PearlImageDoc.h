#pragma once

class CPearlImageDoc : public CDocument {
private:
  PixRect               *m_pixRect;
  // last saved version of image
  PixRect               *m_fileImage;
  CompactArray<PixRect*> m_history;
  int                    m_index; // Invariant: -1 <= m_index < m_history.size()
  CSize                  m_size;
  static const CString defaultName;

  void resetHistory();

  int getHistorySize() const {
    return (int)m_history.size();
  }

  void removeLast();
  void addImage();
  void setFileImage();
  void createPixRect();
protected:
  CPearlImageDoc();
  DECLARE_DYNCREATE(CPearlImageDoc)

public:
  virtual BOOL OnNewDocument();
  virtual BOOL OnOpenDocument(LPCTSTR name);
  virtual BOOL OnSaveDocument(LPCTSTR name);
  virtual void Serialize(CArchive& ar);

  void init();
  bool hasDefaultName() const;
  BOOL IsModified();

  String getInfo() const;

  CSize getSize() const {
    return m_size;
  }

  void setSize(const CSize &newSize);

  inline PixRect *getPixRect() {
    if(m_pixRect == NULL) createPixRect();
    return m_pixRect;
  }

  void setPixRect(PixRect *pixRect);

  void saveState();
  void undo();
  void redo();
  bool canUndo() const;
  bool canRedo() const;

  virtual ~CPearlImageDoc();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:
  DECLARE_MESSAGE_MAP()
};
