#pragma once

#include <EndGameTablebase.h>

class EndGameInfo : public TablebaseInfo {
public:
  String  m_name;
  __int64 m_rawSize;
  __int64 m_compressedSize;
  double  m_compressRatio;

  EndGameInfo(const EndGameTablebase &db);
  String toString(TablebaseInfoStringFormat f, bool plies) const;
  unsigned int getMaxVariant() const;
};


#define NOSORT                   0x000
#define NAME                     0x001
#define POSITIONCOUNT            0x002
#define INDEXSIZE                0x003
#define UNDEFINEDCOUNT           0x004
#define STALEMATECOUNT           0x005
#define WINNERPOSITIONCOUNT      0x006
#define CHECKMATECOUNT           0x007
#define TERMINALPOSITIONCOUNT    0x008
#define NONTERMINALPOSITIONCOUNT 0x009
#define MAXVARIANTLENGTH         0x00a
#define RAWSIZE                  0x00b
#define COMPRESSEDSIZE           0x00c
#define COMPRESSRATIO            0x00d
#define BUILDTIME                0x00e
#define SELECT_WHITEWIN          0x100
#define SELECT_BLACKWIN          0x200

typedef unsigned int SortField;

class EndGameInfoList : public Array<EndGameInfo> {
private:
  __int64    m_totalRawSize, m_totalCompressedSize;
  const bool m_printAsPlies;
  StringArray m_errors;
public:
  EndGameInfoList(const EndGameTablebaseList &tablebaseList, IntArray &workSet, bool printAsPlies);
  EndGameInfoList &sort(SortField sf, bool reverseSort);
  bool add(const EndGameInfo &info);
  String getHeaderString(TablebaseInfoStringFormat f) const;
  String getSummaryString(TablebaseInfoStringFormat f) const;
  String toString(TablebaseInfoStringFormat f, bool includeErrors) const;
};

