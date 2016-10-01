#pragma once

#include <Date.h>

class StopWatch {
private:
  bool         m_countDown;
  unsigned int m_startMSeconds;
  bool         m_running;
  unsigned int m_msecondsPaused;
  Timestamp    m_startTime;
  Timestamp    m_stopTime;
public:
  StopWatch(bool countDown = true);
  void init(unsigned int startSeconds = 0);
  void stop();
  void resume();
  void addSeconds(unsigned int seconds);
  unsigned int getElapsedTime() const;   // in milliseconds
  unsigned int getSecondsRemaining() const; // in seconds. call only if counting down
  inline bool isRunnning() const {
    return m_running;
  }
};

typedef enum {
  CW_STOPPED
 ,CW_RUNNING
 ,CW_PAUSED
} ChessWatchState;

class ChessWatch : OptionsAccessor {
private:
  StopWatch       m_stopWatch[2];
  Player          m_playerInTurn;
  ChessWatchState m_state;
  unsigned int    m_secondsIncr[2];
  void initWatch(Player player, int secondsLeft, int secondsIncr = 0);
public:
  ChessWatch();
  void setPlayerInTurn(Player player);
  void start(bool autoPlay);
  
  inline unsigned int getSecondsRemaining(Player player) const {
    return m_stopWatch[player].getSecondsRemaining();
  }

  void pause();
  void resume();
  void stop();
  void reset(bool autoPlay);
  inline ChessWatchState getState() const {
    return m_state;
  }
  inline bool isRunning() const {
    return getState() == CW_RUNNING;
  }
};
