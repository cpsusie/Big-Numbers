#pragma once

class CD3FunctionDoc : public CDocument, public OptionsUpdater {
private:
  void initOptions(const Options &options);
protected: // create from serialization only
  CD3FunctionDoc();
  DECLARE_DYNCREATE(CD3FunctionDoc)

// Attributes
public:
  CView *add3dView(CView* pNewView);
    // Operations
public:

public:
  virtual BOOL OnNewDocument();
  virtual void Serialize(CArchive& ar);

public:
    virtual ~CD3FunctionDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
  DECLARE_MESSAGE_MAP()
};

