#include "stdafx.h"
#include <MFCUtil/Clipboard.h>
#include "Chess.h"
#include "ChessDlg.h"

// --------------------------------------------- Edit functions -------------------------------------------

#if defined(_DEBUG)
#define TRACESTATE() m_editHistory.printState(__TFUNCTION__)
#else
#define TRACESTATE()
#endif // _DEBUG

void CChessDlg::OnEditStartSetup() {
  try {
    pushDialogMode(EDITMODE);
    TRACESTATE();
  } catch(Exception e) {
    errorMessage(e);
  }
}

void CChessDlg::OnEditTurnBoard() {
  BEGINPAINT();
  setComputerPlayer(GETENEMY(getComputerPlayer()));
  setVisibleClocks();
  ENDPAINT();
  TRACESTATE();
}

void CChessDlg::OnEditEndSetup() {
  BEGINPAINT();
  try {
    m_graphics->unmarkAll();
    if(m_editHistory.isModified()) {
      m_game = m_editHistory.getGame().validateBoard(getOptions().getValidateAfterEdit());
      m_game.endSetup();
      notifyGameChanged(m_game);
    }
    m_editHistory.endEdit();
    popDialogMode();
  } catch(Exception e) {
    errorMessage(e);
  }
  ENDPAINT();
}

void CChessDlg::OnEditRemove() {
  try {
    m_editHistory.saveState().removePieceAtPosition(m_selectedPosition);
    m_editHistory.getGame().initState();
    paintGamePosition();
    TRACESTATE();
  } catch(Exception e) {
    errorMessage(e);
  }
}

void CChessDlg::OnEditClearBoard() {
  try {
    m_editHistory.saveState().clearBoard();
    paintGamePosition();
    TRACESTATE();
  } catch(Exception e) {
    errorMessage(e);
  }
}

void CChessDlg::OnEditSetupStartPosition() {
  try {
    m_editHistory.saveState().newGame(m_editHistory.getGame().getFileName());
    paintGamePosition();
    TRACESTATE();
  } catch(Exception e) {
    errorMessage(e);
  }
}

void CChessDlg::OnEditSwitchPlayerInTurn() {
  setPlayerInTurn(GETENEMY(m_editHistory.getGame().getPlayerInTurn()));
}

void CChessDlg::setPlayerInTurn(Player player) {
  try {
    m_editHistory.saveState().setPlayerInTurn(player);
    paintGamePosition();
    TRACESTATE();
  } catch(Exception e) {
    errorMessage(e);
  }
}

void CChessDlg::OnEditSwapColors() {
  applySymmetricTransformation(TRANSFORM_SWAPPLAYERS, false);
  setComputerPlayer(GETENEMY(getComputerPlayer()));
  invalidate();
}

void CChessDlg::OnEditMirrorColumn() { applySymmetricTransformation(TRANSFORM_MIRRORCOL  ); }
void CChessDlg::OnEditMirrorRow()    { applySymmetricTransformation(TRANSFORM_MIRRORROW  ); }
void CChessDlg::OnEditMirrorDiag1()  { applySymmetricTransformation(TRANSFORM_MIRRORDIAG1); }
void CChessDlg::OnEditMirrorDiag2()  { applySymmetricTransformation(TRANSFORM_MIRRORDIAG2); }
void CChessDlg::OnEditRotateRight()  { applySymmetricTransformation(TRANSFORM_ROTATERIGHT); }
void CChessDlg::OnEditRotateLeft()   { applySymmetricTransformation(TRANSFORM_ROTATELEFT ); }
void CChessDlg::OnEditRotate180()    { applySymmetricTransformation(TRANSFORM_ROTATE180  ); }

void CChessDlg::applySymmetricTransformation(SymmetricTransformation st, bool paint) {
  try {
    GameKey key = m_editHistory.saveState().getKey();
    Game    g   = key.transform(st);
    g.beginSetup();
    m_editHistory.getGame() = g;
    if(paint) {
      paintGamePosition();
      TRACESTATE();
    }
  } catch(Exception e) {
    errorMessage(e);
  }
}

// ------------------------------------- Edit mousehandlers -----------------------------------------

void CChessDlg::OnLButtonDownEditMode(UINT nFlags, CPoint point) {
  Game &game = m_editHistory.saveState();
  const int pos = getBoardPosition(point);
  if(isValidPosition(pos)) {
    m_selectedPosition = pos;
    m_selectedPiece    = game.getPieceKeyAtPosition(pos);
    m_removedPiece     = EMPTYPIECEKEY;
  } else {
    const OffboardPiece *obp = m_graphics->getOffboardPieceByPosition(point);
    if(obp != NULL) {
      m_graphics->beginDragPiece(point, obp);
    }
  }
}

void CChessDlg::OnLButtonUpEditMode(UINT nFlags, CPoint point) {
  Game &game = m_editHistory.saveState();
  if(m_removedPiece != EMPTYPIECEKEY) {
    const Player rp = GET_PLAYER_FROMKEY(m_removedPiece);
    game.resetCapturedPieceTypes(rp);
    m_graphics->repaintOffboardPieces(rp);
  } else if(m_graphics->isDragging()) {
    game.resetCapturedPieceTypes(GET_PLAYER_FROMKEY(m_graphics->getDraggedPiece()));
    m_graphics->endDragPiece();
  }
  game.initState();
  m_selectedPiece = m_removedPiece = EMPTYPIECEKEY;
  TRACESTATE();
  enableUndoRedo();
}

void CChessDlg::OnMouseMoveEditMode(UINT nFlags, CPoint point) {
  const int newPos = getBoardPosition(point);
  if((nFlags & MK_LBUTTON) == 0) {
    m_selectedPiece = m_removedPiece = EMPTYPIECEKEY;
    if(newPos != m_graphics->getMouseField()) {
      m_graphics->markMouse(newPos);
    }
  } else {
    Game &game = m_editHistory.getGame();
    if(m_graphics->isDragging()) {
      if(!isValidPosition(newPos)) {   // drag piece outside board
        m_graphics->dragPiece(point);
      } else {                         // drag piece from outside board to a valid board position
        try {
          const PieceKey dpKey = m_graphics->getDraggedPiece();
          m_removedPiece       = game.setPieceAtPosition(dpKey, newPos); // can throw
          game.initState();

          m_selectedPiece      = dpKey;
          m_selectedPosition   = newPos;

          BEGINPAINT();
          m_graphics->endDragPiece();
          m_graphics->markMouse(newPos);
          ENDPAINT();
        } catch(Exception e) {
//          m_graphics->dragPiece(point);
        }
      }
    } else { // !dragging
      if((m_selectedPiece != EMPTYPIECEKEY) && isValidPosition(m_selectedPosition) && (newPos != m_selectedPosition)) {
        if(!isValidPosition(newPos)) { // drag piece from a valid board position to a position outside the board
          const PieceKey key = game.removePieceAtPosition(m_selectedPosition, false);
          if(key != EMPTYPIECEKEY) {
            game.setPieceAtPosition(m_removedPiece, m_selectedPosition);
            game.initState();
            BEGINPAINT();
            m_graphics->unmarkMouse();
            m_graphics->paintGamePositions();
            m_graphics->beginDragPiece(point, key);
            m_selectedPiece = m_removedPiece = EMPTYPIECEKEY;
            ENDPAINT();
          }
        } else {                       // drag piece from one field to another
          const PieceKey oldKey = game.removePieceAtPosition(m_selectedPosition);
          try {
            game.setPieceAtPosition(m_removedPiece, m_selectedPosition);
            const PieceKey newRemovedPiece = game.setPieceAtPosition(m_selectedPiece, newPos); // can throw
            game.initState();

            BEGINPAINT();
            m_graphics->unmarkMouse();
            m_graphics->paintGamePositions();
            m_removedPiece     = newRemovedPiece;
            m_selectedPosition = newPos;
            m_graphics->markMouse(newPos);
            ENDPAINT();
          } catch(Exception e) {
            game.setPieceAtPosition(oldKey, m_selectedPosition); // set it back again
            game.initState();
          }
        }
      }
    }
  }
}

typedef struct {
  PieceKey  m_key;
  int       m_commandId;
} ContextMenuItem;

void CChessDlg::OnContextMenuEditMode(CWnd *pWnd, CPoint point) {
  static const ContextMenuItem itemArray[2][6] = {
    { WHITEKING   , ID_EDIT_ADD_WHITE_KING
     ,WHITEQUEEN  , ID_EDIT_ADD_WHITE_QUEEN
     ,WHITEROOK   , ID_EDIT_ADD_WHITE_ROOK
     ,WHITEBISHOP , ID_EDIT_ADD_WHITE_BISHOP
     ,WHITEKNIGHT , ID_EDIT_ADD_WHITE_KNIGHT
     ,WHITEPAWN   , ID_EDIT_ADD_WHITE_PAWN
    },
    { BLACKKING   , ID_EDIT_ADD_BLACK_KING
     ,BLACKQUEEN  , ID_EDIT_ADD_BLACK_QUEEN
     ,BLACKROOK   , ID_EDIT_ADD_BLACK_ROOK
     ,BLACKBISHOP , ID_EDIT_ADD_BLACK_BISHOP
     ,BLACKKNIGHT , ID_EDIT_ADD_BLACK_KNIGHT
     ,BLACKPAWN   , ID_EDIT_ADD_BLACK_PAWN
    }
  };

  static const int colorMarkResourceId[] = {
    IDB_BITMAPWHITEMARK
   ,IDB_BITMAPBLACKMARK
  };

  m_selectedPosition = getBoardPosition(point, true);
  if(!isValidPosition(m_selectedPosition)) {
    return;
  }

  const Game &game = m_editHistory.getGame();
  CMenu pieceMenu[2];
  CBitmap bitmaps[2][6];
  bool subMenuCreated = false;
  forEachPlayer(player) {
    CMenu &pm = pieceMenu[player];
    for(int i = 0; i < ARRAYSIZE(itemArray[player]); i++) {
      const ContextMenuItem &item = itemArray[player][i];
      if(game.isAddPieceLegal(item.m_key, m_selectedPosition)) {
        if(pm.m_hMenu == NULL) {
          pm.CreateMenu();
          subMenuCreated = true;
        }
        pm.AppendMenu(0, item.m_commandId, &m_graphics->getResources().getSmallPieceBitmap(bitmaps[player][i], item.m_key));
      }
    }
  }
  if(!game.isPositionEmpty(m_selectedPosition)) {
    subMenuCreated = true;
  }
  if(!subMenuCreated) {
    return;
  }

  CMenu menu;
  menu.CreateMenu();
  CBitmap colorBitmap[2];
  CBitmap deleteBitmap;
  forEachPlayer(p) {
    CMenu &pm = pieceMenu[p];
    if(pm.m_hMenu) {
      CBitmap &cbm = colorBitmap[p];
      cbm.LoadBitmap(colorMarkResourceId[p]);
      menu.AppendMenu(MF_POPUP, (UINT_PTR)((HMENU)pm), &cbm);
    }
  }
  if(!game.isPositionEmpty(m_selectedPosition)) {
    deleteBitmap.LoadBitmap(IDB_BITMAPDELETEMARK);
    menu.AppendMenu(MF_STRING, ID_EDIT_REMOVE, &deleteBitmap);
  }
  CMenu topMenu;
  topMenu.CreateMenu();
  topMenu.AppendMenu(MF_POPUP, (UINT_PTR)((HMENU)menu), _T("Edit"));
  topMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x,point.y, this);
}

void CChessDlg::OnEditAddWhiteKing()   { setPiece(WHITEKING   ); }
void CChessDlg::OnEditAddWhiteQueen()  { setPiece(WHITEQUEEN  ); }
void CChessDlg::OnEditAddWhiteRook()   { setPiece(WHITEROOK   ); }
void CChessDlg::OnEditAddWhiteBishop() { setPiece(WHITEBISHOP ); }
void CChessDlg::OnEditAddWhiteKnight() { setPiece(WHITEKNIGHT ); }
void CChessDlg::OnEditAddWhitePawn()   { setPiece(WHITEPAWN   ); }
void CChessDlg::OnEditAddBlackKing()   { setPiece(BLACKKING   ); }
void CChessDlg::OnEditAddBlackQueen()  { setPiece(BLACKQUEEN  ); }
void CChessDlg::OnEditAddBlackRook()   { setPiece(BLACKROOK   ); }
void CChessDlg::OnEditAddBlackBishop() { setPiece(BLACKBISHOP ); }
void CChessDlg::OnEditAddBlackKnight() { setPiece(BLACKKNIGHT ); }
void CChessDlg::OnEditAddBlackPawn()   { setPiece(BLACKPAWN   ); }

void CChessDlg::setPiece(PieceKey key) {
  try {
    Game &game = m_editHistory.saveState();
    const PieceKey removedPiece = game.setPieceAtPosition(key, m_selectedPosition);
    if(removedPiece != EMPTYPIECEKEY) {
      game.resetCapturedPieceTypes(GET_PLAYER_FROMKEY(removedPiece));
    }
    game.initState();
    paintGamePosition();
    TRACESTATE();
  } catch(Exception e) {
    errorMessage(e);
  }
}

void CChessDlg::OnEditPasteFEN() {
#if !defined(TABLEBASE_BUILDER)
  try {
    const String text = getClipboardText();
    Game tmp;
    tmp.fromFENString(text);
    Game &game = m_editHistory.saveState();
    game = tmp;
    game.beginSetup();
    paintGamePosition();
    TRACESTATE();
  } catch(Exception e) {
    errorMessage(e);
  }
#else
  errorMessage(_T("Paste FEN not available in BUILDER_MODE"));
#endif
}
