#include "stdafx.h"
#include <Date.h>
#include "EndGameInfoList.h"

EndGameInfo::EndGameInfo(const EndGameTablebase &db) : TablebaseInfo(db.getInfo()) {
  m_name    = db.getName();
  try {
    m_rawSize        = db.getTbFileSize(ALLTABLEBASE);
  } catch(...) {
    m_rawSize        = 0;
  }
  try {
    m_compressedSize = db.getTbFileSize(COMPRESSEDTABLEBASE);
  } catch(...) {
    m_compressedSize = 0;
  }
  if(m_rawSize && m_compressedSize) {
    m_compressRatio  = PERCENT(m_compressedSize, m_rawSize);
  } else {
    m_compressRatio  = 0;
  }
}

UINT EndGameInfo::getMaxVariant() const {
  return m_maxPlies.getMax();
}

#define NAMEWIDTH      6
#define RAWSIZEWIDTH  14
#define COMPSIZEWIDTH 14
#define COMPPCTWIDTH   5
static const TCHAR *timestampFormat = _T("dd.MM.yy hh:mm");
static inline int getTimestampWidth() {
  static int w = 0;
  if(w == 0) w = (int)wcslen(timestampFormat);
  return w;
}

#define TIMESTAMPWIDTH getTimestampWidth()

String EndGameInfoList::getHeaderString(TablebaseInfoStringFormat f) const {
  switch(f) {
  case TBIFORMAT_PRINT_COLUMNS1:
    return TablebaseInfo::getColumnHeaders(f
                        ,format(_T("%-*s"), NAMEWIDTH, _T("Name"))
                        ,format(_T("%*s %*s %*s %-*s")
                               ,RAWSIZEWIDTH  , _T("Raw size")
                               ,COMPSIZEWIDTH , _T("Comp.size")
                               ,COMPPCTWIDTH  , _T("Comp%")
                               ,TIMESTAMPWIDTH, _T("Buildtime"))
                        ,m_printAsPlies);
  case TBIFORMAT_PRINT_COLUMNS2:
    return TablebaseInfo::getColumnHeaders(f
                        ,format(_T("%-*s"), NAMEWIDTH, _T("Name"))
                        ,format(_T("%*s %*s %*s %-*s %-*s")
                               ,RAWSIZEWIDTH  , _T("Raw size")
                               ,COMPSIZEWIDTH , _T("Comp.size")
                               ,COMPPCTWIDTH  , _T("Comp%")
                               ,TIMESTAMPWIDTH, _T("Buildtime")
                               ,TIMESTAMPWIDTH, _T("Checktime"))
                        ,m_printAsPlies);
  default:
    return EMPTYSTRING;
  }
}

String EndGameInfo::toString(TablebaseInfoStringFormat f, bool plies) const {
  switch(f) {
  case TBIFORMAT_PRINT_COLUMNS1:
    return format(_T("%-*s%s%*s %*s %*s %-*s")
                 ,NAMEWIDTH     , m_name.cstr()
                 ,__super::toString(f, plies).cstr()
                 ,RAWSIZEWIDTH  , m_rawSize        ? format1000(m_rawSize        ).cstr()     : _T("-")
                 ,COMPSIZEWIDTH , m_compressedSize ? format1000(m_compressedSize ).cstr()     : _T("-")
                 ,COMPPCTWIDTH  , m_compressRatio  ? format(_T("%*.2lf"), COMPPCTWIDTH, m_compressRatio).cstr() : _T("-")
                 ,TIMESTAMPWIDTH,Timestamp((time_t)m_buildTime).toString(timestampFormat).cstr()
                );
  case TBIFORMAT_PRINT_COLUMNS2:
    return format(_T("%-*s%s%*s %*s %*s %-*s %-*s")
                 ,NAMEWIDTH     , m_name.cstr()
                 ,__super::toString(f, plies).cstr()
                 ,RAWSIZEWIDTH  , m_rawSize        ? format1000(m_rawSize        ).cstr()     : _T("-")
                 ,COMPSIZEWIDTH , m_compressedSize ? format1000(m_compressedSize ).cstr()     : _T("-")
                 ,COMPPCTWIDTH  , m_compressRatio  ? format(_T("%*.2lf"), COMPPCTWIDTH, m_compressRatio).cstr() : _T("-")
                 ,TIMESTAMPWIDTH, Timestamp((time_t)m_buildTime).toString(timestampFormat).cstr()
                 ,TIMESTAMPWIDTH, Timestamp((time_t)m_consistencyCheckedTime).toString(timestampFormat).cstr()
                );
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("f=%d"), f);
    return EMPTYSTRING;
  }
}

int EndGameInfoList::getLineLength(TablebaseInfoStringFormat format) { // static
  switch(format) {
  case TBIFORMAT_PRINT_COLUMNS1:
    return TablebaseInfo::getDataStringLength(format)
         + NAMEWIDTH
         + RAWSIZEWIDTH
         + COMPSIZEWIDTH
         + COMPPCTWIDTH
         + TIMESTAMPWIDTH
         + 3;  // 3 spaces exstra
  case TBIFORMAT_PRINT_COLUMNS2:
    return TablebaseInfo::getDataStringLength(format)
         + NAMEWIDTH
         + RAWSIZEWIDTH
         + COMPSIZEWIDTH
         + COMPPCTWIDTH
         + TIMESTAMPWIDTH
         + TIMESTAMPWIDTH
         + 4;  // 4 spaces exstra
  default: return 10;
  }
}

String EndGameInfoList::getSummaryString(TablebaseInfoStringFormat f) const {
  const int dataLength = TablebaseInfo::getDataStringLength(f);
  return format(_T("%-*s %*s %-*s%*s %*s %*.2lf")
               ,NAMEWIDTH    , _T("Total:")
               ,POSITIONWIDTH, format1000(m_totalPositions).cstr()
               ,dataLength-POSITIONWIDTH-2,_T(" ")
               ,RAWSIZEWIDTH , format1000(m_totalRawSize).cstr()
               ,COMPSIZEWIDTH, format1000(m_totalCompressedSize).cstr()
               ,COMPPCTWIDTH , PERCENT(m_totalCompressedSize, m_totalRawSize)
               );
}

class InfoComparator : public Comparator<EndGameInfo> {
private:
  const SortField m_sf;
  const int       m_reverseFactor;
  int compare1(const EndGameInfo &i1, const EndGameInfo &i2);
public:
  InfoComparator(SortField sf, bool reverseSort) : m_sf(sf), m_reverseFactor(reverseSort?-1:1) {
  }
  AbstractComparator *clone() const override {
    return new InfoComparator(*this);
  }

  int compare(const EndGameInfo &i1, const EndGameInfo &i2) override {
    return compare1(i1,i2) * m_reverseFactor;
  }
};

int InfoComparator::compare1(const EndGameInfo &i1, const EndGameInfo &i2) {
  int c;
  switch(m_sf) {
  case NAME                                      : c = 0;                                                                                                      break;
  case POSITIONCOUNT                             : c = sign((INT64)i1.m_totalPositions                    - (INT64)i2.m_totalPositions);                       break;
  case INDEXSIZE                                 : c = sign((INT64)i1.m_indexCapacity                     - (INT64)i2.m_indexCapacity);                        break;
  case UNDEFINEDCOUNT                            : c = sign((INT64)i1.m_undefinedPositions                - (INT64)i2.m_undefinedPositions);                   break;
  case STALEMATECOUNT                            : c = sign((INT64)i1.m_stalematePositions                - (INT64)i2.m_stalematePositions);                   break;
  case WINNERPOSITIONCOUNT                       : c = sign((INT64)i1.getWinnerPositionCount().getTotal() - (INT64)i2.getWinnerPositionCount().getTotal());    break;
  case WINNERPOSITIONCOUNT      | SELECT_WHITEWIN: c = sign((INT64)i1.getWinnerPositionCount(WHITEPLAYER) - (INT64)i2.getWinnerPositionCount(   WHITEPLAYER)); break;
  case WINNERPOSITIONCOUNT      | SELECT_BLACKWIN: c = sign((INT64)i1.getWinnerPositionCount(BLACKPLAYER) - (INT64)i2.getWinnerPositionCount(   BLACKPLAYER)); break;
  case CHECKMATECOUNT                            : c = ::compare(   i1.m_checkMatePositions,         i2.m_checkMatePositions                 ); break;
  case CHECKMATECOUNT           | SELECT_WHITEWIN: c = ::compare(   i1.m_checkMatePositions,         i2.m_checkMatePositions     ,WHITEPLAYER); break;
  case CHECKMATECOUNT           | SELECT_BLACKWIN: c = ::compare(   i1.m_checkMatePositions,         i2.m_checkMatePositions     ,BLACKPLAYER); break;
  case TERMINALPOSITIONCOUNT                     : c = ::compare(   i1.m_terminalWinPositions,       i2.m_terminalWinPositions               ); break;
  case TERMINALPOSITIONCOUNT    | SELECT_WHITEWIN: c = ::compare(   i1.m_terminalWinPositions,       i2.m_terminalWinPositions   ,WHITEPLAYER); break;
  case TERMINALPOSITIONCOUNT    | SELECT_BLACKWIN: c = ::compare(   i1.m_terminalWinPositions,       i2.m_terminalWinPositions   ,BLACKPLAYER); break;
  case NONTERMINALPOSITIONCOUNT                  : c = ::compare(   i1.m_nonTerminalWinPositions,    i2.m_nonTerminalWinPositions            ); break;
  case NONTERMINALPOSITIONCOUNT | SELECT_WHITEWIN: c = ::compare(   i1.m_nonTerminalWinPositions,    i2.m_nonTerminalWinPositions,WHITEPLAYER); break;
  case NONTERMINALPOSITIONCOUNT | SELECT_BLACKWIN: c = ::compare(   i1.m_nonTerminalWinPositions,    i2.m_nonTerminalWinPositions,BLACKPLAYER); break;
  case MAXVARIANTLENGTH                          : c = ::compareMax(i1.m_maxPlies,                   i2.m_maxPlies                           ); break;
  case MAXVARIANTLENGTH         | SELECT_WHITEWIN: c = ::compare(   i1.m_maxPlies,                   i2.m_maxPlies               ,WHITEPLAYER); break;
  case MAXVARIANTLENGTH         | SELECT_BLACKWIN: c = ::compare(   i1.m_maxPlies,                   i2.m_maxPlies               ,BLACKPLAYER); break;
  case RAWSIZE                                   : c = sign(i1.m_rawSize                           - i2.m_rawSize);                             break;
  case COMPRESSEDSIZE                            : c = sign(i1.m_compressedSize                    - i2.m_compressedSize);                      break;
  case COMPRESSRATIO                             : c = sign(i1.m_compressRatio                     - i2.m_compressRatio);                       break;
  case BUILDTIME                                 : c = sign((INT64)i1.m_buildTime                  - (INT64)i2.m_buildTime);                    break;
  default                                        : throwException(_T("InfoComparator:sortField=%u"), m_sf);
  }
  if(c) return c;
  c = sign((intptr_t)i1.m_name.length() - (intptr_t)i2.m_name.length());
  if(c) return c;
  return _tcsicmp(i1.m_name.cstr(), i2.m_name.cstr());
}

EndGameInfoList::EndGameInfoList(const EndGameTablebaseList &tablebaseList, IntArray &workSet, bool printAsPlies) : m_printAsPlies(printAsPlies) {
  m_totalPositions      = 0;
  m_totalRawSize        = 0;
  m_totalCompressedSize = 0;
  for(Iterator<int> it = workSet.getIterator(); it.hasNext();) {
    const EndGameTablebase &db = *tablebaseList[it.next()];
    try {
      add(db);
    } catch(Exception e) {
      m_errors.add(format(_T("%-7s --- %s"), db.getName().cstr(), e.what()));
    }
  }
}

bool EndGameInfoList::add(const EndGameInfo &info) {
  __super::add(info);
  m_totalPositions      += info.m_totalPositions;
  m_totalRawSize        += info.m_rawSize;
  m_totalCompressedSize += info.m_compressedSize;
  return true;
}

EndGameInfoList &EndGameInfoList::sort(SortField sf, bool reverseSort) {
  if(sf != NOSORT) {
    __super::sort(InfoComparator(sf, reverseSort));
  }
  return *this;
}

String EndGameInfoList::toString(TablebaseInfoStringFormat f, bool includeErrors) const {
  const String skillLine = spaceString(getLineLength(f),_T('_')); // (f==TBIFORMAT_PRINT_COLUMNS1)?195:168,

#define ADDLINE(s)  { result += s; result += _T("\n"); }
#define ADDFILLER() ADDLINE(skillLine)

  String result = format(_T("Metric:%s\n"), EndGameKeyDefinition::getMetricName().cstr());
  result += getHeaderString(f);
  ADDFILLER();
  for(size_t i = 0; i < size(); i++) {
    ADDLINE((*this)[i].toString(f, m_printAsPlies));
  }
  ADDFILLER();
  ADDLINE(getSummaryString(f));
  if(includeErrors) {
    for(size_t i = 0; i < m_errors.size(); i++) {
      ADDLINE(m_errors[i]);
    }
    ADDFILLER();
  }
  return result;
}
