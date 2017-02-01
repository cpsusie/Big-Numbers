#pragma once

class CEditTagDlg : public CDialog {
private:
  HACCEL               m_accelTable;
  SimpleLayoutManager  m_layoutManager;
  MediaArray           m_mediaArray;
  const AttributeArray m_origAttributes;
  const AttributeArray m_readOnlyAttributes;
  AttributeArray       m_attributes;
  int                  m_genreAttribute;
  bool                 m_changed;

  void initListControl(CListCtrl &listCtrl, const AttributeArray &attributes, int extendedStyles = 0);
  CComboBox *getGenreCombo();
  int getGenreIndex(const CString &genre);
  void initData();
public:
  CEditTagDlg(MediaFile &mediaFile, CWnd *pParent = NULL);
  CEditTagDlg(MediaArray &mediaArray, CWnd *pParent = NULL);

  bool dataHasChanged() const {
    return m_changed;
  }

  enum { IDD = IDD_EDITTAGDIALOG };
  CListCtrl   m_readWriteListCtrl;
  CListCtrl   m_readOnlyListCtrl;

public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pLResult);
protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult);

protected:
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnBeginLabelEditAttributeList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnEndLabelEditAttributeList(  NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnEditAddAttribute();
    afx_msg void OnEditEdit();
    afx_msg void OnCloseupGenreCombo();
    afx_msg void OnSelendOkGenreCombo();
    afx_msg void OnGotoReadWriteList();
    afx_msg void OnGotoReadOnlyList();
    afx_msg void OnTrackAttributelist(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnColumnclickAttributelist(NMHDR *pNMHDR, LRESULT *pResult);
    DECLARE_MESSAGE_MAP()
};
