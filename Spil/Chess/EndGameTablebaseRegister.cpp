#include "stdafx.h"
#include "EndGame3Men.h"
#include "EndGame4MenNoPawns.h"
#include "EndGame4MenWithPawns.h"
#include "EndGame5MenNoPawns.h"
#include "EndGame5MenWithPawns.h"

const EndGameTablebaseList &EndGameTablebase::getRegisteredEndGameTablebases() { // static 
  static EndGameTablebaseList tablebaseList(143);
  if(tablebaseList.size() == 0) {
    tablebaseList.add(new EndGameKPK());    // King-Pawn                 / King
    tablebaseList.add(new EndGameKQK());    // King-Queen                / King
    tablebaseList.add(new EndGameKRK());    // King-Rook                 / King
    tablebaseList.add(new EndGameKBBK());   // King-Bishop-Bishop        / King
    tablebaseList.add(new EndGameKBKB());   // King-Bishop               / King-Bishop
    tablebaseList.add(new EndGameKBKN());   // King-Bishop               / King-Knight
    tablebaseList.add(new EndGameKBNK());   // King-Bishop-Knight        / King
    tablebaseList.add(new EndGameKBPK());   // King-Bishop-Pawn          / King
    tablebaseList.add(new EndGameKNKN());   // King-Knight               / King-Knight
    tablebaseList.add(new EndGameKNNK());   // King-Knight-Knight        / King
    tablebaseList.add(new EndGameKNPK());   // King-Knight-Pawn          / King
    tablebaseList.add(new EndGameKPKB());   // King-Pawn                 / King-Bishop
    tablebaseList.add(new EndGameKPKN());   // King-Pawn                 / King-Knight
    tablebaseList.add(new EndGameKPKP());   // King-Pawn                 / King-Pawn
    tablebaseList.add(new EndGameKPPK());   // King-Pawn-Pawn            / King
    tablebaseList.add(new EndGameKQBK());   // King-Queen-Bishop         / King
    tablebaseList.add(new EndGameKQKB());   // King-Queen                / King-Bishop
    tablebaseList.add(new EndGameKQKN());   // King-Queen                / King-Knight
    tablebaseList.add(new EndGameKQKP());   // King-Queen                / King-Pawn
    tablebaseList.add(new EndGameKQKQ());   // King-Queen                / King-Queen
    tablebaseList.add(new EndGameKQKR());   // King-Queen                / King-Rook
    tablebaseList.add(new EndGameKQNK());   // King-Queen-Knight         / King
    tablebaseList.add(new EndGameKQPK());   // King-Queen-Pawn           / King
    tablebaseList.add(new EndGameKQQK());   // King-Queen-Queen          / King
    tablebaseList.add(new EndGameKQRK());   // King-Queen-Rook           / King
    tablebaseList.add(new EndGameKRBK());   // King-Rook-Bishop          / King
    tablebaseList.add(new EndGameKRKB());   // King-Rook                 / King-Bishop
    tablebaseList.add(new EndGameKRKN());   // King-Rook                 / King-Knight
    tablebaseList.add(new EndGameKRKP());   // King-Rook                 / King-Pawn
    tablebaseList.add(new EndGameKRKR());   // King-Rook                 / King-Rook
    tablebaseList.add(new EndGameKRNK());   // King-Rook-Knight          / King
    tablebaseList.add(new EndGameKRPK());   // King-Rook-Pawn            / King
    tablebaseList.add(new EndGameKRRK());   // King-Rook-Rook            / King
    tablebaseList.add(new EndGameKBBBK());  // King-Bishop-Bishop-Bishop / King
    tablebaseList.add(new EndGameKBBKB());  // King-Bishop-Bishop        / King-Bishop
    tablebaseList.add(new EndGameKBBKN());  // King-Bishop-Bishop        / King-Knight
    tablebaseList.add(new EndGameKBBKP());  // King-Bishop-Bishop        / King-Pawn
    tablebaseList.add(new EndGameKBBKR());  // King-Bishop-Bishop        / King-Rook
    tablebaseList.add(new EndGameKBBNK());  // King-Bishop-Bishop-Knight / King
    tablebaseList.add(new EndGameKBBPK());  // King-Bishop-Bishop-Pawn   / King
    tablebaseList.add(new EndGameKBNKB());  // King-Bishop-Knight        / King-Bishop
    tablebaseList.add(new EndGameKBNKN());  // King-Bishop-Knight        / King-Knight
    tablebaseList.add(new EndGameKBNKP());  // King-Bishop-Knight        / King-Pawn
    tablebaseList.add(new EndGameKBNKR());  // King-Bishop-Knight        / King-Rook
    tablebaseList.add(new EndGameKBNNK());  // King-Bishop-Knight-Knight / King
    tablebaseList.add(new EndGameKBNPK());  // King-Bishop-Knight-Pawn   / King
    tablebaseList.add(new EndGameKBPKB());  // King-Bishop-Pawn          / King-Bishop
    tablebaseList.add(new EndGameKBPKN());  // King-Bishop-Pawn          / King-Knight
    tablebaseList.add(new EndGameKBPKP());  // King-Bishop-Pawn          / King-Pawn
    tablebaseList.add(new EndGameKBPKQ());  // King-Bishop-Pawn          / King-Queen
    tablebaseList.add(new EndGameKBPKR());  // King-Bishop-Pawn          / King-Rook
    tablebaseList.add(new EndGameKBPPK());  // King-Bishop-Pawn-Pawn     / King
    tablebaseList.add(new EndGameKNNKB());  // King-Knight-Knight        / King-Bishop
    tablebaseList.add(new EndGameKNNKN());  // King-Knight-Knight        / King-Knight
    tablebaseList.add(new EndGameKNNKP());  // King-Knight-Knight        / King-Pawn
    tablebaseList.add(new EndGameKNNNK());  // King-Knight-Knight-Knight / King
    tablebaseList.add(new EndGameKNNPK());  // King-Knight-Knight-Pawn   / King
    tablebaseList.add(new EndGameKNPKB());  // King-Knight-Pawn          / King-Bishop
    tablebaseList.add(new EndGameKNPKN());  // King-Knight-Pawn          / King-Knight
    tablebaseList.add(new EndGameKNPKP());  // King-Knight-Pawn          / King-Pawn
    tablebaseList.add(new EndGameKNPKQ());  // King-Knight-Pawn          / King-Queen
    tablebaseList.add(new EndGameKNPKR());  // King-Knight-Pawn          / King-Rook
    tablebaseList.add(new EndGameKNPPK());  // King-Knight-Pawn-Pawn     / King
    tablebaseList.add(new EndGameKPPKB());  // King-Pawn-Pawn            / King-Bishop
    tablebaseList.add(new EndGameKPPKN());  // King-Pawn-Pawn            / King-Knight
    tablebaseList.add(new EndGameKPPKP());  // King-Pawn-Pawn            / King-Pawn
    tablebaseList.add(new EndGameKPPKQ());  // King-Pawn-Pawn            / King-Queen
    tablebaseList.add(new EndGameKPPKR());  // King-Pawn-Pawn            / King-Rook
    tablebaseList.add(new EndGameKPPPK());  // King-Pawn-Pawn-Pawn       / King
    tablebaseList.add(new EndGameKQBBK());  // King-Queen-Bishop-Bishop  / King
    tablebaseList.add(new EndGameKQBKB());  // King-Queen-Bishop         / King-Bishop
    tablebaseList.add(new EndGameKQBKN());  // King-Queen-Bishop         / King-Knight
    tablebaseList.add(new EndGameKQBKP());  // King-Queen-Bishop         / King-Pawn
    tablebaseList.add(new EndGameKQBKQ());  // King-Queen-Bishop         / King-Queen
    tablebaseList.add(new EndGameKQBKR());  // King-Queen-Bishop         / King-Rook
    tablebaseList.add(new EndGameKQBNK());  // King-Queen-Bishop-Knight  / King
    tablebaseList.add(new EndGameKQBPK());  // King-Queen-Bishop-Pawn    / King
    tablebaseList.add(new EndGameKQKBB());  // King-Queen                / King-Bishop-Bishop
    tablebaseList.add(new EndGameKQKBN());  // King-Queen                / King-Bishop-Knight
    tablebaseList.add(new EndGameKQKNN());  // King-Queen                / King-Knight-Knight
    tablebaseList.add(new EndGameKQKRB());  // King-Queen                / King-Rook-Bishop
    tablebaseList.add(new EndGameKQKRN());  // King-Queen                / King-Rook-Knight
    tablebaseList.add(new EndGameKQKRR());  // King-Queen                / King-Rook-Rook
    tablebaseList.add(new EndGameKQNKB());  // King-Queen-Knight         / King-Bishop
    tablebaseList.add(new EndGameKQNKN());  // King-Queen-Knight         / King-Knight
    tablebaseList.add(new EndGameKQNKP());  // King-Queen-Knight         / King-Pawn
    tablebaseList.add(new EndGameKQNKQ());  // King-Queen-Knight         / King-Queen
    tablebaseList.add(new EndGameKQNKR());  // King-Queen-Knight         / King-Rook
    tablebaseList.add(new EndGameKQNNK());  // King-Queen-Knight-Knight  / King
    tablebaseList.add(new EndGameKQNPK());  // King-Queen-Knight-Pawn    / King
    tablebaseList.add(new EndGameKQPKB());  // King-Queen-Pawn           / King-Bishop
    tablebaseList.add(new EndGameKQPKN());  // King-Queen-Pawn           / King-Knight
    tablebaseList.add(new EndGameKQPKP());  // King-Queen-Pawn           / King-Pawn
    tablebaseList.add(new EndGameKQPKQ());  // King-Queen-Pawn           / King-Queen
    tablebaseList.add(new EndGameKQPKR());  // King-Queen-Pawn           / King-Rook
    tablebaseList.add(new EndGameKQPPK());  // King-Queen-Pawn-Pawn      / King
    tablebaseList.add(new EndGameKQQBK());  // King-Queen-Queen-Bishop   / King
    tablebaseList.add(new EndGameKQQKB());  // King-Queen-Queen          / King-Bishop
    tablebaseList.add(new EndGameKQQKN());  // King-Queen-Queen          / King-Knight
    tablebaseList.add(new EndGameKQQKP());  // King-Queen-Queen          / King-Pawn
    tablebaseList.add(new EndGameKQQKQ());  // King-Queen-Queen          / King-Queen
    tablebaseList.add(new EndGameKQQKR());  // King-Queen-Queen          / King-Rook
    tablebaseList.add(new EndGameKQQNK());  // King-Queen-Queen-Knight   / King
    tablebaseList.add(new EndGameKQQPK());  // King-Queen-Queen-Pawn     / King
    tablebaseList.add(new EndGameKQQQK());  // King-Queen-Queen-Queen    / King
    tablebaseList.add(new EndGameKQQRK());  // King-Queen-Queen-Rook     / King
    tablebaseList.add(new EndGameKQRBK());  // King-Queen-Rook-Bishop    / King
    tablebaseList.add(new EndGameKQRKB());  // King-Queen-Rook           / King-Bishop
    tablebaseList.add(new EndGameKQRKN());  // King-Queen-Rook           / King-Knight
    tablebaseList.add(new EndGameKQRKP());  // King-Queen-Rook           / King-Pawn
    tablebaseList.add(new EndGameKQRKQ());  // King-Queen-Rook           / King-Queen
    tablebaseList.add(new EndGameKQRKR());  // King-Queen-Rook           / King-Rook
    tablebaseList.add(new EndGameKQRNK());  // King-Queen-Rook-Knight    / King
    tablebaseList.add(new EndGameKQRPK());  // King-Queen-Rook-Pawn      / King
    tablebaseList.add(new EndGameKQRRK());  // King-Queen-Rook-Rook      / King
    tablebaseList.add(new EndGameKRBBK());  // King-Rook-Bishop-Bishop   / King
    tablebaseList.add(new EndGameKRBKB());  // King-Rook-Bishop          / King-Bishop
    tablebaseList.add(new EndGameKRBKN());  // King-Rook-Bishop          / King-Knight
    tablebaseList.add(new EndGameKRBKP());  // King-Rook-Bishop          / King-Pawn
    tablebaseList.add(new EndGameKRBKR());  // King-Rook-Bishop          / King-Rook
    tablebaseList.add(new EndGameKRBNK());  // King-Rook-Bishop-Knight   / King
    tablebaseList.add(new EndGameKRBPK());  // King-Rook-Bishop-Pawn     / King
    tablebaseList.add(new EndGameKRKNN());  // King-Rook                 / King-Knight-Knight
    tablebaseList.add(new EndGameKRNKB());  // King-Rook-Knight          / King-Bishop
    tablebaseList.add(new EndGameKRNKN());  // King-Rook-Knight          / King-Knight
    tablebaseList.add(new EndGameKRNKP());  // King-Rook-Knight          / King-Pawn
    tablebaseList.add(new EndGameKRNKR());  // King-Rook-Knight          / King-Rook
    tablebaseList.add(new EndGameKRNNK());  // King-Rook-Knight-Knight   / King
    tablebaseList.add(new EndGameKRNPK());  // King-Rook-Knight-Pawn     / King
    tablebaseList.add(new EndGameKRPKB());  // King-Rook-Pawn            / King-Bishop
    tablebaseList.add(new EndGameKRPKN());  // King-Rook-Pawn            / King-Knight
    tablebaseList.add(new EndGameKRPKP());  // King-Rook-Pawn            / King-Pawn
    tablebaseList.add(new EndGameKRPKQ());  // King-Rook-Pawn            / King-Queen
    tablebaseList.add(new EndGameKRPKR());  // King-Rook-Pawn            / King-Rook
    tablebaseList.add(new EndGameKRPPK());  // King-Rook-Pawn-Pawn       / King
    tablebaseList.add(new EndGameKRRBK());  // King-Rook-Rook-Bishop     / King
    tablebaseList.add(new EndGameKRRKB());  // King-Rook-Rook            / King-Bishop
    tablebaseList.add(new EndGameKRRKN());  // King-Rook-Rook            / King-Knight
    tablebaseList.add(new EndGameKRRKP());  // King-Rook-Rook            / King-Pawn
    tablebaseList.add(new EndGameKRRKR());  // King-Rook-Rook            / King-Rook
    tablebaseList.add(new EndGameKRRNK());  // King-Rook-Rook-Knight     / King
    tablebaseList.add(new EndGameKRRPK());  // King-Rook-Rook-Pawn       / King
    tablebaseList.add(new EndGameKRRRK());  // King-Rook-Rook-Rook       / King

  }
  return tablebaseList;
}

EndGameTablebaseList EndGameTablebase::getExistingEndGameTablebases() { // static
  EndGameTablebaseList list = getRegisteredEndGameTablebases();
  for(int i = list.size()-1; i >= 0; i--) {
    EndGameTablebase &db = *list[i];
    if(!db.exist(COMPRESSEDTABLEBASE)) {
      list.remove(i);
    }
  }
  return list;
}

EndGameTablebase &EndGameTablebase::getInstanceByName(const String &name) { // static 
  const EndGameTablebaseList &list = getRegisteredEndGameTablebases();
  for(size_t i = 0; i < list.size(); i++) {
    EndGameTablebase &db = *list[i];
    if(db.getName().equalsIgnoreCase(name)) {
      return db;
    }
  }
  throwException(_T("Endgame tablebase <%s> does not exist"), name.cstr());
  return *list[0];
}

//#define LIST_MAPCAPACITY

#ifdef TEST_HASHFACTOR

#define LIST_MAPCAPACITY

int PositionSignature::hashFactor =  2716389;

unsigned long PositionSignature::hashCode() const {
  return (m_pieceTypes[WHITEPLAYER].hashCode() * hashFactor ) ^ m_pieceTypes[BLACKPLAYER].hashCode();
}
#define DUMPHASHFACTOR() verbose(_T("HashFactor:%8d, "), PositionSignature::hashFactor);

#else

#define DUMPHASHFACTOR()

#endif

class TablebaseWithSwap {
public:
  EndGameTablebase *m_db;
  bool              m_swap;
  TablebaseWithSwap() : m_db(NULL), m_swap(false) {
  }
  TablebaseWithSwap(EndGameTablebase *db, bool swap) : m_db(db), m_swap(swap) {
  }
};

class SignatureTablebaseMap : public CompactHashMap<PositionSignature, TablebaseWithSwap> {
private:

#ifdef LIST_MAPCAPACITY
  void dump() const;
#define DUMP() dump()
#else
#define DUMP()
#endif

  void init(int capacity);
public:
#ifndef TEST_HASHFACTOR
  SignatureTablebaseMap() {
    init(707);
  }
#else
  SignatureTablebaseMap();
#endif
};

void SignatureTablebaseMap::init(int capacity) {
  const EndGameTablebaseList &list = EndGameTablebase::getRegisteredEndGameTablebases();
  clear();
  setCapacity(capacity);
  for(size_t i = 0; i < list.size(); i++) {
    EndGameTablebase *db = list[i];
    const PositionSignature signature1 = db->getKeyDefinition().getPositionSignature();
    put(signature1, TablebaseWithSwap(db, false));
    const PositionSignature signature2 = signature1.swapPlayers();
    if(signature2 != signature1) {
      put(signature2, TablebaseWithSwap(db, true));
    }
  }
  DUMP();
}

#ifdef TEST_HASHFACTOR
SignatureTablebaseMap::SignatureTablebaseMap() {
  randomize();
  for(int t = 1; t < 300; t++) {
    PositionSignature::hashFactor = randInt();
    verbose(_T("t:%3d,\n"), t);
    for(int c = 281; c < 730; c++) {
      init(c);
    }
  }
}
#endif

#ifdef LIST_MAPCAPACITY
void SignatureTablebaseMap::dump() const {
  const int mcl = getMaxChainLength();
  if(mcl == 1) {
    DUMPHASHFACTOR();
    verbose(_T("capacity:%3d, mcl:%2d, "), getCapacity(), mcl);
    return;
  } else if(mcl == 2) {
    CompactIntArray l = getLength();
    if(l[2] <= 20) {
      DUMPHASHFACTOR();
      verbose(_T("capacity:%3d, mcl:%2d, Chainlengts:%s\n")
             , getCapacity(), mcl, l.toStringBasicType().cstr());
    }
  }
}
#endif

EndGameTablebase *EndGameTablebase::getInstanceBySignature(const PositionSignature &signature, bool &swap) { // static
  static const SignatureTablebaseMap map;

  const TablebaseWithSwap *dbs = map.get(signature);
  if(dbs) {
    swap = dbs->m_swap;
    return dbs->m_db;
  }
  return NULL;
/*
  const EndGameTablebaseList &list = getRegisteredEndGameTablebases();
  for(int i = 0; i < list.size(); i++) {
    EndGameTablebase *db = list[i];
    if(db->getKeyDefinition().getPositionSignature().match(signature, swap)) {
      return db;
    }
  }
  return NULL; // no tablebase matches the given signatur
*/
}

