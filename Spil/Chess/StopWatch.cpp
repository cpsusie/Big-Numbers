#include "stdafx.h"
#include <Math.h>
#include "StopWatch.h"

ChessWatch::ChessWatch() {
  m_state        = CW_STOPPED;
  m_playerInTurn = WHITEPLAYER;
}

void ChessWatch::setPlayerInTurn(Player player) {
  if(player == m_playerInTurn) {
    return;
  }
  if(!isRunning()) {
    m_playerInTurn = player;
  } else {
    m_stopWatch[m_playerInTurn].stop();
    const UINT incr = m_secondsIncr[m_playerInTurn];
    if(incr) {
      m_stopWatch[m_playerInTurn].addSeconds(incr);
    }
    m_playerInTurn = player;
    m_stopWatch[m_playerInTurn].resume();
  }
}

void ChessWatch::start(bool autoPlay) {
  if(!isRunning()) {
    reset(autoPlay);
    m_stopWatch[m_playerInTurn].resume();
    m_state = CW_RUNNING;
  }
}


void ChessWatch::reset(bool autoPlay) {
  stop();
  const Options options = getOptions();
  if(autoPlay) {
    forEachPlayer(p) {
      initWatch(p, (int)options.getLevelTimeout().getTimeout(options.getAutoPlayLevel(p)));
    }
  } else {
    const int level = options.getNormalPlayLevel();
    switch(level) {
    case LEVEL_TIMEDGAME:
      { forEachPlayer(p) {
          const TimeParameters &tm = options.getTimeParameters(p);
          initWatch(p, tm.getSecondsLeft(), tm.getSecondsIncr());
        }
      }
      break;

    case LEVEL_SPEEDCHESS:
      { forEachPlayer(p) {
          const TimeParameters &tm = options.getSpeedTimeParameters(p);
          initWatch(p, tm.getSecondsLeft(), tm.getSecondsIncr());
        }
      }
      break;
    default:
      { forEachPlayer(p) {
          initWatch(p, (int)options.getLevelTimeout().getTimeout(level));
        }
      }
      break;
    }
  }
}

void ChessWatch::initWatch(Player player, int secondsLeft, int secondsIncr) {
  m_stopWatch[player].init(secondsLeft);
  m_secondsIncr[player] = secondsIncr;
}

void ChessWatch::pause() {
  if(m_state == CW_RUNNING) {
    m_stopWatch[m_playerInTurn].stop();
    m_state = CW_PAUSED;
  }
}

void ChessWatch::resume() {
  if(m_state == CW_PAUSED) {
    m_stopWatch[m_playerInTurn].resume();
    m_state = CW_RUNNING;
  }
}

void ChessWatch::stop() {
  forEachPlayer(p) m_stopWatch[p].stop();
  m_state = CW_STOPPED;
}

// -----------------------------------------------------------------------------------------------

StopWatch::StopWatch(bool countDown) {
  m_countDown      = countDown;
  m_running        = false;
  m_msecondsPaused = 0;
}

void StopWatch::init(UINT startSeconds) {
  m_startMSeconds  = startSeconds * 1000;
  m_startTime      = m_stopTime = Timestamp();
  m_running        = false;
  m_msecondsPaused = 0;
}

void StopWatch::stop() {
  if(m_running) {
    m_running  = false;
    m_stopTime = Timestamp();
  }
}

void StopWatch::resume() {
  if(!m_running) {
    m_running = true;
    m_msecondsPaused += (int)diffmsec(m_stopTime, Timestamp());
  }
}

void StopWatch::addSeconds(UINT seconds) {
  m_startMSeconds += 1000 * seconds;
}

UINT StopWatch::getElapsedTime() const { // in milliseconds
  if(m_running) {
    if(m_countDown) {
      return (UINT)diffmsec(m_startTime, Timestamp()) - m_msecondsPaused;
    } else {
      return (UINT)diffmsec(m_startTime, Timestamp()) - m_msecondsPaused + m_startMSeconds;
    }
  } else { // !running
    if(m_countDown) {
      return (UINT)diffmsec(m_startTime, m_stopTime) - m_msecondsPaused;
    } else {
      return (UINT)diffmsec(m_startTime, m_stopTime) - m_msecondsPaused + m_startMSeconds;
    }
  }
}

UINT StopWatch::getSecondsRemaining() const {    // call only if counting down
  if(!m_countDown) {
    return 3999999999;
  } else {
    const int result = m_startMSeconds - getElapsedTime();
    return result < 0 ? 0 : (UINT)ceil((double)result/1000.0);
  }
}
