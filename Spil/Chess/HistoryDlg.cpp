#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include <MFCUtil/Clipboard.h>
#include "ChessGraphics.h"
#include "GameAnalyzeThread.h"
#include "HistoryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CHistoryDlg::CHistoryDlg(Game &game, CWnd* pParent)
: CDialog(CHistoryDlg::IDD, pParent)
, m_gameName(game.getFileName())
, m_gameStartPosition(game.getStartPosition()      )
, m_history(          game.getHistory()            )
, m_startOffset(      game.startWithBlack() ? 1 : 0)
, m_lastPlyIndex(     game.getPlyCount()        - 1)
{
  m_historyStr  = _T("");
  m_markedPly   = -1;
  m_origHistory = m_history;
}

void CHistoryDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_HISTORY, m_historyStr);
}

BEGIN_MESSAGE_MAP(CHistoryDlg, CDialog)
  ON_COMMAND(ID_FILE_ANALYZEPOSITION          , OnFileAnalyzePosition               )
  ON_COMMAND(ID_FILE_PRINT                    , OnFilePrint                         )
  ON_COMMAND(ID_EDIT_SELECT_ALL               , OnEditSelectall                     )
  ON_COMMAND(ID_EDIT_COPY                     , OnEditCopy                          )
  ON_COMMAND(ID_OPTIONS_FORMAT_SHORTNOTATION  , OnOptionsFormatShortNotation        )
  ON_COMMAND(ID_OPTIONS_FORMAT_LONGNOTATION   , OnOptionsFormatLongNotation         )
  ON_WM_HSCROLL(  )
  ON_COMMAND(ID_GOOD_MOVE                     , OnHistoryContextMenuGoodMove        )
  ON_COMMAND(ID_EXCELLENT_MOVE                , OnHistoryContextMenuExcellentMove   )
  ON_COMMAND(ID_BAD_MOVE                      , OnHistoryContextMenuBadMove         )
  ON_COMMAND(ID_VERYBAD_MOVE                  , OnHistoryContextMenuVeryBadMove     )
  ON_COMMAND(ID_INTERESTING_MOVE              , OnHistoryContextMenuInterestingMove )
  ON_COMMAND(ID_DOUBIOUS_MOVE                 , OnHistoryContextMenuDoubiousMove    )
  ON_COMMAND(ID_REMOVE_ANNOTATION             , OnHistoryContextMenuRemoveAnnotation)
  ON_COMMAND(ID_FONTSIZE_75                   , OnFontsize75                        )
  ON_COMMAND(ID_FONTSIZE_100                  , OnFontsize100                       )
  ON_COMMAND(ID_FONTSIZE_125                  , OnFontsize125                       )
  ON_COMMAND(ID_FONTSIZE_150                  , OnFontsize150                       )
  ON_COMMAND(ID_FONTSIZE_175                  , OnFontsize175                       )
  ON_COMMAND(ID_FONTSIZE_200                  , OnFontsize200                       )
  ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CHistoryDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_HISTORY_ACCELERATOR));

  setControlText(IDD, this);

  CFont *font = GetFont();
  LOGFONT lf;
  font->GetLogFont(&lf);
//  _tcscpy(lf.lfFaceName, _T("lucida console"));
    _tcscpy(lf.lfFaceName, _T("courier new"));
  BOOL ret = m_historyFont.CreateFontIndirect(&lf);
  getHistoryBox()->SetFont(&m_historyFont,FALSE);

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_EDIT_HISTORY     , RELATIVE_SIZE     | RESIZE_FONT    );
  m_layoutManager.addControl(IDC_SCROLLBAR_HISTORY, RELATIVE_Y_POS    | RELATIVE_WIDTH );
  m_layoutManager.addControl(IDOK                 , RELATIVE_POSITION    );
  m_layoutManager.addControl(IDCANCEL             , RELATIVE_POSITION    );

  setWindowText(this, getWindowText(this) + _T(" - ") + FileNameSplitter(m_gameName).getFileName());
  setMoveFormat(getOptions().getMoveFormat());

//  GetDlgItem(IDC_SCROLLBAR_HISTORY)->ShowWindow(toggleMenuItem(this,ID_FILE_TRACK_POSITION) ? SW_SHOW : SW_HIDE);
  CScrollBar *scrBar = (CScrollBar*)GetDlgItem(IDC_SCROLLBAR_HISTORY);
  scrBar->SetScrollRange(-1,m_lastPlyIndex, false);
  scrBar->SetScrollPos(m_lastPlyIndex);
  setFontSize(getOptions().getHistoryFontSize(), false);

  scrollToBottom();
  gotoPly(m_lastPlyIndex);
  getHistoryBox()->SetFocus();

  return FALSE;
}

void CHistoryDlg::scrollToBottom() {
  CEdit *e = getHistoryBox();
  e->LineScroll(e->GetLineCount());
}

void CHistoryDlg::OnFilePrint() {
  CPrintDialog dlg(false);
  if(dlg.DoModal() == IDOK) {
/*
    CDC *printDC = CDC::FromHandle(dlg.GetPrinterDC());
    CSize cs1 = printDC ->GetViewportExt();
    CSize cs2 = printDC ->GetWindowExt();
    int dpw = printDC ->GetDeviceCaps(HORZRES);
    int dph = printDC ->GetDeviceCaps(VERTRES);
    printDC->StartDoc(m_game.getName().cstr());
    CWnd *editBox = GetDlgItem(IDC_EDIT_HISTORY);
    CClientDC textDC(editBox);
    CRect r;
    editBox->GetClientRect(&r);

//    CSize printSizePix = pr->getSizeInMillimeters() * m_printInfo->m_rectDraw.Size() / getDCSizeInMillimeters(pDC->m_hAttribDC);
//    pDC->StretchBlt(0, 0, printSizePix.cx, printSizePix.cy, docDC, 0, 0, docSizePix.cx, docSizePix.cy, SRCCOPY);

    printDC->StretchBlt(0,0,dpw, dph, &textDC, 0, 0, r.Width(), r.Height(), SRCCOPY);
    printDC->EndDoc();
*/
  }
}

void CHistoryDlg::OnOptionsFormatShortNotation() { setMoveFormat(MOVE_SHORTFORMAT); }
void CHistoryDlg::OnOptionsFormatLongNotation()  { setMoveFormat(MOVE_LONGFORMAT);  }

void CHistoryDlg::setMoveFormat(MoveStringFormat moveFormat) {
  checkMenuItem(this,ID_OPTIONS_FORMAT_LONGNOTATION , moveFormat == MOVE_LONGFORMAT );
  checkMenuItem(this,ID_OPTIONS_FORMAT_SHORTNOTATION, moveFormat == MOVE_SHORTFORMAT);
  getOptions().setMoveFormat(moveFormat);

  updateHistoryText();
}

void CHistoryDlg::OnFontsize75()  { setFontSize(75 , true); }
void CHistoryDlg::OnFontsize100() { setFontSize(100, true); }
void CHistoryDlg::OnFontsize125() { setFontSize(125, true); }
void CHistoryDlg::OnFontsize150() { setFontSize(150, true); }
void CHistoryDlg::OnFontsize175() { setFontSize(175, true); }
void CHistoryDlg::OnFontsize200() { setFontSize(200, true); }

void CHistoryDlg::setFontSize(int pct, bool redraw) {
  m_layoutManager.scaleFont((double)pct / 100, redraw);
  getOptions().setHistoryFontSize(pct);
  FontSizeMenuManager::setFontSize(this, pct);
}

void CHistoryDlg::updateHistoryText() {
  m_historyStr = m_history.toString(getOptions().getMoveFormat()).cstr();
  TCHAR chessSymbol[2], mateSymbol[2], captureSymbol[2];

  _stprintf(chessSymbol,  _T("%c"), 134);
  _stprintf(mateSymbol ,  _T("%c"), 135);
  _stprintf(captureSymbol,_T("%c"), 215);
  m_historyStr.Replace(_T("+"), chessSymbol  );
  m_historyStr.Replace(_T("#"), mateSymbol   );
  m_historyStr.Replace(_T("x"), captureSymbol);

  UpdateData(FALSE);
}

BOOL CHistoryDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  switch(pMsg->message) {
  case WM_LBUTTONDBLCLK:
    { int ret = CDialog::PreTranslateMessage(pMsg);
      if(getFocusCtrlId(this) == IDC_EDIT_HISTORY) {
        showSelectedMove();
      }
      return ret;
    }

  case WM_RBUTTONUP:
    { const CRect cr = getRelativeClientRect(this, IDC_EDIT_HISTORY);
      CPoint pt = pMsg->pt;
      ScreenToClient(&pt);
      if(cr.PtInRect(pt)) {
        if(showContextMenu(this, pMsg->pt)) {
          return true;
        }
      }
      break;
    }
  }
  return CDialog::PreTranslateMessage(pMsg);
}

bool CHistoryDlg::showContextMenu(CWnd* pWnd, CPoint point) {
  CMenu menu;
  if(!menu.LoadMenu(IDR_HISTORY_CONTEXTMENU)) {
    AfxMessageBox(_T("Loadmenu failed"));
    return false;
  }

  CEdit *e = getHistoryBox();
  CPoint pt = point;
  e->ScreenToClient(&pt);
  const int plyIndex = getPlyIndexByPosition(pt);
  if(plyIndex < 0) {
    return false;
  }
  m_selectedPly = plyIndex;
  if(!m_history[m_selectedPly].hasAnnotation()) {
    removeMenuItem(&menu, ID_REMOVE_ANNOTATION);
  }
  menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x,point.y, this);
  return true;
}

void CHistoryDlg::OnEditSelectall() {
  CEdit *editBox = getHistoryBox();
  CString text;
  editBox->GetWindowText(text);
  editBox->SetSel(0,text.GetLength());
  editBox->RedrawWindow();
//  Invalidate(FALSE);
}

void CHistoryDlg::OnEditCopy() {
  CEdit *editBox = getHistoryBox();
  CString text;
  editBox->GetWindowText(text);
  int startChar, endChar;
  editBox->GetSel(startChar, endChar);
  CString selectedText = text.Mid(startChar,endChar-startChar);
  putClipboard(theApp.GetMainWnd()->m_hWnd,(LPCTSTR)selectedText);
}

CEdit *CHistoryDlg::getHistoryBox() {
  return (CEdit*)GetDlgItem(IDC_EDIT_HISTORY);
}

void CHistoryDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
  switch(nSBCode) {
  case SB_LEFT         : gotoPly(-1);                           break;
  case SB_RIGHT        : gotoPly(m_lastPlyIndex);               break;
  case SB_LINELEFT     : gotoPly(pScrollBar->GetScrollPos()-1); break;
  case SB_LINERIGHT    : gotoPly(pScrollBar->GetScrollPos()+1); break;
  case SB_PAGELEFT     : gotoPly(pScrollBar->GetScrollPos()-2); break;
  case SB_PAGERIGHT    : gotoPly(pScrollBar->GetScrollPos()+2); break;
  case SB_THUMBPOSITION:
  case SB_THUMBTRACK   : gotoPly(nPos);                         break;
  }
  CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

String CHistoryDlg::getHistoryString() {
  CEdit *editBox = getHistoryBox();
  return getWindowText(editBox);
}

void CHistoryDlg::showSelectedMove() {
  CEdit *editBox = getHistoryBox();
  int selStart,selEnd;
  editBox->GetSel(selStart,selEnd);
  if(selEnd > selStart) {
    const String text = getHistoryString();
    String selectedText = substr(text, selStart, selEnd - selStart + 1).trim();
    Tokenizer tok(text, _T("\n\r"));
    StringIndex strIndex;
    for(strIndex = tok.nextIndex(); (int)strIndex.getEnd() < selStart && tok.hasNext(); strIndex = tok.nextIndex());
    if((int)strIndex.getStart() <= selStart && (int)strIndex.getEnd()+1 >= selEnd) {
      const String line = substr(text, strIndex.getStart(), strIndex.getLength());
      int moveNumber;
      if(_stscanf(line.cstr(),_T("%d."), &moveNumber) != 1) {
        return;
      }
      const intptr_t commaIndex = line.find(',');
      const intptr_t index      = line.find(selectedText);
      if(commaIndex < 0 || index < commaIndex) {
        gotoWhiteMove(moveNumber);
      } else {
        gotoBlackMove(moveNumber);
      }
    }
  }
}

void CHistoryDlg::gotoWhiteMove(int nr) {
  gotoPly((nr-1)*2 - startOffset());
}

void CHistoryDlg::gotoBlackMove(int nr) {
  gotoPly((nr-1)*2+1 - startOffset());
}

void CHistoryDlg::gotoPly(int index) {
  index = minMax(index, -1, m_lastPlyIndex);

//  CClientDC dc(this);
//  String s = format(_T("moveIndex=%d"), index);
//  dc.TextOut(10,10, s.cstr(),s.length());
  GetParent()->PostMessage(ID_MSG_SHOW_SELECTED_MOVE, 0, index);
  ((CScrollBar*)GetDlgItem(IDC_SCROLLBAR_HISTORY))->SetScrollPos(index);
  markPly(index);
  m_currentPly = index;
}

int CHistoryDlg::getPlyIndexByPosition(const CPoint &p) { // p relative to CEditBox (IDC_EDIT_HISTORY)
  CEdit       *editBox    = getHistoryBox();
  const String text       = getHistoryString();
  const int    charIndex  = LOWORD(editBox->CharFromPos(p));
  int          i          = -startOffset();

  for(Tokenizer tok(text,_T("\n\r,")); tok.hasNext(); i++) {
    const StringIndex strIndex = tok.nextIndex();
    if(strIndex.contains(charIndex)) {
      return i;
    }
  }
  return -1;
}

void CHistoryDlg::markPly(int index) {
  CEdit *editBox = getHistoryBox();
  if(index < 0) {
    editBox->SetSel(0,0,FALSE);
    m_markedPly = -1;
    return;
  }
  const String text = getHistoryString();
  int i = -startOffset();
  StringIndex strIndex;
  for(Tokenizer tok(text,_T("\n\r,")); tok.hasNext() && i <= index; strIndex = tok.nextIndex(), i++);
  editBox->SetSel((int)strIndex.getStart(), (int)strIndex.getEnd(),FALSE);
  m_markedPly = index;
}

void CHistoryDlg::OnHistoryContextMenuGoodMove()         { setAnnotation(GOOD_MOVE);        }
void CHistoryDlg::OnHistoryContextMenuExcellentMove()    { setAnnotation(EXCELLENT_MOVE);   }
void CHistoryDlg::OnHistoryContextMenuBadMove()          { setAnnotation(BAD_MOVE);         }
void CHistoryDlg::OnHistoryContextMenuVeryBadMove()      { setAnnotation(VERYBAD_MOVE);     }
void CHistoryDlg::OnHistoryContextMenuInterestingMove()  { setAnnotation(INTERESTING_MOVE); }
void CHistoryDlg::OnHistoryContextMenuDoubiousMove()     { setAnnotation(DOUBIOUS_MOVE);    }
void CHistoryDlg::OnHistoryContextMenuRemoveAnnotation() { setAnnotation(NOANNOTATION);     }

void CHistoryDlg::setAnnotation(MoveAnnotation annotation) {
  CEdit *e = getHistoryBox();
  const int    topline   = e->GetFirstVisibleLine();
  const CPoint caret     = e->GetCaretPos();
  const int    markedPly = m_markedPly;

  e->LineScroll(-topline);
  m_history[m_selectedPly].setAnnotation(annotation);
  updateHistoryText();
//  if(markedPly >= 0) {
//    markPly(markedPly);
//  }
  e->LineScroll(topline);
  e->SetCaretPos(caret);
}

void CHistoryDlg::OnCancel() {
  if(isChanged()) {
    if(!confirmCancel(this)) {
      return;
    }
  }
  CDialog::OnCancel();
}

bool CHistoryDlg::isChanged() const {
  return m_history != m_origHistory;
}

void CHistoryDlg::OnFileAnalyzePosition() {
  assert(m_currentPly >= 0 && m_currentPly < (int)m_history.size());
  startNewGameAnalyzeThread(m_gameName, m_gameStartPosition, m_history, m_currentPly);
}

void CHistoryDlg::OnSize(UINT nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}
