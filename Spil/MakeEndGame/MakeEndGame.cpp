#include "stdafx.h"
#include <stddef.h>
#include <Console.h>
#include <HashMap.h>
#include <Date.h>
#include <Regex.h>
#include <BitStream.h>
#include <CompressFilter.h>
#include "EndGameInfoList.h"

#define CMD_BUILD                    0x00000001
#define CMD_BUILDDTM                 0x00000002
#define CMD_NOBUILD                  0x00000004
#define CMD_INFO                     0x00000008
#define CMD_INCLUDEERRORS            0x00000010
#define CMD_INFOPLIES                0x00000020
#define CMD_RECOVER                  0x00000040
#define CMD_LIST                     0x00000080
#define CMD_LISTALL                  0x00000100
#define CMD_LISTMAXVAR               0x00000200
#define CMD_LISTEXACTPLIES           0x00000400
#define CMD_LISTCHECKMATES           0x00000800
#define CMD_LISTSTALEMATES           0x00001000
#define CMD_LISTUNDEFINED            0x00002000
#define CMD_LISTPLIESTOEND           0x00004000
#define CMD_LISTHASHELPINFO          0x00008000
#define CMD_LISTWHITEWIN             0x00010000
#define CMD_LISTBLACKWIN             0x00020000
#define CMD_CHECKKEYDEF              0x00040000
#define CMD_CHECKSYMMETRIES          0x00080000
#define CMD_CHECKCONSISTENCY         0x00100000
#define CMD_CHECKPOSITIONS           0x00200000
#define CMD_CHECKLISTMOVES           0x00400000
#define CMD_CONVERT                  0x00800000
#define CMD_ADDUNDEFINEDKEYS         0x01000000
#define CMD_EXTRACTWINNERBITS        0x02000000

void printSkillLine() {
  verbose(_T("%s\n"),spaceString(158,'_').cstr());
}

static EndGameTablebase &loadIfEmpty(EndGameTablebase &tablebase) {
  if(!tablebase.isLoaded()) {
    tablebase.load();
  }
  return tablebase;
}

class CurrentWork : public PropertyChangeListener {
private:
  const EndGameTablebase *m_currentTablebase;
  void update();
public:
  CurrentWork();
  void setTablebase(const EndGameTablebase *tablebase);
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
};

CurrentWork::CurrentWork() {
  m_currentTablebase  = NULL;
}

void CurrentWork::setTablebase(const EndGameTablebase *tablebase) {
  m_currentTablebase = tablebase;
  update();
}

void CurrentWork::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(id == ENDGAME_METRIC) {
    update();
  }
}

void CurrentWork::update() {
  if(m_currentTablebase == NULL) {
    updateMessageField(_T(""));
  } else {
    updateMessageField(_T(" - Currently working on %s - metric:%s")
                      ,m_currentTablebase->getName().cstr()
                      ,EndGameKeyDefinition::getMetricName()
                      );
  }
}

static void processTablebase(EndGameTablebase &tablebase, int commands, int plies) {
  bool jobDone = false;
  try {
    if(commands & CMD_BUILD) {
      const bool recover = (commands & CMD_RECOVER) ? true : false;
      if(tablebase.needBuild(recover)) {
        if(commands & CMD_NOBUILD) {
          _tprintf(_T("%s/%s needs build\n"), tablebase.getName().cstr(), EndGameKeyDefinition::getMetricName());
          return;
        } else {
          tablebase.build(recover);
          jobDone = true;
        }
      }
    }
  } catch(...) {
    if(tablebase.isLoaded()) {
      tablebase.unload();
    }
    throw;
  }
  if(commands & CMD_CHECKCONSISTENCY) {
    unsigned int checkFlags = CHECK_HEADER;
    if(commands & CMD_CHECKPOSITIONS) {
      checkFlags |= CHECK_POSITIONS;
      if(commands & CMD_CHECKLISTMOVES) {
        checkFlags |= CHECK_LIST_MOVES;
      }
    }
    loadIfEmpty(tablebase).checkConsistency(checkFlags);
    jobDone = true;
  }

  if(commands & CMD_BUILDDTM) {
    tablebase.buildDTM();
  }

  if(commands & CMD_LIST) {
    loadIfEmpty(tablebase);
    if(commands & CMD_LISTALL) {
      tablebase.list(stdout, LIST_ALL);
    } else if(commands & CMD_LISTMAXVAR) {
      tablebase.list(stdout, LIST_MAXVAR     ,(commands & CMD_LISTWHITEWIN)?true:false, (commands & CMD_LISTBLACKWIN)?true:false);
    } else if(commands & CMD_LISTCHECKMATES) {
      tablebase.list(stdout, LIST_CHECKMATES, (commands & CMD_LISTWHITEWIN)?true:false, (commands & CMD_LISTBLACKWIN)?true:false);
    } else if(commands & CMD_LISTEXACTPLIES) {
      tablebase.list(stdout, LIST_EXACTPLIES, (commands & CMD_LISTWHITEWIN)?true:false, (commands & CMD_LISTBLACKWIN)?true:false, plies);
    } else if(commands & CMD_LISTSTALEMATES) {
      tablebase.list(stdout, LIST_STALEMATES);
    } else if(commands & CMD_LISTUNDEFINED) {
      tablebase.list(stdout, LIST_UNDEFINED);
    } else if(commands & CMD_LISTPLIESTOEND) {
      tablebase.list(stdout, LIST_PLIESTOEND);
    } else if(commands & CMD_LISTHASHELPINFO) {
      tablebase.list(stdout, LIST_HASHELPINFO);
    } else {
      tablebase.list(stdout, LIST_WINNER);
    }
    jobDone = true;
  }
  if(tablebase.isLoaded()) {
    tablebase.unload();
  }

  if(commands & CMD_CONVERT) {
    try {
      const TablebaseInfo info = tablebase.getInfo();
      if(info.isConsistent()) {
        verbose(_T("Converting %s...\n"), tablebase.getName().cstr());
        tablebase.convert();
        verbose(_T("Done\n"));
        jobDone = true;
      } else {
        verbose(_T("Skip %-5s. isConsistent:%s\n")
               ,tablebase.getName().cstr()
               ,boolToStr(info.isConsistent())
               );
      }
    } catch(WrongVersionException e) {
      verbose(_T("%s\n"), e.what());
    }
  }
  if(jobDone) {
    printSkillLine();
  }
}

static void extractWinnerBits(EndGameTablebase &tb, BitOutputStream &out) {
  tb.load();
  try {
    const EndGameKeyDefinition &keydef = tb.getKeyDefinition();
    const IndexedMap           &index  = tb.getIndex();
    const EndGameResult        *first  = &index.first();
    const EndGameResult        *last   = &index.last();
    for(int d = 0; d < 2; d++) {
      for(const EndGameResult *e = first+d; e <= last; e += 2) {
        if(!e->isWinner()) {
          out.putBit(0);
        } else {
          out.putBit((STATUSTOWINNER(e->getStatus()) != keydef.indexToKey(e - first).getPlayerInTurn())?1:0);
        }
      }
    }
    tb.unload();
  } catch(...) {
    tb.unload();
    throw;
  }
}

static StringArray getTablebaseNames() {
  const EndGameTablebaseList &tablebaseList = EndGameTablebase::getRegisteredEndGameTablebases();
  StringArray result;
  for(size_t i = 0; i < tablebaseList.size(); i++) {
    const EndGameTablebase &tb = *tablebaseList[i];
    result.add(tb.getName());
  }
  return result;
}

class SpecifiedSequence {
public:
  const int m_argIndex;
  const int m_tableIndex;
  SpecifiedSequence(int argIndex, int tableIndex) : m_argIndex(argIndex), m_tableIndex(tableIndex) {
  }
};

class BuildSequenceComparator : public Comparator<SpecifiedSequence> {
private:
  const EndGameTablebaseList &m_tablebaseList;
public:
  BuildSequenceComparator(const EndGameTablebaseList &tablebaseList) : m_tablebaseList(tablebaseList) {
  }
  int compare(const SpecifiedSequence &s1, const SpecifiedSequence &s2);
  AbstractComparator *clone() const {
    return new BuildSequenceComparator(m_tablebaseList);
  }
};

int BuildSequenceComparator::compare(const SpecifiedSequence &s1, const SpecifiedSequence &s2) {
  const EndGameKeyDefinition &k1 = m_tablebaseList[s1.m_tableIndex]->getKeyDefinition();
  const EndGameKeyDefinition &k2 = m_tablebaseList[s2.m_tableIndex]->getKeyDefinition();
  int c = k1.getPieceCount() - k2.getPieceCount();
  if(c) return c;
  c = k1.getPieceCount(Pawn) - k2.getPieceCount(Pawn);
  if(c) return c;
  return s1.m_argIndex - s2.m_argIndex;
}

IntArray findBuildSequence(const IntArray &workSet, const EndGameTablebaseList &tablebaseList) {
  Array<SpecifiedSequence> tmp(workSet.size());
  for(size_t i = 0; i < workSet.size(); i++) {
    tmp.add(SpecifiedSequence(i, workSet[i]));
  }
  tmp.sort(BuildSequenceComparator(tablebaseList));
  IntArray result(tmp.size());
  for(size_t i = 0; i < tmp.size(); i++) {
    result.add(tmp[i].m_tableIndex);
  }
  return result;
}

static const EndGameTablebaseList selectRegisteredTablebases(int pieceCount) {
  const EndGameTablebaseList &tablebaseList = EndGameTablebase::getRegisteredEndGameTablebases();
  EndGameTablebaseList result;
  for(size_t i = 0; i < tablebaseList.size(); i++) {
    EndGameTablebase &tb = *tablebaseList[i];
    if(tb.getKeyDefinition().getPieceCount() == pieceCount) {
      result.add(&tb);
    }
  }
  return result;
}

static String makeTabebaseNameList() {
  const TCHAR *leftMargin = _T("          ");
  String result;
  for(int pieceCount = 3; pieceCount <= 5; pieceCount++) {
    const EndGameTablebaseList tablebaseList = selectRegisteredTablebases(pieceCount);
    result += format(_T("    %d men:"), pieceCount);
    for(size_t i = 0; i < tablebaseList.size(); i++) {
      const EndGameTablebase &tb = *tablebaseList[i];
      if(i) {
        if(i % 20 == 0) {
          result += format(_T(",\n%s"),leftMargin);
        } else {
          result += _T(", ");
        }
      }
      result += format(_T("%-5s"), tb.getName().cstr());
    }
    result += _T("\n");
  }
  return result;
}

static void usage(char *arg = NULL) {
  if(arg) {
    printf("Invalid argumet:%s\n", arg);
  }
  _tprintf(_T("Usage: MakeEndGame [options] dbnames...\n"
            " Options: -b     : Build tablebase. If not specified, the tablebase will by build and saved.\n"
            "          -r     : Recover build.\n"
            "          -n     : No build. Used together with -b, it will identify the tablebases, that needs to be build, without doing it.\n"
            "          -m[DTC|DTM]:Defines the metric of optimality, depth to conversion or depth to mate.\n"
            "                      Default is depth to conversion.\n"
            "          -l[a|c[wb]|m[wb]|s|u|w]:List positions with result and moves to end.\n"
            "             a     : List all positions. Default only winner positions will be listed.\n"
            "             c[wb] : List checkmate positions.\n"
            "                     If 'w' and/or 'b' specified, only white/black win mate-positions will be listed.\n"
            "                     Default, both white and black win positions will be listed.\n"
            "             m[wb] : List positions with maximum plies to end.\n"
            "                     If 'w' and/or 'b' specified, only white/black win variants will be listed.\n"
            "                     Default, both white and black win positions will be listed.\n"
            "             e[wb]plies: List positions with specified plies to end.\n"
            "                     If 'w' and/or 'b' specified, only white/black win variants will be listed.\n"
            "             s     : List stalemate positions.\n"
            "             u     : List undefined (draw) positions.\n"
            "             p     : List number of winning positions for every depth to win (in plies).\n"
            "          -i[+][-][12][N|p|i|u|s|w[w|b]|c[w|b]|t[w|b]|n[w|b]|v[w|b]|Z|z|r|b]:Print information about specified tablebases.\n"
            "             +     : Print errors too (non existing databases).\n"
            "             -     : Reverse sort.\n"
            "             1     : Print info in format 1. (default)\n"
            "             2     : Print info in format 2.\n"
            "             N     : Sort by men count, name (default).\n"
            "             p     : Sort by #positions.\n"
            "             i     : Sort by index size.\n"
            "             u     : Sort by #undefined positions.\n"
            "             s     : Sort by #stalemates.\n"
            "             w[w|b]: Sort by #winner positions.\n"
            "                     If w or b specified, sort by #white win positions or #black win positions.\n"
            "             c[w|b]: Sort by #checkmates.\n"
            "                     If w or b specified, sort by #checkmates where white or black win.\n"
            "             t[w|b]: Sort by #terminal positions.\n"
            "                     If w or b specified, sort by #terminal positions where white or black win.\n"
            "             n[w|b]: Sort by #non terminal positions.\n"
            "                     If w or b specified, sort by #non terminal positions where white or black win.\n"
            "             v[w|b]: Sort by max variant length.\n"
            "                     If w or b specified, sort by max variant length for white or black.\n"
            "             Z     : Sort by raw size (bytes).\n"
            "             z     : Sort by compressed size (bytes).\n"
            "             r     : Sort by compression ratio.\n"
            "             b     : Sort by buildtime.\n"
            "          -v      : Log verbose messages to c:\\temp\\chess<time>.log.\n"
            "          -p      : Print moves-to-end in plies.\n"
            "          -M      : Build DTM-databases, ,and if needed, rebuild parts of DTC.\n"
            "          -e      : Extract winner database.\n"
            "          -a      : Add undefined keys to ManualPositions.txt, for the specified tablebases.\n"
            "          -c[+]   : Check keydefinition. If '+' specified, symmetries will be checked too (be patient!).\n"
            "          -u[+[+]]: Check undefined/draw positions. If '+' specified, all positions and plies-to-end will be checked.\n"
            "                  : If '++' specified, all the moves for the inconsistent positions will be listed.\n"
            "                    Default only header information is checked.\n"
            "          -Dpath  : Use path to locate tablebase files.\n"
            "          -d[+|-] : Select only tablebases with/without dublets, (2 or 3 equal pieces).\n"
            "                    '+' = only with dublets, '-' = no dublets allowed. '+' is default.\n"
            "          -sCount : Select only endgames with specified number of men.\n"
            "          -#      : Convert header to new version.\n"
            " TablebaseNames: (Use .* to process all tablebases).\n"
            "%s\n")
           ,makeTabebaseNameList().cstr()
         );
  exit(-1);
}

typedef enum {
  SELECT_ALL_DUBLET_FILTER
 ,SELECT_ONLY_DUBLETS_ALLOWED_FILTER
 ,SELECT_NO_DUBLETS_ALLOWED_FILTER
} DubletFilter;

static int getCommandWhiteBlackFlags(const char *&cp) {
  int flags = 0;
  for(;*cp; cp++) {
    switch(*cp) {
    case 'w': flags |= CMD_LISTWHITEWIN; continue;
    case 'b': flags |= CMD_LISTBLACKWIN; continue;
    default : break;
    }
    break;
  }
  return flags ? flags : (CMD_LISTWHITEWIN | CMD_LISTBLACKWIN);
}

int main(int argc, char **argv) {
  const char *cp;
  bool                      logVerbose  = false;
  TablebaseMetric           metric      = DEPTH_TO_CONVERSION;
  int                       plies       = -1;
  int                       menCount    = -1;
  TablebaseInfoStringFormat printFormat = TBIFORMAT_PRINT_COLUMNS1;
  try {

//    extern void test2KingsSym8();
//    test2KingsSym8();


//    extern void testKKWithPawn();
//    testKKWithPawn();

//    extern void testLeftWKKWithPawn();
//    testLeftWKKWithPawn();

//    return 0;


//    generateManualPositionFile();
//    return 0;

    int          commands     = 0;
    SortField    sortField    = NAME;
    bool         reverseSort  = false;
    DubletFilter dubletFilter = SELECT_ALL_DUBLET_FILTER;

    UseSafeNew = 1;

    for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
      for(cp++; *cp; cp++) {
        switch(*cp) {
        case 'b':
          commands |= CMD_BUILD;
          continue;
        case 'n':
          commands |= CMD_NOBUILD;
          continue;
        case 'r':
          commands |= CMD_RECOVER;
          continue;
        case 'm':
          if(_stricmp(cp+1,"dtc") == 0) {
            metric = DEPTH_TO_CONVERSION;
          } else if(_stricmp(cp+1,"dtm") == 0) {
            metric = DEPTH_TO_MATE;
          } else {
            usage(*argv);
          }
          break;
        case 'l':
          if(commands & CMD_LIST) {
            usage();
          }
          commands |= CMD_LIST;
          cp++;
          switch(*cp) {
          case 'a':
            commands |= CMD_LISTALL;
            break;
          case 'm':
            cp++;
            commands |= CMD_LISTMAXVAR     | getCommandWhiteBlackFlags(cp);
            break;
          case 'e':
            cp++;
            commands |= CMD_LISTEXACTPLIES | getCommandWhiteBlackFlags(cp);
            if((sscanf(cp, "%d", &plies) != 1) || (plies < 0)) {
              usage();
            }
            break;
          case 'c':
            cp++;
            commands |= CMD_LISTCHECKMATES | getCommandWhiteBlackFlags(cp);
            break;
          case 's':
            commands |= CMD_LISTSTALEMATES;
            break;
          case 'u':
            commands |= CMD_LISTUNDEFINED;
            break;
          case 'p':
            commands |= CMD_LISTPLIESTOEND;
            break;
          case 'h':
            commands |= CMD_LISTHASHELPINFO;
            break;
          case 0:
            break;
          default :
            usage();
          }
          break;

        case 'i':
          commands |= CMD_INFO;
          for(;;cp++) {
            switch(cp[1]) {
            case '+': commands  |= CMD_INCLUDEERRORS; continue;
            case '-': reverseSort = true;             continue;
            default : break;
            }
            break;
          }
          switch(cp[1]) {
          case '1'  : cp++; break;
          case '2'  : printFormat = TBIFORMAT_PRINT_COLUMNS2;  cp++; break;
          default   : break;
          }
          switch(cp[1]) {
          case 'N': sortField = NAME;            break;
          case 'p': sortField = POSITIONCOUNT;   break;
          case 'i': sortField = INDEXSIZE;       break;
          case 'u': sortField = UNDEFINEDCOUNT;  break;
          case 's': sortField = STALEMATECOUNT;  break;

#define CHECK_WB_FLAGS                                  \
  switch(cp[2]) {                                       \
  case 'w': sortField |= SELECT_WHITEWIN; cp++;  break; \
  case 'b': sortField |= SELECT_BLACKWIN; cp++;  break; \
  }

          case 'w':
            sortField = WINNERPOSITIONCOUNT;
            CHECK_WB_FLAGS;
            break;
          case 'c':
            sortField = CHECKMATECOUNT;
            CHECK_WB_FLAGS;
            break;
          case 't':
            sortField = TERMINALPOSITIONCOUNT;
            CHECK_WB_FLAGS;
            break;
          case 'n':
            sortField = NONTERMINALPOSITIONCOUNT;
            CHECK_WB_FLAGS;
            break;
          case 'v':
            sortField = MAXVARIANTLENGTH;
            CHECK_WB_FLAGS;
            break;
          case 'Z': sortField = RAWSIZE;         break;
          case 'z': sortField = COMPRESSEDSIZE;  break;
          case 'r': sortField = COMPRESSRATIO;   break;
          case 'b': sortField = BUILDTIME;       break;
          case 0  : break;
          default : usage();
          }
          break;
        case 'c':
          commands |= CMD_CHECKKEYDEF;
          if(cp[1] == '+') {
            commands |= CMD_CHECKSYMMETRIES;
            cp++;
          }
          continue;
        case 'u':
          commands |= CMD_CHECKCONSISTENCY;
          if(cp[1] == '+') {
            commands |= CMD_CHECKPOSITIONS;
            cp++;
            if(cp[1] == '+') {
              commands |= CMD_CHECKLISTMOVES;
              cp++;
            }
          }
          continue;
        case 'D':
          EndGameKeyDefinition::setDbPath(cp+1);
          break;
        case 'd':
          switch(cp[1]) {
          case '+': dubletFilter = SELECT_ONLY_DUBLETS_ALLOWED_FILTER; cp++; continue;
          case '-': dubletFilter = SELECT_NO_DUBLETS_ALLOWED_FILTER  ; cp++; continue;
          default : dubletFilter = SELECT_ONLY_DUBLETS_ALLOWED_FILTER;       continue;
          }
          break;

        case 's':
          if((sscanf(cp+1, "%d", &menCount) != 1) || (menCount < 3) || (menCount > 5)) {
            usage();
          }
          break;
        case 'v':
          logVerbose = true;
          continue;
        case 'p':
          commands |= CMD_INFOPLIES;
          continue;
        case 'M':
          commands |= CMD_BUILDDTM;
          continue;
        case 'e':
          commands |= CMD_EXTRACTWINNERBITS;
          continue;
        case 'a':
          commands |= CMD_ADDUNDEFINEDKEYS;
          continue;
        case '#':
          commands |= CMD_CONVERT;
          continue;
        case 'R':
          RemoteEndGameSubTablebase::remoteService(argv2wargv((const char**)argv));
          return 0;
        default :
          usage(*argv);
        }
        break;
      }
    }

    CurrentWork currentWork;
    EndGameKeyDefinition::addPropertyChangeListener(&currentWork);

    EndGameKeyDefinition::setMetric(metric);
    const EndGameTablebaseList &tablebaseList = EndGameTablebase::getRegisteredEndGameTablebases();

    StringArray dbNames = getTablebaseNames();

    IntArray workSet;

    for(;*argv; argv++) {
      Regex regex(*argv, String::upperCaseTranslate);
      bool anyFound = false;
      for(size_t i = 0; i < dbNames.size(); i++) {
        if(regex.match(dbNames[i])) {
          anyFound = true;
          bool inWorkSet = false;
          for(size_t j = 0; j < workSet.size(); j++) {
            if(workSet[j] == i) {
              inWorkSet = true;
              break;
            }
          }
          if(!inWorkSet) {
            const EndGameKeyDefinition &keydef = tablebaseList[i]->getKeyDefinition();
            switch(dubletFilter) {
            case SELECT_ALL_DUBLET_FILTER          :
              break;
            case SELECT_ONLY_DUBLETS_ALLOWED_FILTER:
              if(!keydef.isDupletsAllowed()) {
                continue;
              }
              break;

            case SELECT_NO_DUBLETS_ALLOWED_FILTER  :
              if(keydef.isDupletsAllowed()) {
                continue;
              }
              break;
            }
            if((menCount > 0) && (keydef.getPieceCount() != menCount)) {
              continue;
            }
            workSet.add(i);
          } else {
            _ftprintf(stderr, _T("Warning:%s already specified. Ignored\n"), dbNames[i].cstr());
          }
        }
      }
      if(!anyFound) {
        fprintf(stderr, "%s doesn't match any registered tablebase.\n", *argv);
        usage();
      }
    }

    if(workSet.isEmpty()) {
      fprintf(stderr,"No tablebase specified.\n");
      usage();
    }

    workSet = findBuildSequence(workSet, tablebaseList);

    if(commands & (CMD_BUILD | CMD_INFO)) {
      if(isatty(stderr)) {
        if(Console::getWindowSize(STD_ERROR_HANDLE).X < 180) {
          try {
            Console::setWindowSize( 180,54, STD_ERROR_HANDLE);
          } catch(Exception e) {
            _tprintf(_T("%s\n"), e.what());
            int maxW, maxH;
            Console::getLargestConsoleWindowSize(maxW, maxH, STD_ERROR_HANDLE);
            Console::setWindowAndBufferSize(0,0,maxW, maxH, STD_ERROR_HANDLE);
          }
        }
      }
    }

    if(commands & CMD_INFO) {
      _tprintf(_T("%s"), EndGameInfoList(tablebaseList, workSet, (commands & CMD_INFOPLIES)?true:false).sort(sortField, reverseSort).toString(printFormat, (commands & CMD_INCLUDEERRORS)?true:false).cstr());
      commands &= ~(CMD_INFO | CMD_INCLUDEERRORS);
    }

    if(logVerbose) {
      setVerboseLogging(logVerbose);
    }

    Thread::keepAlive();

    if(commands & CMD_ADDUNDEFINEDKEYS) {
      AllManualPositions amp;
      amp.load();
      for(Iterator<int> it = workSet.getIterator(); it.hasNext();) {
        EndGameTablebase       &tb = *tablebaseList[it.next()];
        EndGameManualPositions &mp = amp.get(tb.getName());
        currentWork.setTablebase(&tb);
        const String fileName = tb.getFileName(UNDEFINEDKEYSLOG);
        const CompactArray<EndGameKey> keyArray = mp.readUndefinedKeysLog(fileName);
        int keysAdded = 0;
        for(size_t i = 0; i < keyArray.size(); i++) {
          if(mp.addKey(keyArray[i])) {
            keysAdded++;
          }
        }
        verbose(_T("%s new manual keys added to %s\n"), format1000(keysAdded).cstr(), tb.getName().cstr());
        currentWork.setTablebase(NULL);
      }
      amp.save();
      commands &= ~CMD_ADDUNDEFINEDKEYS;
    }

    if(commands & CMD_CHECKKEYDEF) {
      UInt64HashMap<bool> keyIndexDone;
      Iterator<int> it = workSet.getIterator();
      for(Iterator<int> it = workSet.getIterator(); it.hasNext();) {
        EndGameTablebase &tb = *tablebaseList[it.next()];
        keyIndexDone.put(tb.getKeyDefinition().getIndexSize(), false);
      }
      for(Iterator<int> it = workSet.getIterator(); it.hasNext();) {
        EndGameTablebase &tb = *tablebaseList[it.next()];
        bool *done = keyIndexDone.get(tb.getKeyDefinition().getIndexSize());
        if(!done) {
          throwException(_T("keyIndexDone.get(%s) == NULL!\n"), tb.getName().cstr());
        }
        if(*done) {
          continue;
        }
        currentWork.setTablebase(&tb);
        tb.getKeyDefinition().doSelfCheck((commands & CMD_CHECKSYMMETRIES) != 0);
        currentWork.setTablebase(NULL);
        *done = true;
      }
      commands &= ~(CMD_CHECKKEYDEF | CMD_CHECKSYMMETRIES);
    }

    if(commands & CMD_EXTRACTWINNERBITS) {
      ByteOutputFile file(_T("c:\\temp\\winnerBits1.dat"));
      CompressFilter compressFilter(file);
      BitOutputStream out(compressFilter);
      for(Iterator<int> it = workSet.getIterator(); it.hasNext();) {
        EndGameTablebase &tb = *tablebaseList[it.next()];
        currentWork.setTablebase(&tb);
        extractWinnerBits(tb, out);
      }
    }

    if(commands == 0) {
      return 0;
    }

    for(Iterator<int> it = workSet.getIterator(); it.hasNext();) {
      EndGameTablebase &tb = *tablebaseList[it.next()];
      try {
        currentWork.setTablebase(&tb);
        processTablebase(tb, commands, plies);
        currentWork.setTablebase(NULL);
      } catch(MissingPositionException e) {
        currentWork.setTablebase(NULL);
        verbose(_T("%s\n"), e.what());
      }
    }
    verbose(_T("All done!\n"));

    Thread::keepAlive(0);

  } catch(Exception e) {
    Thread::keepAlive(0);
    verbose(_T("\n\nException:%s\n"), e.what());
    return -1;
  } catch(...) {
    Thread::keepAlive(0);
    verbose(_T("\n\nUnknown Exception\n"));
    return -1;
  }
  return 0;
}
