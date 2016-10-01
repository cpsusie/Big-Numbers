#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include "PieceValueDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MATFACTOR 37

#define P_VALUE  MATFACTOR* 2
#define N_VALUE  MATFACTOR* 7
#define B_VALUE  MATFACTOR* 8
#define R_VALUE  MATFACTOR*12
#define Q_VALUE  MATFACTOR*23
#define K_VALUE  MATFACTOR*(-1)

static const int pieceValues[] = { 0, K_VALUE, Q_VALUE, R_VALUE, B_VALUE, N_VALUE, P_VALUE };

CPieceValueDlg::CPieceValueDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPieceValueDlg::IDD, pParent)
{
	setInterval(Queen);
	m_step      = 5;
}

void CPieceValueDlg::setInterval(PieceType type) {
  m_from      = max(50,pieceValues[type] - 40);
  m_to        = pieceValues[type]        + 40;
  m_pieceType = (int)(type-1);
}

void CPieceValueDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX , IDC_EDIT_FROM , m_from     );
	DDX_Text(pDX , IDC_EDIT_TO   , m_to       );
	DDX_Text(pDX , IDC_EDIT_STEP , m_step     );
	DDX_Radio(pDX, IDC_RADIO_KING, m_pieceType);
}

BEGIN_MESSAGE_MAP(CPieceValueDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIO_KING  , OnRadioKing  )
	ON_BN_CLICKED(IDC_RADIO_QUEEN , OnRadioQueen )
	ON_BN_CLICKED(IDC_RADIO_ROOK  , OnRadioRook  )
	ON_BN_CLICKED(IDC_RADIO_BISHOP, OnRadioBishop)
	ON_BN_CLICKED(IDC_RADIO_KNIGHT, OnRadioKnight)
	ON_BN_CLICKED(IDC_RADIO_PAWN  , OnRadioPawn  )
END_MESSAGE_MAP()

void CPieceValueDlg::OnRadioKing()   { setInterval(King  ); UpdateData(false); }
void CPieceValueDlg::OnRadioQueen()  { setInterval(Queen ); UpdateData(false); }
void CPieceValueDlg::OnRadioRook()   { setInterval(Rook  ); UpdateData(false); }
void CPieceValueDlg::OnRadioBishop() { setInterval(Bishop); UpdateData(false); }
void CPieceValueDlg::OnRadioKnight() { setInterval(Knight); UpdateData(false); }
void CPieceValueDlg::OnRadioPawn()   { setInterval(Pawn  ); UpdateData(false); }

void CPieceValueDlg::OnOK() {
  if(!UpdateData()) {
    return;
  }
  if(m_from >= m_to) {
    MessageBox(format(_T("Ugyldigt intervael:[%d-%d]"), m_from, m_to).cstr(), _T("Fejl"), MB_ICONWARNING);
    gotoEditBox(this, IDC_EDIT_FROM);
    return;
  }
  if(m_step <= 0) {
    MessageBox( _T("Step skal være > 0"), _T("Fejl"), MB_ICONWARNING); 
    gotoEditBox(this, IDC_EDIT_STEP);
    return;
  }
  m_result.m_from      = m_from;
  m_result.m_to        = m_to;
  m_result.m_step      = m_step;
  m_result.m_pieceType = (PieceType)(m_pieceType+1);

  CDialog::OnOK();
}

const PValuePlotParameters &CPieceValueDlg::getParameters() const {
  return m_result;
}
