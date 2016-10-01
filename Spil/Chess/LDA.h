#pragma once

#include "FieldNames.h"

// Various macroes to handle updates of m_attackTable and m_kingDirection when moving Long Distance Attacking (LDA) pieces.
// (Queen, Rook or Bishop)

#define ATTFIELD(attInfo,field) attInfo.field
#define FIELDATTACKS(state,pos) (state).m_attackTable[pos]
#define ATTINFO(     state,pos) FIELDATTACKS(state,pos).m_attackInfo

#define DPOS_LEFT                (A1 - B1        )
#define DPOS_RIGHT               (-DPOS_LEFT     )
#define DPOS_DOWN                (A1 - A2        )
#define DPOS_UP                  (-DPOS_DOWN     )
#define DPOS_DOWNDIAG1           (A1 - B2        )
#define DPOS_UPDIAG1             (-DPOS_DOWNDIAG1)
#define DPOS_DOWNDIAG2           (B1 - A2        )
#define DPOS_UPDIAG2             (-DPOS_DOWNDIAG2)

#if(DPOS_LEFT < 0)
#define DPOS_LEFT_LE(pos,to)          ((pos) >= (to))
#else
#define DPOS_LEFT_LE(pos,to)          ((pos) <= (to))
#endif

#if(DPOS_RIGHT < 0)
#define DPOS_RIGHT_LE(pos,to)         ((pos) >= (to))
#else
#define DPOS_RIGHT_LE(pos,to)         ((pos) <= (to))
#endif

#if(DPOS_DOWN < 0)
#define DPOS_DOWN_LE(pos,to)          ((pos) >= (to))
#else
#define DPOS_DOWN_LE(pos,to)          ((pos) <= (to))
#endif

#if(DPOS_UP < 0)
#define DPOS_UP_LE(pos,to)            ((pos) >= (to))
#else
#define DPOS_UP_LE(pos,to)            ((pos) <= (to))
#endif

#if(DPOS_DOWNDIAG1 < 0)
#define DPOS_DOWNDIAG1_LE(pos,to)     ((pos) >= (to))
#else
#define DPOS_DOWNDIAG1_LE(pos,to)     ((pos) <= (to))
#endif

#if(DPOS_UPDIAG1 < 0)
#define DPOS_UPDIAG1_LE(pos,to)       ((pos) >= (to))
#else
#define DPOS_UPDIAG1_LE(pos,to)       ((pos) <= (to))
#endif

#if(DPOS_DOWNDIAG2 < 0)
#define DPOS_DOWNDIAG2_LE(pos,to)     ((pos) >= (to))
#else
#define DPOS_DOWNDIAG2_LE(pos,to)     ((pos) <= (to))
#endif

#if(DPOS_UPDIAG2 < 0)
#define DPOS_UPDIAG2_LE(pos,to)       ((pos) >= (to))
#else
#define DPOS_UPDIAG2_LE(pos,to)       ((pos) <= (to))
#endif

#define ATT_LEFT(                attInfo)    ATTFIELD(     attInfo, m_fromRight     )
#define ATT_RIGHT(               attInfo)    ATTFIELD(     attInfo, m_fromLeft      )
#define ATT_DOWN(                attInfo)    ATTFIELD(     attInfo, m_fromAbove     )
#define ATT_UP(                  attInfo)    ATTFIELD(     attInfo, m_fromBelove    )
#define ATT_DOWNDIAG1(           attInfo)    ATTFIELD(     attInfo, m_fromUpperDiag1)
#define ATT_UPDIAG1(             attInfo)    ATTFIELD(     attInfo, m_fromLowerDiag1)
#define ATT_DOWNDIAG2(           attInfo)    ATTFIELD(     attInfo, m_fromUpperDiag2)
#define ATT_UPDIAG2(             attInfo)    ATTFIELD(     attInfo, m_fromLowerDiag2)

#define LDA_LEFT(                state, pos) ATT_LEFT(     ATTINFO(state, pos))
#define LDA_RIGHT(               state, pos) ATT_RIGHT(    ATTINFO(state, pos))
#define LDA_DOWN(                state, pos) ATT_DOWN(     ATTINFO(state, pos))
#define LDA_UP(                  state, pos) ATT_UP(       ATTINFO(state, pos))
#define LDA_DOWNDIAG1(           state, pos) ATT_DOWNDIAG1(ATTINFO(state, pos))
#define LDA_UPDIAG1(             state, pos) ATT_UPDIAG1(  ATTINFO(state, pos))
#define LDA_DOWNDIAG2(           state, pos) ATT_DOWNDIAG2(ATTINFO(state, pos))
#define LDA_UPDIAG2(             state, pos) ATT_UPDIAG2(  ATTINFO(state, pos))

#define GET_LDA_LEFT(            state, pos) LDA_LEFT(     state, pos)
#define GET_LDA_RIGHT(           state, pos) LDA_RIGHT(    state, pos)
#define GET_LDA_DOWN(            state, pos) LDA_DOWN(     state, pos)
#define GET_LDA_UP(              state, pos) LDA_UP(       state, pos)
#define GET_LDA_DOWNDIAG1(       state, pos) LDA_DOWNDIAG1(state, pos)
#define GET_LDA_UPDIAG1(         state, pos) LDA_UPDIAG1(  state, pos)
#define GET_LDA_DOWNDIAG2(       state, pos) LDA_DOWNDIAG2(state, pos)
#define GET_LDA_UPDIAG2(         state, pos) LDA_UPDIAG2(  state, pos)

#define CLR_LDA_LEFT(            state, fieldInfo) clearLeft(      state, fieldInfo)
#define CLR_LDA_RIGHT(           state, fieldInfo) clearRight(     state, fieldInfo)
#define CLR_LDA_DOWN(            state, fieldInfo) clearDown(      state, fieldInfo)
#define CLR_LDA_UP(              state, fieldInfo) clearUp(        state, fieldInfo)
#define CLR_LDA_DOWNDIAG1(       state, fieldInfo) clearDownDiag1( state, fieldInfo)
#define CLR_LDA_UPDIAG1(         state, fieldInfo) clearUpDiag1(   state, fieldInfo)
#define CLR_LDA_DOWNDIAG2(       state, fieldInfo) clearDownDiag2( state, fieldInfo)
#define CLR_LDA_UPDIAG2(         state, fieldInfo) clearUpDiag2(   state, fieldInfo)

#define SET_LDA_LEFT(            state, fieldInfo) setLeft(        state, fieldInfo)
#define SET_LDA_RIGHT(           state, fieldInfo) setRight(       state, fieldInfo)
#define SET_LDA_DOWN(            state, fieldInfo) setDown(        state, fieldInfo)
#define SET_LDA_UP(              state, fieldInfo) setUp(          state, fieldInfo)
#define SET_LDA_DOWNDIAG1(       state, fieldInfo) setDownDiag1(   state, fieldInfo)
#define SET_LDA_UPDIAG1(         state, fieldInfo) setUpDiag1(     state, fieldInfo)
#define SET_LDA_DOWNDIAG2(       state, fieldInfo) setDownDiag2(   state, fieldInfo)
#define SET_LDA_UPDIAG2(         state, fieldInfo) setUpDiag2(     state, fieldInfo)

#define KING_DIRECTION(state, pos) (state).m_attackTable[pos].m_attackInfo.m_kingDirection

#define KING_IS_LEFT(      state, pos) (KING_DIRECTION(state, pos) == MD_LEFT     )
#define KING_IS_RIGHT(     state, pos) (KING_DIRECTION(state, pos) == MD_RIGHT    )
#define KING_IS_DOWN(      state, pos) (KING_DIRECTION(state, pos) == MD_DOWN     )
#define KING_IS_UP(        state, pos) (KING_DIRECTION(state, pos) == MD_UP       )
#define KING_IS_DOWNDIAG1( state, pos) (KING_DIRECTION(state, pos) == MD_DOWNDIAG1)
#define KING_IS_UPDIAG1(   state, pos) (KING_DIRECTION(state, pos) == MD_UPDIAG1  )
#define KING_IS_DOWNDIAG2( state, pos) (KING_DIRECTION(state, pos) == MD_DOWNDIAG2)
#define KING_IS_UPDIAG2(   state, pos) (KING_DIRECTION(state, pos) == MD_UPDIAG2  )

#define SET_NOKING(state, positions, dpos)                                    \
{ const Piece *piece;                                                         \
  for(int count = positions[0], pos = positions[1]; count--; pos += dpos) {   \
    KING_DIRECTION(state, pos) = MD_NONE;                                     \
    if(piece = m_board[pos]) {                                                \
      if(&piece->m_playerState == &state) {                                   \
        piece->m_pinnedState = NOT_PINNED;                                    \
      }                                                                       \
      break;                                                                  \
    }                                                                         \
  }                                                                           \
}

#define SET_NOKING_LEFT(     state, fieldInfo) SET_NOKING(state, fieldInfo.m_rowLine.m_upper  , DPOS_RIGHT    )
#define SET_NOKING_RIGHT(    state, fieldInfo) SET_NOKING(state, fieldInfo.m_rowLine.m_lower  , DPOS_LEFT     )
#define SET_NOKING_DOWN(     state, fieldInfo) SET_NOKING(state, fieldInfo.m_colLine.m_upper  , DPOS_UP       )
#define SET_NOKING_UP(       state, fieldInfo) SET_NOKING(state, fieldInfo.m_colLine.m_lower  , DPOS_DOWN     )
#define SET_NOKING_DOWNDIAG1(state, fieldInfo) SET_NOKING(state, fieldInfo.m_diag1Line.m_upper, DPOS_UPDIAG1  )
#define SET_NOKING_UPDIAG1(  state, fieldInfo) SET_NOKING(state, fieldInfo.m_diag1Line.m_lower, DPOS_DOWNDIAG1)
#define SET_NOKING_DOWNDIAG2(state, fieldInfo) SET_NOKING(state, fieldInfo.m_diag2Line.m_upper, DPOS_UPDIAG2  )
#define SET_NOKING_UPDIAG2(  state, fieldInfo) SET_NOKING(state, fieldInfo.m_diag2Line.m_lower, DPOS_DOWNDIAG2)
