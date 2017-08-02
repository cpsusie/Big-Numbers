#pragma once

class CMyPaintDoc : public CDocument {
private:
  static const CString   s_defaultName;
  PixRect               *m_image, *m_fileImage;
  CompactArray<PixRect*> m_history;
  int                    m_index; // Invariant: 0 <= m_index <= m_history.size()

  int  getHistorySize() const {
    return (int)m_history.size();
  }
  void resetHistory();
  void removeLast();
  void addImage();
  bool canAddImage() const;
  void setFileImage();
  bool isModified() const;
  void init();

#ifdef _DEBUG
  void checkInvariant(int line) const;
#define CHECKINVARIANT checkInvariant(__LINE__)
#else
#define CHECKINVARIANT
#endif

protected:
  CMyPaintDoc();
  DECLARE_DYNCREATE(CMyPaintDoc)
  DECLARE_MESSAGE_MAP()

public:
  virtual BOOL OnNewDocument();
  virtual BOOL OnOpenDocument(LPCTSTR name);
  virtual BOOL OnSaveDocument(LPCTSTR name);
  virtual void Serialize(CArchive& ar);

  bool hasDefaultName() const;
  BOOL IsModified();

  String getInfo() const;

  CSize getSize() const {
    return m_image ? m_image->getSize() : CSize(0,0);
  }

  void setSize(const CSize &newSize);
  void clear();

  PixRect *getImage();
  void setImage(PixRect *image);

  void saveState();
  bool undo();
  bool redo();
  bool canUndo() const;
  bool canRedo() const;

  virtual ~CMyPaintDoc();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif
};
