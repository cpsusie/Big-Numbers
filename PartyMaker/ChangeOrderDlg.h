#pragma once

class CChangeOrderDlg : public CDialog {
private:
  MediaQueue       &m_mediaQueue;
  HACCEL            m_accelTable;
  Array<MediaFile>  m_editList;
  Array<MediaFile>  m_buffer;
  void gotoToListBox();
  CListBox *getListBox();
  void getSelected(CompactIntArray &selected);
  void removeSelected(const CompactIntArray &selected);
  void insertMediaFileIntoListBox(size_t pos, const MediaFile &f);
  void insertMediaFile(size_t pos, const MediaFile &f);
public:
  CChangeOrderDlg(MediaQueue &mediaQueue, CWnd *pParent = NULL);

  enum { IDD = IDD_CHANGEORDERDIALOG };

public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnEditCut();
  afx_msg void OnEditPaste();
  afx_msg void OnEditDelete();
  afx_msg void OnSelchangeEditlist();
  afx_msg void OnEditRandom();
  DECLARE_MESSAGE_MAP()
};

