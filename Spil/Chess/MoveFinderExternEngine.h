#pragma once

#ifndef TABLEBASE_BUILDER

#include "AbstractMoveFinder.h"
#include "ExternEngine.h"
#include "EngineOptionsDlgThread.h"

class MoveFinderExternEngine : public AbstractMoveFinder, public PropertyChangeListener, OptionsAccessor {
private:
  ExternEngine             m_externEngine;
  CEngineOptionsDlgThread *m_optionsDlgThread;
public:
  MoveFinderExternEngine(Player player);
  ~MoveFinderExternEngine();
  ExecutableMove findBestMove(Game &game, const TimeLimit &timeLimit, bool talking, bool hint);
  String getName() const;

  EngineType getEngineType()  const {
    return EXTERN_ENGINE;
  }

  ExternEngine &getEngine() {
    return m_externEngine;
  }

  bool isRemote() const {
    return false;
  }

  PositionType getPositionType() const {
    return NORMAL_POSITION;
  }

  void stopThinking(bool stopImmediately = true);
  void openOptionsDialog();
  void setVerbose(bool verbose);
  void notifyGameChanged(const Game &game);
  void notifyMove(const MoveBase &move);
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);

  String getStateString(Player computerPlayer, bool detailed);

};

#endif
