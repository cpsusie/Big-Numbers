#pragma once

#include <RefCountedObject.h>
#include "SocketChannel.h"

typedef enum { // When put these request into the request-queue, caller never hangs. only deliver the request and return immediately
  REQUEST_FINDMOVE     // Only valid if state == CPS_IDLE. Ask MoveFinderThread the best move.
 ,REQUEST_NULLMOVE     // Only valid if state == CPS_IDLE. Request NullMove, ie. empty move
 ,REQUEST_STOPSEARCH   // if state==CPS_BUSY then set state=CPS_STOPPENDING, and ask the movefinder to stop the search as soon as possible
 ,REQUEST_MOVENOW      // Only valid if state == CPS_BUSY. Stop the current search, at soon as a move is ready and goto CPS_MOVEREADY
 ,REQUEST_FETCHMOVE    // Send from actual moveFinder back to MoveFinderThread
 ,REQUEST_GAMECHANGED  // The game is changed, (new game, edit game, etc.)
 ,REQUEST_MOVEDONE     // A move has been executed
 ,REQUEST_SHOWMESSAGE  // Show a message. mostly used for errormessages from moveFinder. Show and if param.m_reset then reset
 ,REQUEST_RESET        // Stop current search, if any, delete current moveFinder, if any, and goto state MFTS_IDLE
 ,REQUEST_CONNECT      // Only valid if state == CPS_IDLE 
 ,REQUEST_DISCONNECT   // Only valid if connected. Disconnect remoteMoveFinder
 ,REQUEST_KILL         // Stop current search, if any, delete current moveFinder, if any, and set sate to MFTS_KILLED
} ChessPlayerRequestType;

class RequestParamGame : public RefCountedObject {
private:
  const Game m_game;
public:
  RequestParamGame(const Game &game) : m_game(game) {
  }
  inline const Game &getGame() const {
    return m_game;
  }
};

class FindMoveRequestParam : public RequestParamGame {
private:
  const TimeLimit m_timeLimit;
  const bool      m_hint;
  const bool      m_verbose;
public:
  FindMoveRequestParam(const Game &game, const TimeLimit &timeLimit, bool hint, bool verbose)
    : RequestParamGame(game)
    , m_timeLimit(timeLimit)
    , m_hint(hint)
    , m_verbose(verbose)
  {
  }
  inline const TimeLimit &getTimeLimit() const {
    return m_timeLimit;
  }
  inline bool isHint() const {
    return m_hint;
  }
  inline bool isVerbose() const {
    return m_verbose;
  }
};

class GameChangedRequestParam : public RequestParamGame {
public:
  GameChangedRequestParam(const Game &game) : RequestParamGame(game)
  {
  }
};

class MoveDoneRequestParam : public RefCountedObject {
private:
  const PrintableMove m_move;
public:
  MoveDoneRequestParam(const PrintableMove &m) : m_move(m)
  {
  }
  const PrintableMove &getMove() const {
    return m_move;
  }
};

class ConnectRequestParam : public RefCountedObject {
private:
  const SocketChannel m_channel;
public:
  ConnectRequestParam(const SocketChannel &channel)
    : m_channel(channel)
  {
  }
  const inline SocketChannel &getChannel() const {
    return m_channel;
  }
};

class ShowMessageRequestParam : public RefCountedObject {
private:
  const String m_msg;
  const bool   m_error;
public:
  ShowMessageRequestParam(const String &msg, bool error)
    : m_msg(msg), m_error(error) {
  }
  inline const String &getMessage() const {
    return m_msg;
  }
  inline bool isError() const {
    return m_error;
  }
};

class SearchMoveResult {
public:
  MoveBase m_move;
  bool     m_hint;
  SearchMoveResult() {
    clear();
  }
  SearchMoveResult(const MoveBase &m, bool hint) : m_move(m), m_hint(hint) {
  }
  inline void clear() {
    m_hint = false;
    m_move.setNoMove();
  }
  inline bool isMove() const {
    return m_move.isMove();
  }
  bool isHint() const {
    return m_hint;
  }
};

class FetchMoveRequestParam : public RefCountedObject {
private:
  const SearchMoveResult m_result;
public:
  FetchMoveRequestParam(const SearchMoveResult &result) : m_result(result) {
  }
  const SearchMoveResult &getSearchResult() const {
    return m_result;
  }
};

class ChessPlayerRequest {
private:
  ChessPlayerRequestType m_type;
  union {
    FindMoveRequestParam    *m_findMoveParam;    // m_type = REQUEST_FINDMOVE
    FetchMoveRequestParam   *m_fetchMoveParam;   // m_type = REQUEST_FETCHMOVE
    GameChangedRequestParam *m_gameChangedParam; // m_type = REQUEST_GAMECHANGED
    MoveDoneRequestParam    *m_moveDoneParam;    // m_type = REQUEST_MOVEDONE
    ShowMessageRequestParam *m_showMessageParam; // m_type = REQUEST_SHOWMESSAGE
    ConnectRequestParam     *m_connectParam;     // m_type = REQUEST_CONNECT
  } m_data;
  void release();
  void addref();
  void cleanData();
  inline void throwInvalidType(const TCHAR *method) const {
    throwException(_T("%s:Request type is %s"), method, getRequestName());
  }
  inline void checkType(const TCHAR *method, ChessPlayerRequestType expectedType) const {
    if(getType() != expectedType) throwInvalidType(method);
  }
public:
  // REQUEST_FINDMOVE
  ChessPlayerRequest(const Game &game, const TimeLimit &timeLimit, bool hint, bool verbose);
  // REQUEST_FETCHMOVE
  ChessPlayerRequest(const MoveBase &move, bool hint);
  // REQUEST_GAMECHANGED
  ChessPlayerRequest(const Game &game);
  // REQUEST_MOVEDONE
  ChessPlayerRequest(const PrintableMove &move);
  // REQUEST_SHOWMESSAGE
  ChessPlayerRequest(const String &msgText, bool error);
  // REQUEST_CONNECT
  ChessPlayerRequest(const SocketChannel &channel);
  ChessPlayerRequest(ChessPlayerRequestType type);
  ChessPlayerRequest(const ChessPlayerRequest &src);
  ~ChessPlayerRequest();
  ChessPlayerRequest &operator=(const ChessPlayerRequest &src);
  inline ChessPlayerRequestType getType() const {
    return m_type;
  }
  const FindMoveRequestParam     &getFindMoveParam()    const;
  const GameChangedRequestParam  &getGameChangedParam() const;
  const MoveDoneRequestParam     &getMoveDoneParam()    const;
  const FetchMoveRequestParam    &getFetchMoveParam()   const;
  const ShowMessageRequestParam  &getShowMessageParam() const;
  const ConnectRequestParam      &getConnectParam()     const;
  inline const TCHAR             *getRequestName()      const {
    return getRequestName(m_type);
  }
  static const TCHAR *getRequestName(ChessPlayerRequestType request);
  String toString() const;
};

class ChessPlayerRequestQueue : public SynchronizedQueue<ChessPlayerRequest> {
};

class AbstractMoveReceiver {
public:
  virtual void putMove(const MoveBase &move)  = 0;
  virtual void putError(const TCHAR *fmt,...) = 0;
};
