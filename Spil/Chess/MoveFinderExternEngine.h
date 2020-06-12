#pragma once

#if !defined(TABLEBASE_BUILDER)

#include "AbstractMoveFinder.h"
#include "ExternEngine.h"
#include "EngineOptionsDlgThread.h"

class MoveFinderExternEngine : public AbstractMoveFinder, public PropertyChangeListener, OptionsAccessor {
private:
  ExternEngine             m_externEngine;
  CEngineOptionsDlgThread *m_optionsDlgThread;
public:
  MoveFinderExternEngine(Player player, ChessPlayerRequestQueue &msgQueue);
  ~MoveFinderExternEngine();
  void findBestMove(const RequestParamFindMove &param);
  void stopSearch();
  void moveNow();
  String getName() const;

  MoveFinderType getType()  const {
    return EXTERN_ENGINE;
  }

  ExternEngine &getEngine() {
    return m_externEngine;
  }

  PositionType getPositionType() const {
    return NORMAL_POSITION;
  }

  void runOptionsDialog();
  void setVerbose(bool verbose);
  void notifyGameChanged(const Game &game);
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);

  String getStateString(bool detailed);

};

#endif
