#pragma once

#ifndef TABLEBASE_BUILDER

#include <ExternProcess.h>
#include <InputThread.h>
#include "ChessPlayerRequest.h"

typedef enum { // dont swap these. see optionsCmp in ExternEngine.cpp
  OptionTypeSpin
 ,OptionTypeCombo
 ,OptionTypeString
 ,OptionTypeCheckbox
 ,OptionTypeButton
} EngineOptionType;

class EngineOptionDescription {
private:
  String               m_name;
  EngineOptionType     m_type;
  const USHORT         m_index;
  int                  m_min, m_max;
  int                  m_defaultInt;
  bool                 m_defaultBool;
  String               m_defaultString;
  StringArray          m_comboValues;
public:
  EngineOptionDescription(String line, USHORT index);
  const String &getName() const {
    return m_name;
  }
  String getLabelName() const;
  inline EngineOptionType getType() const {
    return m_type;
  }
  USHORT getIndex() const {
    return m_index;
  }
  int getMin() const {
    assert(getType() == OptionTypeSpin);
    return m_min;
  }
  int getMax() const {
    assert(getType() == OptionTypeSpin);
    return m_max;
  }
  const StringArray &getComboValues() const {
    assert(getType() == OptionTypeCombo);
    return m_comboValues;
  }
  int getValueIndex(const String &str) const;
  String toString() const;
  inline bool operator==(const EngineOptionDescription &op) const {
    return toString() == op.toString();
  }
  inline bool operator!=(const EngineOptionDescription &op) const {
    return !(*this == op);
  }
  inline bool getDefaultBool() const {
    assert(getType() == OptionTypeCheckbox);
    return m_defaultBool;
  }
  inline int getDefaultInt() const {
    assert(getType()== OptionTypeSpin);
    return m_defaultInt;
  }
  inline const String &getDefaultString() const {
    assert((getType() == OptionTypeCombo) || (getType() == OptionTypeString));
    return m_defaultString;
  }
  String getDefaultAsString() const;
  static const EngineOptionDescription debugOption;
};

class EngineOptionDescriptionArray : public Array<EngineOptionDescription> {
public:
  const EngineOptionDescription *findOptionByName(const String &optionName) const; // return NULL if not found
  EngineOptionValueArray createDefaultValueArray(const String &engineName) const;
  EngineOptionDescriptionArray &removeOptionsByType(EngineOptionType type); // return *this
  EngineOptionValueArray pruneDefaults(const EngineOptionValueArray &src) const;
};

class EngineInfoLine {
public:
  int     m_depth;
  int     m_seldepth;
  String  m_score;
  int     m_time;    // milliseconds
  UINT64  m_nodes;
  UINT    m_nodesps;
  String  m_pv;
  String  m_string;
  int     m_hashFull;
  int     m_multiPV;
  int     m_cpuLoad;

  inline EngineInfoLine() {
    reset();
  }
  EngineInfoLine &operator+=(const String &line);
  String toString(const EngineVerboseFields &evf) const;
  void reset();
  inline bool isReady() const {
    return m_score.length() > 0;
  }
};

#define EXE_UCIOK         0x1
#define EXE_BUSY          0x2
#define EXE_KILLED        0x4
#define EXE_THREADRUNNING 0x8

#define isStateFlagsSet(flags) ((m_stateFlags &   (flags)) == (flags))

class ExternEngine : public ExternProcess, public Thread, public OptionsAccessor {
private:
  const Player                 m_player;
  EngineDescription            m_desc;
  EngineOptionDescriptionArray m_optionArray;
  InputThread                 *m_inputThread;
  MoveReceiver                *m_moveReceiver;
  mutable Game                 m_game;
  BYTE                         m_stateFlags;
  Semaphore                    m_gate;
  mutable int                  m_callLevel;

  void killProcess();
  void cleanup();
  void deleteInputThread();
  void waitUntilNotBusy(int timeout = 2000);
  void putInterruptLine();
  void sendUCI();
  void sortOptions();
  void sendPosition() const;

  inline void setStateFlags(BYTE flags) {
    m_gate.wait();
    m_stateFlags |= flags;
    m_gate.signal();
  }

  inline void clrStateFlags(BYTE flags) {
    m_gate.wait();
    m_stateFlags &= ~flags;
    m_gate.signal();
  }

  inline void clrAllStateFlags() {
    m_gate.wait();
    m_stateFlags = 0;
    m_gate.signal();
  }

  // timeout in milliseconds. If INFINTE, thread is blocked until input arrives
  String getLine(int timeout = INFINITE); // throw TimeoutException on timeout
#ifdef _DEBUG
  void send(const TCHAR *format,...) const;
#endif
  void   setDebug(bool on);
  String getBeautifiedVariant(const String &pv) const;
  void   setParameterValue(const EngineOptionValue &v);
  void   debugMsg(const TCHAR *format, ...) const;
  bool   hasInput() const {
    return m_inputThread != NULL;
  }
public:
  ExternEngine(Player player, const String &path);
 ~ExternEngine();
  void start(MoveReceiver *mr = NULL);
  void quit();
  inline bool isStarting() const {
    return isStarted() && !isStateFlagsSet(EXE_UCIOK);
  }
  inline bool isReady() const {
    return isStarted() && hasInput() && isStateFlagsSet(EXE_UCIOK);
  }
  inline bool isThreadRunning() const {
    return isStateFlagsSet(EXE_THREADRUNNING);
  }
  inline bool isBusy() const {
    return isReady()   && isStateFlagsSet(EXE_BUSY);
  }
  inline bool isIdle() const {
    return isReady()   && !isStateFlagsSet(EXE_BUSY);
  }
  void  notifyGameChanged(const Game &game);
  const EngineDescription &getDescription() const {
    return m_desc;
  }
  void  findBestMove(const Game &game, const TimeLimit &timeLimit);
  void  stopSearch();
  void  moveNow();
  UINT run();
  String flagsToString() const;
  String toString() const;
  const EngineOptionDescriptionArray &getOptionDescriptionArray() const {
    return m_optionArray;
  }
  void setParameterValue(const EngineOptionDescription &option, int           value); // OptionTypeSpin
  void setParameterValue(const EngineOptionDescription &option, bool          value); // OptionTypeCheckbox
  void setParameterValue(const EngineOptionDescription &option, const String &value); // OptionTypeCombo, OptionTypeString
  void clickButton(      const EngineOptionDescription &option                     ); // for OptionTypeButton

  void setParameters(const EngineOptionValueArray &valueArray);
  static EngineDescription getUCIReply(const String &path);
};

#endif
