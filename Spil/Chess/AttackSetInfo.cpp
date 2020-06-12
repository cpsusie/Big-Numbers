#include "stdafx.h"
#include <DebugLog.h>

static void setBits(FieldSet &set, PositionArray p) {
  if(p == NULL) return;
  for (int c = *(p++); c--;) {
    set.add(*(p++));
  }
}

AttackSetInfo::AttackSetInfo(const FieldInfo &info) {
  m_pos = info.m_pos;
  setBits(m_leftRow       , info.m_rowLine.m_lower  );
  setBits(m_rightRow      , info.m_rowLine.m_upper  );
  setBits(m_lowerCol      , info.m_colLine.m_lower  );
  setBits(m_upperCol      , info.m_colLine.m_upper  );
  setBits(m_lowerDiag1    , info.m_diag1Line.m_lower);
  setBits(m_upperDiag1    , info.m_diag1Line.m_upper);
  setBits(m_lowerDiag2    , info.m_diag2Line.m_lower);
  setBits(m_upperDiag2    , info.m_diag2Line.m_upper);
  setBits(m_kingAttacks   , info.m_kingAttacks      );
  setBits(m_knightAttacks , info.m_knightAttacks    );
  setBits(m_pawnAttacks[0], info.m_whitePawnAttacks );
  setBits(m_pawnAttacks[1], info.m_blackPawnAttacks );
}

#if defined(_DEBUG)
static void dumpFieldSet(const String &name, const FieldSet &set) {
  debugLog(_T("%s:\n"), name.cstr());
  set.dump();
}

#define DUMPSET(s) dumpFieldSet(_T(#s), m_##s)
void AttackSetInfo::dump() const {
  debugLog(_T("FieldSets for %s\n"), getFieldName(m_pos));
  DUMPSET(leftRow                    );
  DUMPSET(rightRow                   );
  DUMPSET(lowerCol                   );
  DUMPSET(upperCol                   );
  DUMPSET(lowerDiag1                 );
  DUMPSET(upperDiag1                 );
  DUMPSET(lowerDiag2                 );
  DUMPSET(upperDiag2                 );
  DUMPSET(kingAttacks                );
  DUMPSET(knightAttacks              );
  DUMPSET(pawnAttacks[0]             );
  DUMPSET(pawnAttacks[1]             );
}
#endif // _DEBUG

AttackSetInfo AttackSetInfo::s_bitBoards[64];

void AttackSetInfo::initArray() { // static
  for (int i = 0; i < 64; i++) {
    AttackSetInfo::s_bitBoards[i] = AttackSetInfo(Game::s_fieldInfo[i]);
//    AttackSetInfo::s_bitBoards[i].dump();
  }
}

#define LR 0
#define RR 1
#define LC 2
#define UC 3
#define LD1 4
#define UD1 5
#define LD2 6
#define UD2 7
#define SD  8

class PlayerAttacks {
public:
  FieldSet m_set[9];
  void clear() {
    for(int i = 0; i < ARRAYSIZE(m_set); i++) {
      m_set[i].clear();
    }
  }
  void setQueen( int pos);
  inline void setRook(  int pos) {
    const AttackSetInfo &a = AttackSetInfo::s_bitBoards[pos];
    m_set[LR]  |= a.m_leftRow;    m_set[RR]  |= a.m_rightRow;
    m_set[LC]  |= a.m_lowerCol;   m_set[UC]  |= a.m_upperCol;
  }
  inline void setBishop(int pos) {
    const AttackSetInfo &a = AttackSetInfo::s_bitBoards[pos];
    m_set[LD1] |= a.m_lowerDiag1; m_set[UD1] |= a.m_upperDiag1;
    m_set[LD2] |= a.m_lowerDiag2; m_set[UD2] |= a.m_upperDiag2;
  }
  void blockDiagonals(int pos);
  void blockRowCol(   int pos);
};

void PlayerAttacks::setQueen(int pos) {
  const AttackSetInfo &a = AttackSetInfo::s_bitBoards[pos];
  m_set[LR]  |= a.m_leftRow;    m_set[RR]  |= a.m_rightRow;
  m_set[LC]  |= a.m_lowerCol;   m_set[UC]  |= a.m_upperCol;
  m_set[LD1] |= a.m_lowerDiag1; m_set[UD1] |= a.m_upperDiag1;
  m_set[LD2] |= a.m_lowerDiag2; m_set[UD2] |= a.m_upperDiag2;
}

void PlayerAttacks::blockDiagonals(int pos) {

}
void PlayerAttacks::blockRowCol(int pos) {
}

class BoardAttacks {
public:
  PlayerAttacks m_attacks[2];
  void clear() {
    forEachPlayer(p) m_attacks[p].clear();
  }
  void blockDiagonals(Player p, int pos);
  void blockRowCol(   Player p, int pos);
  inline void setKing(Player p, int pos) {
    m_attacks[p].m_set[SD] += AttackSetInfo::s_bitBoards[pos].m_kingAttacks;
  }
  inline void setPawn(Player p, int pos) {
    m_attacks[p].m_set[SD] += AttackSetInfo::s_bitBoards[pos].m_pawnAttacks[p];
  }
  inline void setQueen(Player p, int pos) {
    m_attacks[p].setQueen(pos);
  }
  inline void setRook(Player p, int pos) {
    m_attacks[p].setRook(pos);
  }
  inline void setBishop(Player p, int pos) {
    m_attacks[p].setBishop(pos);
  }
};
