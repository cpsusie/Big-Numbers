#pragma once

#include <MFCUtil/PixRect.h>

class CCaptureDoc : public CDocument {
private:
  HBITMAP m_bitmap;
  CSize   m_size; // in pixels

  void resetImage();
protected:
  CCaptureDoc();
  CCaptureDoc(HBITMAP bitmap);
  DECLARE_DYNCREATE(CCaptureDoc)

public:
  virtual BOOL OnNewDocument();
  void OnOpenDocument(TCHAR *fname);
  virtual void Serialize(CArchive& ar);

  inline HBITMAP getBitmap() {
    return m_bitmap;
  }

  void setImage(HBITMAP bitmap);
  void save(const String &fileName);
  inline bool hasImage() const {
    return m_bitmap != nullptr;
  }

  inline const CSize &getSize() const { // in pixels
    return m_size;
  }

  CCaptureDoc *getScaledCopy(double factor);
  CSize getSizeInMillimeters() const;

public:
  virtual ~CCaptureDoc();
#if defined(_DEBUG)
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:
  DECLARE_MESSAGE_MAP()
};
