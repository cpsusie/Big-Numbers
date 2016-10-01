#include "stdafx.h"

// ---------------------------------------- block --------------------------------------

void Game::blockRow(const FieldInfo &fieldInfo) {
  PlayerState &whiteState = m_playerState[WHITEPLAYER];
  PlayerState &blackState = m_playerState[BLACKPLAYER];

  if(GET_LDA_LEFT(            whiteState, fieldInfo.m_pos)) {
    CLR_LDA_LEFT(             whiteState, fieldInfo);
  } else if(GET_LDA_LEFT(     blackState, fieldInfo.m_pos)) {
    CLR_LDA_LEFT(             blackState, fieldInfo);
  }
  if(GET_LDA_RIGHT(           whiteState, fieldInfo.m_pos)) {
    CLR_LDA_RIGHT(            whiteState, fieldInfo);
  } else if(GET_LDA_RIGHT(    blackState, fieldInfo.m_pos)) {
    CLR_LDA_RIGHT(            blackState, fieldInfo);
  }
}

void Game::blockColumn(const FieldInfo &fieldInfo) {
  PlayerState &whiteState = m_playerState[WHITEPLAYER];
  PlayerState &blackState = m_playerState[BLACKPLAYER];

  if(GET_LDA_DOWN(            whiteState, fieldInfo.m_pos)) {
    CLR_LDA_DOWN(             whiteState, fieldInfo);
  } else if(GET_LDA_DOWN(     blackState, fieldInfo.m_pos)) {
    CLR_LDA_DOWN(             blackState, fieldInfo);
  }
  if(GET_LDA_UP(              whiteState, fieldInfo.m_pos)) {
    CLR_LDA_UP(               whiteState, fieldInfo);
  } else if(GET_LDA_UP(       blackState, fieldInfo.m_pos)) {
    CLR_LDA_UP(               blackState, fieldInfo);
  }
}

void Game::blockDiag1(const FieldInfo &fieldInfo) {
  PlayerState &whiteState = m_playerState[WHITEPLAYER];
  PlayerState &blackState = m_playerState[BLACKPLAYER];

  if(GET_LDA_DOWNDIAG1(       whiteState, fieldInfo.m_pos)) {
    CLR_LDA_DOWNDIAG1(        whiteState, fieldInfo);
  } else if(GET_LDA_DOWNDIAG1(blackState, fieldInfo.m_pos)) {
    CLR_LDA_DOWNDIAG1(        blackState, fieldInfo);
  }
  if(GET_LDA_UPDIAG1(         whiteState, fieldInfo.m_pos)) {
    CLR_LDA_UPDIAG1(          whiteState, fieldInfo);
  } else if(GET_LDA_UPDIAG1(  blackState, fieldInfo.m_pos)) {
    CLR_LDA_UPDIAG1(          blackState, fieldInfo);
  }
}

void Game::blockDiag2(const FieldInfo &fieldInfo) {
  PlayerState &whiteState = m_playerState[WHITEPLAYER];
  PlayerState &blackState = m_playerState[BLACKPLAYER];

  if(GET_LDA_DOWNDIAG2(       whiteState, fieldInfo.m_pos)) {
    CLR_LDA_DOWNDIAG2(        whiteState, fieldInfo);
  } else if(GET_LDA_DOWNDIAG2(blackState, fieldInfo.m_pos)) {
    CLR_LDA_DOWNDIAG2(        blackState, fieldInfo);
  }
  if(GET_LDA_UPDIAG2(         whiteState, fieldInfo.m_pos)) {
    CLR_LDA_UPDIAG2(          whiteState, fieldInfo);
  } else if(GET_LDA_UPDIAG2(  blackState, fieldInfo.m_pos)) {
    CLR_LDA_UPDIAG2(          blackState, fieldInfo);
  }
}

void Game::blockDiag12(const FieldInfo &fieldInfo) {
  PlayerState &whiteState = m_playerState[WHITEPLAYER];
  PlayerState &blackState = m_playerState[BLACKPLAYER];

  if(GET_LDA_DOWNDIAG1(       whiteState, fieldInfo.m_pos)) {
    CLR_LDA_DOWNDIAG1(        whiteState, fieldInfo);
  } else if(GET_LDA_DOWNDIAG1(blackState, fieldInfo.m_pos)) {
    CLR_LDA_DOWNDIAG1(        blackState, fieldInfo);
  }
  if(GET_LDA_UPDIAG1(         whiteState, fieldInfo.m_pos)) {
    CLR_LDA_UPDIAG1(          whiteState, fieldInfo);
  } else if(GET_LDA_UPDIAG1(  blackState, fieldInfo.m_pos)) {
    CLR_LDA_UPDIAG1(          blackState, fieldInfo);
  }

  if(GET_LDA_DOWNDIAG2(       whiteState, fieldInfo.m_pos)) {
    CLR_LDA_DOWNDIAG2(        whiteState, fieldInfo);
  } else if(GET_LDA_DOWNDIAG2(blackState, fieldInfo.m_pos)) {
    CLR_LDA_DOWNDIAG2(        blackState, fieldInfo);
  }
  if(GET_LDA_UPDIAG2(         whiteState, fieldInfo.m_pos)) {
    CLR_LDA_UPDIAG2(          whiteState, fieldInfo);
  } else if(GET_LDA_UPDIAG2(  blackState, fieldInfo.m_pos)) {
    CLR_LDA_UPDIAG2(          blackState, fieldInfo);
  }
}

// ---------------------------------------- unblock --------------------------------------

void Game::unblockRow(const FieldInfo &fieldInfo) {
  PlayerState &whiteState = m_playerState[WHITEPLAYER];
  PlayerState &blackState = m_playerState[BLACKPLAYER];

  if(GET_LDA_LEFT(            whiteState, fieldInfo.m_pos)) {
    SET_LDA_LEFT(             whiteState, fieldInfo);
  } else if(GET_LDA_LEFT(     blackState, fieldInfo.m_pos)) {
    SET_LDA_LEFT(             blackState, fieldInfo);
  }
  if(GET_LDA_RIGHT(           whiteState, fieldInfo.m_pos)) {
    SET_LDA_RIGHT(            whiteState, fieldInfo);
  } else if(GET_LDA_RIGHT(    blackState, fieldInfo.m_pos)) {
    SET_LDA_RIGHT(            blackState, fieldInfo);
  }
}

void Game::unblockColumn(const FieldInfo &fieldInfo) {
  PlayerState &whiteState = m_playerState[WHITEPLAYER];
  PlayerState &blackState = m_playerState[BLACKPLAYER];

  if(GET_LDA_DOWN(            whiteState, fieldInfo.m_pos)) {
    SET_LDA_DOWN(             whiteState, fieldInfo);
  } else if(GET_LDA_DOWN(     blackState, fieldInfo.m_pos)) {
    SET_LDA_DOWN(             blackState, fieldInfo);
  }
  if(GET_LDA_UP(              whiteState, fieldInfo.m_pos)) {
    SET_LDA_UP(               whiteState, fieldInfo);
  } else if(GET_LDA_UP(       blackState, fieldInfo.m_pos)) {
    SET_LDA_UP(               blackState, fieldInfo);
  }
}

void Game::unblockDiag1(const FieldInfo &fieldInfo) {
  PlayerState &whiteState = m_playerState[WHITEPLAYER];
  PlayerState &blackState = m_playerState[BLACKPLAYER];

  if(GET_LDA_DOWNDIAG1(       whiteState, fieldInfo.m_pos)) {
    SET_LDA_DOWNDIAG1(        whiteState, fieldInfo);
  } else if(GET_LDA_DOWNDIAG1(blackState, fieldInfo.m_pos)) {
    SET_LDA_DOWNDIAG1(        blackState, fieldInfo);
  }
  if(GET_LDA_UPDIAG1(         whiteState, fieldInfo.m_pos)) {
    SET_LDA_UPDIAG1(          whiteState, fieldInfo);
  } else if(GET_LDA_UPDIAG1(  blackState, fieldInfo.m_pos)) {
    SET_LDA_UPDIAG1(          blackState, fieldInfo);
  }
}

void Game::unblockDiag2(const FieldInfo &fieldInfo) {
  PlayerState &whiteState = m_playerState[WHITEPLAYER];
  PlayerState &blackState = m_playerState[BLACKPLAYER];

  if(GET_LDA_DOWNDIAG2(       whiteState, fieldInfo.m_pos)) {
    SET_LDA_DOWNDIAG2(        whiteState, fieldInfo);
  } else if(GET_LDA_DOWNDIAG2(blackState, fieldInfo.m_pos)) {
    SET_LDA_DOWNDIAG2(        blackState, fieldInfo);
  }
  if(GET_LDA_UPDIAG2(         whiteState, fieldInfo.m_pos)) {
    SET_LDA_UPDIAG2(          whiteState, fieldInfo);
  } else if(GET_LDA_UPDIAG2(  blackState, fieldInfo.m_pos)) {
    SET_LDA_UPDIAG2(          blackState, fieldInfo);
  }
}

void Game::unblockDiag12(const FieldInfo &fieldInfo) {
  PlayerState &whiteState = m_playerState[WHITEPLAYER];
  PlayerState &blackState = m_playerState[BLACKPLAYER];

  if(GET_LDA_DOWNDIAG1(       whiteState, fieldInfo.m_pos)) {
    SET_LDA_DOWNDIAG1(        whiteState, fieldInfo);
  } else if(GET_LDA_DOWNDIAG1(blackState, fieldInfo.m_pos)) {
    SET_LDA_DOWNDIAG1(        blackState, fieldInfo);
  }
  if(GET_LDA_UPDIAG1(         whiteState, fieldInfo.m_pos)) {
    SET_LDA_UPDIAG1(          whiteState, fieldInfo);
  } else if(GET_LDA_UPDIAG1(  blackState, fieldInfo.m_pos)) {
    SET_LDA_UPDIAG1(          blackState, fieldInfo);
  }
  if(GET_LDA_DOWNDIAG2(       whiteState, fieldInfo.m_pos)) {
    SET_LDA_DOWNDIAG2(        whiteState, fieldInfo);
  } else if(GET_LDA_DOWNDIAG2(blackState, fieldInfo.m_pos)) {
    SET_LDA_DOWNDIAG2(        blackState, fieldInfo);
  }
  if(GET_LDA_UPDIAG2(         whiteState, fieldInfo.m_pos)) {
    SET_LDA_UPDIAG2(          whiteState, fieldInfo);
  } else if(GET_LDA_UPDIAG2(  blackState, fieldInfo.m_pos)) {
    SET_LDA_UPDIAG2(          blackState, fieldInfo);
  }
}

void Game::unblockRowEP(const FieldInfo &leftInfo, const FieldInfo &rightInfo) {
  PlayerState &whiteState = m_playerState[WHITEPLAYER];
  PlayerState &blackState = m_playerState[BLACKPLAYER];

  if(GET_LDA_LEFT(            whiteState, rightInfo.m_pos)) {
    SET_LDA_LEFT(             whiteState, rightInfo);
  } else if(GET_LDA_LEFT(     blackState, rightInfo.m_pos)) {
    SET_LDA_LEFT(             blackState, rightInfo);
  }
  if(GET_LDA_RIGHT(           whiteState, leftInfo.m_pos)) {
    SET_LDA_RIGHT(            whiteState, leftInfo);
  } else if(GET_LDA_RIGHT(    blackState, leftInfo.m_pos)) {
    SET_LDA_RIGHT(            blackState, leftInfo);
  }
}
