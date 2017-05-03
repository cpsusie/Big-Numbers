#pragma once

typedef enum { // When put these request into the request-queue, caller never hangs. only deliver the request and return immediately
  REQUEST_FINDMOVE     // Only valid if state == MFTS_IDLE. Ask MoveFinderThread the best move.
 ,REQUEST_NULLMOVE     // Only valid if state == MFTS_IDLE. Request NullMove, ie. empty move
 ,REQUEST_STOPSEARCH   // if state==MFTS_BUSY then set state=MFTS_STOPPENDING, and ask the movefinder to stop the search as soon as possible
 ,REQUEST_MOVENOW      // Only valid if state == MOVE_BUSY. Stop the current search, at soon as a move is ready and goto MFTS_MOVEREADY
 ,REQUEST_FETCHMOVE    // Send from actual moveFinder back to MoveFinderThread
 ,REQUEST_GAMECHANGED  // The game is changed, (new game, edit game, etc.)
 ,REQUEST_RESET        // Stop current search, if any, delete current moveFinder, if any, and goto state MFTS_IDLE
 ,REQUEST_DISCONNECT   // Only valid if connected. Disconnect remoteMoveFinder
 ,REQUEST_KILL         // Stop current search, if any, delete current moveFinder, if any, and set sate to MFTS_KILLED
} MoveFinderThreadRequestType;

class MoveFinderRequestParamGame {
private:
  const Game       m_game;
  BYTE             m_refCount;
public:
  MoveFinderRequestParamGame(const Game &game)
    : m_game(game)
    , m_refCount(1)
  {
  }
  inline void addRef()  { m_refCount++;        }
  inline int  release() { return --m_refCount; }
  inline const Game &getGame() const {
    return m_game;
  }
};

class FindMoveRequestParam : public MoveFinderRequestParamGame {
private:
  const TimeLimit  m_timeLimit;
  const bool       m_hint;
public:
  FindMoveRequestParam(const Game &game, const TimeLimit &timeLimit, bool hint)
    : MoveFinderRequestParamGame(game)
    , m_timeLimit(timeLimit)
    , m_hint(hint)
  {
  }
  inline const TimeLimit getTimeLimit() const {
    return m_timeLimit;
  }
  inline const bool isHint() const {
    return m_hint;
  }
};

class GameChangedRequestParam : public MoveFinderRequestParamGame {
public:
  GameChangedRequestParam(const Game &game)
    : MoveFinderRequestParamGame(game)
  {
  }
};

class SearchMoveResult {
public:
  MoveBase m_move;
  bool     m_hint;
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

class FetchMoveRequestParam : public SearchMoveResult {
};

class MoveFinderThreadRequest {
private:
  MoveFinderThreadRequestType m_type;
  union {
    FindMoveRequestParam    *m_findMoveParam;
    GameChangedRequestParam *m_gameChangedParam;
    FetchMoveRequestParam    m_fetchMoveParam;
  } m_data;
  void release();
  void addRef();
  void cleanData();
  void throwInvalidType(const TCHAR *method) const;
public:
  // REQUEST_FINDMOVE
  MoveFinderThreadRequest(const Game &game, const TimeLimit &timeLimit, bool hint);
  // REQUEST_GAMECHANGED  
  MoveFinderThreadRequest(const Game &game);
  // REQUEST_FETCHMOVE    
  MoveFinderThreadRequest(const MoveBase &move, bool hint);
  MoveFinderThreadRequest(MoveFinderThreadRequestType type);
  MoveFinderThreadRequest(const MoveFinderThreadRequest &src);
  ~MoveFinderThreadRequest();
  MoveFinderThreadRequest &operator=(const MoveFinderThreadRequest &src);
  inline MoveFinderThreadRequestType getType() const {
    return m_type;
  }
  const FindMoveRequestParam     &getFindMoveParam()    const;
  const GameChangedRequestParam  &getGameChangedParam() const;
  const FetchMoveRequestParam    &getFetchMoveParam()   const;
  inline const TCHAR             *getRequestName()      const {
    return getRequestName(m_type);
  }
  static const TCHAR *getRequestName(MoveFinderThreadRequestType request);
  String toString() const;
};

class MFTRQueue : public SynchronizedQueue<MoveFinderThreadRequest> {
};
