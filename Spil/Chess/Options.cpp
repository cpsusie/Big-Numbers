#include "stdafx.h"
#include <Math.h>
#include <Registry.h>
#include <Tokenizer.h>
#include <Language.h>

static const TCHAR *registryEntry = _T("Software\\JGMData\\Chess");

class InitRegistry {
public:
  InitRegistry();
};

InitRegistry::InitRegistry() {
  try {
    RegistryKey(HKEY_CURRENT_USER).createOrOpenPath(registryEntry);
  } catch(Exception) {
    // ignore
  }
}

static InitRegistry dummy;

static RegistryKey getKey() {
  return RegistryKey(HKEY_CURRENT_USER, registryEntry).createOrOpenKey(_T("Settings"));
}

static RegistryKey getSubKey(const String &name) {
  return getKey().createOrOpenKey(name);
}

static RegistryKey getSubKey(Player player) {
  return getSubKey(getPlayerNameEnglish(player));
}

static RegistryKey getEngineSubKey(RegistryKey key) {
  return key.createOrOpenKey(_T("Engine"));
}

static RegistryKey getEngineSubKey(int i) {
  return getEngineSubKey(getKey()).createOrOpenKey(format(_T("%02d"), i));
}

static RegistryKey getEngineOptionsSubKey(Player player, const String &engineName) {
  return getSubKey(player).createOrOpenKey(_T("EngineOptions")).createOrOpenKey(engineName);
}

DefaultOptions::DefaultOptions() {
  setDefault();
}

void DefaultOptions::setDefault() {
  const CSize ss = getScreenSize(false);
  m_showFieldNames                   = false;
  m_showLegalMoves                   = true;
  m_showPlayerInTurn                 = false;
  m_animateMoves                     = true;
  m_animateCheckmate                 = true;
  m_askForNewGame                    = true;
  m_computerPlayer                   = BLACKPLAYER;
  m_validateAfterEdit                = true;
  m_traceWindowVisible               = false;
  m_traceFontSize                    = 8;
  m_historyFontSize                  = 8;
  m_langID                           = Language::getBestSupportedLanguage(GetUserDefaultLangID()).m_langID;
  m_boardSize                        = CSize(875, 487);
  m_boardWindowPos                   = CPoint((ss.cx-m_boardSize.cx)/2, (ss.cy-m_boardSize.cy)/2);
  m_traceWindowPos                   = CPoint(m_boardWindowPos.x + m_boardSize.cx+10, m_boardWindowPos.y);
  m_traceWindowSize                  = CSize(200, m_boardSize.cy);
  m_normalPlayLevel                  = 1;
  m_levelTimeout.setDefault();
  m_openingLibraryEnabled            = true;
  m_endGameTablebaseEnabled          = false;
  m_endGameTablebasePath             = _T("C:\\Users\\All Users\\Application Data\\Chess\\EndGameTablebase");
  m_endGameTablebaseMetric           = DEPTH_TO_MATE;
  m_endGameDefendStrength            = 100;
  m_maxMovesWithoutCaptureOrPawnMove = 50;
  m_moveFormat                       = MOVE_SHORTFORMAT;
  m_depthInPlies                     = false;
  m_connectedToServer                = false;
  m_serverComputerName               = EMPTYSTRING;
  m_testMenuEnabled                  = false;
  m_showEngineConsole                = false;
  m_engineVerboseFields.setDefault();

  forEachPlayer(p) {
    m_playerOptions[p].setDefault();
  }
}

PlayerOptions::PlayerOptions() {
  setDefault();
}

void PlayerOptions::setDefault() {
  m_autoPlayLevel = 1;
  m_timeParameters.setDefault();
  m_speedChessParameters.setDefaultSpeedChess();
}

bool EngineDescription::operator==(const EngineDescription &desc) const {
  return m_path.equalsIgnoreCase(desc.m_path)
      && m_name   == desc.m_name
      && m_author == desc.m_author;
}

TimeParameters::TimeParameters() {
  setDefault();
}

TimeParameters::TimeParameters(int movesLeft, int secondsLeft, int secondsIncr) {
  m_movesLeft   = movesLeft;
  m_secondsLeft = secondsLeft;
  m_secondsIncr = secondsIncr;
}

void TimeParameters::setDefault() {
  m_movesLeft   = 100;
  m_secondsLeft = 60*30;
  m_secondsIncr = 0;
}

void TimeParameters::setDefaultSpeedChess() {
  m_movesLeft   = -1;
  m_secondsLeft = 60*5;
  m_secondsIncr = 0;
}

bool TimeParameters::operator==(const TimeParameters &tp) const {
  return m_movesLeft   == tp.m_movesLeft
      && m_secondsLeft == tp.m_secondsLeft
      && m_secondsIncr == tp.m_secondsIncr;
}

bool TimeParameters::operator!=(const TimeParameters &tp) const {
  return !(*this == tp);
}

String TimeLimit::toString() const {
  const double sec = (double)m_timeout / 1000.0;
  return format(_T("%d:%.3lf%s"), GET_MINUTES(sec), fmod(sec, 60), m_softLimit?_T(" +"):EMPTYSTRING);
}

LevelTimeout::LevelTimeout() {
  setDefault();
}

void LevelTimeout::setDefault() {
  m_timeout[0]         = 0;
  m_timeout[1]         = 1;
  m_timeout[2]         = 5;
  m_timeout[3]         = 10;
  m_timeout[4]         = 25;
  m_timeout[5]         = 45;
}

void LevelTimeout::checkLevel(const TCHAR *method, int level) const {
  if(level < 1 || level > LEVELCOUNT) {
    throwInvalidArgumentException(method, _T("level=%d. Must be [1..%d]"), level, LEVELCOUNT);
  }
}

void LevelTimeout::setTimeout(int level, double seconds) {
  DEFINEMETHODNAME;
  checkLevel(method, level);
  if(seconds < 0) {
    throwInvalidArgumentException(method, _T("seconds=%lf. Must be >= 0"), seconds);
  }
  m_timeout[level-1] = (float)seconds;
}

double LevelTimeout::getTimeout(int level) const { // in seconds
  DEFINEMETHODNAME;
  checkLevel(method, level);
  return m_timeout[level-1];
}

bool LevelTimeout::operator==(const LevelTimeout &rhs) const {
  return memcmp(this, &rhs, sizeof(LevelTimeout)) == 0;
}

bool LevelTimeout::operator!=(const LevelTimeout &rhs) const {
  return !(*this == rhs);
}

const String &EngineOptionValue::getStringValue() const {
  assert(isStringType());
  return m_stringValue;
}

int EngineOptionValue::getIntValue() const {
  assert(!isStringType());
  return m_intValue;
}

bool EngineOptionValue::getBoolValue() const {
  assert(!isStringType() && (m_intValue == 0 || m_intValue == 1));
  return m_intValue ? true : false;
}

void EngineOptionValue::setValue(const String &value) {
  assert(isStringType());
  m_stringValue = value;
}

void EngineOptionValue::setValue(int value) {
  assert(!isStringType());
  m_intValue = value;
}

void EngineOptionValue::setValue(bool value) {
  assert(!isStringType() && (m_intValue == 0 || m_intValue == 1));
  m_intValue = value ? 1 : 0;
}

bool EngineOptionValue::operator==(const EngineOptionValue &v) const {
  if((m_isString != v.m_isString) || (m_name != v.m_name)) {
    return false;
  }
  if(m_isString) {
    return m_stringValue == v.m_stringValue;
  } else {
    return m_intValue == v.m_intValue;
  }
}

int EngineOptionValueArray::getOptionIndex(const String &optionName) const {
  for(UINT i = 0; i < size(); i++) {
    const EngineOptionValue &opt = (*this)[i];
    if(opt.getName() == optionName) {
      return i;
    }
  }
  return -1;
}

EngineOptionValue *EngineOptionValueArray::getValue(const String &optionName) {
  const int index = getOptionIndex(optionName);
  return (index < 0) ? NULL : &((*this)[index]);
}

const EngineOptionValue *EngineOptionValueArray::getValue(const String &optionName) const {
  const int index = getOptionIndex(optionName);
  return (index < 0) ? NULL : &((*this)[index]);
}

#define SETENGINEOPTIONVALUE(name, value)       \
  const int index = getOptionIndex(name);       \
  if(index < 0) {                               \
    add(EngineOptionValue(name, value));        \
  } else {                                      \
    (*this)[index].setValue(value);             \
  }

void EngineOptionValueArray::setValue(const String &optionName, const String &value) {
  SETENGINEOPTIONVALUE(optionName, value);
}

void EngineOptionValueArray::setValue(const String &optionName, int value) {
  SETENGINEOPTIONVALUE(optionName, value);
}

void EngineOptionValueArray::setValue(const String &optionName, bool value) {
  SETENGINEOPTIONVALUE(optionName, value);
}

Options::Options() {
  load();
}

static const TCHAR *SHOWFIELDNAMES           = _T("ShowFieldNames");
static const TCHAR *SHOWLEGALMOVES           = _T("ShowLegalMoves");
static const TCHAR *SHOWPLAYERTOMOVE         = _T("ShowPlayerToMove");
static const TCHAR *ANIMATEMOVES             = _T("AnimateMoves");
static const TCHAR *ANIMATECHECKMATE         = _T("AnimateCheckmate");
static const TCHAR *ASKFORNEWGAME            = _T("AskForNewGame");
static const TCHAR *COMPUTERPLAYER           = _T("ComputerPlayer");
static const TCHAR *SHOWCOMPUTERTIME         = _T("ShowComputerTime");
static const TCHAR *VALIDATE                 = _T("ValidateAfterEdit");
static const TCHAR *TRACEWINDOW              = _T("TraceWindow");
static const TCHAR *TRACEVISIBLE             = _T("TraceVisible");
static const TCHAR *TRACEFONTSIZE            = _T("TraceFontSize");
static const TCHAR *HISTORYFONTSIZE          = _T("HistoryFontSize");
static const TCHAR *GAMEINITIALDIR           = _T("GameInitialDir");
static const TCHAR *LANGUAGE                 = _T("Language");
static const TCHAR *_X                       = _T("X");
static const TCHAR *_Y                       = _T("Y");
static const TCHAR *WIDTH                    = _T("W");
static const TCHAR *HEIGHT                   = _T("H");
static const TCHAR *BOARDWINDOW              = _T("BoardWindow");
static const TCHAR *NORMALPLAYLEVEL          = _T("NormalPlayLevel");
static const TCHAR *OPENINGLIBRARYENABLED    = _T("OpeningLibraryEnabled");
static const TCHAR *ENDGAMETABLEBASEENABLED  = _T("EndGameTablebaseEnabled");
static const TCHAR *ENDGAMETABLEBASEPATH     = _T("EndGameTablebasePath");
static const TCHAR *ENDGAMETABLEBASEMETRIC   = _T("EndGameTablebaseMetric");
static const TCHAR *ENDGAMEDEFENDSTRENGTH    = _T("EndGameDefendStrength");
static const TCHAR *MAXMOVESWITHOUTCAPTURE   = _T("FiftyMovesRule");

static const TCHAR *HISTORYFORMAT            = _T("HistoryFormat");
static const TCHAR *DEPTHINPLIES             = _T("DepthInPlies");
static const TCHAR *CONNECTEDTOSERVER        = _T("Connected");
static const TCHAR *SERVERCOMPUTERNAME       = _T("ServerName");
static const TCHAR *TESTMENUENABLED          = _T("TestMenuEnabled");
static const TCHAR *SHOWENGINECONSOLE        = _T("ShowEngineConsole");
static const TCHAR *MOVESLEFT                = _T("MovesLeft");
static const TCHAR *TIMELEFT                 = _T("TimeLeft");
static const TCHAR *INCREMENT                = _T("Increment");
static const TCHAR *SPEEDTIMELEFT            = _T("SpeedTimeLeft");
static const TCHAR *SPEEDINCREMENT           = _T("SpeedIncrement");
static const TCHAR *AUTOPLAYLEVEL            = _T("AutoPlayLevel");
static const TCHAR *TIMEOUT                  = _T("Timeout");

static const TCHAR *ENGINE                   = _T("Engine");
static const TCHAR *EEAUTHOR                 = _T("Author");
static const TCHAR *EEPATH                   = _T("Path");

static const TCHAR *ENGINEVERBOSEFIELDS      = _T("EngineVerboseFields");
static const TCHAR *EVFDEPTH                 = _T("Depth");
static const TCHAR *EVFSELDEPTH              = _T("SelDepth");
static const TCHAR *EVFSCORE                 = _T("Score");
static const TCHAR *EVFTIME                  = _T("Time");
static const TCHAR *EVFNODES                 = _T("Nodes");
static const TCHAR *EVFNODESPS               = _T("NodesPs");
static const TCHAR *EVFPV                    = _T("PV");
static const TCHAR *EVFSTRING                = _T("String");
static const TCHAR *EVFHASHFULL              = _T("HashFull");
static const TCHAR *EVFMULTIPV               = _T("MultiPV");
static const TCHAR *EVFCPULOAD               = _T("CPULoad");
static const TCHAR *PVVARIANTLENGTH          = _T("VariantLength");

static void savePos(RegistryKey key, const CPoint &p) {
  key.setValue(_X , p.x);
  key.setValue(_Y , p.y);
}

static CPoint loadPos(RegistryKey key, const CPoint &defaultValue) {
  CPoint result;
  result.x = key.getInt(_X, defaultValue.x);
  result.y = key.getInt(_Y, defaultValue.y);
  return result;
}

static void saveSize(RegistryKey key, const CSize &s) {
  key.setValue(WIDTH  , s.cx);
  key.setValue(HEIGHT , s.cy);
}

static CSize loadSize(RegistryKey key, const CSize &defaultValue) {
  CSize result;
  result.cx = key.getInt(WIDTH , defaultValue.cx);
  result.cy = key.getInt(HEIGHT, defaultValue.cy);
  return result;
}

static void saveRect(RegistryKey key, const CRect &r) {
  savePos( key, r.TopLeft());
  saveSize(key, r.Size());
}

static CRect loadRect(RegistryKey key, const CRect &defaultValue) {
  const CPoint tl = loadPos( key, defaultValue.TopLeft());
  const CSize  sz = loadSize(key, defaultValue.Size());
  return CRect(tl, sz);
}

static void saveLevelTimeout(RegistryKey key, const LevelTimeout &lt) {
  for(int level = 1; level <= LEVELCOUNT; level++) {
    key.setValue(format(_T("%d"),level), format(_T("%.2lf"), lt.getTimeout(level)));
  }
}

static LevelTimeout loadLevelTimeout(RegistryKey key, const LevelTimeout &defaultValue) {
  LevelTimeout result;
  for(int level = 1; level <= LEVELCOUNT; level++) {
    const String tmp = key.getString(format(_T("%d"), level),EMPTYSTRING);
    double timeout;
    if(_stscanf(tmp.cstr(),_T("%le"), &timeout) != 1 || timeout <= 0) {
      timeout = defaultValue.getTimeout(level);
    }
    result.setTimeout(level,timeout);
  }
  return result;
}

static void saveEngineDescription(RegistryKey key, const EngineDescription &desc) {
  key.setValue(EMPTYSTRING, desc.getName());
  key.setValue(EEAUTHOR   , desc.getAuthor());
  key.setValue(EEPATH     , desc.getPath());
}

static void loadEngineDescription(RegistryKey key, EngineDescription &desc) {
  desc.m_name   = key.getString(EMPTYSTRING, EMPTYSTRING);
  desc.m_author = key.getString(EEAUTHOR   , EMPTYSTRING);
  desc.m_path   = key.getString(EEPATH     , EMPTYSTRING);
}

EngineVerboseFields::EngineVerboseFields() {
  setDefault();
}

void EngineVerboseFields::setDefault() {
  m_depth           = true;
  m_seldepth        = false;
  m_score           = true;
  m_time            = false;
  m_nodes           = true;
  m_nodesps         = false;
  m_pv              = false;
  m_string          = false;
  m_hashfull        = false;
  m_multipv         = false;
  m_cpuLoad         = false;
  m_pvVariantLength = 5;
}

bool EngineVerboseFields::operator==(const EngineVerboseFields &evf) const {
  return m_depth           == evf.m_depth
      && m_seldepth        == evf.m_seldepth
      && m_score           == evf.m_score
      && m_time            == evf.m_time
      && m_nodes           == evf.m_nodes
      && m_nodesps         == evf.m_nodesps
      && m_pv              == evf.m_pv
      && m_string          == evf.m_string
      && m_hashfull        == evf.m_hashfull
      && m_multipv         == evf.m_multipv
      && m_cpuLoad         == evf.m_cpuLoad
      && m_pvVariantLength == evf.m_pvVariantLength;
}

static void saveEngineVerboseFields(RegistryKey key, const EngineVerboseFields &evf) {
  key.setValue(EVFDEPTH       , evf.m_depth          );
  key.setValue(EVFSELDEPTH    , evf.m_seldepth       );
  key.setValue(EVFSCORE       , evf.m_score          );
  key.setValue(EVFTIME        , evf.m_time           );
  key.setValue(EVFNODES       , evf.m_nodes          );
  key.setValue(EVFNODESPS     , evf.m_nodesps        );
  key.setValue(EVFPV          , evf.m_pv             );
  key.setValue(EVFSTRING      , evf.m_string         );
  key.setValue(EVFHASHFULL    , evf.m_hashfull       );
  key.setValue(EVFMULTIPV     , evf.m_multipv        );
  key.setValue(EVFCPULOAD     , evf.m_cpuLoad        );
  key.setValue(PVVARIANTLENGTH, evf.m_pvVariantLength);
}

static EngineVerboseFields loadEngineVerboseFields(RegistryKey key, const EngineVerboseFields &defaultValue) {
  EngineVerboseFields evf;
  evf.m_depth           = key.getBool(EVFDEPTH       , defaultValue.m_depth          );
  evf.m_seldepth        = key.getBool(EVFSELDEPTH    , defaultValue.m_seldepth       );
  evf.m_score           = key.getBool(EVFSCORE       , defaultValue.m_score          );
  evf.m_time            = key.getBool(EVFTIME        , defaultValue.m_time           );
  evf.m_nodes           = key.getBool(EVFNODES       , defaultValue.m_nodes          );
  evf.m_nodesps         = key.getBool(EVFNODESPS     , defaultValue.m_nodesps        );
  evf.m_pv              = key.getBool(EVFPV          , defaultValue.m_pv             );
  evf.m_string          = key.getBool(EVFSTRING      , defaultValue.m_string         );
  evf.m_hashfull        = key.getBool(EVFHASHFULL    , defaultValue.m_hashfull       );
  evf.m_multipv         = key.getBool(EVFMULTIPV     , defaultValue.m_multipv        );
  evf.m_cpuLoad         = key.getBool(EVFCPULOAD     , defaultValue.m_cpuLoad        );
  evf.m_pvVariantLength = key.getUint(PVVARIANTLENGTH, defaultValue.m_pvVariantLength);
  return evf;
}

void Options::save() {
  try {

    RegistryKey key = getKey();

    key.setValue(SHOWFIELDNAMES          , m_showFieldNames         );
    key.setValue(SHOWLEGALMOVES          , m_showLegalMoves         );
    key.setValue(SHOWPLAYERTOMOVE        , m_showPlayerInTurn       );
    key.setValue(ANIMATEMOVES            , m_animateMoves           );
    key.setValue(ANIMATECHECKMATE        , m_animateCheckmate       );
    key.setValue(ASKFORNEWGAME           , m_askForNewGame          );
    key.setValue(COMPUTERPLAYER          , m_computerPlayer         );
    key.setValue(SHOWCOMPUTERTIME        , m_showComputerTime       );
    key.setValue(VALIDATE                , m_validateAfterEdit      );
    saveRect(getSubKey(TRACEWINDOW)      , CRect(m_traceWindowPos, m_traceWindowSize));
    key.setValue(TRACEVISIBLE            , m_traceWindowVisible     );
    key.setValue(TRACEFONTSIZE           , m_traceFontSize          );
    key.setValue(HISTORYFONTSIZE         , m_historyFontSize        );
    key.setValue(GAMEINITIALDIR          , m_gameInitialDir         );
    key.setValue(LANGUAGE                , m_langID                 );

    saveRect(getSubKey(BOARDWINDOW)      , CRect(m_boardWindowPos, m_boardSize));
    key.setValue(NORMALPLAYLEVEL         , m_normalPlayLevel        );
    saveLevelTimeout(getSubKey(TIMEOUT)  , m_levelTimeout           );
    key.setValue(OPENINGLIBRARYENABLED   , m_openingLibraryEnabled  );
    key.setValue(ENDGAMETABLEBASEENABLED , m_endGameTablebaseEnabled);
    key.setValue(ENDGAMETABLEBASEPATH    , m_endGameTablebasePath   );
    key.setValue(ENDGAMETABLEBASEMETRIC  , EndGameKeyDefinition::getMetricName(m_endGameTablebaseMetric));
    key.setValue(ENDGAMEDEFENDSTRENGTH   , m_endGameDefendStrength  );
    key.setValue(MAXMOVESWITHOUTCAPTURE  , m_maxMovesWithoutCaptureOrPawnMove );
    key.setValue(HISTORYFORMAT           , m_moveFormat             );
    key.setValue(DEPTHINPLIES            , m_depthInPlies           );
    key.setValue(CONNECTEDTOSERVER       , m_connectedToServer      );
    key.setValue(SERVERCOMPUTERNAME      , m_serverComputerName     );
    key.setValue(TESTMENUENABLED         , m_testMenuEnabled        );
    key.setValue(SHOWENGINECONSOLE       , m_showEngineConsole      );

    saveEngineVerboseFields(getSubKey(ENGINEVERBOSEFIELDS), m_engineVerboseFields);

    forEachPlayer(p) {
      RegistryKey subKey = getSubKey(p);
      const PlayerOptions       &po     = m_playerOptions[p];
      const TimeParameters      &tp     = po.m_timeParameters;
      const TimeParameters      &stp    = po.m_speedChessParameters;

      subKey.setValue(AUTOPLAYLEVEL , po.m_autoPlayLevel );
      subKey.setValue(MOVESLEFT     , tp.getMovesLeft()  );
      subKey.setValue(TIMELEFT      , tp.getSecondsLeft());
      subKey.setValue(INCREMENT     , tp.getSecondsIncr());
      subKey.setValue(SPEEDTIMELEFT ,stp.getSecondsLeft());
      subKey.setValue(SPEEDINCREMENT,stp.getSecondsIncr());
      subKey.setValue(ENGINE        , po.m_engineName    );
    }
    clrDirty();

  } catch(Exception e) {
    showException(e);
  }
}

static TablebaseMetric stringToMetric(const String &s, TablebaseMetric defaultValue) {
  if(s == EndGameKeyDefinition::getMetricName(DEPTH_TO_CONVERSION)) {
    return DEPTH_TO_CONVERSION;
  } else if(s == EndGameKeyDefinition::getMetricName(DEPTH_TO_MATE)) {
    return DEPTH_TO_MATE;
  } else {
    return defaultValue;
  }
}

void Options::load() {
  try {
    RegistryKey key = getKey();
    const DefaultOptions defaultOptions;
    m_showFieldNames           = key.getBool(  SHOWFIELDNAMES          , defaultOptions.getShowFieldNames()         );
    m_showLegalMoves           = key.getBool(  SHOWLEGALMOVES          , defaultOptions.getShowLegalMoves()         );
    m_showPlayerInTurn         = key.getBool(  SHOWPLAYERTOMOVE        , defaultOptions.getShowPlayerInTurn()       );
    m_animateMoves             = key.getBool(  ANIMATEMOVES            , defaultOptions.getAnimateMoves()           );
    m_animateCheckmate         = key.getBool(  ANIMATECHECKMATE        , defaultOptions.getAnimateCheckmate()       );
    m_askForNewGame            = key.getBool(  ASKFORNEWGAME           , defaultOptions.getAskForNewGame()          );
    m_computerPlayer           = key.getInt(   COMPUTERPLAYER          , defaultOptions.getComputerPlayer()         )
                               ? BLACKPLAYER
                               : WHITEPLAYER;
    m_showComputerTime         = key.getBool(  SHOWCOMPUTERTIME        , defaultOptions.getShowComputerTime()       );
    m_validateAfterEdit        = key.getBool(  VALIDATE                , defaultOptions.getValidateAfterEdit()      );
    m_traceWindowPos           = loadPos( getSubKey(TRACEWINDOW)       , defaultOptions.getTraceWindowPos()         );
    m_traceWindowSize          = loadSize(getSubKey(TRACEWINDOW)       , defaultOptions.getTraceWindowSize()        );
    m_traceWindowVisible       = key.getBool(  TRACEVISIBLE            , defaultOptions.getTraceWindowVisible()     );
    m_traceFontSize            = key.getInt(   TRACEFONTSIZE           , defaultOptions.getTraceFontSize()          );
    m_historyFontSize          = key.getInt(   HISTORYFONTSIZE         , defaultOptions.getHistoryFontSize()        );
    m_gameInitialDir           = key.getString(GAMEINITIALDIR          , defaultOptions.getGameInitialDir()         );
    m_langID                   = key.getInt(   LANGUAGE                , defaultOptions.getLangID()                 );
    m_boardWindowPos           = loadPos( getSubKey(BOARDWINDOW)       , defaultOptions.getBoardWindowPos()         );
    m_boardSize                = loadSize(getSubKey(BOARDWINDOW)       , defaultOptions.getBoardSize()              );
    m_normalPlayLevel          = key.getInt(   NORMALPLAYLEVEL         , defaultOptions.getNormalPlayLevel()        );
    m_levelTimeout             = loadLevelTimeout(getSubKey(TIMEOUT)   , defaultOptions.getLevelTimeout()           );
    m_openingLibraryEnabled    = key.getBool(  OPENINGLIBRARYENABLED   , defaultOptions.isOpeningLibraryEnabled()   );
    m_endGameTablebaseEnabled  = key.getBool(  ENDGAMETABLEBASEENABLED , defaultOptions.isEndGameTablebaseEnabled() );
    m_endGameTablebasePath     = key.getString(ENDGAMETABLEBASEPATH    , defaultOptions.getEndGameTablebasePath()   );
    m_endGameTablebaseMetric   = stringToMetric(key.getString(ENDGAMETABLEBASEMETRIC,EMPTYSTRING), defaultOptions.getEndGameTablebaseMetric());

    m_endGameDefendStrength    = key.getInt(   ENDGAMEDEFENDSTRENGTH   , defaultOptions.getEndGameDefendStrength()  );
    m_maxMovesWithoutCaptureOrPawnMove = key.getInt(MAXMOVESWITHOUTCAPTURE  , defaultOptions.getMaxMovesWithoutCaptureOrPawnMove());

    m_moveFormat               = key.getInt(   HISTORYFORMAT           , defaultOptions.getMoveFormat())
                               ? MOVE_LONGFORMAT
                               : MOVE_SHORTFORMAT;
    m_depthInPlies             = key.getBool(  DEPTHINPLIES            , defaultOptions.getDepthInPlies()           );
    m_connectedToServer        = key.getBool(  CONNECTEDTOSERVER       , defaultOptions.isConnectedToServer()       );
    m_serverComputerName       = key.getString(SERVERCOMPUTERNAME      , defaultOptions.getServerComputerName()     );
    m_testMenuEnabled          = key.getBool(  TESTMENUENABLED         , defaultOptions.hasTestMenu()               );
    m_showEngineConsole        = key.getBool(  SHOWENGINECONSOLE       , defaultOptions.getShowEngineConsole()      );
    m_engineVerboseFields      = loadEngineVerboseFields(getSubKey(ENGINEVERBOSEFIELDS), defaultOptions.getengineVerboseFields());

    forEachPlayer(p) {
      RegistryKey                subKey = getSubKey(p);
      PlayerOptions             &po     = m_playerOptions[p];

      const PlayerOptions       &defPo  = defaultOptions.getPlayerOptions(p);
      const TimeParameters      &defTp  = defPo.m_timeParameters;
      const TimeParameters      &defStp = defPo.m_speedChessParameters;

      const int       movesLeft         = subKey.getInt(MOVESLEFT     ,  defTp.getMovesLeft());
      const int       secondsLeft       = subKey.getInt(TIMELEFT      ,  defTp.getSecondsLeft());
      const int       secondsIncr       = subKey.getInt(INCREMENT     ,  defTp.getSecondsIncr());
      const int       speedSecondsLeft  = subKey.getInt(SPEEDTIMELEFT , defStp.getSecondsLeft());
      const int       speedSecondsIncr  = subKey.getInt(SPEEDINCREMENT, defStp.getSecondsIncr());
      po.m_autoPlayLevel                = subKey.getInt(AUTOPLAYLEVEL , defPo.m_autoPlayLevel);
      po.m_timeParameters               = TimeParameters(movesLeft, secondsLeft, secondsIncr);
      po.m_speedChessParameters         = TimeParameters(-1, speedSecondsLeft, speedSecondsIncr);
      po.m_engineName                   = subKey.getString(ENGINE     , defPo.m_engineName   );
    }
  } catch(Exception e) {
    setDefault();
  }
  clrDirty();
}

void Options::load(FILE *f) {
  FSEEK(f,0);
  String line;
  for(;;) {
    if(!readLine(f, line)) {
      return;
    }
    if(line.equalsIgnoreCase(_T("Settings"))) {
      break;
    }
  }

  if(feof(f)) {
    return;
  }
  DefaultOptions defaultOptions;
  for(;;) {
    if(!readLine(f, line)) {
      break;
    }
    Tokenizer tok(line, _T(":"));
    String tag;
    int    intValue;
    String strValue;
    if(tok.hasNext()) {
      tag = tok.next();
      if(tag.equalsIgnoreCase(_T("EndSettings"))) {
        break;
      }
      if(tag.equalsIgnoreCase(COMPUTERPLAYER)) {
        if(tok.hasNext()) {
          strValue = tok.next();
          if(strValue.equalsIgnoreCase(getPlayerNameEnglish(WHITEPLAYER))) {
            setComputerPlayer(WHITEPLAYER);
          } else if(strValue.equalsIgnoreCase(getPlayerNameEnglish(BLACKPLAYER)) ) {
            setComputerPlayer(BLACKPLAYER);
          } else {
            setComputerPlayer(defaultOptions.getComputerPlayer());
          }
        }
      } else {
        continue;
        if(tok.hasNext()) {
          if(_stscanf(tok.next().cstr(), _T("%d"), &intValue) != 1) {
            break;
          }
        }
      }
    }
  }
}

void Options::save(FILE *f) {
  _ftprintf(f,_T("Settings\n"));
  _ftprintf(f,_T("%s:%s\n"), COMPUTERPLAYER, getPlayerNameEnglish(m_computerPlayer));
  _ftprintf(f,_T("EndSettings\n"));
}

void Options::setShowFieldNames(bool show) {
  if(show != m_showFieldNames) {
    m_showFieldNames = show;
    setDirty();
  }
}

void Options::setShowLegalMoves(bool show) {
  if(show != m_showLegalMoves) {
    m_showLegalMoves = show;
    setDirty();
  }
}

void Options::setShowPlayerInTurn(bool show) {
  if(show != m_showPlayerInTurn) {
    m_showPlayerInTurn = show;
    setDirty();
  }
}

void Options::setAnimateMoves(bool animate) {
  if(animate != m_animateMoves) {
    m_animateMoves = animate;
    setDirty();
  }
}

void Options::setAnimateCheckmate(bool animate) {
  if(animate != m_animateCheckmate) {
    m_animateCheckmate = animate;
    setDirty();
  }
}

void Options::setAskForNewGame(bool ask) {
  if(ask != m_askForNewGame) {
    m_askForNewGame = ask;
    setDirty();
  }
}

void Options::setComputerPlayer(Player computerPlayer) {
  if(computerPlayer != m_computerPlayer) {
    m_computerPlayer = computerPlayer;
    setDirty();
  }
}

void Options::setShowComputerTime(bool show) {
  if(show != m_showComputerTime) {
    m_showComputerTime = show;
    setDirty();
  }
}

void Options::setValidateAfterEdit(bool validate) {
  if(validate != m_validateAfterEdit) {
    m_validateAfterEdit = validate;
    setDirty();
  }
}

void Options::setTraceWindowPos(const CPoint &pos) {
  if(pos != m_traceWindowPos) {
    m_traceWindowPos = pos;
    setDirty();
  }
}

void Options::setTraceWindowSize(const CSize &size) {
  if(size != m_traceWindowSize) {
    m_traceWindowSize = size;
    setDirty();
  }
}

void Options::setTraceWindowVisible(bool visible) {
  if(visible != m_traceWindowVisible) {
    m_traceWindowVisible = visible;
    setDirty();
  }
}

void Options::setTraceFontSize(int size) {
  if(size != m_traceFontSize) {
    m_traceFontSize = size;
    setDirty();
  }
}

void Options::setHistoryFontSize(int size) {
  if(size != m_historyFontSize) {
    m_historyFontSize = size;
    setDirty();
  }
}

void Options::setGameInitialDir(const String &dir) {
  String tmp = dir;
  const intptr_t len = tmp.length();
  if(len > 1 && tmp[len-1] == '\\') {
    tmp = left(tmp, len-1);
  }

  if(!tmp.equalsIgnoreCase(m_gameInitialDir)) {
    m_gameInitialDir = tmp;
    setDirty();
  }
}

bool Options::setLangID(LANGID langID) {
  if(PRIMARYLANGID(langID) != PRIMARYLANGID(m_langID)) {
    m_langID = Language::getBestSupportedLanguage(langID).m_langID;
    setDirty();
    return true;
  }
  return false;
}

int Options::getSelectedLanguageIndex() const {
  const Array<Language> &spla = Language::getSupportedLanguages();
  for(UINT i = 0; i < spla.size(); i++) {
    if(spla[i].m_langID == m_langID) {
      return i;
    }
  }
  return -1;
}

void Options::setBoardWindowPos(const CPoint &pos) {
  if(pos != m_boardWindowPos) {
    m_boardWindowPos = pos;
    setDirty();
  }
}

void Options::setBoardSize(const CSize &size) {
  if(size != m_boardSize) {
    m_boardSize = size;
    setDirty();
  }
}

void Options::setNormalPlayLevel(int level) {
  if(level != m_normalPlayLevel) {
    m_normalPlayLevel = level;
    setDirty();
  }
}

void Options::setLevelTimeout(const LevelTimeout &lt) {
  if(lt != m_levelTimeout) {
    m_levelTimeout = lt;
    setDirty();
  }
}

void Options::enableOpeningLibrary(bool enabled) {
  if(enabled != m_openingLibraryEnabled) {
    m_openingLibraryEnabled = enabled;
    setDirty();
  }
}

void Options::enableEndGameTablebase(bool enabled) {
  if(enabled != m_endGameTablebaseEnabled) {
    m_endGameTablebaseEnabled = enabled;
    setDirty();
  }
}

void Options::setEndGameTablebasePath(const String &path) {
  if(path != m_endGameTablebasePath) {
    m_endGameTablebasePath = path;
    setDirty();
  }
}

void Options::setEndGameTablebaseMetric(TablebaseMetric metric) {
  if(metric != m_endGameTablebaseMetric) {
    m_endGameTablebaseMetric = metric;
    setDirty();
  }
}

void Options::setEndGameDefendStrength(int defendStrength) {
  if(defendStrength != m_endGameDefendStrength) {
    m_endGameDefendStrength = defendStrength;
    setDirty();
  }
}

void Options::setMaxMovesWithoutCaptureOrPawnMove(int maxMoves) {
  if(maxMoves != m_maxMovesWithoutCaptureOrPawnMove) {
    m_maxMovesWithoutCaptureOrPawnMove = maxMoves;
    setDirty();
  }
}

void Options::setMoveFormat(MoveStringFormat moveFormat) {
  if(moveFormat != m_moveFormat) {
    m_moveFormat = moveFormat;
    setDirty();
  }
}

void Options::setDepthInPlies(bool plies) {
  if(plies != m_depthInPlies) {
    m_depthInPlies = plies;
    setDirty();
  }
}

void Options::setConnectedToServer(bool connected) {
  if(connected != m_connectedToServer) {
    m_connectedToServer = connected;
    setDirty();
  }
}

void Options::setServerComputerName(const String &computerName) {
  const String s = trim(computerName);
  if(s != m_serverComputerName) {
    m_serverComputerName = s;
    setDirty();
  }
}

void Options::enableTestMenu(bool enabled) {
  if(enabled != m_testMenuEnabled) {
    m_testMenuEnabled = enabled;
    setDirty();
  }
}

void Options::setShowEngineConsole(bool show) {
  if(show != m_showEngineConsole) {
    m_showEngineConsole = show;
    setDirty();
  }
}

void Options::setEngineVerboseFields(const EngineVerboseFields &evf) {
  if (evf != m_engineVerboseFields) {
    m_engineVerboseFields = evf;
    setDirty();
  }
}

void Options::setAutoPlayLevel(Player player, int level) {
  PlayerOptions &po = m_playerOptions[player];
  if(level != po.m_autoPlayLevel) {
    po.m_autoPlayLevel = level;
    setDirty();
  }
}

void Options::setTimeParameters(Player player, const TimeParameters &parameters) {
  PlayerOptions &po = m_playerOptions[player];
  if(parameters != po.m_timeParameters) {
    po.m_timeParameters = parameters;
    setDirty();
  }
}

void Options::setSpeedTimeParameters(Player player, const TimeParameters &parameters) {
  PlayerOptions &po = m_playerOptions[player];
  if(parameters != po.m_speedChessParameters) {
    po.m_speedChessParameters = parameters;
    setDirty();
  }
}

void Options::setEngineName(Player player, const String &name) {
  PlayerOptions &po = m_playerOptions[player];
  if(name != po.m_engineName) {
    po.m_engineName = name;
    setDirty();
  }
}

int Options::getCurrentEngineIndex(Player player) const {
  return getEngineRegister().getIndexByName(getPlayerOptions(player).m_engineName);
}

Options &Options::getOptions() { // static
  return OptionsAccessor().getOptions();
}

EngineRegister Options::s_engineRegister;

EngineRegister &Options::getEngineRegister() { // static
  if(s_engineRegister.size() == 0) {
    loadEngineRegister();
  }
  return s_engineRegister;
}

const String &Options::getEnginePathByPlayer(Player player) { // static
  return getEngineRegister().getPathByName(getOptions().getPlayerOptions(player).m_engineName);
}

void Options::loadEngineRegister() { // static
  s_engineRegister.clear();
  RegistryKey key = getEngineSubKey(getKey());
  for(Iterator<String> it = key.getSubKeyIterator(); it.hasNext();) {
    const String keyName = it.next();
    RegistryKey engineKey = key.openKey(keyName);
    EngineDescription desc;
    loadEngineDescription(engineKey, desc);
    s_engineRegister.add(desc);
  }
}

void Options::saveEngineRegister(EngineRegister &engines) { // static
  RegistryKey key = getEngineSubKey(getKey());
  key.deleteSubKeys();
  for(size_t i = 0; i < engines.size(); i++) {
    RegistryKey subKey = key.createKey(format(_T("%02d"), (int)i));
    saveEngineDescription(subKey, engines[i]);
  }
  s_engineRegister = engines;
  forEachPlayer(p) {
    if(getOptions().getCurrentEngineIndex(p) < 0) {
      getOptions().setEngineName(p, EMPTYSTRING);
    }
  }
}

const String &EngineRegister::getPathByName(const String &name) const {
  const int index = getIndexByName(name);
  if(index >= 0) {
    return (*this)[index].getPath();
  }
  throwException(_T("Unknown engine:%s"), name.cstr());
  static String dummyString;
  return dummyString;
}

int EngineRegister::getIndexByName(const String &name) const {
  for(size_t i = 0; i < size(); i++) {
    const EngineDescription &desc = (*this)[i];
    if(desc.getName() == name) {
      return (int)i;
    }
  }
  return -1;
}

void Options::saveEngineOptionValues(Player player, const EngineOptionValueArray &valueArray) { // static
  RegistryKey key = getEngineOptionsSubKey(player, valueArray.getEngineName());
  key.deleteValues();
  for(ConstIterator<EngineOptionValue> it = valueArray.getIterator(); it.hasNext();) {
    const EngineOptionValue &option = it.next();
    if(option.isStringType()) {
      key.setValue(option.getName(), option.getStringValue());
    } else {
      key.setValue(option.getName(), option.getIntValue());
    }
  }
}

EngineOptionValueArray Options::getEngineOptionValues(Player player, const String &engineName) { // static
  EngineOptionValueArray result(engineName);
  RegistryKey key = getEngineOptionsSubKey(player, engineName);
  for(Iterator<RegistryValue> it = key.getValueIterator(); it.hasNext();) {
    RegistryValue value = it.next();
    if(value.type() == REG_SZ) {
      result.setValue(value.name(), (String)value);
    } else {
      result.setValue(value.name(), (int)((ULONG)value));
    }
  }
  return result;
}

void setSelectedLanguageForThread() {
  Language::setLanguageForThread(Options::getOptions().getLangID());
}
