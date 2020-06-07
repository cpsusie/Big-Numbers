#pragma once

class CD3FunctionDoc : public CDocument, public OptionsUpdater {
private:
  void initOptions(const Options &options);
protected:
  CD3FunctionDoc();
  DECLARE_DYNCREATE(CD3FunctionDoc)

public:
  virtual BOOL OnNewDocument();
  virtual void Serialize(CArchive& ar);
  virtual ~CD3FunctionDoc();
#if defined(_DEBUG)
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:
  DECLARE_MESSAGE_MAP()
};
