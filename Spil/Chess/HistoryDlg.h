#pragma once

class CHistoryDlg : public CDialog, OptionsAccessor {
private:
  HACCEL              m_accelTable;
  const String        m_gameName;
  const GameKey       m_gameStartPosition;
  GameHistory         m_history, m_origHistory;
  const int           m_lastPlyIndex;
  int                 m_currentPly, m_selectedPly, m_markedPly;
  int                 m_startOffset;
  CFont               m_historyFont;
  void   setMoveFormat(MoveStringFormat moveFormat);
  void   setAnnotation(MoveAnnotation annotation);
  void   scrollToBottom();
  void   showSelectedMove();
  void   gotoWhiteMove(int nr);
  void   gotoBlackMove(int nr);
  void   gotoPly(int index);
  void   markPly(int index);
  bool   showContextMenu(CWnd *pWnd, CPoint point);
  int    getPlyIndexByPosition(const CPoint &p); // p relative to CEditBox (IDC_EDIT_HISTORY)
  CEdit *getHistoryBox();
  String getHistoryString();
  void   setFontSize(int pct, bool redraw);

  int    startOffset() const {
    return m_startOffset;
  }

public:
  CHistoryDlg(Game &game, CWnd *pParent = nullptr);

  bool isChanged() const;
  MoveAnnotation getAnnotation(int ply) const {
    return m_history[ply].getAnnotation();
  }

  enum { IDD = IDD_HISTORY_DIALOG };
  CString m_historyStr;

  void updateHistoryText();
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual void OnCancel();
  virtual BOOL OnInitDialog();
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  afx_msg void OnFileAnalyzePosition();
  afx_msg void OnFilePrint();
  afx_msg void OnEditSelectall();
  afx_msg void OnEditCopy();
  afx_msg void OnOptionsFormatShortNotation();
  afx_msg void OnOptionsFormatLongNotation();
  afx_msg void OnHistoryContextMenuGoodMove();
  afx_msg void OnHistoryContextMenuExcellentMove();
  afx_msg void OnHistoryContextMenuBadMove();
  afx_msg void OnHistoryContextMenuVeryBadMove();
  afx_msg void OnHistoryContextMenuInterestingMove();
  afx_msg void OnHistoryContextMenuDoubiousMove();
  afx_msg void OnHistoryContextMenuRemoveAnnotation();
  afx_msg void OnFontsize75();
  afx_msg void OnFontsize100();
  afx_msg void OnFontsize125();
  afx_msg void OnFontsize150();
  afx_msg void OnFontsize175();
  afx_msg void OnFontsize200();
  DECLARE_MESSAGE_MAP()
};

