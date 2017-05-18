#pragma once

class EngineOptionValue {
private:
  const bool             m_isString;
  const String           m_name;
  String                 m_stringValue;
  int                    m_intValue;
public:
  EngineOptionValue(const String &name, const String &value)
    : m_name(name)
    , m_isString(true)
    , m_stringValue(value)
  {
  }
  EngineOptionValue(const String &name, int value)
    : m_name(name)
    , m_isString(false)
    , m_intValue(value)
  {
  }
  EngineOptionValue(const String &name, bool value)
    : m_name(name)
    , m_isString(false)
    , m_intValue(value?1:0)
  {
  }

  bool isStringType() const {
    return m_isString;
  }

  const String &getName() const {
    return m_name;
  }
  const String &getStringValue() const;
  int           getIntValue()    const;
  bool          getBoolValue()   const;
  void          setValue(const String &value);
  void          setValue(int           value);
  void          setValue(bool          value);
  bool operator==(const EngineOptionValue &v) const;
  bool operator!=(const EngineOptionValue &v) const {
    return !(*this == v);
  }
};

class EngineOptionValueArray : public Array<EngineOptionValue> {
private:
  int getOptionIndex(const String &optionName) const;
  String m_engineName;
public:
  EngineOptionValueArray(const String &engineName=_T("")) : m_engineName(engineName) {
  }
  const String &getEngineName() const {
    return m_engineName;
  }
        EngineOptionValue *getValue(const String &optionName);
  const EngineOptionValue *getValue(const String &optionName) const;
  void               setValue(const String &optionName, const String &value);
  void               setValue(const String &optionName, int           value);
  void               setValue(const String &optionName, bool          value);
  Iterator<EngineOptionValue> getIterator() const {
    return ((EngineOptionValueArray*)this)->Array<EngineOptionValue>::getIterator();
  }
};

class EngineDescription {
public:
  String m_path;
  String m_name;
  String m_author;

  EngineDescription(const String &path=EMPTYSTRING) : m_path(path) {
  }
  const String &getPath() const {
    return m_path;
  }
  const String getName() const {
    return m_name;
  }
  const String getAuthor() const {
    return m_author;
  }
  void clear() {
    m_path = m_name = m_author = EMPTYSTRING;
  }
  bool operator==(const EngineDescription &desc) const;
  bool operator!=(const EngineDescription &desc) const {
    return !(*this == desc);
  }
};

class EngineRegister : public Array<EngineDescription> {
public:
  const String &getPathByName( const String &name) const; // throw Exception if not found
  int           getIndexByName(const String &name) const; // return -1 if not found
};

class EngineVerboseFields {
public:
  EngineVerboseFields();
  void setDefault();

  bool m_depth   ;
  bool m_seldepth;
  bool m_score   ;
  bool m_time    ;
  bool m_nodes   ;
  bool m_nodesps ;
  bool m_pv      ;
  bool m_string  ;
  bool m_hashfull;
  bool m_multipv ;
  bool m_cpuLoad ;
  bool operator==(const EngineVerboseFields &evf) const;
  inline bool operator!=(const EngineVerboseFields &evf) const {
    return !(*this == evf);
  }
};

#define LEVEL_SPEEDCHESS -1
#define LEVEL_TIMEDGAME   0
#define LEVELCOUNT        6

class TimeParameters {
private:
  int              m_movesLeft;
  int              m_secondsLeft;
  int              m_secondsIncr;
public:
  TimeParameters();
  TimeParameters(int movesLeft, int secondsLeft, int secondsIncr);
  void setDefault();
  void setDefaultSpeedChess();
  int getMovesLeft() const {
    return m_movesLeft;
  }
  bool isSpeedChess() const {
    return m_movesLeft < 0;
  }
  int getSecondsLeft() const {
    return m_secondsLeft;
  }
  int getSecondsIncr() const {
    return m_secondsIncr;
  }
  bool operator==(const TimeParameters &rhs) const;
  bool operator!=(const TimeParameters &rhs) const;
};

class LevelTimeout {
private:
  float                m_timeout[LEVELCOUNT];  // in seconds
  void checkLevel(const TCHAR *function, int level) const;
public:
  LevelTimeout();
  void setDefault();
  double getTimeout(int level) const; // in seconds. level = [1..LEVELCOUNT]
  void setTimeout(int level, double seconds);
  bool operator==(const LevelTimeout &rhs) const;
  bool operator!=(const LevelTimeout &rhs) const;
};

class TimeLimit {
public:
  int  m_timeout; // in milliseconds / INFINTE if search forever
  bool m_softLimit;
  TimeLimit(int timeout=500, bool softLimit=false) : m_timeout(timeout), m_softLimit(softLimit) {
  }
  String toString() const;
};

class PlayerOptions {
public:
  PlayerOptions();
  void setDefault();
  int                  m_autoPlayLevel;
  TimeParameters       m_timeParameters, m_speedChessParameters;
  String               m_engineName;
};

class DefaultOptions {
protected:
  bool                  m_showFieldNames;
  bool                  m_showLegalMoves;
  bool                  m_showPlayerInTurn;
  bool                  m_animateMoves;
  bool                  m_animateCheckmate;
  bool                  m_askForNewGame;
  Player                m_computerPlayer;
  bool                  m_showComputerTime;
  bool                  m_validateAfterEdit;
  bool                  m_traceWindowVisible;
  int                   m_traceFontSize;
  int                   m_historyFontSize;
  String                m_gameInitialDir;
  LANGID                m_langID;
  CSize                 m_boardSize;                 // size in pixels
  int                   m_normalPlayLevel;           // for normal play
  LevelTimeout          m_levelTimeout;
  bool                  m_openingLibraryEnabled;
  bool                  m_endGameTablebaseEnabled;
  String                m_endGameTablebasePath;
  TablebaseMetric       m_endGameTablebaseMetric;
  char                  m_endGameDefendStrength;
  int                   m_maxMovesWithoutCaptureOrPawnMove;
  MoveStringFormat      m_moveFormat;
  bool                  m_depthInPlies;
  bool                  m_connectedToServer;
  String                m_serverComputerName;
  bool                  m_testMenuEnabled;
  bool                  m_showEngineConsole;
  EngineVerboseFields   m_engineVerboseFields;
  PlayerOptions         m_playerOptions[2];
public:
  void setDefault();
  DefaultOptions();
  bool                      getShowFieldNames()                   const { return m_showFieldNames;                               }
  bool                      getShowLegalMoves()                   const { return m_showLegalMoves;                               }
  bool                      getShowPlayerInTurn()                 const { return m_showPlayerInTurn;                             }
  bool                      getAnimateMoves()                     const { return m_animateMoves;                                 }
  bool                      getAnimateCheckmate()                 const { return m_animateCheckmate;                             }
  bool                      getAskForNewGame()                    const { return m_askForNewGame;                                }
  Player                    getComputerPlayer()                   const { return m_computerPlayer;                               }
  bool                      getShowComputerTime()                 const { return m_showComputerTime;                             }
  bool                      getValidateAfterEdit()                const { return m_validateAfterEdit;                            }
  bool                      getTraceWindowVisible()               const { return m_traceWindowVisible;                           }
  int                       getTraceFontSize()                    const { return m_traceFontSize;                                }
  int                       getHistoryFontSize()                  const { return m_historyFontSize;                              }
  const String             &getGameInitialDir()                   const { return m_gameInitialDir;                               }
  LANGID                    getLangID()                           const { return m_langID;                                       }
  const CSize              &getBoardSize()                        const { return m_boardSize;                                    }
  int                       getNormalPlayLevel()                  const { return m_normalPlayLevel;                              }
  const LevelTimeout       &getLevelTimeout()                     const { return m_levelTimeout;                                 }
  bool                      isOpeningLibraryEnabled()             const { return m_openingLibraryEnabled;                        }
  bool                      isEndGameTablebaseEnabled()           const { return m_endGameTablebaseEnabled;                      }
  const String             &getEndGameTablebasePath()             const { return m_endGameTablebasePath;                         }
  TablebaseMetric           getEndGameTablebaseMetric()           const { return m_endGameTablebaseMetric;                       }
  int                       getEndGameDefendStrength()            const { return m_endGameDefendStrength;                        }
  int                       getMaxMovesWithoutCaptureOrPawnMove() const { return m_maxMovesWithoutCaptureOrPawnMove;             }
  MoveStringFormat          getMoveFormat()                       const { return m_moveFormat;                                   }
  bool                      getDepthInPlies()                     const { return m_depthInPlies;                                 }
  bool                      isConnectedToServer()                 const { return m_connectedToServer;                            }
  const String             &getServerComputerName()               const { return m_serverComputerName;                           }
  bool                      hasTestMenu()                         const { return m_testMenuEnabled;                              }
  bool                      getShowEngineConsole()                const { return m_showEngineConsole;                            }
  const EngineVerboseFields getengineVerboseFields()              const { return m_engineVerboseFields;                          }
  const PlayerOptions      &getPlayerOptions(      Player player) const { return m_playerOptions[player];                        }
  int                       getAutoPlayLevel(      Player player) const { return getPlayerOptions(player).m_autoPlayLevel;       }
  const TimeParameters     &getTimeParameters(     Player player) const { return getPlayerOptions(player).m_timeParameters;      }
  const TimeParameters     &getSpeedTimeParameters(Player player) const { return getPlayerOptions(player).m_speedChessParameters;}
};

class Options : public DefaultOptions {
private:
  static EngineRegister s_engineRegister;
  static void loadEngineRegister();
  bool m_dirty;
  inline void setDirty() {
    m_dirty = true;
  }
  inline void clrDirty() {
    m_dirty = false;
  }
public:
  Options();
  void save(); // to registry
  void load();
  void save(FILE *f);
  void load(FILE *f);
  bool isDirty() const {
    return m_dirty;
  }
  void setShowFieldNames(        bool                show                                );
  void setShowLegalMoves(        bool                show                                );
  void setShowPlayerInTurn(      bool                show                                );
  void setAnimateMoves(          bool                animate                             );
  void setAnimateCheckmate(      bool                animate                             );
  void setAskForNewGame(         bool                ask                                 );
  void setComputerPlayer(        Player              computerPlayer                      );
  void setShowComputerTime(      bool                show                                );
  void setValidateAfterEdit(     bool                validate                            );
  void setTraceWindowVisible(    bool                visible                             );
  void setTraceFontSize(         int                 size                                );
  void setHistoryFontSize(       int                 size                                );
  void setGameInitialDir(        const String       &dir                                 );
  bool setLangID(                LANGID              langID                              );
  void setBoardSize(             const CSize        &size                                );
  void setNormalPlayLevel(       int                 level                               );
  void setLevelTimeout(          const LevelTimeout &timeout                             );
  void enableOpeningLibrary(     bool                enabled                             );
  void enableEndGameTablebase(   bool                enabled                             );
  void setEndGameTablebasePath(  const String       &path                                );
  void setEndGameTablebaseMetric(TablebaseMetric     metric                              );
  void setEndGameDefendStrength( int                 defendStrength                      );    // 0-100% 0 = random defend, 100 = optimal
  void setMaxMovesWithoutCaptureOrPawnMove(int       maxMoves                            )   ; // FIDE = 50
  void setMoveFormat(            MoveStringFormat    moveFormat                          );
  void setDepthInPlies(          bool                plies                               );
  void setConnectedToServer(     bool                connected                           );
  void setServerComputerName(    const String       &computerName                        );
  void enableTestMenu(           bool                enable                              );
  void setShowEngineConsole(     bool                show                                );
  void setEngineVerboseFields(   const EngineVerboseFields &evf                          );
  void setAutoPlayLevel(         Player player, int level                                );
  void setTimeParameters(        Player player, const TimeParameters      &parameters    );
  void setSpeedTimeParameters(   Player player, const TimeParameters      &parameters    );
  void setEngineName(            Player player, const String              &name          );
  int  getCurrentEngineIndex(    Player player) const; // return -1 if not found
  int  getSelectedLanguageIndex()       const;
  static Options &getOptions();
  static EngineRegister        &getEngineRegister();
  static const String          &getEnginePathByPlayer( Player player);
  static void                   saveEngineRegister(    EngineRegister &engines);
  static void                   saveEngineOptionValues(Player player, const EngineOptionValueArray &valueArray);
  static EngineOptionValueArray getEngineOptionValues( Player player, const String &engineName);
};

void setSelectedLanguageForThread();

class OptionsAccessor {
public:
  Options &getOptions();
  const Options &getOptions() const;
};
