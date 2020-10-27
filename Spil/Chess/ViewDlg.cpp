#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include "ViewDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CViewDlg::CViewDlg(CWnd *pParent /*=nullptr*/) : CDialog(CViewDlg::IDD, pParent) {
  const Options &options = getOptions();

  m_animateCheckmate = options.getAnimateCheckmate();
  m_animateMoves     = options.getAnimateMoves();
  m_askForNewGame    = options.getAskForNewGame();
  m_showPlayerInTurn = options.getShowPlayerInTurn();
  m_showComputerTime = options.getShowComputerTime();
  m_showFieldNames   = options.getShowFieldNames();
  m_showLegalMoves   = options.getShowLegalMoves();
}

void CViewDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Check(pDX, IDC_CHECK_ANIMATECHECKMATE    , m_animateCheckmate);
    DDX_Check(pDX, IDC_CHECK_ANIMATEMOVES        , m_animateMoves    );
    DDX_Check(pDX, IDC_CHECK_ASKFORNEWGAME       , m_askForNewGame   );
    DDX_Check(pDX, IDC_CHECK_INDICATEPLAYERINTURN, m_showPlayerInTurn);
    DDX_Check(pDX, IDC_CHECK_SHOWCOMPUTERTIME    , m_showComputerTime);
    DDX_Check(pDX, IDC_CHECK_SHOWFIELDNAMES      , m_showFieldNames  );
    DDX_Check(pDX, IDC_CHECK_SHOWLEGALMOVES      , m_showLegalMoves  );
}

BEGIN_MESSAGE_MAP(CViewDlg, CDialog)
END_MESSAGE_MAP()

BOOL CViewDlg::OnInitDialog() {
  __super::OnInitDialog();
  setControlText(IDD, this);

  return TRUE;
}

void CViewDlg::OnOK() {
  UpdateData();

  Options &options = getOptions();
  options.setAnimateCheckmate(m_animateCheckmate?true:false);
  options.setAnimateMoves(    m_animateMoves    ?true:false);
  options.setAskForNewGame(   m_askForNewGame   ?true:false);
  options.setShowPlayerInTurn(m_showPlayerInTurn?true:false);
  options.setShowComputerTime(m_showComputerTime?true:false);
  options.setShowFieldNames(  m_showFieldNames  ?true:false);
  options.setShowLegalMoves(  m_showLegalMoves  ?true:false);

  __super::OnOK();
}
