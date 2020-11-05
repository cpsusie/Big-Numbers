#pragma once

#include <FastSemaphore.h>
#include "IdentifyDialog.h"
#include "Whist3EPlayer.h"
#include "Whist3Player.h"
#include "Whist3Eplayer.h"

class CWhist3Dialog : public CDialog, public Whist3PlayerCallback, public OptionsAccessor {
private:
  HICON             m_hIcon;
  HBRUSH            m_backgroundBrush;
  HACCEL            m_accelTable;
public:
  CWhist3Dialog(CWnd *pParent = nullptr);

  Whist3Player                *m_player;
  CompactArray<Whist3Player *> m_computerPlayers;
  CBitmap                      m_workBitmap;
  CDC                          m_workDC;
  CSize                        m_workSize;
  FastSemaphore                m_sync;
  String                       m_gameTypeMessage, m_message, m_stateMessage;
  GameDescription              m_tmpDescription;
  int                          m_tmpCardIndex;
  CardIndexSet                 m_tmpCardSet;
  GameHistory                  m_tmpHistory;

  void   postStateChange(GameState oldState, GameState newState);
  void   postCommand(int command);
  void   endApp();
  void   fatalError(_In_z_ _Printf_format_string_ TCHAR const * const format,...);
  void   myMessage( _In_z_ _Printf_format_string_ TCHAR const * const format,...);
  void   createWorkBitmap(const CSize &size);
  void   repaint();
  void   paintAll(  CDC &dc);
  void   paintCards(CDC &dc);
  void   paintCenteredText(CDC &dc, int y, const String &s);
  String getPlayerInfo(int playerId) const;
  CRect  getCardRect(int index);
  CPoint getCardPosition(int index);
  void   initTablePos(int myId);
  void   initGameTypeMessage();
  void   setGameTypeMessage();
  void   saveScore();
  void   markUnmarkCard(const CPoint &p);
  int    findCardIndex( const CPoint &p);
  void   playCard(      const CPoint &p);

  void            init(                 const Whist3Player &player) override;
  GameDescription getDecidedGame(       const Whist3Player &player) override;
  CardIndexSet    getCardsToSubstitute( const Whist3Player &player) override;
  UINT            getCardToPlay(        const Whist3Player &player) override;
  bool            deleteOnEndOfUse()    const                       override {
    return false;
  }
  void handlePropertyChanged(const class PropertyContainer *source, int id, const void *oldValue, const void *newValue) override;

  enum { IDD = IDD_WHIST3_DIALOG };

public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnPaint();
  virtual void OnOK();
  virtual void OnCancel();
  afx_msg void OnGameSummary();
  afx_msg void OnGameLastTrick();
  afx_msg void OnGameBackside();
  afx_msg void OnGameAutoPlay();
  afx_msg void OnGameTrain();
  afx_msg void OnGameExit();
  afx_msg void OnHelpRegler();
  afx_msg void OnHelpAbout();
  afx_msg void OnSubstituteOk();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnStartButton();
  afx_msg void OnDecideGame();
  afx_msg void OnEndGame();
  afx_msg void OnAskPlayAgain();
  afx_msg LRESULT OnMsgStateChanged(WPARAM wp, LPARAM lp);
  DECLARE_MESSAGE_MAP()
  afx_msg void OnClose();
};

