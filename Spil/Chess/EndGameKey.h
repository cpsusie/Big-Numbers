#pragma once

#define MAX_ENDGAME_PIECECOUNT 6

class EndGameKey {
private:
  union {
    UINT64 m_hashCode;
    BYTE   m_pos[8];
  };
public:
  inline EndGameKey() {
    m_hashCode = 0;
  }

  EndGameKey(const EndGameKeyDefinition &keydef, Player playerInTurn, ...);
  GameKey getGameKey(const EndGameKeyDefinition &keydef) const;

  inline Player getPlayerInTurn() const {
    return (Player)m_pos[6];
  }

  inline void setPlayerInTurn(Player player) {
    m_pos[6] = player;
  }

  inline UINT getPosition(UINT pIndex) const {
    assert(pIndex < MAX_ENDGAME_PIECECOUNT);
    return m_pos[pIndex];
  }

  inline void setPosition(UINT pIndex, UINT pos) {
    assert((pIndex < MAX_ENDGAME_PIECECOUNT) && (pos < 64));
    m_pos[pIndex] = pos;
  }

  inline bool isEmpty() const {
    return m_hashCode == 0;
  }

  inline void clear() {
    m_hashCode = 0;
  }

  inline UINT getPosition0() const { return getPosition(0); }
  inline UINT getPosition1() const { return getPosition(1); }
  inline UINT getPosition2() const { return getPosition(2); }
  inline UINT getPosition3() const { return getPosition(3); }
  inline UINT getPosition4() const { return getPosition(4); }
  inline UINT getPosition5() const { return getPosition(5); }

  inline void setPosition0(UINT pos) { setPosition(0,pos); }
  inline void setPosition1(UINT pos) { setPosition(1,pos); }
  inline void setPosition2(UINT pos) { setPosition(2,pos); }
  inline void setPosition3(UINT pos) { setPosition(3,pos); }
  inline void setPosition4(UINT pos) { setPosition(4,pos); }
  inline void setPosition5(UINT pos) { setPosition(5,pos); }

  UINT getP2OffDiagIndex()          const;
  UINT getP3OffDiagIndex()          const;
  UINT getP4OffDiagIndex()          const;
  UINT getP5OffDiagIndex()          const;

  UINT getP3OffDiagIndexEqualP23()  const;
  UINT getP4OffDiagIndexEqualP34()  const;
  UINT getP4OffDiagIndexEqualP234() const;

  UINT getP2DiagIndex()             const;
  UINT getP3DiagIndex()             const;
  UINT getP4DiagIndex()             const;
  UINT getP5DiagIndex()             const;

  UINT getP3DiagIndexEqualP23()     const;
  UINT getP4DiagIndexEqualP34()     const;
  UINT getP4DiagIndexEqualP234()    const;

  UINT getP3Pawn2Index()            const;
  UINT getP4Pawn3Index()            const;
  UINT getP4Pawn3IndexEqualP34()    const;
  UINT getP4With2PawnsIndex()       const;

  void p2IndexToOffDiagPos();
  void p3IndexToOffDiagPos();
  void p4IndexToOffDiagPos();
  void p5IndexToOffDiagPos();
  void p23IndexToOffDiagPos();
  void p234IndexToOffDiagPos();
  void p34IndexToOffDiagPos();
  void p45IndexToOffDiagPos();
  void p345IndexToOffDiagPos();
  void p2345IndexToOffDiagPos();

  void p3IndexToOffDiagPosEqualP23();
  void p4IndexToOffDiagPosEqualP34();
  void p4IndexToOffDiagPosEqualP234();
  void p23IndexToOffDiagPosEqualP23();
  void p34IndexToOffDiagPosEqualP34();
  void p234IndexToOffDiagPosEqualP234();

  void p2IndexToDiagPos();
  void p3IndexToDiagPos();
  void p4IndexToDiagPos();
  void p5IndexToDiagPos();
  void p23IndexToDiagPos();
  void p234IndexToDiagPos();
  void p2345IndexToDiagPos();

  void p3IndexToDiagPosEqualP23();
  void p4IndexToDiagPosEqualP34();
  void p4IndexToDiagPosEqualP234();
  void p23IndexToDiagPosEqualP23();
  void p34IndexToDiagPosEqualP34();
  void p34IndexToDiagPosEqualP234();
  void p234IndexToDiagPosEqualP234();

  void p3IndexToPawn2Pos();
  void p4IndexToPawn3Pos();
  void p4IndexToPawn3PosEqualP34();
  void p34IndexToPawn23Pos();
  void p34IndexToPawn23PosEqualP34();

  void p3IndexToPawn1Pos();

  inline UINT getWhiteKingPosition() const {
    return getPosition0();
  }

  inline UINT getBlackKingPosition() const {
    return getPosition1();
  }

  inline void setWhiteKingPosition(UINT pos) {
    setPosition0(pos);
  }

  inline void setBlackKingPosition(UINT pos) {
    setPosition1(pos);
  }

  inline bool kingsOnMainDiag1() const {
    return IS_ONMAINDIAG1(getWhiteKingPosition()) && IS_ONMAINDIAG1(getBlackKingPosition());
  }

  inline bool p2OnMainDiag1() const {
    return IS_ONMAINDIAG1(getPosition2());
  }

  inline bool p2AboveMainDiag1() const {
    return IS_ABOVEMAINDIAG1(getPosition2());
  }

  inline bool p2BelowMainDiag1() const {
    return IS_BELOWMAINDIAG1(getPosition2());
  }

  inline bool p3OnMainDiag1() const {
    return IS_ONMAINDIAG1(getPosition3());
  }

  inline bool p3AboveMainDiag1() const {
    return IS_ABOVEMAINDIAG1(getPosition3());
  }

  inline bool p3BelowMainDiag1() const {
    return IS_BELOWMAINDIAG1(getPosition3());
  }

  inline bool p4OnMainDiag1() const {
    return IS_ONMAINDIAG1(getPosition4());
  }

  inline bool p4AboveMainDiag1() const {
    return IS_ABOVEMAINDIAG1(getPosition4());
  }

  inline bool p4BelowMainDiag1() const {
    return IS_BELOWMAINDIAG1(getPosition4());
  }

  inline bool p5OnMainDiag1() const {
    return IS_ONMAINDIAG1(getPosition5());
  }

  inline bool p5AboveMainDiag1() const {
    return IS_ABOVEMAINDIAG1(getPosition5());
  }

  inline bool p5BelowMainDiag1() const {
    return IS_BELOWMAINDIAG1(getPosition5());
  }

  inline friend bool operator==(const EndGameKey &k1, const EndGameKey &k2) {
    return k1.m_hashCode == k2.m_hashCode;
  }

  inline friend bool operator!=(const EndGameKey &k1, const EndGameKey &k2) {
    return k1.m_hashCode != k2.m_hashCode;
  }

  inline ULONG hashCode() const {
    return uint64Hash(m_hashCode);
  }

  String toString(const EndGameKeyDefinition &keydef, bool initFormat=false) const;
};

#ifdef TABLEBASE_BUILDER

class EndGameKeyWithOccupiedPositions : public EndGameKey {
private:
  FieldSet m_occupiedPositions;

public:
  inline void setWhiteKingPosition(UINT pos) {
    setPosition0(pos);
  }

  inline void setBlackKingPosition(UINT pos) {
    setPosition1(pos);
  }
  void setPosition( UINT pIndex, UINT pos);
  void setPosition0(UINT pos);
  void setPosition1(UINT pos);
  void setPosition2(UINT pos);
  void setPosition3(UINT pos);
  void setPosition4(UINT pos);
  void setPosition5(UINT pos);
  inline bool isOccupied(  UINT pos) const {
    return m_occupiedPositions.contains(pos);
  }
  inline void clearField(UINT pos) {
    m_occupiedPositions.remove(pos);
  }
  inline void clear() {
    EndGameKey::clear();
    m_occupiedPositions.clear();
  }
};

#endif
