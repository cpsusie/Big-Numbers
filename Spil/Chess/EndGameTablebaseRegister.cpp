#include "stdafx.h"
#include "EndGame3Men.h"
#include "EndGame4MenNoPawns.h"
#include "EndGame4MenWithPawns.h"
#include "EndGame5MenNoPawns.h"
#include "EndGame5MenWithPawns.h"
#include "EndGame6MenNoPawns.h"

class TablebaseRegister : public EndGameTablebaseList {
public:
  TablebaseRegister();
  ~TablebaseRegister();
};

TablebaseRegister::TablebaseRegister() {
  // 3 men
  add(new EndGameKPK());    // King-Pawn                 / King
  add(new EndGameKQK());    // King-Queen                / King
  add(new EndGameKRK());    // King-Rook                 / King
    // 4 men
  add(new EndGameKBBK());   // King-Bishop-Bishop        / King
  add(new EndGameKBKB());   // King-Bishop               / King-Bishop
  add(new EndGameKBKN());   // King-Bishop               / King-Knight
  add(new EndGameKBNK());   // King-Bishop-Knight        / King
  add(new EndGameKBPK());   // King-Bishop-Pawn          / King
  add(new EndGameKNKN());   // King-Knight               / King-Knight
  add(new EndGameKNNK());   // King-Knight-Knight        / King
  add(new EndGameKNPK());   // King-Knight-Pawn          / King
  add(new EndGameKPKB());   // King-Pawn                 / King-Bishop
  add(new EndGameKPKN());   // King-Pawn                 / King-Knight
  add(new EndGameKPKP());   // King-Pawn                 / King-Pawn
  add(new EndGameKPPK());   // King-Pawn-Pawn            / King
  add(new EndGameKQBK());   // King-Queen-Bishop         / King
  add(new EndGameKQKB());   // King-Queen                / King-Bishop
  add(new EndGameKQKN());   // King-Queen                / King-Knight
  add(new EndGameKQKP());   // King-Queen                / King-Pawn
  add(new EndGameKQKQ());   // King-Queen                / King-Queen
  add(new EndGameKQKR());   // King-Queen                / King-Rook
  add(new EndGameKQNK());   // King-Queen-Knight         / King
  add(new EndGameKQPK());   // King-Queen-Pawn           / King
  add(new EndGameKQQK());   // King-Queen-Queen          / King
  add(new EndGameKQRK());   // King-Queen-Rook           / King
  add(new EndGameKRBK());   // King-Rook-Bishop          / King
  add(new EndGameKRKB());   // King-Rook                 / King-Bishop
  add(new EndGameKRKN());   // King-Rook                 / King-Knight
  add(new EndGameKRKP());   // King-Rook                 / King-Pawn
  add(new EndGameKRKR());   // King-Rook                 / King-Rook
  add(new EndGameKRNK());   // King-Rook-Knight          / King
  add(new EndGameKRPK());   // King-Rook-Pawn            / King
  add(new EndGameKRRK());   // King-Rook-Rook            / King
    // 5 men
  add(new EndGameKBBBK());  // King-Bishop-Bishop-Bishop / King
  add(new EndGameKBBKB());  // King-Bishop-Bishop        / King-Bishop
  add(new EndGameKBBKN());  // King-Bishop-Bishop        / King-Knight
  add(new EndGameKBBKP());  // King-Bishop-Bishop        / King-Pawn
  add(new EndGameKBBKR());  // King-Bishop-Bishop        / King-Rook
  add(new EndGameKBBNK());  // King-Bishop-Bishop-Knight / King
  add(new EndGameKBBPK());  // King-Bishop-Bishop-Pawn   / King
  add(new EndGameKBNKB());  // King-Bishop-Knight        / King-Bishop
  add(new EndGameKBNKN());  // King-Bishop-Knight        / King-Knight
  add(new EndGameKBNKP());  // King-Bishop-Knight        / King-Pawn
  add(new EndGameKBNKR());  // King-Bishop-Knight        / King-Rook
  add(new EndGameKBNNK());  // King-Bishop-Knight-Knight / King
  add(new EndGameKBNPK());  // King-Bishop-Knight-Pawn   / King
  add(new EndGameKBPKB());  // King-Bishop-Pawn          / King-Bishop
  add(new EndGameKBPKN());  // King-Bishop-Pawn          / King-Knight
  add(new EndGameKBPKP());  // King-Bishop-Pawn          / King-Pawn
  add(new EndGameKBPKQ());  // King-Bishop-Pawn          / King-Queen
  add(new EndGameKBPKR());  // King-Bishop-Pawn          / King-Rook
  add(new EndGameKBPPK());  // King-Bishop-Pawn-Pawn     / King
  add(new EndGameKNNKB());  // King-Knight-Knight        / King-Bishop
  add(new EndGameKNNKN());  // King-Knight-Knight        / King-Knight
  add(new EndGameKNNKP());  // King-Knight-Knight        / King-Pawn
  add(new EndGameKNNNK());  // King-Knight-Knight-Knight / King
  add(new EndGameKNNPK());  // King-Knight-Knight-Pawn   / King
  add(new EndGameKNPKB());  // King-Knight-Pawn          / King-Bishop
  add(new EndGameKNPKN());  // King-Knight-Pawn          / King-Knight
  add(new EndGameKNPKP());  // King-Knight-Pawn          / King-Pawn
  add(new EndGameKNPKQ());  // King-Knight-Pawn          / King-Queen
  add(new EndGameKNPKR());  // King-Knight-Pawn          / King-Rook
  add(new EndGameKNPPK());  // King-Knight-Pawn-Pawn     / King
  add(new EndGameKPPKB());  // King-Pawn-Pawn            / King-Bishop
  add(new EndGameKPPKN());  // King-Pawn-Pawn            / King-Knight
  add(new EndGameKPPKP());  // King-Pawn-Pawn            / King-Pawn
  add(new EndGameKPPKQ());  // King-Pawn-Pawn            / King-Queen
  add(new EndGameKPPKR());  // King-Pawn-Pawn            / King-Rook
  add(new EndGameKPPPK());  // King-Pawn-Pawn-Pawn       / King
  add(new EndGameKQBBK());  // King-Queen-Bishop-Bishop  / King
  add(new EndGameKQBKB());  // King-Queen-Bishop         / King-Bishop
  add(new EndGameKQBKN());  // King-Queen-Bishop         / King-Knight
  add(new EndGameKQBKP());  // King-Queen-Bishop         / King-Pawn
  add(new EndGameKQBKQ());  // King-Queen-Bishop         / King-Queen
  add(new EndGameKQBKR());  // King-Queen-Bishop         / King-Rook
  add(new EndGameKQBNK());  // King-Queen-Bishop-Knight  / King
  add(new EndGameKQBPK());  // King-Queen-Bishop-Pawn    / King
  add(new EndGameKQKBB());  // King-Queen                / King-Bishop-Bishop
  add(new EndGameKQKBN());  // King-Queen                / King-Bishop-Knight
  add(new EndGameKQKNN());  // King-Queen                / King-Knight-Knight
  add(new EndGameKQKRB());  // King-Queen                / King-Rook-Bishop
  add(new EndGameKQKRN());  // King-Queen                / King-Rook-Knight
  add(new EndGameKQKRR());  // King-Queen                / King-Rook-Rook
  add(new EndGameKQNKB());  // King-Queen-Knight         / King-Bishop
  add(new EndGameKQNKN());  // King-Queen-Knight         / King-Knight
  add(new EndGameKQNKP());  // King-Queen-Knight         / King-Pawn
  add(new EndGameKQNKQ());  // King-Queen-Knight         / King-Queen
  add(new EndGameKQNKR());  // King-Queen-Knight         / King-Rook
  add(new EndGameKQNNK());  // King-Queen-Knight-Knight  / King
  add(new EndGameKQNPK());  // King-Queen-Knight-Pawn    / King
  add(new EndGameKQPKB());  // King-Queen-Pawn           / King-Bishop
  add(new EndGameKQPKN());  // King-Queen-Pawn           / King-Knight
  add(new EndGameKQPKP());  // King-Queen-Pawn           / King-Pawn
  add(new EndGameKQPKQ());  // King-Queen-Pawn           / King-Queen
  add(new EndGameKQPKR());  // King-Queen-Pawn           / King-Rook
  add(new EndGameKQPPK());  // King-Queen-Pawn-Pawn      / King
  add(new EndGameKQQBK());  // King-Queen-Queen-Bishop   / King
  add(new EndGameKQQKB());  // King-Queen-Queen          / King-Bishop
  add(new EndGameKQQKN());  // King-Queen-Queen          / King-Knight
  add(new EndGameKQQKP());  // King-Queen-Queen          / King-Pawn
  add(new EndGameKQQKQ());  // King-Queen-Queen          / King-Queen
  add(new EndGameKQQKR());  // King-Queen-Queen          / King-Rook
  add(new EndGameKQQNK());  // King-Queen-Queen-Knight   / King
  add(new EndGameKQQPK());  // King-Queen-Queen-Pawn     / King
  add(new EndGameKQQQK());  // King-Queen-Queen-Queen    / King
  add(new EndGameKQQRK());  // King-Queen-Queen-Rook     / King
  add(new EndGameKQRBK());  // King-Queen-Rook-Bishop    / King
  add(new EndGameKQRKB());  // King-Queen-Rook           / King-Bishop
  add(new EndGameKQRKN());  // King-Queen-Rook           / King-Knight
  add(new EndGameKQRKP());  // King-Queen-Rook           / King-Pawn
  add(new EndGameKQRKQ());  // King-Queen-Rook           / King-Queen
  add(new EndGameKQRKR());  // King-Queen-Rook           / King-Rook
  add(new EndGameKQRNK());  // King-Queen-Rook-Knight    / King
  add(new EndGameKQRPK());  // King-Queen-Rook-Pawn      / King
  add(new EndGameKQRRK());  // King-Queen-Rook-Rook      / King
  add(new EndGameKRBBK());  // King-Rook-Bishop-Bishop   / King
  add(new EndGameKRBKB());  // King-Rook-Bishop          / King-Bishop
  add(new EndGameKRBKN());  // King-Rook-Bishop          / King-Knight
  add(new EndGameKRBKP());  // King-Rook-Bishop          / King-Pawn
  add(new EndGameKRBKR());  // King-Rook-Bishop          / King-Rook
  add(new EndGameKRBNK());  // King-Rook-Bishop-Knight   / King
  add(new EndGameKRBPK());  // King-Rook-Bishop-Pawn     / King
  add(new EndGameKRKNN());  // King-Rook                 / King-Knight-Knight
  add(new EndGameKRNKB());  // King-Rook-Knight          / King-Bishop
  add(new EndGameKRNKN());  // King-Rook-Knight          / King-Knight
  add(new EndGameKRNKP());  // King-Rook-Knight          / King-Pawn
  add(new EndGameKRNKR());  // King-Rook-Knight          / King-Rook
  add(new EndGameKRNNK());  // King-Rook-Knight-Knight   / King
  add(new EndGameKRNPK());  // King-Rook-Knight-Pawn     / King
  add(new EndGameKRPKB());  // King-Rook-Pawn            / King-Bishop
  add(new EndGameKRPKN());  // King-Rook-Pawn            / King-Knight
  add(new EndGameKRPKP());  // King-Rook-Pawn            / King-Pawn
  add(new EndGameKRPKQ());  // King-Rook-Pawn            / King-Queen
  add(new EndGameKRPKR());  // King-Rook-Pawn            / King-Rook
  add(new EndGameKRPPK());  // King-Rook-Pawn-Pawn       / King
  add(new EndGameKRRBK());  // King-Rook-Rook-Bishop     / King
  add(new EndGameKRRKB());  // King-Rook-Rook            / King-Bishop
  add(new EndGameKRRKN());  // King-Rook-Rook            / King-Knight
  add(new EndGameKRRKP());  // King-Rook-Rook            / King-Pawn
  add(new EndGameKRRKR());  // King-Rook-Rook            / King-Rook
  add(new EndGameKRRNK());  // King-Rook-Rook-Knight     / King
  add(new EndGameKRRPK());  // King-Rook-Rook-Pawn       / King
  add(new EndGameKRRRK());  // King-Rook-Rook-Rook       / King

    // 6 men
  add(new EndGameKBKBBB()); // 3 equal (no pawns)
  add(new EndGameKBKNNN());
  add(new EndGameKBKQQQ());
  add(new EndGameKBKRRR());
  add(new EndGameKNKBBB());
  add(new EndGameKNKNNN());
  add(new EndGameKNKQQQ());
  add(new EndGameKNKRRR());
  add(new EndGameKQKBBB());
  add(new EndGameKQKNNN());
  add(new EndGameKQKQQQ());
  add(new EndGameKQKRRR());
  add(new EndGameKRKBBB());
  add(new EndGameKRKNNN());
  add(new EndGameKRKQQQ());
  add(new EndGameKRKRRR());
  add(new EndGameKBBKBB()); // 2 pairs (no pawns)
  add(new EndGameKBBKNN());
  add(new EndGameKNNKNN());
  add(new EndGameKQQKBB());
  add(new EndGameKQQKNN());
  add(new EndGameKQQKQQ());
  add(new EndGameKQQKRR());
  add(new EndGameKRRKBB());
  add(new EndGameKRRKNN());
  add(new EndGameKRRKRR());
  add(new EndGameKQRKBN());
  add(new EndGameKQRKRR()); // 2 equal (no pawns)
}

TablebaseRegister::~TablebaseRegister() {
  for(size_t i = 0; i < size(); i++) {
    delete (*this)[i];
  }
  clear();
}

const EndGameTablebaseList &EndGameTablebase::getRegisteredEndGameTablebases() { // static
  static TablebaseRegister allTablebases;
  return allTablebases;
}

EndGameTablebaseList EndGameTablebase::getExistingEndGameTablebases() { // static
  EndGameTablebaseList list = getRegisteredEndGameTablebases();
  for(int i = (int)list.size()-1; i >= 0; i--) {
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

ULONG PositionSignature::hashCode() const {
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

