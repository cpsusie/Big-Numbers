#pragma once

class CShowGrafDoc : public CDocument {
protected:
  CShowGrafDoc();
  DECLARE_DYNCREATE(CShowGrafDoc)

public:

  public:
  virtual BOOL OnNewDocument();
  virtual void Serialize(CArchive& ar);

public:
  virtual ~CShowGrafDoc();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:
  DECLARE_MESSAGE_MAP()
};
