#include "stdafx.h"
#include <Date.h>
#include "EndGameInfoList.h"

EndGameInfo::EndGameInfo(const EndGameTablebase &db) : TablebaseInfo(db.getInfo()) {
  m_name    = db.getName();
  try {
    m_rawSize        = STAT64(db.getFileName(ALLTABLEBASE)).st_size;
  } catch(...) {
    m_rawSize        = 0;
  }
  try {
    m_compressedSize = STAT64(db.getFileName(COMPRESSEDTABLEBASE)).st_size;
  } catch(...) {
    m_compressedSize = 0;
  }
  if(m_rawSize && m_compressedSize) {
    m_compressRatio  = PERCENT(m_compressedSize, m_rawSize);
  } else {
    m_compressRatio  = 0;
  }
}

unsigned int EndGameInfo::getMaxVariant() const {
  return m_maxPlies.getMax();
}

#define NAMEWIDTH      6
#define RAWSIZEWIDTH  13
#define COMPSIZEWIDTH 13
#define COMPPCTWIDTH   5

String EndGameInfoList::getHeaderString(TablebaseInfoStringFormat f) const {
  switch(f) {
  case TBIFORMAT_PRINT_COLUMNS1:
    return TablebaseInfo::getColumnHeaders(f
                        ,format(_T("%-*s"), NAMEWIDTH, _T("Name"))
                        ,format(_T("%*s %*s %*s %-14s")
                               ,RAWSIZEWIDTH , _T("Raw size")
                               ,COMPSIZEWIDTH, _T("Comp.size")
                               ,COMPPCTWIDTH , _T("Comp%")
                               ,_T("Buildtime"))
                        ,m_printAsPlies);
  case TBIFORMAT_PRINT_COLUMNS2:
    return TablebaseInfo::getColumnHeaders(f
                        ,format(_T("%-*s"), NAMEWIDTH, _T("Name"))
                        ,format(_T("%*s %*s %*s %-14s %-14s")
                               ,RAWSIZEWIDTH , _T("Raw size")
                               ,COMPSIZEWIDTH, _T("Comp.size")
                               ,COMPPCTWIDTH , _T("Comp%")
                               ,_T("Buildtime")
                               ,_T("Checktime"))
                        ,m_printAsPlies);
  default:
    return _T("");
  }
}

String EndGameInfo::toString(TablebaseInfoStringFormat f, bool plies) const {
  static const String tsFormat = _T("dd.MM.yy hh:mm");
  switch(f) {
  case TBIFORMAT_PRINT_COLUMNS1:
    return format(_T("%-*s%s%*s %*s %*s %14s")
                 ,NAMEWIDTH    , m_name.cstr()
                 ,TablebaseInfo::toString(f, plies).cstr()
                 ,RAWSIZEWIDTH , m_rawSize        ? format1000(m_rawSize        ).cstr()     : _T("-")
                 ,COMPSIZEWIDTH, m_compressedSize ? format1000(m_compressedSize ).cstr()     : _T("-")
                 ,COMPPCTWIDTH , m_compressRatio  ? format(_T("%5.2lf"), m_compressRatio).cstr() : _T("-")
                 ,Timestamp((time_t)m_buildTime).toString(tsFormat).cstr()
                );
  case TBIFORMAT_PRINT_COLUMNS2:
    return format(_T("%-*s%s%*s %*s %*s %14s %14s")
                 ,NAMEWIDTH    , m_name.cstr()
                 ,TablebaseInfo::toString(f, plies).cstr()
                 ,RAWSIZEWIDTH , m_rawSize        ? format1000(m_rawSize        ).cstr()     : _T("-")
                 ,COMPSIZEWIDTH, m_compressedSize ? format1000(m_compressedSize ).cstr()     : _T("-")
                 ,COMPPCTWIDTH , m_compressRatio  ? format(_T("%5.2lf"), m_compressRatio).cstr() : _T("-")
                 ,Timestamp((time_t)m_buildTime).toString(tsFormat).cstr()
                 ,Timestamp((time_t)m_consistencyCheckedTime).toString(tsFormat).cstr()
                );
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("f=%d"), f);
    return _T("");
  }
}

String EndGameInfoList::getSummaryString(TablebaseInfoStringFormat f) const {
  const int dataLength = (f == TBIFORMAT_PRINT_COLUMNS1) ? 141 : 85;
  return format(_T("Total:%-*s%*s %*s %*.2lf")
               ,dataLength,_T(" ")
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
  AbstractComparator *clone() const {
    return new InfoComparator(*this);
  }

  int compare(const EndGameInfo &i1, const EndGameInfo &i2) {
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
  default                                        : throwException(_T("InfoComparator:sortField=%d"), m_sf);
  }
  if(c) return c;
  c = sign((intptr_t)i1.m_name.length() - (intptr_t)i2.m_name.length());
  if(c) return c;
  return _tcsicmp(i1.m_name.cstr(), i2.m_name.cstr());
}

EndGameInfoList::EndGameInfoList(const EndGameTablebaseList &tablebaseList, IntArray &workSet, bool printAsPlies) : m_printAsPlies(printAsPlies) {
  m_totalRawSize        = 0;
  m_totalCompressedSize = 0;
  for(Iterator<int> it = workSet.getIterator(); it.hasNext();) {
    EndGameTablebase &db = *tablebaseList[it.next()];
    try {
      add(db);
    } catch(Exception e) {
      m_errors.add(format(_T("%-7s --- %s"), db.getName().cstr(), e.what()));
    }
  }
}

bool EndGameInfoList::add(const EndGameInfo &info) {
  Array<EndGameInfo>::add(info);
  m_totalRawSize        += info.m_rawSize;
  m_totalCompressedSize += info.m_compressedSize;
  return true;
}

EndGameInfoList &EndGameInfoList::sort(SortField sf, bool reverseSort) {
  if(sf != NOSORT) {
    Array<EndGameInfo>::sort(InfoComparator(sf, reverseSort));
  }
  return *this;
}

String EndGameInfoList::toString(TablebaseInfoStringFormat f, bool includeErrors) const {
#define ADDLINE(s)  { result += s; result += _T("\n"); }
#define ADDFILLER() ADDLINE(spaceString((f==TBIFORMAT_PRINT_COLUMNS1)?195:157,'_'))

  String result = format(_T("Metric:%s\n"), EndGameKeyDefinition::getMetricName());
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
