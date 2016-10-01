#pragma once

#include "InputThread.h"

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
  const unsigned short m_index;
  int                  m_min, m_max;
  int                  m_defaultInt;
  bool                 m_defaultBool;
  String               m_defaultString;
  StringArray          m_comboValues;
public:
  EngineOptionDescription(String line, unsigned short index);
  const String &getName() const {
    return m_name;
  }
  String getLabelName() const;
  inline EngineOptionType getType() const {
    return m_type;
  }
  unsigned short getIndex() const {
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
  bool operator==(const EngineOptionDescription &op) const {
    return toString() == op.toString();
  }
  bool operator!=(const EngineOptionDescription &op) const {
    return !(*this == op);
  }
  bool getDefaultBool() const {
    assert(getType() == OptionTypeCheckbox);
    return m_defaultBool;
  }
  int getDefaultInt() const {
    assert(getType()== OptionTypeSpin);
    return m_defaultInt;
  }
  const String &getDefaultString() const {
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

class ExternEngine : public EngineDescription {
private:
  FILE                        *m_input;
  FILE                        *m_output;
  HANDLE                       m_processHandle;
  InputThread                 *m_inputThread;
  Game                        *m_tmpGame;
  bool                         m_busy;
  bool                         m_verbose;
  EngineOptionDescriptionArray m_optionArray;
  void stop();
  void start(bool silent, const String program, ...); // cannot use String &, because va_start will fail
  void vstartSpawn(        const String &program, va_list argptr);
  void vstartCreateProcess(const String &program, va_list argptr);
  void waitUntilIdle();
  void send(const TCHAR *format,...) const;
  void cleanup();
  void killProcess();
  void killInputThread();
  String getLine(int milliseconds = INFINITE); // throw TimeoutException on timeout

  void setBusy(         bool newValue) {
    m_busy = newValue;
  }
  void setProcessHandle(HANDLE handle) {
    m_processHandle = handle;
  }
  void sendUCI();
  void sortOptions();
  void sendPosition(const Game &game) const;
  void setDebug(bool on);
  String getBeautifiedVariant(const String &pv) const;
  void setParameterValue(const EngineOptionValue &v);
public:
  ExternEngine(const String &path);
 ~ExternEngine();
  void start();
  bool isStarted() const {
    return m_input != NULL;
  }
  bool isBusy() const {
    return m_busy;
  }
  void  notifyGameChanged(const Game &game);
  ExecutableMove findBestMove(const Game &game, const TimeLimit &timeLimit, bool hint);
  void  stopSearch();
  void  moveNow();
  void  setVerbose(bool verbose);

  String toString() const;
  static String getPositionString(const Game &game);
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
