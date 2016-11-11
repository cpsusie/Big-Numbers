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
  inline UINT getWhiteKingPosition() const {
    return getPosition(0);
  }
  inline UINT getBlackKingPosition() const {
    return getPosition(1);
  }
  inline void setWhiteKingPosition(UINT pos) {
    setPosition(0,pos);
  }
  inline void setBlackKingPosition(UINT pos) {
    setPosition(1,pos);
  }

  UINT getP2OffDiagIndex()          const;
  UINT getP3OffDiagIndex()          const;
  UINT getP4OffDiagIndex()          const;
  UINT getP5OffDiagIndex()          const;

  UINT getP3OffDiagIndexEqualP23()  const;
  UINT getP4OffDiagIndexEqualP34()  const;
  UINT getP5OffDiagIndexEqualP45()  const;
  UINT getP4OffDiagIndexEqualP234() const;
  UINT getP5OffDiagIndexEqualP345() const;

  UINT getP2DiagIndex()             const;
  UINT getP3DiagIndex()             const;
  UINT getP4DiagIndex()             const;
  UINT getP5DiagIndex()             const;

  UINT getP3DiagIndexEqualP23()     const;
  UINT getP4DiagIndexEqualP34()     const;
  UINT getP5DiagIndexEqualP45()     const;
  UINT getP4DiagIndexEqualP234()    const;
  UINT getP5DiagIndexEqualP345()    const;

  UINT getP3Pawn2Index()            const;
  UINT getP4Pawn3Index()            const;
  UINT getP4Pawn3IndexEqualP34()    const;

  void p2IndexToOffDiagPos();
  void p3IndexToOffDiagPos();
  void p4IndexToOffDiagPos();
  void p5IndexToOffDiagPos();

  inline void p23IndexToOffDiagPos() {
    p2IndexToOffDiagPos();
    p3IndexToOffDiagPos();
  }
  inline void p34IndexToOffDiagPos() {
    p3IndexToOffDiagPos();
    p4IndexToOffDiagPos();
  }
  inline void p45IndexToOffDiagPos() {
    p4IndexToOffDiagPos();
    p5IndexToOffDiagPos();
  }
  inline void p234IndexToOffDiagPos() {
    p23IndexToOffDiagPos();
    p4IndexToOffDiagPos();
  }
  inline void p345IndexToOffDiagPos() {
    p34IndexToOffDiagPos();
    p5IndexToOffDiagPos();
  }
  inline void p2345IndexToOffDiagPos() {
    p234IndexToOffDiagPos();
    p5IndexToOffDiagPos();
  }

  void p3IndexToOffDiagPosEqualP23();
  void p4IndexToOffDiagPosEqualP34();
  void p5IndexToOffDiagPosEqualP45();

  inline void p23IndexToOffDiagPosEqualP23() {
    p2IndexToOffDiagPos();
    p3IndexToOffDiagPosEqualP23();
  }
  inline void p34IndexToOffDiagPosEqualP34() {
    p3IndexToOffDiagPos();
    p4IndexToOffDiagPosEqualP34();
  }
  inline void p45IndexToOffDiagPosEqualP45() {
    p4IndexToOffDiagPos();
    p5IndexToOffDiagPosEqualP45();
  }

  void p4IndexToOffDiagPosEqualP234();
  void p5IndexToOffDiagPosEqualP345();

  inline void p234IndexToOffDiagPosEqualP234() {
    p23IndexToOffDiagPosEqualP23();
    p4IndexToOffDiagPosEqualP234();
  }
  inline void p345IndexToOffDiagPosEqualP345() {
    p34IndexToOffDiagPosEqualP34();
    p5IndexToOffDiagPosEqualP345();
  }
  void p2IndexToDiagPos();
  void p3IndexToDiagPos();
  void p4IndexToDiagPos();
  void p5IndexToDiagPos();

  inline void p23IndexToDiagPos() {
    p2IndexToDiagPos();
    p3IndexToDiagPos();
  }
  inline void p234IndexToDiagPos() {
    p23IndexToDiagPos();
    p4IndexToDiagPos();
  }
  inline void p2345IndexToDiagPos() {
    p234IndexToDiagPos();
    p5IndexToDiagPos();
  }

  void p3IndexToDiagPosEqualP23();
  void p4IndexToDiagPosEqualP34();
  void p5IndexToDiagPosEqualP45();

  inline void p23IndexToDiagPosEqualP23() {
    p2IndexToDiagPos();
    p3IndexToDiagPosEqualP23();
  }
  inline void p34IndexToDiagPosEqualP34() {
    p3IndexToDiagPos();
    p4IndexToDiagPosEqualP34();
  }
  inline void p45IndexToDiagPosEqualP45() {
    p4IndexToDiagPos();
    p5IndexToDiagPosEqualP45();
  }

  void p4IndexToDiagPosEqualP234();
  void p5IndexToDiagPosEqualP345();

  inline void p34IndexToDiagPosEqualP234() {
    p3IndexToDiagPosEqualP23();
    p4IndexToDiagPosEqualP234();
  }
  inline void p45IndexToDiagPosEqualP345() {
    p4IndexToDiagPosEqualP34();
    p5IndexToDiagPosEqualP345();
  }
  inline void p234IndexToDiagPosEqualP234() {
    p23IndexToDiagPosEqualP23();
    p4IndexToDiagPosEqualP234();
  }
  inline void p345IndexToDiagPosEqualP345() {
    p34IndexToDiagPosEqualP34();
    p5IndexToDiagPosEqualP345();
  }
  void p3IndexToPawn2Pos();
  void p4IndexToPawn3Pos();
  void p4IndexToPawn3PosEqualP34();

  inline void p34IndexToPawn23Pos() {
    p3IndexToPawn2Pos();
    p4IndexToPawn3Pos();
  }

  void p34IndexToPawn23PosEqualP34();

  inline bool kingsOnMainDiag1() const {
    return IS_ONMAINDIAG1(getWhiteKingPosition()) && IS_ONMAINDIAG1(getBlackKingPosition());
  }

  inline bool p2OnMainDiag1() const {
    return IS_ONMAINDIAG1(getPosition(2));
  }
  inline bool p2AboveMainDiag1() const {
    return IS_ABOVEMAINDIAG1(getPosition(2));
  }
  inline bool p2BelowMainDiag1() const {
    return IS_BELOWMAINDIAG1(getPosition(2));
  }

  inline bool p3OnMainDiag1() const {
    return IS_ONMAINDIAG1(getPosition(3));
  }
  inline bool p3AboveMainDiag1() const {
    return IS_ABOVEMAINDIAG1(getPosition(3));
  }
  inline bool p3BelowMainDiag1() const {
    return IS_BELOWMAINDIAG1(getPosition(3));
  }

  inline bool p4OnMainDiag1() const {
    return IS_ONMAINDIAG1(getPosition(4));
  }
  inline bool p4AboveMainDiag1() const {
    return IS_ABOVEMAINDIAG1(getPosition(4));
  }
  inline bool p4BelowMainDiag1() const {
    return IS_BELOWMAINDIAG1(getPosition(4));
  }

  inline bool p5OnMainDiag1() const {
    return IS_ONMAINDIAG1(getPosition(5));
  }
  inline bool p5AboveMainDiag1() const {
    return IS_ABOVEMAINDIAG1(getPosition(5));
  }
  inline bool p5BelowMainDiag1() const {
    return IS_BELOWMAINDIAG1(getPosition(5));
  }

  inline friend bool operator==(EndGameKey k1, EndGameKey k2) {
    return k1.m_hashCode == k2.m_hashCode;
  }

  inline friend bool operator!=(EndGameKey k1, EndGameKey k2) {
    return k1.m_hashCode != k2.m_hashCode;
  }

  inline ULONG hashCode() const {
    return uint64Hash(m_hashCode);
  }

  String toString(const EndGameKeyDefinition &keydef, bool initFormat=false) const;

  inline void swapPos(UINT i1, UINT i2) {
    const BYTE tmp = m_pos[i1]; m_pos[i1] = m_pos[i2]; m_pos[i2] = tmp;
  }
  inline void sort2Pos(UINT i1, UINT i2) {
    if(m_pos[i1] > m_pos[i2]) swapPos(i1,i2);
  }
  inline void sort3Pos(UINT i1, UINT i2, UINT i3) {
    sort2Pos(i1,i2);
    if(m_pos[i2] > m_pos[i3]) {
      swapPos(i2,i3);
      sort2Pos(i1,i2);
    }
  }
};

#ifdef TABLEBASE_BUILDER

class EndGameKeyWithOccupiedPositions : public EndGameKey {
private:
  FieldSet m_occupiedPositions;

public:
  inline void setPosition(UINT pIndex, UINT pos) {
    EndGameKey::setPosition(pIndex, pos);
    m_occupiedPositions.add(pos);
  }
  inline bool isOccupied( UINT pos) const {
    return m_occupiedPositions.contains(pos);
  }
  inline void setWhiteKingPosition(UINT pos) {
    setPosition(0,pos);
  }
  inline void setBlackKingPosition(UINT pos) {
    setPosition(1,pos);
  }
  inline void clearField( UINT pos) {
    m_occupiedPositions.remove(pos);
  }
  inline void clear() {
    EndGameKey::clear();
    m_occupiedPositions.clear();
  }
};

#endif
