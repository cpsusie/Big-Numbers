#include "stdafx.h"
#include <Random.h>
#include <Tokenizer.h>
#include "Whist3.h"
#include "DecideGameTrainingDialog.h"
#include "CardBitmap.h"

DecideGameTrainingDialog::DecideGameTrainingDialog(CWnd* pParent) : CDialog(DecideGameTrainingDialog::IDD, pParent) {
    m_pointPerStik = -1;
    m_gameType = -1;

  m_trainerThread = NULL;
  m_timerIsRunning = false;
  m_bpn.load();
}

void DecideGameTrainingDialog::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
    DDX_Radio(pDX, IDC_1POINT_RADIO, m_pointPerStik);
    DDX_Radio(pDX, IDC_SOL_RADIO, m_gameType);
}


BEGIN_MESSAGE_MAP(DecideGameTrainingDialog, CDialog)
    ON_WM_PAINT()
    ON_BN_CLICKED(IDC_BUTTON_NEXTDATA, OnButtonNextdata)
    ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
    ON_BN_CLICKED(IDC_BUTTONTRAIN, OnButtontrain)
    ON_BN_CLICKED(IDC_BUTTONDECIDE, OnButtondecide)
    ON_WM_TIMER()
END_MESSAGE_MAP()


BOOL DecideGameTrainingDialog::OnInitDialog() {
  CDialog::OnInitDialog();

  randomize();

  CRect rect;
  GetDlgItem(IDC_CARDFRAME)->GetClientRect(&rect);
  m_cardFrameSize = rect.Size();
  m_backgroundBrush = CreateSolidBrush(RGB(0,128,0));

  generateRandomTestData();
  return TRUE;
}

CPoint DecideGameTrainingDialog::getCardPosition(int index) const {
#define CARDDIST 15
#define CARDXPOS(i) (m_cardFrameSize.cx / 2 + (i-7)*CARDDIST)
#define CARDYPOS(i) (m_cardFrameSize.cy - CardBitmap::getCardHeight() - 2)

  return CPoint(CARDXPOS(index),CARDYPOS(index));
}

void DecideGameTrainingDialog::OnPaint() {
  CPaintDC dc(this);

  CClientDC cardFrameDC(GetDlgItem(IDC_CARDFRAME));
  cardFrameDC.SelectObject(m_backgroundBrush);
  cardFrameDC.Rectangle(0,0,m_cardFrameSize.cx,m_cardFrameSize.cy);
  cardFrameDC.SetBkColor(RGB(0,128,0));


  const CardHand &hand = m_trainingSet.getHand();
  for(int i = 0; i < 13; i++) {
    const Card card = hand.getCard(i);
    if(card >= 0) {
      CardBitmap::paintCard(cardFrameDC, getCardPosition(i),card);
    }
  }

  enableButtons();
}

GameType DecideGameTrainingDialog::getSelectedGameType() const {
  if(IsDlgButtonChecked(IDC_SOL_RADIO)) {
    return GAMETYPE_SOL;
  } else if(IsDlgButtonChecked(IDC_SANS_RADIO)) {
    return GAMETYPE_SANS;
  } else if(IsDlgButtonChecked(IDC_SPAR_RADIO)) {
    return GAMETYPE_SPAR;
  } else if(IsDlgButtonChecked(IDC_FARVE_RADIO)) {
    return GAMETYPE_FARVE;
  } 
  return GAMETYPE_SOL;
}

int DecideGameTrainingDialog::getSelectedPointsPerTrick() const {
  if(IsDlgButtonChecked(IDC_1POINT_RADIO)) {
    return 1;
  } else if(IsDlgButtonChecked(IDC_2POINT_RADIO)) {
    return 2;
  } else if(IsDlgButtonChecked(IDC_3POINT_RADIO)) {
    return 3;
  } else if(IsDlgButtonChecked(IDC_4POINT_RADIO)) {
    return 4;
  }
  return 0;
}

void DecideGameTrainingDialog::OnButtonNextdata() {
  generateRandomTestData();
  Invalidate(FALSE);
}

void DecideGameTrainingDialog::generateRandomTestData() {
  m_trainingSet.generateRandomTestData();
  enableButtons();
  OnButtondecide();
}

void DecideGameTrainingDialog::enableButtons() {
  const GamesPlayed &gamesPlayed = m_trainingSet.getGamesPlayed();
  GetDlgItem(IDC_SOL_RADIO   )->EnableWindow(!gamesPlayed.isGameUsed(GAMETYPE_SOL  ));
  GetDlgItem(IDC_SANS_RADIO  )->EnableWindow(!gamesPlayed.isGameUsed(GAMETYPE_SANS ));
  GetDlgItem(IDC_SPAR_RADIO  )->EnableWindow(!gamesPlayed.isGameUsed(GAMETYPE_SPAR ));
  GetDlgItem(IDC_FARVE_RADIO )->EnableWindow(!gamesPlayed.isGameUsed(GAMETYPE_FARVE));

  GetDlgItem(IDC_1POINT_RADIO)->EnableWindow(!gamesPlayed.isPointsUsed(1));
  GetDlgItem(IDC_2POINT_RADIO)->EnableWindow(!gamesPlayed.isPointsUsed(2));
  GetDlgItem(IDC_3POINT_RADIO)->EnableWindow(!gamesPlayed.isPointsUsed(3));
  GetDlgItem(IDC_4POINT_RADIO)->EnableWindow(!gamesPlayed.isPointsUsed(4));
}

void DecideGameTrainingDialog::setDefaultSelection() {
  const GamesPlayed &gamesPlayed = m_trainingSet.getGamesPlayed();
  if(!gamesPlayed.isGameUsed(GAMETYPE_SOL)) {
    m_gameType = GAMETYPE_SOL;
  } else if(!gamesPlayed.isGameUsed(GAMETYPE_SANS)) {
    m_gameType = GAMETYPE_SANS;
  } else if(!gamesPlayed.isGameUsed(GAMETYPE_SPAR)) {
    m_gameType = GAMETYPE_SPAR;
  } else if(!gamesPlayed.isGameUsed(GAMETYPE_FARVE)) {
    m_gameType = GAMETYPE_FARVE;
  }
  for(int i = 1; i <= 4; i++) {
    if(!gamesPlayed.isPointsUsed(i)) {
      m_pointPerStik = i-1;
      break;
    }
  }
  UpdateData(FALSE);
}

void DecideGameTrainingDialog::OnButtonSave() {
  try {
    m_trainingSet.setSelectedGameAndPoint(getSelectedGameType(),getSelectedPointsPerTrick());
    m_trainingSet.save();
    OnButtonNextdata();
  } catch(Exception e) {
    MessageBox(e.what(),_T("Skrivefejl"));
  }
}

void DecideGameTrainingDialog::OnButtontrain() {
  if(m_trainerThread == NULL) {
    try {
      m_trainerThread = new TrainerThread(this);
      m_trainerThread->start();
      GetDlgItem(IDC_BUTTONTRAIN)->SetWindowText(_T("St&op training"));
      startTimer();
    } catch(Exception e) {
      MessageBox(e.what());
    }
  } else {
    stopTimer();
    m_trainerThread->stopTraining();
    while(m_trainerThread->stillActive());
    delete m_trainerThread;
    m_trainerThread = NULL;
    m_bpn.load();
    GetDlgItem(IDC_BUTTONTRAIN)->SetWindowText(_T("S&tart training"));
  } 
}

class GameTypeScoreField {
public:
  const GameType m_gameType;
  const int      m_fieldId;
  GameTypeScoreField(GameType gameType, int fieldId) : m_gameType(gameType), m_fieldId(fieldId) {
  }
};

class PoinScoreField {
public:
  const int m_pointsPerTrick;
  const int m_fieldId;
  PoinScoreField(int pointsPerTrick, int fieldId) : m_pointsPerTrick(pointsPerTrick), m_fieldId(fieldId) {
  }
};

static const GameTypeScoreField gameTypeScoreFields[] = {
  GameTypeScoreField(GAMETYPE_SOL  , IDC_STATICSOLVALUE  )
 ,GameTypeScoreField(GAMETYPE_SANS , IDC_STATICSANSVALUE )
 ,GameTypeScoreField(GAMETYPE_SPAR , IDC_STATICSPARVALUE )
 ,GameTypeScoreField(GAMETYPE_FARVE, IDC_STATICFARVEVALUE)
};

static const PoinScoreField pointScoreFields[] = {
  PoinScoreField(1, IDC_STATIC1POINTVALUE)
 ,PoinScoreField(2, IDC_STATIC2POINTVALUE)
 ,PoinScoreField(3, IDC_STATIC3POINTVALUE)
 ,PoinScoreField(4, IDC_STATIC4POINTVALUE)
};

void DecideGameTrainingDialog::OnButtondecide() {
  m_bpn.recognize(m_trainingSet);
  m_gameType     = m_trainingSet.getSelectedGameType();
  m_pointPerStik = m_trainingSet.getSelectedPointsPerTrick() - 1;

  for(int i = 0; i < ARRAYSIZE(gameTypeScoreFields); i++) {
    const GameTypeScoreField &gf = gameTypeScoreFields[i];
    CStatic *field = (CStatic*)GetDlgItem(gf.m_fieldId);
    double value = m_trainingSet.getGameTypeScore(gf.m_gameType);
    field->SetWindowText(format(_T("%.5lf"), value).cstr());
  }

  for(int i = 0; i < ARRAYSIZE(pointScoreFields); i++) {
    const PoinScoreField &pf = pointScoreFields[i];
    CStatic *field = (CStatic*)GetDlgItem(pf.m_fieldId);
    double value = m_trainingSet.getPointScore(pf.m_pointsPerTrick);
    field->SetWindowText(format(_T("%.5lf"), value).cstr());
  }

  UpdateData(FALSE);
}

void DecideGameTrainingDialog::OnTimer(UINT_PTR nIDEvent) {
  if(m_trainerThread != NULL) {
    double errorSum = m_trainerThread->getErrorSum();

    GetDlgItem(IDC_TRAININGINFO)->SetWindowText(format(_T("Errorsum:%lf"),errorSum).cstr());
    CDialog::OnTimer(nIDEvent);
  }
}

#define TIMERUPDATERATE 1000

void DecideGameTrainingDialog::startTimer() {
  if(!m_timerIsRunning && SetTimer(1,TIMERUPDATERATE,NULL)) {
    m_timerIsRunning = true;
  }
}

void DecideGameTrainingDialog::stopTimer() {
  if(m_timerIsRunning) {
    KillTimer(1);
    m_timerIsRunning = false;
  }
}

TrainerThread::TrainerThread(DecideGameTrainingDialog *dlg) : m_dlg(*dlg), m_sync(0) {
  m_trainingData = DecideGameTrainingSet::loadTrainingData();
  m_doStop = false;
}

TrainerThread::~TrainerThread() {
}

double TrainerThread::getErrorSum() const {
  return m_errorSum;
}

UINT TrainerThread::run() {
  m_errorSum = 0;
  while(!m_doStop) {
    double errorSum = 0;  
    for(size_t i = 0; i < m_trainingData.size(); i++) {
      m_bpn.learn(m_trainingData[i]);
      errorSum += m_bpn.getPatternError(m_trainingData[i]);
    }
    m_errorSum = errorSum;
  }
  m_bpn.save();
  m_sync.signal();
  return 0;
}

void TrainerThread::stopTraining() {
  m_doStop = true;
  m_sync.wait();
}
