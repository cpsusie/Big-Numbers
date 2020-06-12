//*****************************************************************************
//                               Micro-Max,
// A chess program smaller than 2KB (of non-blank source), by H.G. Muller
// Modified by Jesper Grønning Mikkelsen.
// Dropped the orignal idea of making the programsource as short as possible,
// in order to make it more readable
// Added under-promotion
// Added timerthread to trigger "timeout and move" when the timelimit mseconds
// has been used
// Read input in a separate thread, to enable interaction with program while
// searching
//*****************************************************************************
// version 4.8 features:
// - recursive negamax search
// - all-capture quiescence search with MVV/LVA priority
// - (internal) iterative deepening
// - best-move-first 'sorting'
// - a hash table storing score and best move
// - futility pruning
// - king safety through magnetic frozen king
// - null-move pruning
// - Late-move reductions
// - full FIDE rules and move-legality checking
// - keep hash + rep-draw detect
// - end-game Pawn-push bonus, new piece values, gradual promotion
// Rehash sacrificed, simpler retrieval. Some characters squeezed out
// No hash-table clear, single-call move-legality checking based on from==I
// Fused to generic Winboard driver

#include "stdafx.h"
#include <Timer.h>
#include <FileVersion.h>
#include <Tokenizer.h>
#if !defined(_DEBUG)
#include <DebugLog.h>
#endif
#include "Board.h"

static int    maxTime       = 5*60*1000; // Time left in milliseconds to do maxMoves moves. default 5 minutes
static int    maxMoves      = 100;       // Number of moves to be done in the given by maxTime
static int    timeInc       = 0;         // Time increment per move in milliseconds
static int    timeLeft      = maxTime;   // Time left in milliseconds
static int    movesLeft     = maxMoves;  // Moves to be done within timeLeft milliseconds

static Board  board;

static void sendOptions() {
  printf("option name Nullmove type check default true\n");
  printf("option name Selectivity type spin default 2 min 0 max 4\n");
  printf("option name Style type combo default Normal var Solid var Normal var Risky\n");
  printf("option name NalimovPath type string default c:\\\n");
  printf("option name Clear Hash type button\n");
}

static String getFileVersion() {
  const FileVersion version(getModuleFileName());
  return version.getFileVersion();
}

static int getThinkTime(int timeLimit) {
  if(timeLimit) {
    return timeLimit;
  } else if(movesLeft == 1) {
    return timeLeft;
  } else {
    const int m = movesLeft <= 0 ? 40 : movesLeft;
    return (int)(0.6*(timeLeft+(m-1)*timeInc)/(m+7));
  }
}

typedef enum {
  FINDMOVE
 ,KILL
} CommandType;

class Command {
private:
  CommandType m_type;
  int         m_param;

public:
  Command(CommandType type, int param = 0) : m_type(type), m_param(param) {
  }
  inline CommandType getType() const {
    return m_type;
  }
  inline int getParam() const {
    return m_param;
  }
};

class MoveFinder : public Thread, public TimeoutHandler {
private:
  SynchronizedQueue<Command> m_msgQueue;
  InputThread               &m_input;
  bool                       m_busy;
  Timer                      m_timer;
  Move                       m_bestMove;
  String                     m_errorMessage;
  UINT                       m_searchTime;

  Move evaluateMove(int timeLimit);
  void updateTimeLeft();
  void kill();
public:
  MoveFinder(InputThread &input);
  ~MoveFinder();
  UINT run();
  void handleTimeout(Timer &timer);
  void findBestMove(int timeLimit) {              // timelimit in msec
    m_msgQueue.put(Command(FINDMOVE, getThinkTime(timeLimit)));
  }
  void stopSearch();
  void moveNow();
  const Move &getBestMove() const {
    return m_bestMove;
  }
  const TCHAR *getErrorMessage() const {
    return m_errorMessage.cstr();
  }
  UINT getSearchTime() const {
    return m_searchTime;
  }
  void replyAnswer() const;
  void waitWhileSearchBusy();
};

MoveFinder::MoveFinder(InputThread &input) : Thread(_T("MoveFinder")), m_input(input), m_timer(1) {
  setDemon(true);
  m_busy = false;
#if !defined(_DEBUG)
  const String fileName = format(_T("c:\\temp\\MicroMax%s.txt"), getFileVersion().cstr());
  redirectDebugLog(false, fileName.cstr());
#endif
  resume();
}

MoveFinder::~MoveFinder() {
  kill();
}

void MoveFinder::kill() {
  stopSearch();
  m_msgQueue.put(KILL);
  while(stillActive()) {
    Sleep(20);
  }
}

void MoveFinder::handleTimeout(Timer &timer) {
  moveNow();
}

void MoveFinder::stopSearch() {
  board.stopSearch();
}

void MoveFinder::moveNow() {
  board.moveNow();
}

#if defined(_DEBUG)
static void printState(int player = BLACK, bool detailed = true) {
  replyMessage(
     _T("Max. moves      : %d\n"
        "Max. time       : %s\n"
        "Moves left      : %d\n"
        "Time left       : %s\n"
        "Time increment  : %s\n"
        "Max. depth      : %d\n"
        "Node count      : %s\n"
        "History size    : %d\n"
        "Game over       : %s\n"
        "%s")
       ,maxMoves
       ,formatTime(maxTime,true).cstr()
       ,movesLeft
       ,formatTime(timeLeft,true).cstr()
       ,formatTime(timeInc ,true).cstr()
       ,board.getMaxDepth()
       ,format1000(board.getNodeCount()).cstr()
       ,board.getHistorySize()
       ,boolToStr(board.isGameOver())
       ,board.toString(player, detailed).cstr()
       );
}
#endif

// Handle commands given in m_msgQueue
UINT MoveFinder::run() {
#if defined(_DEBUG)
  replyMessage(_T("MoveFinderThread:Id:%d"), getThreadId());
#endif
// movesLeft moves have to be done within timeLeft+(movesLeft-1)*timeInc
// If movesLeft < 0, all remaining moves of the game have to be done in this time.
// If maxMoves = 1 any leftover time is lost
  try {
    bool killed = false;
    for(m_busy = false; !killed; m_busy = false) {
      const Command cmd = m_msgQueue.get();
      m_busy = true;
      switch(cmd.getType()) {
      case FINDMOVE: // evaluate best move and measure time used
        { const double startTime = getThreadTime();
          try {
            m_bestMove = evaluateMove(cmd.getParam());
          } catch(Exception e) {
            m_errorMessage = e.what();
            m_bestMove.setNoMove();
          } catch(...) {
            m_errorMessage = "Unknown error!";
            m_bestMove.setNoMove();
          }
          m_searchTime = (UINT)((getThreadTime() - startTime) / 1000.0);
          m_input.putMessage(_T("#searchdone#"));
        }
        break;
      case KILL: // terminate loop
        killed = true;
        break;
      default:
        reply(_T("Unknown command (%d) in messagequeue\n"), cmd.getType());
        break;
      }
    }
  } catch(Exception e) {
    reply(_T("Exception in movefinder:%s\n.\n"), e.what());
  }
  return 0;
}

// Think up a move. hangs until timelimit or stopCode is catched
// When a search is started, we start a timer at the same time, which will call this->timeoutHandler() when
// the given timelimit mseconds has elapsed.
Move MoveFinder::evaluateMove(int timeLimit) {
  m_timer.startTimer(timeLimit, *this);
  try {
    const bool found = board.findMove();
    m_timer.stopTimer();
    if(found) {
      return board.getBestMove();
    }
  } catch(int stopCode) {
    m_timer.stopTimer();
    if(stopCode & STOP_IMMEDIATELY) {
      throwException("Search cancelled");
    }
    return board.getBestMove();
  }
  return Move();
}

void MoveFinder::updateTimeLeft() {
  const int timeUsed = board.getTimeUsed();

  // time-control accounting
  timeLeft -= timeUsed;
  timeLeft += timeInc;
  if(--movesLeft == 0) {
    movesLeft = maxMoves;
    if(maxMoves == 1) {
      timeLeft  = maxTime;
    } else {
      timeLeft += maxTime;
    }
  }
}

static void handleUCI() {
  const FileVersion version(getModuleFileName());
#if defined(_DEBUG)
  reply(_T("id name Micromax %s Debug\n"), version.getFileVersion().cstr());
#else
  reply(_T("id name Dummy Engine %s\n"), version.getFileVersion().cstr());
#endif
  reply(_T("id author %s\n"), version.m_fileInfo[0].comments.cstr());

  board.initGame();

  sendOptions();
  reply(_T("uciok\n"));
}

static bool handleStdCommands(const String &line) {
  Tokenizer tok(line, _T(" "));
  if(!tok.hasNext()) {
    return true;
  }
  const String command = tok.next();
  if(command == _T("isready")) {
    reply(_T("readyok\n"));
    return true;
  }
  if(command == _T("uci")) {
    handleUCI();
    return true;
  }

#if defined(_DEBUG)
  if(command == _T("statew") ) {
    printState(WHITE);
    return true;
  }
  if(command == _T("stateb") || command == _T("state")) {
    printState(BLACK);
    return true;
  }
#endif
  return false;
}

void MoveFinder::waitWhileSearchBusy() {
  String lastLine;
  for(;;) {
    if(m_input.endOfInput()) {
      break;
    }
    const String line = m_input.getLine();
    if(handleStdCommands(line)) {
      continue;
    }
    Tokenizer tok(line, _T(" "));
    const String command = tok.next();
    if(command == _T("stop")) {
      moveNow();
    } else if(command == _T("#searchdone#")) {
      if(lastLine.length() > 0) {
        m_input.putMessage(lastLine);
      }
      return;
    } else {
      lastLine = line;
      stopSearch();
    }
  }
}

void MoveFinder::replyAnswer() const {
  const Move m = getBestMove();
  if(m.isMove()) {
    const UINT nodeCount   = Board::getNodeCount();
    const UINT msec        = getSearchTime();
    const UINT nodesPerSec = msec ? ((UINT64)nodeCount*1000)/msec : 0;
#if !defined(_DEBUG)
    debugLog(_T("%lu\n"), nodesPerSec);
#endif
    reply(_T("bestmove %s\n"), m.toString().cstr());
  } else if(Board::isGameOver()) {
    reply(_T("info string %s\n"), board.getGameResultString(board.getGameResult()).cstr());
    reply(_T("bestmove 0000\n"));
  } else {
    reply(_T("info string %s\n"), getErrorMessage());
    reply(_T("bestmove 0000\n"));
  }
}

int main(int argc, char **argv) {
  InputThread input;
  MoveFinder  moveFinder(input);

#if defined(_DEBUG)
  replyMessage(_T("main:threadId:%d"), GetCurrentThreadId());
#endif

  for(;;) {
    String line;
    try {
      fflush(stdout);
      if(input.endOfInput()) break;
      line = input.getLine();
      if(input.endOfInput()) break;

      if(handleStdCommands(line)) {
        continue;
      }
      Tokenizer tok(line, _T(" "));
      const String command = tok.next();
      if(command == _T("ucinewgame")) {
        // start new game
        board.initGame();
        continue;
      }
      if (command == _T("read")) {
        if(!tok.hasNext()) continue;
        String fileName = tok.next();
        input.readTextFile(fileName);
        continue;
      }

      if(command == _T("quit")) {
        break;
      }
      if(command == _T("go")) {
        UINT timeLimit = INFINITE;
        bool ponder    = false;
        UINT maxNodes  = INFINITE;
        UINT maxDepth  = Board::getMaxDepth();
        while(tok.hasNext()) {
          const String option = tok.next();
          if(option == _T("infinite")) {
            timeLimit = INFINITE;
          } else if(option == _T("movetime")) {
            timeLimit = tok.getInt();
          } else if(option == _T("nodes")) {
            maxNodes = tok.getInt();
          } else if(option == _T("depth")) {
            maxDepth = tok.getInt();
            Board::setMaxDepth(maxDepth);
          } else if(option == _T("ponder")) {
            ponder = true;
          } else if(option == _T("wtime")) {
            timeLeft = tok.getInt();
          } else if(option == _T("btime")) {
            timeLeft = tok.getInt();
          } else if(option == _T("winc")) {
            timeInc  = tok.getInt();
          } else if(option == _T("binc")) {
            timeInc  = tok.getInt();
          } else if(option == _T("movestogo")) {
            movesLeft = tok.getInt();
          }
        }
        moveFinder.findBestMove(timeLimit);
        moveFinder.waitWhileSearchBusy();
        moveFinder.replyAnswer();
        continue;
      }
      if(command == _T("position")) {
        try {
          board.setup(tok.getRemaining());
        } catch(...) {
          board.initGame();
          throw;
        }
        continue;
      }
    } catch(Exception e) {
      replyMessage(_T("Exception:%s"), e.what());
    } catch(...) {
      replyMessage(_T("Unknown Exception"));
    }
  }
  return 0;
}
