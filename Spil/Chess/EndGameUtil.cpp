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

#if defined(NEWCOMPRESSION)
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


#if defined(TABLEBASE_BUILDER)
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
  case TBIFORMAT_PRINT_ALL      :
    { const TCHAR *timestampFormat = _T("dd.MM.yy hh:mm");
      return format(_T("Version                    :%s\n"
                       "Index capacity             :%11s\n"
                       "Total positions            :%11s\n"
                       "Undefined positions        :%11s\n"
                       "Stalemate positions        :%11s\n"
                       "Draw positions             :%11s\n"
                       "Checkmate positions        :%s\n"
                       "Terminal winnerpositions   :%s\n"
                       "Nonterminal winnerpositions:%s\n"
                       "Winner positions           :%s\n"
                       "Max variants (in %s)    :%s\n"
                       "Can win                    :w=%s, b=%s\n"
                       "Consistent                 :%s\n"
                       "State flags                :%#02x\n"
                       "Buildtime                  :%s\n"
                       "ConsistencyChecked         :%s\n"
                      )
                   ,getVersion().cstr()
                   ,format1000(m_indexCapacity     ).cstr()
                   ,format1000(m_totalPositions    ).cstr()
                   ,format1000(m_undefinedPositions).cstr()
                   ,format1000(m_stalematePositions).cstr()
                   ,format1000(m_drawPositions     ).cstr()
                   ,m_checkMatePositions.toStringWithTotal(     _T('+'), 11).cstr()
                   ,m_terminalWinPositions.toStringWithTotal(   _T('+'), 11).cstr()
                   ,m_nonTerminalWinPositions.toStringWithTotal(_T('+'), 11).cstr()
                   ,getWinnerPositionCount().toStringWithTotal( _T('+'), 11).cstr()
                   ,plies?_T("plies"):_T("moves"), formatMaxVariants(plies).cstr()
                   ,boolToStr(m_canWin[WHITEPLAYER]),boolToStr(m_canWin[BLACKPLAYER])
                   ,boolToStr(isConsistent())
                   ,m_stateFlags
                   ,Timestamp((time_t)m_buildTime).toString(timestampFormat).cstr()
                   ,Timestamp((time_t)m_consistencyCheckedTime).toString(timestampFormat).cstr()
                   );
    }
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

#if defined(TABLEBASE_BUILDER)

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

#endif // TABLEBASE_BUILDER
