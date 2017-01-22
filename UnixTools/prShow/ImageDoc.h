#pragma once

#include <MFCUtil/PixRect.h>

class CImageDoc : public CDocument {
private:
  String   m_name;
  PixRect *m_pixRect;
protected:
  CImageDoc();
  DECLARE_DYNCREATE(CImageDoc)

public:
  public:
  virtual BOOL OnNewDocument();
  virtual void Serialize(CArchive& ar);

public:
  void init();

  CSize getSize() const {
    return m_pixRect ? m_pixRect->getSize() : CSize(100,100);
  }

  const String getName() const {
    return m_name;
  }

  void setName(const String &name) {
    m_name = name;
  }

  PixRect *getPixRect() {
    return m_pixRect;
  }

  void setPixRect(PixRect *pixRect);
  CSize getSizeInMillimeters() const;

  virtual ~CImageDoc();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:
  DECLARE_MESSAGE_MAP()
};

