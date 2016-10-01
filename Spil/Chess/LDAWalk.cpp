#include "stdafx.h"

// ---------------------------------- LDA walk ----------------------------------------
void Game::LDAwalkLeft(const FieldInfo &fieldInfo, int from) {
  const DoubleLine &line       = fieldInfo.m_rowLine;
  PlayerState      &state      = m_playerState[PLAYERINTURN];
  PlayerState      &enemyState = m_playerState[CURRENTENEMY];

  if(GET_LDA_RIGHT(state, fieldInfo.m_pos)) {
    if(!GET_LDA_LEFT(state, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_RIGHT) {
        LDA_LEFT(state, pos) = 0;
      }
      if(GET_LDA_LEFT(enemyState, from)) {
        for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_RIGHT) {
          LDA_LEFT(enemyState, pos) = 1;
        }
      }
    }
  } else if(GET_LDA_LEFT(state, from)) {
    for(int pos = line.m_upper[1]; DPOS_RIGHT_LE(pos,from); pos += DPOS_RIGHT) {
      LDA_RIGHT(state, pos) = 1;
    }
    if(GET_LDA_RIGHT(enemyState, fieldInfo.m_pos)) {
      for(int pos = line.m_upper[1]; DPOS_RIGHT_LE(pos,from); pos += DPOS_RIGHT) {
        LDA_RIGHT(enemyState, pos) = 0;
      }
    }
  } else {
    LDA_LEFT(state,fieldInfo.m_pos) = 0;
    for(int pos = line.m_upper[1];; pos += DPOS_RIGHT) {
      AttackInfo &info = ATTINFO(state, pos);
      ATT_RIGHT(info) = 1;
      if(pos == from) {
        break;
      }
      ATT_LEFT(info) = 0;
    }
    if(GET_LDA_RIGHT(enemyState, fieldInfo.m_pos)) {
      if(GET_LDA_LEFT(enemyState, from)) {
        LDA_LEFT(enemyState, fieldInfo.m_pos) = 1;
        for(int pos = line.m_upper[1];; pos += DPOS_RIGHT) {
          AttackInfo &info = ATTINFO(enemyState, pos);
          ATT_RIGHT(info) = 0;
          if(pos == from) {
            break;
          }
          ATT_LEFT(info) = 1;
        }
      } else {
        for(int pos = line.m_upper[1]; DPOS_RIGHT_LE(pos,from); pos += DPOS_RIGHT) {
          LDA_RIGHT(enemyState, pos) = 0;
        }
      }
    } else if(GET_LDA_LEFT(enemyState, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_RIGHT) {
        LDA_LEFT(enemyState, pos) = 1;
      }
    }
  }
}

void Game::LDAwalkRight(const FieldInfo &fieldInfo, int from) {
  const DoubleLine &line       = fieldInfo.m_rowLine;
  PlayerState      &state      = m_playerState[PLAYERINTURN];
  PlayerState      &enemyState = m_playerState[CURRENTENEMY];

  if(GET_LDA_LEFT(state, fieldInfo.m_pos)) {
    if(!GET_LDA_RIGHT(state, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_LEFT) {
        LDA_RIGHT(state, pos) = 0;
      }
      if(GET_LDA_RIGHT(enemyState, from)) {
        for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_LEFT) {
          LDA_RIGHT(enemyState, pos) = 1;
        }
      }
    }
  } else if(GET_LDA_RIGHT(state, from)) {
    for(int pos = line.m_lower[1]; DPOS_LEFT_LE(pos,from); pos += DPOS_LEFT) {
      LDA_LEFT(state, pos) = 1;
    }
    if(GET_LDA_LEFT(enemyState, fieldInfo.m_pos)) {
      for(int pos = line.m_lower[1]; DPOS_LEFT_LE(pos,from); pos += DPOS_LEFT) {
        LDA_LEFT(enemyState, pos) = 0;
      }
    }
  } else {
    LDA_RIGHT(state,fieldInfo.m_pos) = 0;
    for(int pos = line.m_lower[1];; pos += DPOS_LEFT) {
      AttackInfo &info = ATTINFO(state, pos);
      ATT_LEFT(info) = 1;
      if(pos == from) {
        break;
      }
      ATT_RIGHT(info) = 0;
    }
    if(GET_LDA_LEFT(enemyState, fieldInfo.m_pos)) {
      if(GET_LDA_RIGHT(enemyState, from)) {
        LDA_RIGHT(enemyState, fieldInfo.m_pos) = 1;
        for(int pos = line.m_lower[1];; pos += DPOS_LEFT) {
          AttackInfo &info = ATTINFO(enemyState, pos);
          ATT_LEFT(info) = 0;
          if(pos == from) {
            break;
          }
          ATT_RIGHT(info) = 1;
        }
      } else {
        for(int pos = line.m_lower[1]; DPOS_LEFT_LE(pos,from); pos += DPOS_LEFT) {
          LDA_LEFT(enemyState, pos) = 0;
        }
      }
    } else if(GET_LDA_RIGHT(enemyState, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_LEFT) {
        LDA_RIGHT(enemyState, pos) = 1;
      }
    }
  }
}

void Game::LDAwalkDown(const FieldInfo &fieldInfo, int from) {
  const DoubleLine &line       = fieldInfo.m_colLine;
  PlayerState      &state      = m_playerState[PLAYERINTURN];
  PlayerState      &enemyState = m_playerState[CURRENTENEMY];

  if(GET_LDA_UP(state, fieldInfo.m_pos)) {
    if(!GET_LDA_DOWN(state, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_UP) {
        LDA_DOWN(state, pos) = 0;
      }
      if(GET_LDA_DOWN(enemyState, from)) {
        for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_UP) {
          LDA_DOWN(enemyState, pos) = 1;
        }
      }
    }
  } else if(GET_LDA_DOWN(state, from)) {
    for(int pos = line.m_upper[1]; DPOS_UP_LE(pos,from); pos += DPOS_UP) {
      LDA_UP(state, pos) = 1;
    }
    if(GET_LDA_UP(enemyState, fieldInfo.m_pos)) {
      for(int pos = line.m_upper[1]; DPOS_UP_LE(pos,from); pos += DPOS_UP) {
        LDA_UP(enemyState, pos) = 0;
      }
    }
  } else {
    LDA_DOWN(state,fieldInfo.m_pos) = 0;
    for(int pos = line.m_upper[1];; pos += DPOS_UP) {
      AttackInfo &info = ATTINFO(state, pos);
      ATT_UP(info) = 1;
      if(pos == from) {
        break;
      }
      ATT_DOWN(info) = 0;
    }
    if(GET_LDA_UP(enemyState, fieldInfo.m_pos)) {
      if(GET_LDA_DOWN(enemyState, from)) {
        LDA_DOWN(enemyState, fieldInfo.m_pos) = 1;
        for(int pos = line.m_upper[1];; pos += DPOS_UP) {
          AttackInfo &info = ATTINFO(enemyState, pos);
          ATT_UP(info) = 0;
          if(pos == from) {
            break;
          }
          ATT_DOWN(info) = 1;
        }
      } else {
        for(int pos = line.m_upper[1]; DPOS_UP_LE(pos,from); pos += DPOS_UP) {
          LDA_UP(enemyState, pos) = 0;
        }
      }
    } else if(GET_LDA_DOWN(enemyState, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_UP) {
        LDA_DOWN(enemyState, pos) = 1;
      }
    }
  }
}

void Game::LDAwalkUp(const FieldInfo &fieldInfo, int from) {
  const DoubleLine &line       = fieldInfo.m_colLine;
  PlayerState      &state      = m_playerState[PLAYERINTURN];
  PlayerState      &enemyState = m_playerState[CURRENTENEMY];

  if(GET_LDA_DOWN(state, fieldInfo.m_pos)) {
    if(!GET_LDA_UP(state, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_DOWN) {
        LDA_UP(state, pos) = 0;
      }
      if(GET_LDA_UP(enemyState, from)) {
        for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_DOWN) {
          LDA_UP(enemyState, pos) = 1;
        }
      }
    }
  } else if(GET_LDA_UP(state, from)) {
    for(int pos = line.m_lower[1]; DPOS_DOWN_LE(pos,from); pos += DPOS_DOWN) {
      LDA_DOWN(state, pos) = 1;
    }
    if(GET_LDA_DOWN(enemyState, fieldInfo.m_pos)) {
      for(int pos = line.m_lower[1]; DPOS_DOWN_LE(pos,from); pos += DPOS_DOWN) {
        LDA_DOWN(enemyState, pos) = 0;
      }
    }
  } else {
    LDA_UP(state,fieldInfo.m_pos) = 0;
    for(int pos = line.m_lower[1];; pos += DPOS_DOWN) {
      AttackInfo &info = ATTINFO(state, pos);
      ATT_DOWN(info) = 1;
      if(pos == from) {
        break;
      }
      ATT_UP(info) = 0;
    }
    if(GET_LDA_DOWN(enemyState, fieldInfo.m_pos)) {
      if(GET_LDA_UP(enemyState, from)) {
        LDA_UP(enemyState, fieldInfo.m_pos) = 1;
        for(int pos = line.m_lower[1];; pos += DPOS_DOWN) {
          AttackInfo &info = ATTINFO(enemyState, pos);
          ATT_DOWN(info) = 0;
          if(pos == from) {
            break;
          }
          ATT_UP(info) = 1;
        }
      } else {
        for(int pos = line.m_lower[1]; DPOS_DOWN_LE(pos,from); pos += DPOS_DOWN) {
          LDA_DOWN(enemyState, pos) = 0;
        }
      }
    } else if(GET_LDA_UP(enemyState, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_DOWN) {
        LDA_UP(enemyState, pos) = 1;
      }
    }
  }
}

void Game::LDAwalkDownDiag1(const FieldInfo &fieldInfo, int from) {
  const DoubleLine &line       = fieldInfo.m_diag1Line;
  PlayerState      &state      = m_playerState[PLAYERINTURN];
  PlayerState      &enemyState = m_playerState[CURRENTENEMY];

  if(GET_LDA_UPDIAG1(state, fieldInfo.m_pos)) {
    if(!GET_LDA_DOWNDIAG1(state, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_UPDIAG1) {
        LDA_DOWNDIAG1(state, pos) = 0;
      }
      if(GET_LDA_DOWNDIAG1(enemyState, from)) {
        for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_UPDIAG1) {
          LDA_DOWNDIAG1(enemyState, pos) = 1;
        }
      }
    }
  } else if(GET_LDA_DOWNDIAG1(state, from)) {
    for(int pos = line.m_upper[1]; DPOS_UPDIAG1_LE(pos,from); pos += DPOS_UPDIAG1) {
      LDA_UPDIAG1(state, pos) = 1;
    }
    if(GET_LDA_UPDIAG1(enemyState, fieldInfo.m_pos)) {
      for(int pos = line.m_upper[1]; DPOS_UPDIAG1_LE(pos,from); pos += DPOS_UPDIAG1) {
        LDA_UPDIAG1(enemyState, pos) = 0;
      }
    }
  } else {
    LDA_DOWNDIAG1(state,fieldInfo.m_pos) = 0;
    for(int pos = line.m_upper[1];; pos += DPOS_UPDIAG1) {
      AttackInfo &info = ATTINFO(state, pos);
      ATT_UPDIAG1(info) = 1;
      if(pos == from) {
        break;
      }
      ATT_DOWNDIAG1(info) = 0;
    }
    if(GET_LDA_UPDIAG1(enemyState, fieldInfo.m_pos)) {
      if(GET_LDA_DOWNDIAG1(enemyState, from)) {
        LDA_DOWNDIAG1(enemyState, fieldInfo.m_pos) = 1;
        for(int pos = line.m_upper[1];; pos += DPOS_UPDIAG1) {
          AttackInfo &info = ATTINFO(enemyState, pos);
          ATT_UPDIAG1(info) = 0;
          if(pos == from) {
            break;
          }
          ATT_DOWNDIAG1(info) = 1;
        }
      } else {
        for(int pos = line.m_upper[1]; DPOS_UPDIAG1_LE(pos,from); pos += DPOS_UPDIAG1) {
          LDA_UPDIAG1(enemyState, pos) = 0;
        }
      }
    } else if(GET_LDA_DOWNDIAG1(enemyState, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_UPDIAG1) {
        LDA_DOWNDIAG1(enemyState, pos) = 1;
      }
    }
  }
}

void Game::LDAwalkUpDiag1(const FieldInfo &fieldInfo, int from) {
  const DoubleLine &line       = fieldInfo.m_diag1Line;
  PlayerState      &state      = m_playerState[PLAYERINTURN];
  PlayerState      &enemyState = m_playerState[CURRENTENEMY];

  if(GET_LDA_DOWNDIAG1(state, fieldInfo.m_pos)) {
    if(!GET_LDA_UPDIAG1(state, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_DOWNDIAG1) {
        LDA_UPDIAG1(state, pos) = 0;
      }
      if(GET_LDA_UPDIAG1(enemyState, from)) {
        for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_DOWNDIAG1) {
          LDA_UPDIAG1(enemyState, pos) = 1;
        }
      }
    }
  } else if(GET_LDA_UPDIAG1(state, from)) {
    for(int pos = line.m_lower[1]; DPOS_DOWNDIAG1_LE(pos,from); pos += DPOS_DOWNDIAG1) {
      LDA_DOWNDIAG1(state, pos) = 1;
    }
    if(GET_LDA_DOWNDIAG1(enemyState, fieldInfo.m_pos)) {
      for(int pos = line.m_lower[1]; DPOS_DOWNDIAG1_LE(pos,from); pos += DPOS_DOWNDIAG1) {
        LDA_DOWNDIAG1(enemyState, pos) = 0;
      }
    }
  } else {
    LDA_UPDIAG1(state,fieldInfo.m_pos) = 0;
    for(int pos = line.m_lower[1];; pos += DPOS_DOWNDIAG1) {
      AttackInfo &info = ATTINFO(state, pos);
      ATT_DOWNDIAG1(info) = 1;
      if(pos == from) {
        break;
      }
      ATT_UPDIAG1(info) = 0;
    }
    if(GET_LDA_DOWNDIAG1(enemyState, fieldInfo.m_pos)) {
      if(GET_LDA_UPDIAG1(enemyState, from)) {
        LDA_UPDIAG1(enemyState, fieldInfo.m_pos) = 1;
        for(int pos = line.m_lower[1];; pos += DPOS_DOWNDIAG1) {
          AttackInfo &info = ATTINFO(enemyState, pos);
          ATT_DOWNDIAG1(info) = 0;
          if(pos == from) {
            break;
          }
          ATT_UPDIAG1(info) = 1;
        }
      } else {
        for(int pos = line.m_lower[1]; DPOS_DOWNDIAG1_LE(pos,from); pos += DPOS_DOWNDIAG1) {
          LDA_DOWNDIAG1(enemyState, pos) = 0;
        }
      }
    } else if(GET_LDA_UPDIAG1(enemyState, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_DOWNDIAG1) {
        LDA_UPDIAG1(enemyState, pos) = 1;
      }
    }
  }
}

void Game::LDAwalkDownDiag2(const FieldInfo &fieldInfo, int from) {
  const DoubleLine &line       = fieldInfo.m_diag2Line;
  PlayerState      &state      = m_playerState[PLAYERINTURN];
  PlayerState      &enemyState = m_playerState[CURRENTENEMY];

  if(GET_LDA_UPDIAG2(state, fieldInfo.m_pos)) {
    if(!GET_LDA_DOWNDIAG2(state, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_UPDIAG2) {
        LDA_DOWNDIAG2(state, pos) = 0;
      }
      if(GET_LDA_DOWNDIAG2(enemyState, from)) {
        for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_UPDIAG2) {
          LDA_DOWNDIAG2(enemyState, pos) = 1;
        }
      }
    }
  } else if(GET_LDA_DOWNDIAG2(state, from)) {
    for(int pos = line.m_upper[1]; DPOS_UPDIAG2_LE(pos,from); pos += DPOS_UPDIAG2) {
      LDA_UPDIAG2(state, pos) = 1;
    }
    if(GET_LDA_UPDIAG2(enemyState, fieldInfo.m_pos)) {
      for(int pos = line.m_upper[1]; DPOS_UPDIAG2_LE(pos,from); pos += DPOS_UPDIAG2) {
        LDA_UPDIAG2(enemyState, pos) = 0;
      }
    }
  } else {
    LDA_DOWNDIAG2(state,fieldInfo.m_pos) = 0;
    for(int pos = line.m_upper[1];; pos += DPOS_UPDIAG2) {
      AttackInfo &info = ATTINFO(state, pos);
      ATT_UPDIAG2(info) = 1;
      if(pos == from) {
        break;
      }
      ATT_DOWNDIAG2(info) = 0;
    }
    if(GET_LDA_UPDIAG2(enemyState, fieldInfo.m_pos)) {
      if(GET_LDA_DOWNDIAG2(enemyState, from)) {
        LDA_DOWNDIAG2(enemyState, fieldInfo.m_pos) = 1;
        for(int pos = line.m_upper[1];; pos += DPOS_UPDIAG2) {
          AttackInfo &info = ATTINFO(enemyState, pos);
          ATT_UPDIAG2(info) = 0;
          if(pos == from) {
            break;
          }
          ATT_DOWNDIAG2(info) = 1;
        }
      } else {
        for(int pos = line.m_upper[1]; DPOS_UPDIAG2_LE(pos,from); pos += DPOS_UPDIAG2) {
          LDA_UPDIAG2(enemyState, pos) = 0;
        }
      }
    } else if(GET_LDA_DOWNDIAG2(enemyState, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_UPDIAG2) {
        LDA_DOWNDIAG2(enemyState, pos) = 1;
      }
    }
  }
}

void Game::LDAwalkUpDiag2(const FieldInfo &fieldInfo, int from) {
  const DoubleLine &line       = fieldInfo.m_diag2Line;
  PlayerState      &state      = m_playerState[PLAYERINTURN];
  PlayerState      &enemyState = m_playerState[CURRENTENEMY];

  if(GET_LDA_DOWNDIAG2(state, fieldInfo.m_pos)) {
    if(!GET_LDA_UPDIAG2(state, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_DOWNDIAG2) {
        LDA_UPDIAG2(state, pos) = 0;
      }
      if(GET_LDA_UPDIAG2(enemyState, from)) {
        for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_DOWNDIAG2) {
          LDA_UPDIAG2(enemyState, pos) = 1;
        }
      }
    }
  } else if(GET_LDA_UPDIAG2(state, from)) {
    for(int pos = line.m_lower[1]; DPOS_DOWNDIAG2_LE(pos,from); pos += DPOS_DOWNDIAG2) {
      LDA_DOWNDIAG2(state, pos) = 1;
    }
    if(GET_LDA_DOWNDIAG2(enemyState, fieldInfo.m_pos)) {
      for(int pos = line.m_lower[1]; DPOS_DOWNDIAG2_LE(pos,from); pos += DPOS_DOWNDIAG2) {
        LDA_DOWNDIAG2(enemyState, pos) = 0;
      }
    }
  } else {
    LDA_UPDIAG2(state,fieldInfo.m_pos) = 0;
    for(int pos = line.m_lower[1];; pos += DPOS_DOWNDIAG2) {
      AttackInfo &info = ATTINFO(state, pos);
      ATT_DOWNDIAG2(info) = 1;
      if(pos == from) {
        break;
      }
      ATT_UPDIAG2(info) = 0;
    }
    if(GET_LDA_DOWNDIAG2(enemyState, fieldInfo.m_pos)) {
      if(GET_LDA_UPDIAG2(enemyState, from)) {
        LDA_UPDIAG2(enemyState, fieldInfo.m_pos) = 1;
        for(int pos = line.m_lower[1];; pos += DPOS_DOWNDIAG2) {
          AttackInfo &info = ATTINFO(enemyState, pos);
          ATT_DOWNDIAG2(info) = 0;
          if(pos == from) {
            break;
          }
          ATT_UPDIAG2(info) = 1;
        }
      } else {
        for(int pos = line.m_lower[1]; DPOS_DOWNDIAG2_LE(pos,from); pos += DPOS_DOWNDIAG2) {
          LDA_DOWNDIAG2(enemyState, pos) = 0;
        }
      }
    } else if(GET_LDA_UPDIAG2(enemyState, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_DOWNDIAG2) {
        LDA_UPDIAG2(enemyState, pos) = 1;
      }
    }
  }
}


// ---------------------------------- LDA walk capture ----------------------------------------
void Game::LDAwalkLeftCapture(const FieldInfo &fieldInfo, int from) {
  const DoubleLine &line  = fieldInfo.m_rowLine;
  PlayerState      &state = m_playerState[PLAYERINTURN];

  if(GET_LDA_LEFT(state, from)) {
    for(int pos = line.m_upper[1]; DPOS_RIGHT_LE(pos,from); pos += DPOS_RIGHT) {
      LDA_RIGHT(state, pos) = 1;
    }
  } else {
    LDA_LEFT(state,fieldInfo.m_pos) = 0;
    for(int pos = line.m_upper[1];; pos += DPOS_RIGHT) {
      AttackInfo &info = ATTINFO(state, pos);
      ATT_RIGHT(info) = 1;
      if(pos == from) {
        break;
      }
      ATT_LEFT(info) = 0;
    }
    PlayerState &enemyState = m_playerState[CURRENTENEMY];
    if(GET_LDA_LEFT(enemyState, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_RIGHT) {
        LDA_LEFT(enemyState, pos) = 1;
      }
    }
  }
  if(line.m_lower) {
    SET_LDA_LEFT(state, fieldInfo);
  }
}

void Game::LDAwalkRightCapture(const FieldInfo &fieldInfo, int from) {
  const DoubleLine &line  = fieldInfo.m_rowLine;
  PlayerState      &state = m_playerState[PLAYERINTURN];

  if(GET_LDA_RIGHT(state, from)) {
    for(int pos = line.m_lower[1]; DPOS_LEFT_LE(pos,from); pos += DPOS_LEFT) {
      LDA_LEFT(state, pos) = 1;
    }
  } else {
    LDA_RIGHT(state,fieldInfo.m_pos) = 0;
    for(int pos = line.m_lower[1];; pos += DPOS_LEFT) {
      AttackInfo &info = ATTINFO(state, pos);
      ATT_LEFT(info) = 1;
      if(pos == from) {
        break;
      }
      ATT_RIGHT(info) = 0;
    }
    PlayerState &enemyState = m_playerState[CURRENTENEMY];
    if(GET_LDA_RIGHT(enemyState, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_LEFT) {
        LDA_RIGHT(enemyState, pos) = 1;
      }
    }
  }
  if(line.m_upper) {
    SET_LDA_RIGHT(state, fieldInfo);
  }
}

void Game::LDAwalkDownCapture(const FieldInfo &fieldInfo, int from) {
  const DoubleLine &line  = fieldInfo.m_colLine;
  PlayerState      &state = m_playerState[PLAYERINTURN];

  if(GET_LDA_DOWN(state, from)) {
    for(int pos = line.m_upper[1]; DPOS_UP_LE(pos,from); pos += DPOS_UP) {
      LDA_UP(state, pos) = 1;
    }
  } else {
    LDA_DOWN(state,fieldInfo.m_pos) = 0;
    for(int pos = line.m_upper[1];; pos += DPOS_UP) {
      AttackInfo &info = ATTINFO(state, pos);
      ATT_UP(info) = 1;
      if(pos == from) {
        break;
      }
      ATT_DOWN(info) = 0;
    }
    PlayerState &enemyState = m_playerState[CURRENTENEMY];
    if(GET_LDA_DOWN(enemyState, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_UP) {
        LDA_DOWN(enemyState, pos) = 1;
      }
    }
  }
  if(line.m_lower) {
    SET_LDA_DOWN(state, fieldInfo);
  }
}

void Game::LDAwalkUpCapture(const FieldInfo &fieldInfo, int from) {
  const DoubleLine &line  = fieldInfo.m_colLine;
  PlayerState      &state = m_playerState[PLAYERINTURN];

  if(GET_LDA_UP(state, from)) {
    for(int pos = line.m_lower[1]; DPOS_DOWN_LE(pos,from); pos += DPOS_DOWN) {
      LDA_DOWN(state, pos) = 1;
    }
  } else {
    LDA_UP(state,fieldInfo.m_pos) = 0;
    for(int pos = line.m_lower[1];; pos += DPOS_DOWN) {
      AttackInfo &info = ATTINFO(state, pos);
      ATT_DOWN(info) = 1;
      if(pos == from) {
        break;
      }
      ATT_UP(info) = 0;
    }
    PlayerState &enemyState = m_playerState[CURRENTENEMY];
    if(GET_LDA_UP(enemyState, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_DOWN) {
        LDA_UP(enemyState, pos) = 1;
      }
    }
  }
  if(line.m_upper) {
    SET_LDA_UP(state, fieldInfo);
  }
}

void Game::LDAwalkDownDiag1Capture(const FieldInfo &fieldInfo, int from) {
  const DoubleLine &line  = fieldInfo.m_diag1Line;
  PlayerState      &state = m_playerState[PLAYERINTURN];

  if(GET_LDA_DOWNDIAG1(state, from)) {
    for(int pos = line.m_upper[1]; DPOS_UPDIAG1_LE(pos,from); pos += DPOS_UPDIAG1) {
      LDA_UPDIAG1(state, pos) = 1;
    }
  } else {
    LDA_DOWNDIAG1(state,fieldInfo.m_pos) = 0;
    for(int pos = line.m_upper[1];; pos += DPOS_UPDIAG1) {
      AttackInfo &info = ATTINFO(state, pos);
      ATT_UPDIAG1(info) = 1;
      if(pos == from) {
        break;
      }
      ATT_DOWNDIAG1(info) = 0;
    }
    PlayerState &enemyState = m_playerState[CURRENTENEMY];
    if(GET_LDA_DOWNDIAG1(enemyState, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_UPDIAG1) {
        LDA_DOWNDIAG1(enemyState, pos) = 1;
      }
    }
  }
  if(line.m_lower) {
    SET_LDA_DOWNDIAG1(state, fieldInfo);
  }
}

void Game::LDAwalkUpDiag1Capture(const FieldInfo &fieldInfo, int from) {
  const DoubleLine &line  = fieldInfo.m_diag1Line;
  PlayerState      &state = m_playerState[PLAYERINTURN];

  if(GET_LDA_UPDIAG1(state, from)) {
    for(int pos = line.m_lower[1]; DPOS_DOWNDIAG1_LE(pos,from); pos += DPOS_DOWNDIAG1) {
      LDA_DOWNDIAG1(state, pos) = 1;
    }
  } else {
    LDA_UPDIAG1(state,fieldInfo.m_pos) = 0;
    for(int pos = line.m_lower[1];; pos += DPOS_DOWNDIAG1) {
      AttackInfo &info = ATTINFO(state, pos);
      ATT_DOWNDIAG1(info) = 1;
      if(pos == from) {
        break;
      }
      ATT_UPDIAG1(info) = 0;
    }
    PlayerState &enemyState = m_playerState[CURRENTENEMY];
    if(GET_LDA_UPDIAG1(enemyState, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_DOWNDIAG1) {
        LDA_UPDIAG1(enemyState, pos) = 1;
      }
    }
  }
  if(line.m_upper) {
    SET_LDA_UPDIAG1(state, fieldInfo);
  }
}

void Game::LDAwalkDownDiag2Capture(const FieldInfo &fieldInfo, int from) {
  const DoubleLine &line  = fieldInfo.m_diag2Line;
  PlayerState      &state = m_playerState[PLAYERINTURN];

  if(GET_LDA_DOWNDIAG2(state, from)) {
    for(int pos = line.m_upper[1]; DPOS_UPDIAG2_LE(pos,from); pos += DPOS_UPDIAG2) {
      LDA_UPDIAG2(state, pos) = 1;
    }
  } else {
    LDA_DOWNDIAG2(state,fieldInfo.m_pos) = 0;
    for(int pos = line.m_upper[1];; pos += DPOS_UPDIAG2) {
      AttackInfo &info = ATTINFO(state, pos);
      ATT_UPDIAG2(info) = 1;
      if(pos == from) {
        break;
      }
      ATT_DOWNDIAG2(info) = 0;
    }
    PlayerState &enemyState = m_playerState[CURRENTENEMY];
    if(GET_LDA_DOWNDIAG2(enemyState, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_UPDIAG2) {
        LDA_DOWNDIAG2(enemyState, pos) = 1;
      }
    }
  }
  if(line.m_lower) {
    SET_LDA_DOWNDIAG2(state, fieldInfo);
  }
}

void Game::LDAwalkUpDiag2Capture(const FieldInfo &fieldInfo, int from) {
  const DoubleLine &line  = fieldInfo.m_diag2Line;
  PlayerState      &state = m_playerState[PLAYERINTURN];

  if(GET_LDA_UPDIAG2(state, from)) {
    for(int pos = line.m_lower[1]; DPOS_DOWNDIAG2_LE(pos,from); pos += DPOS_DOWNDIAG2) {
      LDA_DOWNDIAG2(state, pos) = 1;
    }
  } else {
    LDA_UPDIAG2(state,fieldInfo.m_pos) = 0;
    for(int pos = line.m_lower[1];; pos += DPOS_DOWNDIAG2) {
      AttackInfo &info = ATTINFO(state, pos);
      ATT_DOWNDIAG2(info) = 1;
      if(pos == from) {
        break;
      }
      ATT_UPDIAG2(info) = 0;
    }
    PlayerState &enemyState = m_playerState[CURRENTENEMY];
    if(GET_LDA_UPDIAG2(enemyState, from)) {
      for(int pos = fieldInfo.m_pos; pos != from; pos += DPOS_DOWNDIAG2) {
        LDA_UPDIAG2(enemyState, pos) = 1;
      }
    }
  }
  if(line.m_upper) {
    SET_LDA_UPDIAG2(state, fieldInfo);
  }
}
