#include "stdafx.h"

// ---------------------------------------- King leave row/column/diagonal1/2 --------------------------------------

void Game::kingLeaveRow(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_rowLine.m_isBlockable) {
    unblockRow(fieldInfo);
  }
  if(fieldInfo.m_rowLine.m_lower) {
    SET_NOKING_RIGHT(state, fieldInfo);
  }
  if(fieldInfo.m_rowLine.m_upper) {
    SET_NOKING_LEFT( state, fieldInfo);
  }
}

void Game::kingLeaveColumn(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_colLine.m_isBlockable  ) {
    unblockColumn(fieldInfo);
  }
  if(fieldInfo.m_colLine.m_lower) {
    SET_NOKING_UP(   state, fieldInfo);
  }
  if(fieldInfo.m_colLine.m_upper) {
    SET_NOKING_DOWN( state, fieldInfo);
  }
}

void Game::kingLeaveRC(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_rowLine.m_isBlockable) {
    unblockRow(fieldInfo);
  }
  if(fieldInfo.m_colLine.m_isBlockable) {
    unblockColumn(fieldInfo);
  }

  if(fieldInfo.m_rowLine.m_lower) {
    SET_NOKING_RIGHT(state, fieldInfo);
  }
  if(fieldInfo.m_rowLine.m_upper) {
    SET_NOKING_LEFT( state, fieldInfo);
  }
  if(fieldInfo.m_colLine.m_lower) {
    SET_NOKING_UP(   state, fieldInfo);
  }
  if(fieldInfo.m_colLine.m_upper) {
    SET_NOKING_DOWN( state, fieldInfo);
  }
}

void Game::kingLeaveDiag1(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_diag1Line.m_isBlockable) {
    unblockDiag1(fieldInfo);
  }
  if(fieldInfo.m_diag1Line.m_lower) {
    SET_NOKING_UPDIAG1(  state, fieldInfo);
  }
  if(fieldInfo.m_diag1Line.m_upper) {
    SET_NOKING_DOWNDIAG1(state, fieldInfo);
  }
}

void Game::kingLeaveDiag2( const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_diag2Line.m_isBlockable) {
    unblockDiag2(fieldInfo);
  }
  if(fieldInfo.m_diag2Line.m_lower) {
    SET_NOKING_UPDIAG2(  state, fieldInfo);
  }
  if(fieldInfo.m_diag2Line.m_upper) {
    SET_NOKING_DOWNDIAG2(state, fieldInfo);
  }
}

void Game::kingLeaveDiag12(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_innerField       ) {
    unblockDiag12(fieldInfo);
  }
  if(fieldInfo.m_diag1Line.m_lower) {
    SET_NOKING_UPDIAG1(  state, fieldInfo);
  }
  if(fieldInfo.m_diag1Line.m_upper) {
    SET_NOKING_DOWNDIAG1(state, fieldInfo);
  }
  if(fieldInfo.m_diag2Line.m_lower) {
    SET_NOKING_UPDIAG2(  state, fieldInfo);
  }
  if(fieldInfo.m_diag2Line.m_upper) {
    SET_NOKING_DOWNDIAG2(state, fieldInfo);
  }
}

// ---------------------------------------- King enter row/column/diagonal1/2 --------------------------------------

void Game::kingEnterRow(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_rowLine.m_isBlockable) {
    if(GET_LDA_LEFT( state, fieldInfo.m_pos)) {
      CLR_LDA_LEFT(  state, fieldInfo);
    }
    if(GET_LDA_RIGHT(state, fieldInfo.m_pos)) {
      CLR_LDA_RIGHT( state, fieldInfo);
    }
  }
  if(fieldInfo.m_rowLine.m_lower) {
    setKingRight(state, fieldInfo);
  }
  if(fieldInfo.m_rowLine.m_upper) {
    setKingLeft( state, fieldInfo);
  }
}

void Game::kingEnterColumn(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_colLine.m_isBlockable) {
    if(GET_LDA_DOWN(state, fieldInfo.m_pos)) {
      CLR_LDA_DOWN( state, fieldInfo);
    }
    if(GET_LDA_UP(  state, fieldInfo.m_pos)) {
      CLR_LDA_UP(   state, fieldInfo);
    }
  }
  if(fieldInfo.m_colLine.m_lower) {
    setKingUp(  state, fieldInfo);
  }
  if(fieldInfo.m_colLine.m_upper) {
    setKingDown(state, fieldInfo);
  }
}

void Game::kingEnterRC(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_rowLine.m_isBlockable) {
    if(GET_LDA_LEFT( state, fieldInfo.m_pos)) {
      CLR_LDA_LEFT(  state, fieldInfo);
    }
    if(GET_LDA_RIGHT(state, fieldInfo.m_pos)) {
      CLR_LDA_RIGHT( state, fieldInfo);
    }
  }
  if(fieldInfo.m_colLine.m_isBlockable) {
    if(GET_LDA_DOWN( state, fieldInfo.m_pos)) {
      CLR_LDA_DOWN(  state, fieldInfo);
    }
    if(GET_LDA_UP(   state, fieldInfo.m_pos)) {
      CLR_LDA_UP(    state, fieldInfo);
    }
  }
  if(fieldInfo.m_rowLine.m_lower) {
    setKingRight(state, fieldInfo);
  }
  if(fieldInfo.m_rowLine.m_upper) {
    setKingLeft(state, fieldInfo);
  }
  if(fieldInfo.m_colLine.m_lower) {
    setKingUp(  state, fieldInfo);
  }
  if(fieldInfo.m_colLine.m_upper) {
    setKingDown(state, fieldInfo);
  }
}

void Game::kingEnterDiag1(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_diag1Line.m_isBlockable) {
    if(GET_LDA_DOWNDIAG1(state, fieldInfo.m_pos)) {
      CLR_LDA_DOWNDIAG1( state, fieldInfo);
    }
    if(GET_LDA_UPDIAG1(  state, fieldInfo.m_pos)) {
      CLR_LDA_UPDIAG1(   state, fieldInfo);
    }
  }
  if(fieldInfo.m_diag1Line.m_lower) {
    setKingUpDiag1(  state, fieldInfo);
  }
  if(fieldInfo.m_diag1Line.m_upper) {
    setKingDownDiag1(state, fieldInfo);
  }
}

void Game::kingEnterDiag2(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_diag2Line.m_isBlockable) {
    if(GET_LDA_DOWNDIAG2(state, fieldInfo.m_pos)) {
      CLR_LDA_DOWNDIAG2( state, fieldInfo);
    }
    if(GET_LDA_UPDIAG2(  state, fieldInfo.m_pos)) {
      CLR_LDA_UPDIAG2(   state, fieldInfo);
    }
  }
  if(fieldInfo.m_diag2Line.m_lower) {
    setKingUpDiag2(  state, fieldInfo);
  }
  if(fieldInfo.m_diag2Line.m_upper) {
    setKingDownDiag2(state, fieldInfo);
  }
}

void Game::kingEnterDiag12(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_innerField) {
    if(GET_LDA_DOWNDIAG1(state, fieldInfo.m_pos)) {
      CLR_LDA_DOWNDIAG1( state, fieldInfo);
    }
    if(GET_LDA_UPDIAG1(  state, fieldInfo.m_pos)) {
      CLR_LDA_UPDIAG1(   state, fieldInfo);
    }
    if(GET_LDA_DOWNDIAG2(state, fieldInfo.m_pos)) {
      CLR_LDA_DOWNDIAG2( state, fieldInfo);
    }
    if(GET_LDA_UPDIAG2(  state, fieldInfo.m_pos)) {
      CLR_LDA_UPDIAG2(   state, fieldInfo);
    }
  }
  if(fieldInfo.m_diag1Line.m_lower) {
    setKingUpDiag1(  state, fieldInfo);
  }
  if(fieldInfo.m_diag1Line.m_upper) {
    setKingDownDiag1(state, fieldInfo);
  }
  if(fieldInfo.m_diag2Line.m_lower) {
    setKingUpDiag2(  state, fieldInfo);
  }
  if(fieldInfo.m_diag2Line.m_upper) {
    setKingDownDiag2(state, fieldInfo);
  }
}
// ---------------------------------------- King enterCapture --------------------------------------

void Game::kingEnterRowCapture(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_rowLine.m_lower) {
    setKingRight(state, fieldInfo);
  }
  if(fieldInfo.m_rowLine.m_upper) {
    setKingLeft(state, fieldInfo);
  }
}

void Game::kingEnterColumnCapture(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_colLine.m_lower) {
    setKingUp(state, fieldInfo);
  }
  if(fieldInfo.m_colLine.m_upper) {
    setKingDown(state, fieldInfo);
  }
}

void Game::kingEnterRCCapture(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_rowLine.m_lower) {
    setKingRight(state, fieldInfo);
  }
  if(fieldInfo.m_rowLine.m_upper) {
    setKingLeft(state, fieldInfo);
  }
  if(fieldInfo.m_colLine.m_lower) {
    setKingUp(state, fieldInfo);
  }
  if(fieldInfo.m_colLine.m_upper) {
    setKingDown(state, fieldInfo);
  }
}

void Game::kingEnterDiag1Capture(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_diag1Line.m_lower) {
    setKingUpDiag1(state, fieldInfo);
  }
  if(fieldInfo.m_diag1Line.m_upper) {
    setKingDownDiag1(state, fieldInfo);
  }
}

void Game::kingEnterDiag2Capture(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_diag2Line.m_lower) {
    setKingUpDiag2(state, fieldInfo);
  }
  if(fieldInfo.m_diag2Line.m_upper) {
    setKingDownDiag2(state, fieldInfo);
  }
}

void Game::kingEnterDiag12Capture(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_diag1Line.m_lower) {
    setKingUpDiag1(  state, fieldInfo);
  }
  if(fieldInfo.m_diag1Line.m_upper) {
    setKingDownDiag1(state, fieldInfo);
  }
  if(fieldInfo.m_diag2Line.m_lower) {
    setKingUpDiag2(  state, fieldInfo);
  }
  if(fieldInfo.m_diag2Line.m_upper) {
    setKingDownDiag2(state, fieldInfo);
  }
}

void Game::updateKingDir(const Move &m) {
  switch(m.m_direction) {
  case MD_NONE     : updateKingDirMovingKnight(   m); break;
  case MD_LEFT     : updateKingDirMovingLeft(     m); break;
  case MD_RIGHT    : updateKingDirMovingRight(    m); break;
  case MD_DOWN     : updateKingDirMovingDown(     m); break;
  case MD_UP       : updateKingDirMovingUp(       m); break;
  case MD_DOWNDIAG1: updateKingDirMovingDownDiag1(m); break;
  case MD_UPDIAG1  : updateKingDirMovingUpDiag1(  m); break;
  case MD_DOWNDIAG2: updateKingDirMovingDownDiag2(m); break;
  case MD_UPDIAG2  : updateKingDirMovingUpDiag2(  m); break;
  default          : throwInvalidArgumentException(__TFUNCTION__, _T("m.direction=%d"), m.m_direction);
  }
}

#define IF_BLOCKABLE_SET_KING_LEFT(       state, fieldInfo) { if(fieldInfo.m_innerCol  ) setKingLeft(         state, fieldInfo); }
#define IF_BLOCKABLE_SET_KING_RIGHT(      state, fieldInfo) { if(fieldInfo.m_innerCol  ) setKingRight(        state, fieldInfo); }
#define IF_BLOCKABLE_SET_KING_DOWN(       state, fieldInfo) { if(fieldInfo.m_innerRow  ) setKingDown(         state, fieldInfo); }
#define IF_BLOCKABLE_SET_KING_UP(         state, fieldInfo) { if(fieldInfo.m_innerRow  ) setKingUp(           state, fieldInfo); }
#define IF_BLOCKABLE_SET_KING_DOWNDIAG1(  state, fieldInfo) { if(fieldInfo.m_innerField) setKingDownDiag1(    state, fieldInfo); }
#define IF_BLOCKABLE_SET_KING_UPDIAG1(    state, fieldInfo) { if(fieldInfo.m_innerField) setKingUpDiag1(      state, fieldInfo); }
#define IF_BLOCKABLE_SET_KING_DOWNDIAG2(  state, fieldInfo) { if(fieldInfo.m_innerField) setKingDownDiag2(    state, fieldInfo); }
#define IF_BLOCKABLE_SET_KING_UPDIAG2(    state, fieldInfo) { if(fieldInfo.m_innerField) setKingUpDiag2(      state, fieldInfo); }

#define IF_BLOCKABLE_SET_NOKING_LEFT(     state, fieldInfo) { if(fieldInfo.m_innerCol  ) SET_NOKING_LEFT(     state, fieldInfo); }
#define IF_BLOCKABLE_SET_NOKING_RIGHT(    state, fieldInfo) { if(fieldInfo.m_innerCol  ) SET_NOKING_RIGHT(    state, fieldInfo); }
#define IF_BLOCKABLE_SET_NOKING_DOWN(     state, fieldInfo) { if(fieldInfo.m_innerRow  ) SET_NOKING_DOWN(     state, fieldInfo); }
#define IF_BLOCKABLE_SET_NOKING_UP(       state, fieldInfo) { if(fieldInfo.m_innerRow  ) SET_NOKING_UP(       state, fieldInfo); }
#define IF_BLOCKABLE_SET_NOKING_DOWNDIAG1(state, fieldInfo) { if(fieldInfo.m_innerField) SET_NOKING_DOWNDIAG1(state, fieldInfo); }
#define IF_BLOCKABLE_SET_NOKING_UPDIAG1(  state, fieldInfo) { if(fieldInfo.m_innerField) SET_NOKING_UPDIAG1(  state, fieldInfo); }
#define IF_BLOCKABLE_SET_NOKING_DOWNDIAG2(state, fieldInfo) { if(fieldInfo.m_innerField) SET_NOKING_DOWNDIAG2(state, fieldInfo); }
#define IF_BLOCKABLE_SET_NOKING_UPDIAG2(  state, fieldInfo) { if(fieldInfo.m_innerField) SET_NOKING_UPDIAG2(  state, fieldInfo); }

// ----------------------------- updateKingDirMoving (not king moving)-------------------------------------------

void Game::updateKingDirMovingLeft(      const Move &m) {
  const FieldInfo &fromInfo = fieldInfo[m.m_from];

  forEachPlayer(p) {
    PlayerState &state = m_playerState[p];
    switch(KING_DIRECTION(state, m.m_from)) {
    case MD_NONE       :
      break;
    case MD_LEFT       :
      { if(m.m_capturedPiece == NULL) {
          for(int pos = fromInfo.m_pos; pos != m.m_to; pos += DPOS_LEFT ) {
            KING_DIRECTION(state, pos) = MD_NONE;
          }
        }
      }
      break;
    case MD_RIGHT      :
      { for(int pos = fromInfo.m_rowLine.m_lower[1]; DPOS_LEFT_LE( pos, m.m_to); pos += DPOS_LEFT ) {
          KING_DIRECTION(state, pos) = MD_RIGHT;
        }
      }
      break;
    case MD_DOWN       : IF_BLOCKABLE_SET_KING_DOWN(       state, fromInfo); break;
    case MD_UP         : IF_BLOCKABLE_SET_KING_UP(         state, fromInfo); break;
    case MD_DOWNDIAG1  : IF_BLOCKABLE_SET_KING_DOWNDIAG1(  state, fromInfo); break;
    case MD_UPDIAG1    : IF_BLOCKABLE_SET_KING_UPDIAG1(    state, fromInfo); break;
    case MD_DOWNDIAG2  : IF_BLOCKABLE_SET_KING_DOWNDIAG2(  state, fromInfo); break;
    case MD_UPDIAG2    : IF_BLOCKABLE_SET_KING_UPDIAG2(    state, fromInfo); break;
    }
  }
  if(m.m_capturedPiece == NULL) {
    const FieldInfo &toInfo = fieldInfo[m.m_to];
    forEachPlayer(p) {
      PlayerState &state = m_playerState[p];
      switch(KING_DIRECTION(state, m.m_to)) {
      case MD_NONE     :
      case MD_LEFT     :
      case MD_RIGHT    : break;
      case MD_DOWN     : IF_BLOCKABLE_SET_NOKING_DOWN(     state, toInfo);   break;
      case MD_UP       : IF_BLOCKABLE_SET_NOKING_UP(       state, toInfo);   break;
      case MD_DOWNDIAG1: IF_BLOCKABLE_SET_NOKING_DOWNDIAG1(state, toInfo);   break;
      case MD_UPDIAG1  : IF_BLOCKABLE_SET_NOKING_UPDIAG1(  state, toInfo);   break;
      case MD_DOWNDIAG2: IF_BLOCKABLE_SET_NOKING_DOWNDIAG2(state, toInfo);   break;
      case MD_UPDIAG2  : IF_BLOCKABLE_SET_NOKING_UPDIAG2(  state, toInfo);   break;
      }
    }
  }
}





void Game::updateKingDirMovingRight(     const Move &m) {
  const FieldInfo &fromInfo = fieldInfo[m.m_from];

  forEachPlayer(p) {
    PlayerState &state = m_playerState[p];
    switch(KING_DIRECTION(state, m.m_from)) {
    case MD_NONE       :
      break;
    case MD_RIGHT      :
      { if(m.m_capturedPiece == NULL) {
          for(int pos = fromInfo.m_pos; pos != m.m_to; pos += DPOS_RIGHT) {
            KING_DIRECTION(state, pos) = MD_NONE;
          }
        }
      }
      break;
    case MD_LEFT       :
      { for(int pos = fromInfo.m_rowLine.m_upper[1]; DPOS_RIGHT_LE(pos, m.m_to); pos += DPOS_RIGHT) {
          KING_DIRECTION(state, pos) = MD_LEFT;
        }
      }
      break;
    case MD_DOWN       : IF_BLOCKABLE_SET_KING_DOWN(       state, fromInfo); break;
    case MD_UP         : IF_BLOCKABLE_SET_KING_UP(         state, fromInfo); break;
    case MD_DOWNDIAG1  : IF_BLOCKABLE_SET_KING_DOWNDIAG1(  state, fromInfo); break;
    case MD_UPDIAG1    : IF_BLOCKABLE_SET_KING_UPDIAG1(    state, fromInfo); break;
    case MD_DOWNDIAG2  : IF_BLOCKABLE_SET_KING_DOWNDIAG2(  state, fromInfo); break;
    case MD_UPDIAG2    : IF_BLOCKABLE_SET_KING_UPDIAG2(    state, fromInfo); break;
    }
  }
  if(m.m_capturedPiece == NULL) {
    const FieldInfo &toInfo = fieldInfo[m.m_to];
    forEachPlayer(p) {
      PlayerState &state = m_playerState[p];
      switch(KING_DIRECTION(state, m.m_to)) {
      case MD_NONE     :
      case MD_LEFT     :
      case MD_RIGHT    : break;
      case MD_DOWN     : IF_BLOCKABLE_SET_NOKING_DOWN(     state, toInfo);   break;
      case MD_UP       : IF_BLOCKABLE_SET_NOKING_UP(       state, toInfo);   break;
      case MD_DOWNDIAG1: IF_BLOCKABLE_SET_NOKING_DOWNDIAG1(state, toInfo);   break;
      case MD_UPDIAG1  : IF_BLOCKABLE_SET_NOKING_UPDIAG1(  state, toInfo);   break;
      case MD_DOWNDIAG2: IF_BLOCKABLE_SET_NOKING_DOWNDIAG2(state, toInfo);   break;
      case MD_UPDIAG2  : IF_BLOCKABLE_SET_NOKING_UPDIAG2(  state, toInfo);   break;
      }
    }
  }
}





void Game::updateKingDirMovingDown(      const Move &m) {
  const FieldInfo &fromInfo = fieldInfo[m.m_from];

  forEachPlayer(p) {
    PlayerState &state = m_playerState[p];
    switch(KING_DIRECTION(state, m.m_from)) {
    case MD_NONE       :
      break;
    case MD_LEFT       : IF_BLOCKABLE_SET_KING_LEFT(       state, fromInfo); break;
    case MD_RIGHT      : IF_BLOCKABLE_SET_KING_RIGHT(      state, fromInfo); break;
    case MD_DOWN       :
      { if(m.m_capturedPiece == NULL) {
          for(int pos = fromInfo.m_pos; pos != m.m_to; pos += DPOS_DOWN) {
            KING_DIRECTION(state, pos) = MD_NONE;
          }
        }
      }
      break;
    case MD_UP         :
      { for(int pos = fromInfo.m_colLine.m_lower[1]; DPOS_DOWN_LE(pos, m.m_to); pos += DPOS_DOWN) {
          KING_DIRECTION(state, pos) = MD_UP;
        }
      }
      break;
    case MD_DOWNDIAG1  : IF_BLOCKABLE_SET_KING_DOWNDIAG1(  state, fromInfo); break;
    case MD_UPDIAG1    : IF_BLOCKABLE_SET_KING_UPDIAG1(    state, fromInfo); break;
    case MD_DOWNDIAG2  : IF_BLOCKABLE_SET_KING_DOWNDIAG2(  state, fromInfo); break;
    case MD_UPDIAG2    : IF_BLOCKABLE_SET_KING_UPDIAG2(    state, fromInfo); break;
    }
  }
  if(m.m_capturedPiece == NULL) {
    const FieldInfo &toInfo = fieldInfo[m.m_to];
    forEachPlayer(p) {
      PlayerState &state = m_playerState[p];
      switch(KING_DIRECTION(state, m.m_to)) {
      case MD_NONE     : break;
      case MD_LEFT     : IF_BLOCKABLE_SET_NOKING_LEFT(     state, toInfo);   break;
      case MD_RIGHT    : IF_BLOCKABLE_SET_NOKING_RIGHT(    state, toInfo);   break;
      case MD_DOWN     :
      case MD_UP       : break;
      case MD_DOWNDIAG1: IF_BLOCKABLE_SET_NOKING_DOWNDIAG1(state, toInfo);   break;
      case MD_UPDIAG1  : IF_BLOCKABLE_SET_NOKING_UPDIAG1(  state, toInfo);   break;
      case MD_DOWNDIAG2: IF_BLOCKABLE_SET_NOKING_DOWNDIAG2(state, toInfo);   break;
      case MD_UPDIAG2  : IF_BLOCKABLE_SET_NOKING_UPDIAG2(  state, toInfo);   break;
      }
    }
  }
}





void Game::updateKingDirMovingUp(        const Move &m) {
  const FieldInfo &fromInfo = fieldInfo[m.m_from];

  forEachPlayer(p) {
    PlayerState &state = m_playerState[p];
    switch(KING_DIRECTION(state, m.m_from)) {
    case MD_NONE       :
      break;
    case MD_LEFT       : IF_BLOCKABLE_SET_KING_LEFT(       state, fromInfo); break;
    case MD_RIGHT      : IF_BLOCKABLE_SET_KING_RIGHT(      state, fromInfo); break;
    case MD_UP         :
      { if(m.m_capturedPiece == NULL) {
          for(int pos = fromInfo.m_pos; pos != m.m_to; pos += DPOS_UP) {
            KING_DIRECTION(state, pos) = MD_NONE;
          }
        }
      }
      break;
    case MD_DOWN       :
      { for(int pos = fromInfo.m_colLine.m_upper[1]; DPOS_UP_LE(  pos, m.m_to); pos += DPOS_UP  ) {
          KING_DIRECTION(state, pos) = MD_DOWN;
        }
      }
      break;
    case MD_DOWNDIAG1  : IF_BLOCKABLE_SET_KING_DOWNDIAG1(  state, fromInfo); break;
    case MD_UPDIAG1    : IF_BLOCKABLE_SET_KING_UPDIAG1(    state, fromInfo); break;
    case MD_DOWNDIAG2  : IF_BLOCKABLE_SET_KING_DOWNDIAG2(  state, fromInfo); break;
    case MD_UPDIAG2    : IF_BLOCKABLE_SET_KING_UPDIAG2(    state, fromInfo); break;
    }
  }
  if(m.m_capturedPiece == NULL) {
    const FieldInfo &toInfo = fieldInfo[m.m_to];
    forEachPlayer(p) {
      PlayerState &state = m_playerState[p];
      switch(KING_DIRECTION(state, m.m_to)) {
      case MD_NONE     : break;
      case MD_LEFT     : IF_BLOCKABLE_SET_NOKING_LEFT(     state, toInfo);   break;
      case MD_RIGHT    : IF_BLOCKABLE_SET_NOKING_RIGHT(    state, toInfo);   break;
      case MD_DOWN     :
      case MD_UP       : break;
      case MD_DOWNDIAG1: IF_BLOCKABLE_SET_NOKING_DOWNDIAG1(state, toInfo);   break;
      case MD_UPDIAG1  : IF_BLOCKABLE_SET_NOKING_UPDIAG1(  state, toInfo);   break;
      case MD_DOWNDIAG2: IF_BLOCKABLE_SET_NOKING_DOWNDIAG2(state, toInfo);   break;
      case MD_UPDIAG2  : IF_BLOCKABLE_SET_NOKING_UPDIAG2(  state, toInfo);   break;
      }
    }
  }
}





void Game::updateKingDirMovingDownDiag1( const Move &m) {
  const FieldInfo &fromInfo = fieldInfo[m.m_from];

  forEachPlayer(p) {
    PlayerState &state = m_playerState[p];
    switch(KING_DIRECTION(state, m.m_from)) {
    case MD_NONE       :
      break;
    case MD_LEFT       : IF_BLOCKABLE_SET_KING_LEFT(       state, fromInfo); break;
    case MD_RIGHT      : IF_BLOCKABLE_SET_KING_RIGHT(      state, fromInfo); break;
    case MD_DOWN       : IF_BLOCKABLE_SET_KING_DOWN(       state, fromInfo); break;
    case MD_UP         : IF_BLOCKABLE_SET_KING_UP(         state, fromInfo); break;
    case MD_DOWNDIAG1:
      { if(m.m_capturedPiece == NULL) {
          for(int pos = fromInfo.m_pos; pos != m.m_to; pos += DPOS_DOWNDIAG1) {
            KING_DIRECTION(state, pos) = MD_NONE;
          }
        }
      }
      break;
    case MD_UPDIAG1    :
      { for(int pos = fromInfo.m_diag1Line.m_lower[1]; DPOS_DOWNDIAG1_LE(pos, m.m_to); pos += DPOS_DOWNDIAG1) {
          KING_DIRECTION(state, pos) = MD_UPDIAG1;
        }
      }
      break;
    case MD_DOWNDIAG2  : IF_BLOCKABLE_SET_KING_DOWNDIAG2(  state, fromInfo); break;
    case MD_UPDIAG2    : IF_BLOCKABLE_SET_KING_UPDIAG2(    state, fromInfo); break;
    }
  }
  if(m.m_capturedPiece == NULL) {
    const FieldInfo &toInfo = fieldInfo[m.m_to];
    forEachPlayer(p) {
      PlayerState &state = m_playerState[p];
      switch(KING_DIRECTION(state, m.m_to)) {
      case MD_NONE     : break;
      case MD_LEFT     : IF_BLOCKABLE_SET_NOKING_LEFT(     state, toInfo);   break;
      case MD_RIGHT    : IF_BLOCKABLE_SET_NOKING_RIGHT(    state, toInfo);   break;
      case MD_DOWN     : IF_BLOCKABLE_SET_NOKING_DOWN(     state, toInfo);   break;
      case MD_UP       : IF_BLOCKABLE_SET_NOKING_UP(       state, toInfo);   break;
      case MD_DOWNDIAG1:
      case MD_UPDIAG1  : break;
      case MD_DOWNDIAG2: IF_BLOCKABLE_SET_NOKING_DOWNDIAG2(state, toInfo);   break;
      case MD_UPDIAG2  : IF_BLOCKABLE_SET_NOKING_UPDIAG2(  state, toInfo);   break;
      }
    }
  }
}





void Game::updateKingDirMovingUpDiag1(   const Move &m) {
  const FieldInfo &fromInfo = fieldInfo[m.m_from];

  forEachPlayer(p) {
    PlayerState &state = m_playerState[p];
    switch(KING_DIRECTION(state, m.m_from)) {
    case MD_NONE       :
      break;
    case MD_LEFT       : IF_BLOCKABLE_SET_KING_LEFT(       state, fromInfo); break;
    case MD_RIGHT      : IF_BLOCKABLE_SET_KING_RIGHT(      state, fromInfo); break;
    case MD_DOWN       : IF_BLOCKABLE_SET_KING_DOWN(       state, fromInfo); break;
    case MD_UP         : IF_BLOCKABLE_SET_KING_UP(         state, fromInfo); break;
    case MD_UPDIAG1    :
      { if(m.m_capturedPiece == NULL) {
          for(int pos = fromInfo.m_pos; pos != m.m_to; pos += DPOS_UPDIAG1  ) {
            KING_DIRECTION(state, pos) = MD_NONE;
          }
        }
      }
      break;
    case MD_DOWNDIAG1  :
      { for(int pos = fromInfo.m_diag1Line.m_upper[1]; DPOS_UPDIAG1_LE(  pos, m.m_to); pos += DPOS_UPDIAG1  ) {
          KING_DIRECTION(state, pos) = MD_DOWNDIAG1;
        }
      }
      break;
    case MD_DOWNDIAG2  : IF_BLOCKABLE_SET_KING_DOWNDIAG2(  state, fromInfo); break;
    case MD_UPDIAG2    : IF_BLOCKABLE_SET_KING_UPDIAG2(    state, fromInfo); break;
    }
  }
  if(m.m_capturedPiece == NULL) {
    const FieldInfo &toInfo = fieldInfo[m.m_to];
    forEachPlayer(p) {
      PlayerState &state = m_playerState[p];
      switch(KING_DIRECTION(state, m.m_to)) {
      case MD_NONE     : break;
      case MD_LEFT     : IF_BLOCKABLE_SET_NOKING_LEFT(     state, toInfo);   break;
      case MD_RIGHT    : IF_BLOCKABLE_SET_NOKING_RIGHT(    state, toInfo);   break;
      case MD_DOWN     : IF_BLOCKABLE_SET_NOKING_DOWN(     state, toInfo);   break;
      case MD_UP       : IF_BLOCKABLE_SET_NOKING_UP(       state, toInfo);   break;
      case MD_DOWNDIAG1:
      case MD_UPDIAG1  : break;
      case MD_DOWNDIAG2: IF_BLOCKABLE_SET_NOKING_DOWNDIAG2(state, toInfo);   break;
      case MD_UPDIAG2  : IF_BLOCKABLE_SET_NOKING_UPDIAG2(  state, toInfo);   break;
      }
    }
  }
}





void Game::updateKingDirMovingDownDiag2( const Move &m) {
  const FieldInfo &fromInfo = fieldInfo[m.m_from];

  forEachPlayer(p) {
    PlayerState &state = m_playerState[p];
    switch(KING_DIRECTION(state, m.m_from)) {
    case MD_NONE       :
      break;
    case MD_LEFT       : IF_BLOCKABLE_SET_KING_LEFT(       state, fromInfo); break;
    case MD_RIGHT      : IF_BLOCKABLE_SET_KING_RIGHT(      state, fromInfo); break;
    case MD_DOWN       : IF_BLOCKABLE_SET_KING_DOWN(       state, fromInfo); break;
    case MD_UP         : IF_BLOCKABLE_SET_KING_UP(         state, fromInfo); break;
    case MD_DOWNDIAG1  : IF_BLOCKABLE_SET_KING_DOWNDIAG1(  state, fromInfo); break;
    case MD_UPDIAG1    : IF_BLOCKABLE_SET_KING_UPDIAG1(    state, fromInfo); break;
    case MD_DOWNDIAG2:
      { if(m.m_capturedPiece == NULL) {
          for(int pos = fromInfo.m_pos; pos != m.m_to; pos += DPOS_DOWNDIAG2) {
            KING_DIRECTION(state, pos) = MD_NONE;
          }
        }
      }
      break;
    case MD_UPDIAG2    :
      { for(int pos = fromInfo.m_diag2Line.m_lower[1]; DPOS_DOWNDIAG2_LE(pos, m.m_to); pos += DPOS_DOWNDIAG2) {
          KING_DIRECTION(state, pos) = MD_UPDIAG2;
        }
      }
      break;
    }
  }
  if(m.m_capturedPiece == NULL) {
    const FieldInfo &toInfo = fieldInfo[m.m_to];
    forEachPlayer(p) {
      PlayerState &state = m_playerState[p];
      switch(KING_DIRECTION(state, m.m_to)) {
      case MD_NONE     : break;
      case MD_LEFT     : IF_BLOCKABLE_SET_NOKING_LEFT(     state, toInfo);   break;
      case MD_RIGHT    : IF_BLOCKABLE_SET_NOKING_RIGHT(    state, toInfo);   break;
      case MD_DOWN     : IF_BLOCKABLE_SET_NOKING_DOWN(     state, toInfo);   break;
      case MD_UP       : IF_BLOCKABLE_SET_NOKING_UP(       state, toInfo);   break;
      case MD_DOWNDIAG1: IF_BLOCKABLE_SET_NOKING_DOWNDIAG1(state, toInfo);   break;
      case MD_UPDIAG1  : IF_BLOCKABLE_SET_NOKING_UPDIAG1(  state, toInfo);   break;
      case MD_DOWNDIAG2:
      case MD_UPDIAG2  : break;
      }
    }
  }
}





void Game::updateKingDirMovingUpDiag2(   const Move &m) {
  const FieldInfo &fromInfo = fieldInfo[m.m_from];

  forEachPlayer(p) {
    PlayerState &state = m_playerState[p];
    switch(KING_DIRECTION(state, m.m_from)) {
    case MD_NONE       :
      break;
    case MD_LEFT       : IF_BLOCKABLE_SET_KING_LEFT(       state, fromInfo); break;
    case MD_RIGHT      : IF_BLOCKABLE_SET_KING_RIGHT(      state, fromInfo); break;
    case MD_DOWN       : IF_BLOCKABLE_SET_KING_DOWN(       state, fromInfo); break;
    case MD_UP         : IF_BLOCKABLE_SET_KING_UP(         state, fromInfo); break;
    case MD_DOWNDIAG1  : IF_BLOCKABLE_SET_KING_DOWNDIAG1(  state, fromInfo); break;
    case MD_UPDIAG1    : IF_BLOCKABLE_SET_KING_UPDIAG1(    state, fromInfo); break;
    case MD_UPDIAG2    :
      { if(m.m_capturedPiece == NULL) {
          for(int pos = fromInfo.m_pos; pos != m.m_to; pos += DPOS_UPDIAG2) {
            KING_DIRECTION(state, pos) = MD_NONE;
          }
        }
      }
      break;
    case MD_DOWNDIAG2  :
      { for(int pos = fromInfo.m_diag2Line.m_upper[1]; DPOS_UPDIAG2_LE(  pos, m.m_to); pos += DPOS_UPDIAG2  ) {
          KING_DIRECTION(state, pos) = MD_DOWNDIAG2;
        }
      }
      break;
    }
  }
  if(m.m_capturedPiece == NULL) {
    const FieldInfo &toInfo = fieldInfo[m.m_to];
    forEachPlayer(p) {
      PlayerState &state = m_playerState[p];
      switch(KING_DIRECTION(state, m.m_to)) {
      case MD_NONE     : break;
      case MD_LEFT     : IF_BLOCKABLE_SET_NOKING_LEFT(     state, toInfo);   break;
      case MD_RIGHT    : IF_BLOCKABLE_SET_NOKING_RIGHT(    state, toInfo);   break;
      case MD_DOWN     : IF_BLOCKABLE_SET_NOKING_DOWN(     state, toInfo);   break;
      case MD_UP       : IF_BLOCKABLE_SET_NOKING_UP(       state, toInfo);   break;
      case MD_DOWNDIAG1: IF_BLOCKABLE_SET_NOKING_DOWNDIAG1(state, toInfo);   break;
      case MD_UPDIAG1  : IF_BLOCKABLE_SET_NOKING_UPDIAG1(  state, toInfo);   break;
      case MD_DOWNDIAG2:
      case MD_UPDIAG2  : break;
      }
    }
  }
}





void Game::updateKingDirMovingKnight(    const Move &m) {
  const FieldInfo &fromInfo = fieldInfo[m.m_from];

  forEachPlayer(p) {
    PlayerState &state = m_playerState[p];
    switch(KING_DIRECTION(state, m.m_from)) {
    case MD_NONE       :
      break;
    case MD_LEFT       : IF_BLOCKABLE_SET_KING_LEFT(       state, fromInfo); break;
    case MD_RIGHT      : IF_BLOCKABLE_SET_KING_RIGHT(      state, fromInfo) ;break;
    case MD_DOWN       : IF_BLOCKABLE_SET_KING_DOWN(       state, fromInfo); break;
    case MD_UP         : IF_BLOCKABLE_SET_KING_UP(         state, fromInfo); break;
    case MD_DOWNDIAG1  : IF_BLOCKABLE_SET_KING_DOWNDIAG1(  state, fromInfo); break;
    case MD_UPDIAG1    : IF_BLOCKABLE_SET_KING_UPDIAG1(    state, fromInfo); break;
    case MD_DOWNDIAG2  : IF_BLOCKABLE_SET_KING_DOWNDIAG2(  state, fromInfo); break;
    case MD_UPDIAG2    : IF_BLOCKABLE_SET_KING_UPDIAG2(    state, fromInfo); break;
    }
  }
  if(m.m_capturedPiece == NULL) {
    const FieldInfo &toInfo = fieldInfo[m.m_to];
    forEachPlayer(p) {
      PlayerState &state = m_playerState[p];
      switch(KING_DIRECTION(state, m.m_to)) {
      case MD_NONE     : break;
      case MD_LEFT     : IF_BLOCKABLE_SET_NOKING_LEFT(     state, toInfo);   break;
      case MD_RIGHT    : IF_BLOCKABLE_SET_NOKING_RIGHT(    state, toInfo);   break;
      case MD_DOWN     : IF_BLOCKABLE_SET_NOKING_DOWN(     state, toInfo);   break;
      case MD_UP       : IF_BLOCKABLE_SET_NOKING_UP(       state, toInfo);   break;
      case MD_DOWNDIAG1: IF_BLOCKABLE_SET_NOKING_DOWNDIAG1(state, toInfo);   break;
      case MD_UPDIAG1  : IF_BLOCKABLE_SET_NOKING_UPDIAG1(  state, toInfo);   break;
      case MD_DOWNDIAG2: IF_BLOCKABLE_SET_NOKING_DOWNDIAG2(state, toInfo);   break;
      case MD_UPDIAG2  : IF_BLOCKABLE_SET_NOKING_UPDIAG2(  state, toInfo);   break;
      }
    }
  }
}

// ----------------------------- updateKingDirKingMoving -------------------------------------------
void Game::updateKingDirKingMove(const Move &m) {
  switch(m.m_direction) {
  case MD_LEFT     : updateKingDirKingMovingLeft(     m); break;
  case MD_RIGHT    : updateKingDirKingMovingRight(    m); break;
  case MD_DOWN     : updateKingDirKingMovingDown(     m); break;
  case MD_UP       : updateKingDirKingMovingUp(       m); break;
  case MD_DOWNDIAG1: updateKingDirKingMovingDownDiag1(m); break;
  case MD_UPDIAG1  : updateKingDirKingMovingUpDiag1(  m); break;
  case MD_DOWNDIAG2: updateKingDirKingMovingDownDiag2(m); break;
  case MD_UPDIAG2  : updateKingDirKingMovingUpDiag2(  m); break;
  default          : throwInvalidArgumentException(__TFUNCTION__, _T("m.direction=%d"), m.m_direction);
  }
}

#define IF_NOCAPTURE_SET_NOKING_FROM(state,m) { if(m.m_capturedPiece == NULL) KING_DIRECTION(state, m.m_from) = MD_NONE; }

void Game::updateKingDirKingMovingLeft(      const Move &m) {
  const FieldInfo &fromInfo = fieldInfo[m.m_from];
  PlayerState     &state    = m_playerState[CURRENTENEMY];

  switch(KING_DIRECTION(state, m.m_from)) {
  case MD_NONE     : break;
  case MD_LEFT     : IF_NOCAPTURE_SET_NOKING_FROM(state,m);                break;
  case MD_RIGHT    : KING_DIRECTION(state, m.m_to) = MD_RIGHT;             break;
  case MD_DOWN     : IF_BLOCKABLE_SET_KING_DOWN(         state, fromInfo); break;
  case MD_UP       : IF_BLOCKABLE_SET_KING_UP(           state, fromInfo); break;
  case MD_DOWNDIAG1: IF_BLOCKABLE_SET_KING_DOWNDIAG1(    state, fromInfo); break;
  case MD_UPDIAG1  : IF_BLOCKABLE_SET_KING_UPDIAG1(      state, fromInfo); break;
  case MD_DOWNDIAG2: IF_BLOCKABLE_SET_KING_DOWNDIAG2(    state, fromInfo); break;
  case MD_UPDIAG2  : IF_BLOCKABLE_SET_KING_UPDIAG2(      state, fromInfo); break;
  }
  if(m.m_capturedPiece == NULL) {
    const FieldInfo &toInfo = fieldInfo[m.m_to];
    switch(KING_DIRECTION(state, m.m_to)) {
    case MD_NONE     :
    case MD_LEFT     :
    case MD_RIGHT    : break;
    case MD_DOWN     : IF_BLOCKABLE_SET_NOKING_DOWN(     state, toInfo);   break;
    case MD_UP       : IF_BLOCKABLE_SET_NOKING_UP(       state, toInfo);   break;
    case MD_DOWNDIAG1: IF_BLOCKABLE_SET_NOKING_DOWNDIAG1(state, toInfo);   break;
    case MD_UPDIAG1  : IF_BLOCKABLE_SET_NOKING_UPDIAG1(  state, toInfo);   break;
    case MD_DOWNDIAG2: IF_BLOCKABLE_SET_NOKING_DOWNDIAG2(state, toInfo);   break;
    case MD_UPDIAG2  : IF_BLOCKABLE_SET_NOKING_UPDIAG2(  state, toInfo);   break;
    }
  }
}























void Game::updateKingDirKingMovingRight(     const Move &m) {
  const FieldInfo &fromInfo = fieldInfo[m.m_from];
  PlayerState     &state    = m_playerState[CURRENTENEMY];

  switch(KING_DIRECTION(state, m.m_from)) {
  case MD_NONE     : break;
  case MD_LEFT     : KING_DIRECTION(state, m.m_to) = MD_LEFT;              break;
  case MD_RIGHT    : IF_NOCAPTURE_SET_NOKING_FROM(state, m);               break;
  case MD_DOWN     : IF_BLOCKABLE_SET_KING_DOWN(         state, fromInfo); break;
  case MD_UP       : IF_BLOCKABLE_SET_KING_UP(           state, fromInfo); break;
  case MD_DOWNDIAG1: IF_BLOCKABLE_SET_KING_DOWNDIAG1(    state, fromInfo); break;
  case MD_UPDIAG1  : IF_BLOCKABLE_SET_KING_UPDIAG1(      state, fromInfo); break;
  case MD_DOWNDIAG2: IF_BLOCKABLE_SET_KING_DOWNDIAG2(    state, fromInfo); break;
  case MD_UPDIAG2  : IF_BLOCKABLE_SET_KING_UPDIAG2(      state, fromInfo); break;
  }
  if(m.m_capturedPiece == NULL) {
    const FieldInfo &toInfo = fieldInfo[m.m_to];
    switch(KING_DIRECTION(state, m.m_to)) {
    case MD_NONE     :
    case MD_LEFT     :
    case MD_RIGHT    : break;
    case MD_DOWN     : IF_BLOCKABLE_SET_NOKING_DOWN(     state, toInfo);   break;
    case MD_UP       : IF_BLOCKABLE_SET_NOKING_UP(       state, toInfo);   break;
    case MD_DOWNDIAG1: IF_BLOCKABLE_SET_NOKING_DOWNDIAG1(state, toInfo);   break;
    case MD_UPDIAG1  : IF_BLOCKABLE_SET_NOKING_UPDIAG1(  state, toInfo);   break;
    case MD_DOWNDIAG2: IF_BLOCKABLE_SET_NOKING_DOWNDIAG2(state, toInfo);   break;
    case MD_UPDIAG2  : IF_BLOCKABLE_SET_NOKING_UPDIAG2(  state, toInfo);   break;
    }
  }
}























void Game::updateKingDirKingMovingDown(      const Move &m) {
  const FieldInfo &fromInfo = fieldInfo[m.m_from];
  PlayerState     &state    = m_playerState[CURRENTENEMY];

  switch(KING_DIRECTION(state, m.m_from)) {
  case MD_NONE     : break;
  case MD_LEFT     : IF_BLOCKABLE_SET_KING_LEFT(         state, fromInfo); break;
  case MD_RIGHT    : IF_BLOCKABLE_SET_KING_RIGHT(        state, fromInfo); break;
  case MD_DOWN     : IF_NOCAPTURE_SET_NOKING_FROM(state,m);                break;
  case MD_UP       : KING_DIRECTION(state, m.m_to) = MD_UP;                break;
  case MD_DOWNDIAG1: IF_BLOCKABLE_SET_KING_DOWNDIAG1(    state, fromInfo); break;
  case MD_UPDIAG1  : IF_BLOCKABLE_SET_KING_UPDIAG1(      state, fromInfo); break;
  case MD_DOWNDIAG2: IF_BLOCKABLE_SET_KING_DOWNDIAG2(    state, fromInfo); break;
  case MD_UPDIAG2  : IF_BLOCKABLE_SET_KING_UPDIAG2(      state, fromInfo); break;
  }
  if(m.m_capturedPiece == NULL) {
    const FieldInfo &toInfo = fieldInfo[m.m_to];
    switch(KING_DIRECTION(state, m.m_to)) {
    case MD_NONE     : break;
    case MD_LEFT     : IF_BLOCKABLE_SET_NOKING_LEFT(     state, toInfo);   break;
    case MD_RIGHT    : IF_BLOCKABLE_SET_NOKING_RIGHT(    state, toInfo);   break;
    case MD_DOWN     :
    case MD_UP       : break;
    case MD_DOWNDIAG1: IF_BLOCKABLE_SET_NOKING_DOWNDIAG1(state, toInfo);   break;
    case MD_UPDIAG1  : IF_BLOCKABLE_SET_NOKING_UPDIAG1(  state, toInfo);   break;
    case MD_DOWNDIAG2: IF_BLOCKABLE_SET_NOKING_DOWNDIAG2(state, toInfo);   break;
    case MD_UPDIAG2  : IF_BLOCKABLE_SET_NOKING_UPDIAG2(  state, toInfo);   break;
    }
  }
}























void Game::updateKingDirKingMovingUp(        const Move &m) {
  const FieldInfo &fromInfo = fieldInfo[m.m_from];
  PlayerState     &state    = m_playerState[CURRENTENEMY];

  switch(KING_DIRECTION(state, m.m_from)) {
  case MD_NONE     : break;
  case MD_LEFT     : IF_BLOCKABLE_SET_KING_LEFT(         state, fromInfo); break;
  case MD_RIGHT    : IF_BLOCKABLE_SET_KING_RIGHT(        state, fromInfo); break;
  case MD_DOWN     : KING_DIRECTION(state, m.m_to) = MD_DOWN;              break;
  case MD_UP       : IF_NOCAPTURE_SET_NOKING_FROM(state, m);               break;
  case MD_DOWNDIAG1: IF_BLOCKABLE_SET_KING_DOWNDIAG1(    state, fromInfo); break;
  case MD_UPDIAG1  : IF_BLOCKABLE_SET_KING_UPDIAG1(      state, fromInfo); break;
  case MD_DOWNDIAG2: IF_BLOCKABLE_SET_KING_DOWNDIAG2(    state, fromInfo); break;
  case MD_UPDIAG2  : IF_BLOCKABLE_SET_KING_UPDIAG2(      state, fromInfo); break;
  }
  if(m.m_capturedPiece == NULL) {
    const FieldInfo &toInfo = fieldInfo[m.m_to];
    switch(KING_DIRECTION(state, m.m_to)) {
    case MD_NONE     : break;
    case MD_LEFT     : IF_BLOCKABLE_SET_NOKING_LEFT(     state, toInfo);   break;
    case MD_RIGHT    : IF_BLOCKABLE_SET_NOKING_RIGHT(    state, toInfo);   break;
    case MD_DOWN     :
    case MD_UP       : break;
    case MD_DOWNDIAG1: IF_BLOCKABLE_SET_NOKING_DOWNDIAG1(state, toInfo);   break;
    case MD_UPDIAG1  : IF_BLOCKABLE_SET_NOKING_UPDIAG1(  state, toInfo);   break;
    case MD_DOWNDIAG2: IF_BLOCKABLE_SET_NOKING_DOWNDIAG2(state, toInfo);   break;
    case MD_UPDIAG2  : IF_BLOCKABLE_SET_NOKING_UPDIAG2(  state, toInfo);   break;
    }
  }
}























void Game::updateKingDirKingMovingDownDiag1( const Move &m) {
  const FieldInfo &fromInfo = fieldInfo[m.m_from];
  PlayerState     &state    = m_playerState[CURRENTENEMY];

  switch(KING_DIRECTION(state, m.m_from)) {
  case MD_NONE     : break;
  case MD_LEFT     : IF_BLOCKABLE_SET_KING_LEFT(         state, fromInfo); break;
  case MD_RIGHT    : IF_BLOCKABLE_SET_KING_RIGHT(        state, fromInfo); break;
  case MD_DOWN     : IF_BLOCKABLE_SET_KING_DOWN(         state, fromInfo); break;
  case MD_UP       : IF_BLOCKABLE_SET_KING_UP(           state, fromInfo); break;
  case MD_DOWNDIAG1: IF_NOCAPTURE_SET_NOKING_FROM(state, m);               break;
  case MD_UPDIAG1  : KING_DIRECTION(state, m.m_to) = MD_UPDIAG1;           break;
  case MD_DOWNDIAG2: IF_BLOCKABLE_SET_KING_DOWNDIAG2(    state, fromInfo); break;
  case MD_UPDIAG2  : IF_BLOCKABLE_SET_KING_UPDIAG2(      state, fromInfo); break;
  }
  if(m.m_capturedPiece == NULL) {
    const FieldInfo &toInfo = fieldInfo[m.m_to];
    switch(KING_DIRECTION(state, m.m_to)) {
    case MD_NONE     : break;
    case MD_LEFT     : IF_BLOCKABLE_SET_NOKING_LEFT(     state, toInfo);   break;
    case MD_RIGHT    : IF_BLOCKABLE_SET_NOKING_RIGHT(    state, toInfo);   break;
    case MD_DOWN     : IF_BLOCKABLE_SET_NOKING_DOWN(     state, toInfo);   break;
    case MD_UP       : IF_BLOCKABLE_SET_NOKING_UP(       state, toInfo);   break;
    case MD_DOWNDIAG1:
    case MD_UPDIAG1  : break;
    case MD_DOWNDIAG2: IF_BLOCKABLE_SET_NOKING_DOWNDIAG2(state, toInfo);   break;
    case MD_UPDIAG2  : IF_BLOCKABLE_SET_NOKING_UPDIAG2(  state, toInfo);   break;
    }
  }
}























void Game::updateKingDirKingMovingUpDiag1(   const Move &m) {
  const FieldInfo &fromInfo = fieldInfo[m.m_from];
  PlayerState     &state    = m_playerState[CURRENTENEMY];

  switch(KING_DIRECTION(state, m.m_from)) {
  case MD_NONE     : break;
  case MD_LEFT     : IF_BLOCKABLE_SET_KING_LEFT(         state, fromInfo); break;
  case MD_RIGHT    : IF_BLOCKABLE_SET_KING_RIGHT(        state, fromInfo); break;
  case MD_DOWN     : IF_BLOCKABLE_SET_KING_DOWN(         state, fromInfo); break;
  case MD_UP       : IF_BLOCKABLE_SET_KING_UP(           state, fromInfo); break;
  case MD_DOWNDIAG1: KING_DIRECTION(state, m.m_to) = MD_DOWNDIAG1;         break;
  case MD_UPDIAG1  : IF_NOCAPTURE_SET_NOKING_FROM(state, m);               break;
  case MD_DOWNDIAG2: IF_BLOCKABLE_SET_KING_DOWNDIAG2(    state, fromInfo); break;
  case MD_UPDIAG2  : IF_BLOCKABLE_SET_KING_UPDIAG2(      state, fromInfo); break;
  }
  if(m.m_capturedPiece == NULL) {
    const FieldInfo &toInfo = fieldInfo[m.m_to];
    switch(KING_DIRECTION(state, m.m_to)) {
    case MD_NONE     : break;
    case MD_LEFT     : IF_BLOCKABLE_SET_NOKING_LEFT(     state, toInfo);   break;
    case MD_RIGHT    : IF_BLOCKABLE_SET_NOKING_RIGHT(    state, toInfo);   break;
    case MD_DOWN     : IF_BLOCKABLE_SET_NOKING_DOWN(     state, toInfo);   break;
    case MD_UP       : IF_BLOCKABLE_SET_NOKING_UP(       state, toInfo);   break;
    case MD_DOWNDIAG1:
    case MD_UPDIAG1  : break;
    case MD_DOWNDIAG2: IF_BLOCKABLE_SET_NOKING_DOWNDIAG2(state, toInfo);   break;
    case MD_UPDIAG2  : IF_BLOCKABLE_SET_NOKING_UPDIAG2(  state, toInfo);   break;
    }
  }
}























void Game::updateKingDirKingMovingDownDiag2( const Move &m) {
  const FieldInfo &fromInfo = fieldInfo[m.m_from];
  PlayerState     &state    = m_playerState[CURRENTENEMY];

  switch(KING_DIRECTION(state, m.m_from)) {
  case MD_NONE     : break;
  case MD_LEFT     : IF_BLOCKABLE_SET_KING_LEFT(         state, fromInfo); break;
  case MD_RIGHT    : IF_BLOCKABLE_SET_KING_RIGHT(        state, fromInfo); break;
  case MD_DOWN     : IF_BLOCKABLE_SET_KING_DOWN(         state, fromInfo); break;
  case MD_UP       : IF_BLOCKABLE_SET_KING_UP(           state, fromInfo); break;
  case MD_DOWNDIAG1: IF_BLOCKABLE_SET_KING_DOWNDIAG1(    state, fromInfo); break;
  case MD_UPDIAG1  : IF_BLOCKABLE_SET_KING_UPDIAG1(      state, fromInfo); break;
  case MD_DOWNDIAG2: IF_NOCAPTURE_SET_NOKING_FROM(state, m);               break;
  case MD_UPDIAG2  : KING_DIRECTION(state, m.m_to) = MD_UPDIAG2;           break;
  }
  if(m.m_capturedPiece == NULL) {
    const FieldInfo &toInfo = fieldInfo[m.m_to];
    switch(KING_DIRECTION(state, m.m_to)) {
    case MD_NONE     : break;
    case MD_LEFT     : IF_BLOCKABLE_SET_NOKING_LEFT(     state, toInfo);   break;
    case MD_RIGHT    : IF_BLOCKABLE_SET_NOKING_RIGHT(    state, toInfo);   break;
    case MD_DOWN     : IF_BLOCKABLE_SET_NOKING_DOWN(     state, toInfo);   break;
    case MD_UP       : IF_BLOCKABLE_SET_NOKING_UP(       state, toInfo);   break;
    case MD_DOWNDIAG1: IF_BLOCKABLE_SET_NOKING_DOWNDIAG1(state, toInfo);   break;
    case MD_UPDIAG1  : IF_BLOCKABLE_SET_NOKING_UPDIAG1(  state, toInfo);   break;
    case MD_DOWNDIAG2:
    case MD_UPDIAG2  : break;
    }
  }
}























void Game::updateKingDirKingMovingUpDiag2(   const Move &m) {
  const FieldInfo &fromInfo = fieldInfo[m.m_from];
  PlayerState     &state    = m_playerState[CURRENTENEMY];

  switch(KING_DIRECTION(state, m.m_from)) {
  case MD_NONE     : break;
  case MD_LEFT     : IF_BLOCKABLE_SET_KING_LEFT(         state, fromInfo); break;
  case MD_RIGHT    : IF_BLOCKABLE_SET_KING_RIGHT(        state, fromInfo); break;
  case MD_DOWN     : IF_BLOCKABLE_SET_KING_DOWN(         state, fromInfo); break;
  case MD_UP       : IF_BLOCKABLE_SET_KING_UP(           state, fromInfo); break;
  case MD_DOWNDIAG1: IF_BLOCKABLE_SET_KING_DOWNDIAG1(    state, fromInfo); break;
  case MD_UPDIAG1  : IF_BLOCKABLE_SET_KING_UPDIAG1(      state, fromInfo); break;
  case MD_DOWNDIAG2: KING_DIRECTION(state, m.m_to) = MD_DOWNDIAG2;         break;
  case MD_UPDIAG2  : IF_NOCAPTURE_SET_NOKING_FROM(state, m);               break;
  }
  if(m.m_capturedPiece == NULL) {
    const FieldInfo &toInfo = fieldInfo[m.m_to];
    switch(KING_DIRECTION(state, m.m_to)) {
    case MD_NONE     : break;
    case MD_LEFT     : IF_BLOCKABLE_SET_NOKING_LEFT(     state, toInfo);   break;
    case MD_RIGHT    : IF_BLOCKABLE_SET_NOKING_RIGHT(    state, toInfo);   break;
    case MD_DOWN     : IF_BLOCKABLE_SET_NOKING_DOWN(     state, toInfo);   break;
    case MD_UP       : IF_BLOCKABLE_SET_NOKING_UP(       state, toInfo);   break;
    case MD_DOWNDIAG1: IF_BLOCKABLE_SET_NOKING_DOWNDIAG1(state, toInfo);   break;
    case MD_UPDIAG1  : IF_BLOCKABLE_SET_NOKING_UPDIAG1(  state, toInfo);   break;
    case MD_DOWNDIAG2:
    case MD_UPDIAG2  : break;
    }
  }
}




















// ----------------------------- updateKingDirKingMoving -------------------------------------------

// Black pawn captures EP going down and left.
void Game::updateKingDirEPDownDiag1(const EnPassantMove &m) {
  forEachPlayer(p) {
    PlayerState &state = m_playerState[p];

    switch(KING_DIRECTION(state, m.m_move.m_from)) {
    case MD_LEFT     : throwException(_T("updateKingDirEPDownDiag1:King is left for %s. Should be a pawn"), getFieldName(m.m_move.m_from));
    case MD_RIGHT    : setKingRight(                     state, m.m_fromInfo        );                break; // always blockable
    case MD_DOWN     : setKingDown(                      state, m.m_fromInfo        );                break; // always blockable
    case MD_UP       : setKingUp(                        state, m.m_fromInfo        );                break; // always blockable
    case MD_DOWNDIAG1: KING_DIRECTION(                   state, m.m_move.m_from     ) = MD_NONE;      break;
    case MD_UPDIAG1  : KING_DIRECTION(                   state, m.m_move.m_to       ) = MD_UPDIAG1;   break;
    case MD_DOWNDIAG2: setKingDownDiag2(                 state, m.m_fromInfo        );                break; // always blockable
    case MD_UPDIAG2  : IF_BLOCKABLE_SET_KING_UPDIAG2(    state, m.m_fromInfo        );                break;
    default          :                                                                                break;
    }

    switch(KING_DIRECTION(state, m.m_move.m_to)) {
    case MD_LEFT     : SET_NOKING_LEFT(                  state, m.m_toInfo          );                break; // always blockable
    case MD_RIGHT    : IF_BLOCKABLE_SET_NOKING_RIGHT(    state, m.m_toInfo          );                break;
    case MD_DOWN     : KING_DIRECTION(                   state, m.m_capturedPosition) = MD_NONE;      break;
    case MD_UP       : throwException(_T("updateKingDirEPDownDiag1:King is above %s. Should be a pawn"), getFieldName(m.m_move.m_to));
    case MD_DOWNDIAG1: KING_DIRECTION(                   state, m.m_move.m_from     ) = MD_NONE;      break;
    case MD_UPDIAG1  : KING_DIRECTION(                   state, m.m_move.m_to       ) = MD_UPDIAG1;   break;
    case MD_DOWNDIAG2: IF_BLOCKABLE_SET_NOKING_DOWNDIAG2(state, m.m_toInfo          );                break;
    case MD_UPDIAG2  : IF_BLOCKABLE_SET_NOKING_UPDIAG2(  state, m.m_toInfo          );                break;
    default          :                                                                                break;
    }

    switch(KING_DIRECTION(state, m.m_capturedPosition)) {
    case MD_LEFT     : setKingLeft(                      state, m.m_capturedInfo    );                break; // always blockable
    case MD_RIGHT    :                                                                                break; // handled by first switch
    case MD_DOWN     : KING_DIRECTION(                   state, m.m_capturedPosition) = MD_NONE;      break;
    case MD_UP       : KING_DIRECTION(                   state, m.m_move.m_to       ) = MD_UP;        break;
    case MD_DOWNDIAG1: IF_BLOCKABLE_SET_KING_DOWNDIAG1(  state, m.m_capturedInfo    );                break;
    case MD_UPDIAG1  : IF_BLOCKABLE_SET_KING_UPDIAG1(    state, m.m_capturedInfo    );                break;
    case MD_DOWNDIAG2: IF_BLOCKABLE_SET_KING_DOWNDIAG2(  state, m.m_capturedInfo    );                break;
    case MD_UPDIAG2  : setKingUpDiag2(                   state, m.m_capturedInfo    );                break; // always blockable
    default          :                                                                                break;
    }
  }
}











// Black pawn captures EP going down and right
void Game::updateKingDirEPDownDiag2(const EnPassantMove &m) {
  forEachPlayer(p) {
    PlayerState &state = m_playerState[p];

    switch(KING_DIRECTION(state, m.m_move.m_from)) {
    case MD_LEFT     : setKingLeft(                      state, m.m_fromInfo        );                break; // always blockable
    case MD_RIGHT    : throwException(_T("updateKingDirEPDownDiag2:King is right for %s. Should be a pawn"), getFieldName(m.m_move.m_from));
    case MD_DOWN     : setKingDown(                      state, m.m_fromInfo        );                break; // always blockable
    case MD_UP       : setKingUp(                        state, m.m_fromInfo        );                break; // always blockable
    case MD_DOWNDIAG1: setKingDownDiag1(                 state, m.m_fromInfo        );                break; // always blockable
    case MD_UPDIAG1  : IF_BLOCKABLE_SET_KING_UPDIAG1(    state, m.m_fromInfo        );                break;
    case MD_DOWNDIAG2: KING_DIRECTION(                   state, m.m_move.m_from     ) = MD_NONE;      break;
    case MD_UPDIAG2  : KING_DIRECTION(                   state, m.m_move.m_to       ) = MD_UPDIAG2;   break;
    default          :                                                                                break;
    }

    switch(KING_DIRECTION(state, m.m_move.m_to)) {
    case MD_LEFT     : IF_BLOCKABLE_SET_NOKING_LEFT(     state, m.m_toInfo          );                break;
    case MD_RIGHT    : SET_NOKING_RIGHT(                 state, m.m_toInfo          );                break; // always blockable
    case MD_DOWN     : KING_DIRECTION(                   state, m.m_capturedPosition) = MD_NONE;      break;
    case MD_UP       : throwException(_T("updateKingDirEPDownDiag2:King is above %s. Should be a pawn"), getFieldName(m.m_move.m_to));
    case MD_DOWNDIAG1: IF_BLOCKABLE_SET_NOKING_DOWNDIAG1(state, m.m_toInfo          );                break;
    case MD_UPDIAG1  : IF_BLOCKABLE_SET_NOKING_UPDIAG1(  state, m.m_toInfo          );                break;
    case MD_DOWNDIAG2: KING_DIRECTION(                   state, m.m_move.m_from     ) = MD_NONE;      break;
    case MD_UPDIAG2  : KING_DIRECTION(                   state, m.m_move.m_to       ) = MD_UPDIAG2;   break;
    default          :                                                                                break;
    }

    switch(KING_DIRECTION(state, m.m_capturedPosition)) {
    case MD_LEFT     :                                                                                break; // handled by first switch
    case MD_RIGHT    : setKingRight(                     state, m.m_capturedInfo    );                break; // always blockable
    case MD_DOWN     : KING_DIRECTION(                   state, m.m_capturedPosition) = MD_NONE;      break;
    case MD_UP       : KING_DIRECTION(                   state, m.m_move.m_to       ) = MD_UP;        break;
    case MD_DOWNDIAG1: IF_BLOCKABLE_SET_KING_DOWNDIAG1(  state, m.m_capturedInfo    );                break;
    case MD_UPDIAG1  : setKingUpDiag1(                   state, m.m_capturedInfo    );                break; // always blockable
    case MD_DOWNDIAG2: IF_BLOCKABLE_SET_KING_DOWNDIAG2(  state, m.m_capturedInfo    );                break;
    case MD_UPDIAG2  : IF_BLOCKABLE_SET_KING_UPDIAG2(    state, m.m_capturedInfo    );                break;
    default          :                                                                                break;
    }
  }
}











// White pawn captures EP going up and right.
void Game::updateKingDirEPUpDiag1(  const EnPassantMove &m) {
  forEachPlayer(p) {
    PlayerState &state = m_playerState[p];

    switch(KING_DIRECTION(state, m.m_move.m_from)) {
    case MD_LEFT     : setKingLeft(                      state, m.m_fromInfo        );                break; // always blockable
    case MD_RIGHT    : throwException(_T("updateKingDirEPUpDiag1:King is right for %s. Should be a pawn"), getFieldName(m.m_move.m_from));
    case MD_DOWN     : setKingDown(                      state, m.m_fromInfo        );                break; // always blockable
    case MD_UP       : setKingUp(                        state, m.m_fromInfo        );                break; // always blockable
    case MD_DOWNDIAG1: KING_DIRECTION(                   state, m.m_move.m_to       ) = MD_DOWNDIAG1; break;
    case MD_UPDIAG1  : KING_DIRECTION(                   state, m.m_move.m_from     ) = MD_NONE;      break;
    case MD_DOWNDIAG2: IF_BLOCKABLE_SET_KING_DOWNDIAG2(  state, m.m_fromInfo        );                break;
    case MD_UPDIAG2  : setKingUpDiag2(                   state, m.m_fromInfo        );                break; // always blockable
    default          :                                                                                break; // always blockable
    }

    switch(KING_DIRECTION(state, m.m_move.m_to)) {
    case MD_LEFT     : IF_BLOCKABLE_SET_NOKING_LEFT(     state, m.m_toInfo          );                break;
    case MD_RIGHT    : SET_NOKING_RIGHT(                 state, m.m_toInfo          );                break; // always blockable
    case MD_DOWN     : throwException(_T("updateKingDirEPUpDiag1:King is belove %s. Should be a pawn"), getFieldName(m.m_move.m_to));
    case MD_UP       : KING_DIRECTION(                   state, m.m_capturedPosition) = MD_NONE;      break;
    case MD_DOWNDIAG1: KING_DIRECTION(                   state, m.m_move.m_to       ) = MD_DOWNDIAG1; break;
    case MD_UPDIAG1  : KING_DIRECTION(                   state, m.m_move.m_from     ) = MD_NONE;      break;
    case MD_DOWNDIAG2: IF_BLOCKABLE_SET_NOKING_DOWNDIAG2(state, m.m_toInfo          );                break;
    case MD_UPDIAG2  : IF_BLOCKABLE_SET_NOKING_UPDIAG2(  state, m.m_toInfo          );                break;
    default          :                                                                                break;
    }

    switch(KING_DIRECTION(state, m.m_capturedPosition)) {
    case MD_LEFT     :                                                                                break; // handled by first switch
    case MD_RIGHT    : setKingRight(                     state, m.m_capturedInfo    );                break; // always blockable
    case MD_DOWN     : KING_DIRECTION(                   state, m.m_move.m_to       ) = MD_DOWN;      break;
    case MD_UP       : KING_DIRECTION(                   state, m.m_capturedPosition) = MD_NONE;      break;
    case MD_DOWNDIAG1: IF_BLOCKABLE_SET_KING_DOWNDIAG1(  state, m.m_capturedInfo    );                break;
    case MD_UPDIAG1  : IF_BLOCKABLE_SET_KING_UPDIAG1(    state, m.m_capturedInfo    );                break;
    case MD_DOWNDIAG2: setKingDownDiag2(                 state, m.m_capturedInfo    );                break; // always blockable
    case MD_UPDIAG2  : IF_BLOCKABLE_SET_KING_UPDIAG2(    state, m.m_capturedInfo    );                break;
    default          :                                                                                break;
    }
  }
}











// White pawn captures EP going up and left
void Game::updateKingDirEPUpDiag2(  const EnPassantMove &m) {
  forEachPlayer(p) {
    PlayerState &state = m_playerState[p];

    switch(KING_DIRECTION(state, m.m_move.m_from)) {
    case MD_LEFT     : throwException(_T("updateKingDirEPUpDiag2:King is left for %s. Should be a pawn"), getFieldName(m.m_move.m_from));
    case MD_RIGHT    : setKingRight(                     state, m.m_fromInfo        );                break; // always blockable
    case MD_DOWN     : setKingDown(                      state, m.m_fromInfo        );                break; // always blockable
    case MD_UP       : setKingUp(                        state, m.m_fromInfo        );                break; // always blockable
    case MD_DOWNDIAG1: IF_BLOCKABLE_SET_KING_DOWNDIAG1(  state, m.m_fromInfo        );                break;
    case MD_UPDIAG1  : setKingUpDiag1(                   state, m.m_fromInfo        );                break; // always blockable
    case MD_DOWNDIAG2: KING_DIRECTION(                   state, m.m_move.m_to       ) = MD_DOWNDIAG2; break;
    case MD_UPDIAG2  : KING_DIRECTION(                   state, m.m_move.m_from     ) = MD_NONE;      break;
    default          :                                                                                break;
    }

    switch(KING_DIRECTION(state, m.m_move.m_to)) {
    case MD_LEFT     : SET_NOKING_LEFT(                  state, m.m_toInfo          );                break; // always blockable
    case MD_RIGHT    : IF_BLOCKABLE_SET_NOKING_RIGHT(    state, m.m_toInfo          );                break;
    case MD_DOWN     : throwException(_T("updateKingDirEPUpDiag2:King is belove %s. Should be a pawn"), getFieldName(m.m_move.m_to));
    case MD_UP       : KING_DIRECTION(                   state, m.m_capturedPosition) = MD_NONE;      break;
    case MD_DOWNDIAG1: IF_BLOCKABLE_SET_NOKING_DOWNDIAG1(state, m.m_toInfo          );                break;
    case MD_UPDIAG1  : IF_BLOCKABLE_SET_NOKING_UPDIAG1(  state, m.m_toInfo          );                break;
    case MD_DOWNDIAG2: KING_DIRECTION(                   state, m.m_move.m_to       ) = MD_DOWNDIAG2; break;
    case MD_UPDIAG2  : KING_DIRECTION(                   state, m.m_move.m_from     ) = MD_NONE;      break;
    default          :                                                                                break;
    }

    switch(KING_DIRECTION(state, m.m_capturedPosition)) {
    case MD_LEFT     : setKingLeft(                      state, m.m_capturedInfo    );                break; // always blockable
    case MD_RIGHT    :                                                                                break; // handled by first switch
    case MD_DOWN     : KING_DIRECTION(                   state, m.m_move.m_to       ) = MD_DOWN;      break;
    case MD_UP       : KING_DIRECTION(                   state, m.m_capturedPosition) = MD_NONE;      break;
    case MD_DOWNDIAG1: setKingDownDiag1(                 state, m.m_capturedInfo    );                break; // always blockable
    case MD_UPDIAG1  : IF_BLOCKABLE_SET_KING_UPDIAG1(    state, m.m_capturedInfo    );                break;
    case MD_DOWNDIAG2: IF_BLOCKABLE_SET_KING_DOWNDIAG2(  state, m.m_capturedInfo    );                break;
    case MD_UPDIAG2  : IF_BLOCKABLE_SET_KING_UPDIAG2(    state, m.m_capturedInfo    );                break;
    default          :                                                                                break;
    }
  }
}

// ------------------------------------------------------------------------

void Game::setKingLeft(PlayerState &state, const FieldInfo &fieldInfo) {
  PositionArray positions = fieldInfo.m_rowLine.m_upper;
  const Piece  *piece;
  for(int count = positions[0], pos = positions[1]; count--; pos += DPOS_RIGHT) {
    KING_DIRECTION(state, pos) = MD_LEFT;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState == &state && GET_LDA_LEFT(piece->m_enemyState, pos)) {
        piece->m_pinnedState = PINNED_TO_ROW;
      }
      return;
    }
  }
}

void Game::setKingRight(PlayerState &state, const FieldInfo &fieldInfo) {
  PositionArray positions = fieldInfo.m_rowLine.m_lower;
  const Piece  *piece;
  for(int count = positions[0], pos = positions[1]; count--; pos += DPOS_LEFT) {
    KING_DIRECTION(state, pos) = MD_RIGHT;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState == &state && GET_LDA_RIGHT(piece->m_enemyState, pos)) {
        piece->m_pinnedState = PINNED_TO_ROW;
      }
      return;
    }
  }
}

void Game::setKingDown(PlayerState &state, const FieldInfo &fieldInfo) {
  PositionArray positions = fieldInfo.m_colLine.m_upper;
  const Piece  *piece;
  for(int count = positions[0], pos = positions[1]; count--; pos += DPOS_UP) {
    KING_DIRECTION(state, pos) = MD_DOWN;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState == &state && GET_LDA_DOWN(piece->m_enemyState, pos)) {
        piece->m_pinnedState = PINNED_TO_COL;
      }
      return;
    }
  }
}

void Game::setKingUp(PlayerState &state, const FieldInfo &fieldInfo) {
  PositionArray positions = fieldInfo.m_colLine.m_lower;
  const Piece  *piece;
  for(int count = positions[0], pos = positions[1]; count--; pos += DPOS_DOWN) {
    KING_DIRECTION(state, pos) = MD_UP;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState == &state && GET_LDA_UP(piece->m_enemyState, pos)) {
        piece->m_pinnedState = PINNED_TO_COL;
      }
      return;
    }
  }
}

void Game::setKingDownDiag1(PlayerState &state, const FieldInfo &fieldInfo) {
  PositionArray positions = fieldInfo.m_diag1Line.m_upper;
  const Piece  *piece;
  for(int count = positions[0], pos = positions[1]; count--; pos += DPOS_UPDIAG1) {
    KING_DIRECTION(state, pos) = MD_DOWNDIAG1;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState == &state && GET_LDA_DOWNDIAG1(piece->m_enemyState, pos)) {
        piece->m_pinnedState = PINNED_TO_DIAG1;
      }
      return;
    }
  }
}

void Game::setKingUpDiag1(PlayerState &state, const FieldInfo &fieldInfo) {
  PositionArray positions = fieldInfo.m_diag1Line.m_lower;
  Piece        *piece;
  for(int count = positions[0], pos = positions[1]; count--; pos += DPOS_DOWNDIAG1) {
    KING_DIRECTION(state, pos) = MD_UPDIAG1;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState == &state && GET_LDA_UPDIAG1(piece->m_enemyState, pos)) {
        piece->m_pinnedState = PINNED_TO_DIAG1;
      }
      return;
    }
  }
}

void Game::setKingDownDiag2(PlayerState &state, const FieldInfo &fieldInfo) {
  PositionArray positions = fieldInfo.m_diag2Line.m_upper;
  const Piece  *piece;
  for(int count = positions[0], pos = positions[1]; count--; pos += DPOS_UPDIAG2) {
    KING_DIRECTION(state, pos) = MD_DOWNDIAG2;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState == &state && GET_LDA_DOWNDIAG2(piece->m_enemyState, pos)) {
        piece->m_pinnedState = PINNED_TO_DIAG2;
      }
      return;
    }
  }
}

void Game::setKingUpDiag2(PlayerState &state, const FieldInfo &fieldInfo) {
  PositionArray positions = fieldInfo.m_diag2Line.m_lower;
  const Piece  *piece;
  for(int count = positions[0], pos = positions[1]; count--; pos += DPOS_DOWNDIAG2) {
    KING_DIRECTION(state, pos) = MD_UPDIAG2;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState == &state && GET_LDA_UPDIAG2(piece->m_enemyState, pos)) {
        piece->m_pinnedState = PINNED_TO_DIAG2;
      }
      return;
    }
  }
}

PinnedState Game::getPinnedState(int pos) {
  switch(KING_DIRECTION(m_playerState[PLAYERINTURN], pos)) {
  case MD_NONE     : return NOT_PINNED;
  case MD_LEFT     : return GET_LDA_LEFT(     m_playerState[CURRENTENEMY], pos) ? PINNED_TO_ROW   : NOT_PINNED;
  case MD_RIGHT    : return GET_LDA_RIGHT(    m_playerState[CURRENTENEMY], pos) ? PINNED_TO_ROW   : NOT_PINNED;
  case MD_DOWN     : return GET_LDA_DOWN(     m_playerState[CURRENTENEMY], pos) ? PINNED_TO_COL   : NOT_PINNED;
  case MD_UP       : return GET_LDA_UP(       m_playerState[CURRENTENEMY], pos) ? PINNED_TO_COL   : NOT_PINNED;
  case MD_DOWNDIAG1: return GET_LDA_DOWNDIAG1(m_playerState[CURRENTENEMY], pos) ? PINNED_TO_DIAG1 : NOT_PINNED;
  case MD_UPDIAG1  : return GET_LDA_UPDIAG1(  m_playerState[CURRENTENEMY], pos) ? PINNED_TO_DIAG1 : NOT_PINNED;
  case MD_DOWNDIAG2: return GET_LDA_DOWNDIAG2(m_playerState[CURRENTENEMY], pos) ? PINNED_TO_DIAG2 : NOT_PINNED;
  case MD_UPDIAG2  : return GET_LDA_UPDIAG2(  m_playerState[CURRENTENEMY], pos) ? PINNED_TO_DIAG2 : NOT_PINNED;
  default          : throwException(_T("getPinnedState:Invalid kingdirection %d at position %s")
                                    ,KING_DIRECTION(m_playerState[PLAYERINTURN], pos)
                                    ,getFieldName(pos));
                     return NOT_PINNED;
  }
}
