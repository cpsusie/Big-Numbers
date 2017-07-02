#include "stdafx.h"
#include <io.h>
#include "EndGameUtil.h"

// _________________________________________ 1.4 _________________________________________

const char *TablebaseInfo::s_programVersion = "1.4";

void TablebaseInfo::checkVersion() const {
  if(getVersion() != getProgramVersion()) {
    throw WrongVersionException(format(_T("Wrong fileversion:%s. Programversion=%s"), getVersion().cstr(), getProgramVersion().cstr()).cstr());
  }
}

void TablebaseInfo::clear() {
  memset(this, 0, sizeof(TablebaseInfo));
}

void TablebaseInfo::save(ByteOutputStream &s) const {
  memset(m_version, 0, sizeof(m_version));
  strcpy(m_version, s_programVersion);
  s.putBytes((BYTE*)this, sizeof(TablebaseInfo));
}

void TablebaseInfo::load(ByteInputStream &s) {
  s.getBytesForced((BYTE*)this, sizeof(TablebaseInfo));
  checkVersion();
}

#ifdef NEWCOMPRESSION
template<class T> BigEndianOutputStream &operator<<(BigEndianOutputStream &s, const TwoCountersTemplate<T> &c) {
  return s << c.m_count[0] << c.m_count[1];
}

template<class T> BigEndianInputStream &operator>>(BigEndianInputStream &s, TwoCountersTemplate<T> &c) {
  return s >> c.m_count[0] >> c.m_count[1];
}

void TablebaseInfo::save(BigEndianOutputStream &s) const {
  memset(m_version, 0, sizeof(m_version));
  strcpy(m_version, s_programVersion);
  s.putBytes((BYTE*)m_version, sizeof(m_version));
  s << m_totalPositions
    << m_indexCapacity
    << m_undefinedPositions
    << m_stalematePositions
    << m_drawPositions
    << m_checkMatePositions
    << m_terminalWinPositions
    << m_nonTerminalWinPositions
    << m_maxPlies
    << m_canWin[0] << m_canWin[1]
    << m_stateFlags
    << m_buildTime
    << m_consistencyCheckedTime;
}

void TablebaseInfo::load(BigEndianInputStream &s) {
  s.getBytes((BYTE*)m_version, sizeof(m_version));
  checkVersion();

  s >> m_totalPositions
    >> m_indexCapacity
    >> m_undefinedPositions
    >> m_stalematePositions
    >> m_drawPositions
    >> m_checkMatePositions
    >> m_terminalWinPositions
    >> m_nonTerminalWinPositions
    >> m_maxPlies
    >> m_canWin[0] >> m_canWin[1]
    >> m_stateFlags
    >> m_buildTime
    >> m_consistencyCheckedTime;
}
#endif // NEWCOMPRESSION


#ifdef TABLEBASE_BUILDER
static const TCHAR *POSITIONSTR     = _T("Positions");
static const TCHAR *INDEXSIZESTR    = _T("Indexsize");
static const TCHAR *UNDEFINEDSTR    = _T("Undefined");
static const TCHAR *STALEMATESTR    = _T("Stalemates");
static const TCHAR *MATEPOSSTR      = _T("Checkmates");
static const TCHAR *TERMPOSSTR      = _T("Terminal positions");
static const TCHAR *NTPOSSTR        = _T("Non term positions");
static const TCHAR *MAXVARSTR       = _T("Max var");
static const TCHAR *VERSIONSTR      = _T("Vers");
static const TCHAR *WINNERPOSSTR    = _T("Winnerpositions");

#define FILLER(l) spaceString(l,_T(' ')).cstr()

static String wbHeader1(const TCHAR *title, int fieldWidth) {
  const UINT titleWidth = WBWIDTH(fieldWidth);
  String result = title;
  while (result.length() < titleWidth) {
    result += _T('-');
    if (result.length() < titleWidth) {
      result = _T("-") + result;
    }
  }
  return result;
}

static String wbHeader2(int fieldWidth) {
  return format(_T("%*s %*s"), fieldWidth, _T("W.win"), fieldWidth, _T("B.win"));
}

String TablebaseInfo::getColumnHeaders(TablebaseInfoStringFormat f, const String &headerLeft, const String &headerRight, bool plies) {   // static
  switch(f) {
  case TBIFORMAT_PRINT_COLUMNS1:
    { const String h1 = format(_T("%*s %*s %*s %*s %s %s %s %*s %-*s")
                              ,POSITIONWIDTH       , POSITIONSTR
                              ,INDEXSIZEWIDTH      , INDEXSIZESTR
                              ,UNDEFINEDWIDTH      , UNDEFINEDSTR
                              ,STALEMATEWIDTH      , STALEMATESTR
                              ,wbHeader1(MATEPOSSTR, MATEPOSWIDTH).cstr()
                              ,wbHeader1(TERMPOSSTR, TERMPOSWIDTH).cstr()
                              ,wbHeader1(NTPOSSTR  , NTPOSWIDTH  ).cstr()
                              ,MAXVARWIDTH         , MAXVARSTR
                              ,VERSIONWIDTH        , VERSIONSTR
                              );
      const String h2 = format(_T("%s %s %s %s %*s %s")
                              ,FILLER(POSITIONWIDTH + INDEXSIZEWIDTH + UNDEFINEDWIDTH + STALEMATEWIDTH + 3)
                              ,wbHeader2(MATEPOSWIDTH).cstr()
                              ,wbHeader2(TERMPOSWIDTH).cstr()
                              ,wbHeader2(NTPOSWIDTH  ).cstr()
                              ,MAXVARWIDTH , plies ? _T("(plies)") : _T("(moves)")
                              ,FILLER(VERSIONWIDTH)
                              );
      return format(
        _T("%s %s%s\n")
        _T("%s %s%s\n")
        ,headerLeft.cstr()            ,h1.cstr(), headerRight.cstr()
        ,FILLER(headerLeft.length())  ,h2.cstr(), FILLER(headerRight.length())
        );
    }
  case TBIFORMAT_PRINT_COLUMNS2:
    { const String h1 = format(_T("%*s %*s %*s %*s %s %*s %-*s")
                               ,POSITIONWIDTH  , POSITIONSTR
                               ,INDEXSIZEWIDTH , INDEXSIZESTR
                               ,UNDEFINEDWIDTH , UNDEFINEDSTR
                               ,STALEMATEWIDTH , STALEMATESTR
                               ,wbHeader1(WINNERPOSSTR, WINNERPOSWIDTH).cstr()
                               ,MAXVARWIDTH    , MAXVARSTR
                               ,VERSIONWIDTH   , VERSIONSTR
                               );
      const String h2 = format(_T("%s %s %*s %s")
                              ,FILLER(POSITIONWIDTH + INDEXSIZEWIDTH + UNDEFINEDWIDTH + STALEMATEWIDTH + 3)
                              ,wbHeader2(WINNERPOSWIDTH).cstr()
                              ,MAXVARWIDTH, plies ? _T("(plies)") : _T("(moves)")
                              ,FILLER(VERSIONWIDTH)
                              );
      return format(
        _T("%s %s%s\n"
           "%s %s%s\n")
        ,headerLeft.cstr()          , h1.cstr(), headerRight.cstr()
        ,FILLER(headerLeft.length()), h2.cstr(), FILLER(headerRight.length())
      );
    }
  default:
    return EMPTYSTRING;
  }
}

String TablebaseInfo::toString(TablebaseInfoStringFormat f, bool plies) const {
  switch(f) {
  case TBIFORMAT_PRINT_TERMINALS:
    return format(_T("Udef:%*s Checkmates:(%s) Stalemates:%*s Tm:(%s)")
                 ,POSITIONWIDTH, format1000(m_undefinedPositions      ).cstr()
                 ,m_checkMatePositions.toString(  _T(','), MATEPOSWIDTH).cstr()
                 ,STALEMATEWIDTH, format1000(m_stalematePositions      ).cstr()
                 ,m_terminalWinPositions.toString(_T(','), TERMPOSWIDTH).cstr()
                 );

  case TBIFORMAT_PRINT_NONTERMINALS:
    return format(_T("Ps:%*s Udef:%*s(%5.2lf%%) Tm:(%s) Win:(%s) MaxVar:%s")
                 ,POSITIONWIDTH, format1000(m_totalPositions    ).cstr()
                 ,POSITIONWIDTH, format1000(m_undefinedPositions).cstr()
                 ,PERCENT(m_undefinedPositions, m_totalPositions )
                 ,(m_checkMatePositions + m_terminalWinPositions ).toString(_T(','), WINNERPOSWIDTH).cstr()
                 ,m_nonTerminalWinPositions.toString(_T(','), NTPOSWIDTH).cstr()
                 ,formatMaxVariants(plies).cstr()
                 );

  case TBIFORMAT_PRINT_UNDEFANDWINS:
    return format(_T("Ps:%*s Udef:%*s(%5.2lf%%) Win:(%s)")
                 ,POSITIONWIDTH, format1000(m_totalPositions    ).cstr()
                 ,POSITIONWIDTH, format1000(m_undefinedPositions).cstr()
                 ,PERCENT(m_undefinedPositions, m_totalPositions )
                 ,getWinnerPositionCount().toString(_T(','), WINNERPOSWIDTH).cstr()
                 );

  case TBIFORMAT_PRINT_COLUMNS1:
    return format(_T(" %*s %*s %*s %*s %s %s %s %s %-*s")
                 ,POSITIONWIDTH , format1000(m_totalPositions    ).cstr()
                 ,INDEXSIZEWIDTH, format1000(m_indexCapacity     ).cstr()
                 ,UNDEFINEDWIDTH, format1000(m_undefinedPositions).cstr()
                 ,STALEMATEWIDTH, format1000(m_stalematePositions).cstr()
                 ,m_checkMatePositions.toString(     _T(' '), MATEPOSWIDTH).cstr()
                 ,m_terminalWinPositions.toString(   _T(' '), TERMPOSWIDTH).cstr()
                 ,m_nonTerminalWinPositions.toString(_T(' '), NTPOSWIDTH  ).cstr()
                 ,formatMaxVariants(plies).cstr()
                 ,VERSIONWIDTH, format(_T("%s%s"), getVersion().cstr(), isConsistent()?_T("*"):format(_T("%#02x"), m_stateFlags).cstr()).cstr()
                 );
  case TBIFORMAT_PRINT_COLUMNS2:
    return format(_T(" %*s %*s %*s %*s %s %s %-*s")
                 ,POSITIONWIDTH , format1000(m_totalPositions    ).cstr()
                 ,INDEXSIZEWIDTH, format1000(m_indexCapacity     ).cstr()
                 ,UNDEFINEDWIDTH, format1000(m_undefinedPositions).cstr()
                 ,STALEMATEWIDTH, format1000(m_stalematePositions).cstr()
                 ,getWinnerPositionCount().toString(_T(' '), WINNERPOSWIDTH).cstr()
                 ,formatMaxVariants(plies).cstr()
                 ,VERSIONWIDTH, format(_T("%s%s"), getVersion().cstr(), isConsistent()?_T("*"):format(_T("%#02x"), m_stateFlags).cstr()).cstr()
                 );
  default:
    return format(_T("Unknown print-format:%d"), f);
  }
}

#define FORMATMAXVARIANT(v,plies) ((v) ? format(_T("%*d"), MAXVARFLDWIDTH, plies ? v : PLIESTOMOVES(v)).cstr() : _T("---"))

String TablebaseInfo::formatMaxVariants(bool plies) const {
  return format(_T("%s/%s")
               ,FORMATMAXVARIANT(m_maxPlies.m_count[WHITEPLAYER], plies)
               ,FORMATMAXVARIANT(m_maxPlies.m_count[BLACKPLAYER], plies)
               );
}

int TablebaseInfo::getDataStringLength(TablebaseInfoStringFormat format) { // static
  switch(format) {
  case TBIFORMAT_PRINT_COLUMNS1:
    return POSITIONWIDTH
         + INDEXSIZEWIDTH
         + UNDEFINEDWIDTH
         + STALEMATEWIDTH
         + WBWIDTH(MATEPOSWIDTH)
         + WBWIDTH(TERMPOSWIDTH)
         + WBWIDTH(NTPOSWIDTH  )
         + MAXVARWIDTH
         + VERSIONWIDTH
         + 9; // 1 extra space for each field
  case TBIFORMAT_PRINT_COLUMNS2:
    return POSITIONWIDTH
         + INDEXSIZEWIDTH
         + UNDEFINEDWIDTH
         + STALEMATEWIDTH
         + WBWIDTH(WINNERPOSWIDTH)
         + MAXVARWIDTH
         + VERSIONWIDTH
         + 7; // 1 extra space for each field
  default                      :
    return 0;
  }
}
#endif // TABLEBASE_BUILDER

PositionCount64 TablebaseInfo::getWinnerPositionCount() const {
  return m_checkMatePositions + m_terminalWinPositions + m_nonTerminalWinPositions;
}

UINT64 TablebaseInfo::getWinnerPositionCount(Player winner) const {
  return m_checkMatePositions.m_count[winner]
       + m_terminalWinPositions.m_count[winner]
       + m_nonTerminalWinPositions.m_count[winner];
}

#ifdef NEWCOMPRESSION
void TablebaseInfo::setNPFlags(BYTE npIndex) { // comes from PositionTypeCounters.getMinCounterIndex()
  assert(npIndex < 4);
  assert(isConsistent());
  m_stateFlags &= TBISTATE_MASK;
  m_stateFlags |= npIndex << 5;
}

BYTE TablebaseInfo::getNPFlags() const {
  return (m_stateFlags >> 5) & 3;
}
#endif

//----------------------------------------------------------------------------------------------------
class MoveComparator : public Comparator<MoveWithResult> {
private:
  const Player m_playerInTurn;

  bool isWinnerMove(const MoveWithResult &m) const {
    return m.m_result.getStatus() == EG_WHITEWIN && m_playerInTurn == WHITEPLAYER
        || m.m_result.getStatus() == EG_BLACKWIN && m_playerInTurn == BLACKPLAYER;
  }
  bool isDrawMove(const MoveWithResult &m) const {
    return m.m_result.getStatus() == EG_DRAW || m.m_result.getStatus() == EG_UNDEFINED;
  }
  int positionCompare(const MoveWithResult &m1, const MoveWithResult &m2) const;
public:
  MoveComparator(Player playerInTurn) : m_playerInTurn(playerInTurn) {
  }
  int compare(const MoveWithResult &m1, const MoveWithResult &m2);
  AbstractComparator *clone() const {
    return new MoveComparator(m_playerInTurn);
  }
};

int MoveComparator::compare(const MoveWithResult &m1, const MoveWithResult &m2) {
  int c;
  switch(FBOOL2MASK(isWinnerMove, m1, m2)) {
  case 3:
    c = m1.m_result.getMovesToEnd() - m2.m_result.getMovesToEnd();
    return c ? c : positionCompare(m1, m2);
  case 2: return  1;
  case 1: return -1;
  case 0:
    switch(FBOOL2MASK(isDrawMove, m1, m2)) {
    case 3: return positionCompare(m1, m2);
    case 2: return  1;
    case 1: return -1;
    case 0:
      c = m2.m_result.getMovesToEnd() - m1.m_result.getMovesToEnd();
      return c ? c : positionCompare(m1,m2);
    }
  }
  throwException(_T("%s(%s,%s) dropped to the end")
                ,__TFUNCTION__
                ,m1.toString().cstr()
                ,m2.toString().cstr()
                );
  return 0;
}

int MoveComparator::positionCompare(const MoveWithResult &m1, const MoveWithResult &m2) const {
  int c = m1.getFrom() - m2.getFrom();
  if(c) return c;
  c = m1.getTo() - m2.getTo();
  if(c) return c;
  return pieceTypeCmp(m1.getPromoteTo(), m2.getPromoteTo());
}

CompactIntArray MoveResultArray::findShortestWinnerMoves() const {
  CompactIntArray tmp;
  int minWinDistance = 9999;
  const EndGamePositionStatus usableStatus = (m_playerInTurn==WHITEPLAYER) ? EG_WHITEWIN : EG_BLACKWIN;
  for(size_t i = 0; i < size(); i++) {
    const MoveWithResult &m = (*this)[i];
    if(m.m_result.getStatus() != usableStatus) {
      tmp.add(-1);
    } else {
      const int pliesToEnd = m.m_result.getPliesToEnd();
      tmp.add(pliesToEnd);
      if(pliesToEnd < minWinDistance) {
        minWinDistance = pliesToEnd;
      }
    }
  }
  CompactIntArray result;
  for(UINT i = 0; i < tmp.size(); i++) {
    if(tmp[i] == minWinDistance) {
      result.add(i);
    }
  }
  return result;
}

CompactIntArray MoveResultArray::findLongestLoosingMoves(int defendStrength) const {
  CompactIntArray tmp;
  int maxLooseDistance = 0;
  const EndGamePositionStatus usableStatus = (m_playerInTurn==WHITEPLAYER) ? EG_BLACKWIN : EG_WHITEWIN;
  for(size_t i = 0; i < size(); i++) {
    const MoveWithResult &m = (*this)[i];
    if(m.m_result.getStatus() != usableStatus) {
      tmp.add(-1);
    } else {
      const int pliesToEnd = m.m_result.getPliesToEnd();
      tmp.add(pliesToEnd);
      if(pliesToEnd > maxLooseDistance) {
        maxLooseDistance = pliesToEnd;
      }
    }
  }
  const int minLooseDistance = maxLooseDistance * defendStrength / 100;

  CompactIntArray result;
  for(UINT i = 0; i < tmp.size(); i++) {
    int p = tmp[i];
    if((p >= 0) && (p >= minLooseDistance)) {
      result.add(i);
    }
  }
  return result;
}

CompactIntArray MoveResultArray::findDrawMoves() const {
  CompactIntArray result;
  for(UINT i = 0; i < size(); i++) {
    if((*this)[i].m_result.getStatus() == EG_DRAW) {
      result.add(i);
    }
  }
  return result;
}

MoveResultArray &MoveResultArray::sort() {
  CompactArray<MoveWithResult>::sort(MoveComparator(m_playerInTurn));
  return *this;
}

MoveWithResult MoveResultArray::selectBestMove(int defendStrength) const {
  CompactIntArray moves = findShortestWinnerMoves();
  if(!moves.isEmpty()) {
    return (*this)[moves.select()];
  } else if(!(moves = findDrawMoves()).isEmpty()) {
    return (*this)[moves.select()];
  } else {
    return selectLongestLoosingMove(defendStrength);
  }
}

String MoveResultArray::toString(const Game &game, MoveStringFormat mf, bool depthInPlies) {
  sort();
  StringArray msa;
  for(size_t i = 0; i < size(); i++) {
    msa.add(PrintableMove(game,(*this)[i]).toString(mf));
  }
  const int maxMoveStrLength = (int)msa.maxLength();
  String result;
  for(size_t i = 0; i < size(); i++) {
    result += format(_T("%-*s - %s\n"), maxMoveStrLength, msa[i].cstr(), (*this)[i].m_result.toString(m_playerInTurn, depthInPlies).cstr());
  }
  return result;
}

StreamProgress::StreamProgress(const EndGameTablebase *tb) : m_timer(1) {
  m_total     = sizeof(TablebaseInfo) + tb->getKeyDefinition().getIndexSize() * sizeof(EndGameResult);
  m_timer.startTimer(1500, *this, true);
}

StreamProgress::StreamProgress(const String &fileName) : m_timer(1) {
  m_total     = STAT(fileName).st_size;
  m_timer.startTimer(1500, *this, true);
}

StreamProgress::~StreamProgress() {
  m_timer.stopTimer();
  verbose(_T("        \b")); // clear the decimals written in handleTimeout
  verbose(_T("100%%\n"));
}

void StreamProgress::handleTimeout(Timer &timer) {
  verbose(_T("%.2lf%%\b"), PERCENT(getCount(), m_total));
}

#ifdef TABLEBASE_BUILDER

IntervalChecker::IntervalChecker(const TCHAR *name) : m_name(name) {
  m_minValue = -1;
  m_maxValue = 0;
}

IntervalChecker::~IntervalChecker() {
  _tprintf(_T("Interval[%s]:[%s..%s]\n"), m_name, format1000(m_minValue).cstr(), format1000(m_maxValue).cstr());
}


void IntervalChecker::update(EndGamePosIndex v) {
  if(v < m_minValue) {
    m_minValue = v;
  }
  if(v > m_maxValue) {
    m_maxValue = v;
  }
}

#ifdef NEWCOMPRESSION
NotPrunedFlags PositionTypeCounters::getBestNPFlags() const {
  UINT64         currentMin = m_count[0][0];
  NotPrunedFlags result     = NPFLAGS(0,0);
  forEachPlayer(player) {
    for(BYTE parity = 0; parity <= 1; parity++) {
      const UINT64 v = m_count[player][parity];
      if(v < currentMin) {
        result = NPFLAGS(player, parity);
        currentMin = v;
      }
    }
  }
  return result;
}

#endif // NEWCOMPRESSION

#endif // TABLEBASE_BUILDER
