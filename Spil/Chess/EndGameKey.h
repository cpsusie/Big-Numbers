#pragma once

class EndGameKey { // Cannot contain virtual functions.
private:
  union {
    struct {
      UINT m_piece0Pos    :  6;
      UINT m_piece1Pos    :  6;
      UINT m_piece2Pos    :  6;
      UINT m_piece3Pos    :  6;
      UINT m_piece4Pos    :  6;
      UINT m_playerInTurn :  1;
      //                    Total : 31 bits.
    } m_state;
    unsigned long m_hashCode;
  };

public:
  EndGameKey() {
    m_hashCode = 0;
  }

  EndGameKey(const EndGameKeyDefinition &keydef, Player playerInTurn, ...);
  GameKey getGameKey(const EndGameKeyDefinition &keydef) const;

  inline Player getPlayerInTurn() const {
    return (Player)m_state.m_playerInTurn;
  }

  inline void setPlayerInTurn(Player player) {
    m_state.m_playerInTurn = player;
  }

  inline bool isEmpty() const {
    return m_hashCode == 0;
  }

  inline void clear() {
    m_hashCode = 0;
  }

  inline UINT getPosition0() const { return m_state.m_piece0Pos; }
  inline UINT getPosition1() const { return m_state.m_piece1Pos; }
  inline UINT getPosition2() const { return m_state.m_piece2Pos; }
  inline UINT getPosition3() const { return m_state.m_piece3Pos; }
  inline UINT getPosition4() const { return m_state.m_piece4Pos; }

  inline void setPosition0(UINT pos) { m_state.m_piece0Pos = pos; }
  inline void setPosition1(UINT pos) { m_state.m_piece1Pos = pos; }
  inline void setPosition2(UINT pos) { m_state.m_piece2Pos = pos; }
  inline void setPosition3(UINT pos) { m_state.m_piece3Pos = pos; }
  inline void setPosition4(UINT pos) { m_state.m_piece4Pos = pos; }

  UINT getP2OffDiagIndex()          const;
  UINT getP3OffDiagIndex()          const;
  UINT getP4OffDiagIndex()          const;

  UINT getP3OffDiagIndexEqualP23()  const;
  UINT getP4OffDiagIndexEqualP34()  const;
  UINT getP4OffDiagIndexEqualP234() const;

  UINT getP2DiagIndex()             const;
  UINT getP3DiagIndex()             const;
  UINT getP4DiagIndex()             const;

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
  void p23IndexToOffDiagPos();
  void p234IndexToOffDiagPos();
  void p34IndexToOffDiagPos();

  void p3IndexToOffDiagPosEqualP23();
  void p4IndexToOffDiagPosEqualP34();
  void p4IndexToOffDiagPosEqualP234();
  void p23IndexToOffDiagPosEqualP23();
  void p34IndexToOffDiagPosEqualP34();
  void p234IndexToOffDiagPosEqualP234();

  void p2IndexToDiagPos();
  void p3IndexToDiagPos();
  void p4IndexToDiagPos();
  void p23IndexToDiagPos();
  void p234IndexToDiagPos();

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

  inline UINT getPosition(UINT pIndex) const {
    assert(pIndex <= 4);
    return (m_hashCode >> (6*pIndex)) & 0x3f;
  }

  inline void setPosition(UINT pIndex, UINT pos) {
    assert((pIndex <= 4) && (pos < 64));
    pIndex *= 6;
    m_hashCode &= ~((0x3f << pIndex)); m_hashCode |= (pos << pIndex);
  }

  inline UINT getWhiteKingPosition() const { 
    return m_state.m_piece0Pos;
  }

  inline UINT getBlackKingPosition() const {
    return m_state.m_piece1Pos;
  }

  inline void setWhiteKingPosition(UINT pos) {
    m_state.m_piece0Pos = pos;
  }

  inline void setBlackKingPosition(UINT pos) {
    m_state.m_piece1Pos = pos;
  }

  inline bool kingsOnMainDiag1() const {
    return IS_ONMAINDIAG1(m_state.m_piece0Pos) && IS_ONMAINDIAG1(m_state.m_piece1Pos);
  }

  inline bool p2OnMainDiag1() const {
    return IS_ONMAINDIAG1(m_state.m_piece2Pos);
  }

  inline bool p2AboveMainDiag1() const {
    return IS_ABOVEMAINDIAG1(m_state.m_piece2Pos);
  }

  inline bool p2BelowMainDiag1() const {
    return IS_BELOWMAINDIAG1(m_state.m_piece2Pos);
  }

  inline bool p3OnMainDiag1() const {
    return IS_ONMAINDIAG1(m_state.m_piece3Pos);
  }

  inline bool p3AboveMainDiag1() const {
    return IS_ABOVEMAINDIAG1(m_state.m_piece3Pos);
  }

  inline bool p3BelowMainDiag1() const {
    return IS_BELOWMAINDIAG1(m_state.m_piece3Pos);
  }

  inline bool p4OnMainDiag1() const {
    return IS_ONMAINDIAG1(m_state.m_piece4Pos);
  }

  inline bool p4AboveMainDiag1() const {
    return IS_ABOVEMAINDIAG1(m_state.m_piece4Pos);
  }

  inline bool p4BelowMainDiag1() const {
    return IS_BELOWMAINDIAG1(m_state.m_piece4Pos);
  }

  inline friend bool operator==(const EndGameKey &k1, const EndGameKey &k2) {
    return k1.m_hashCode == k2.m_hashCode;
  }

  inline friend bool operator!=(const EndGameKey &k1, const EndGameKey &k2) {
    return k1.m_hashCode != k2.m_hashCode;
  }

  inline unsigned long hashCode() const {
    return m_hashCode;
  }

  String toString(const EndGameKeyDefinition &keydef, bool initFormat=false) const;
};

#ifdef TABLEBASE_BUILDER

class EndGameKeyWithOccupiedPositions : public EndGameKey {
private:
  FieldSet m_occupiedPositions;

public:
  void setWhiteKingPosition(UINT pos) {
    setPosition0(pos);
  }

  void setBlackKingPosition(UINT pos) {
    setPosition1(pos);
  }
  void setPosition( UINT pIndex, UINT pos);
  void setPosition0(UINT pos);
  void setPosition1(UINT pos);
  void setPosition2(UINT pos);
  void setPosition3(UINT pos);
  void setPosition4(UINT pos);
  bool isOccupied(  UINT pos) const {
    return m_occupiedPositions.contains(pos);
  }
  void clearField(UINT pos) {
    m_occupiedPositions.remove(pos);
  }
  void clear() {
    EndGameKey::clear();
    m_occupiedPositions.clear();
  }
};

#endif
