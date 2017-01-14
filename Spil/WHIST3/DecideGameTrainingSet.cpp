#include "stdafx.h"
#include <Random.h>
#include <Tokenizer.h>
#include "Whist3.h"
#include "DecideGameTrainingDialog.h"
#include "CardBitmap.h"

static const char *thisFile = __FILE__;

String DecideGameTrainingSet::getFileName() {
  return FileNameSplitter(thisFile).setFileName(_T("DecideGameTrainingData")).setExtension(_T("txt")).getFullPath();
}

DecideGameTrainingSet::DecideGameTrainingSet() {
  generateRandomTestData();
}

DecideGameTrainingSet::DecideGameTrainingSet(const CardHand &hand, const GamesPlayed &gamesPlayed) {
  init(hand,gamesPlayed);
}

DecideGameTrainingSet::DecideGameTrainingSet(const String &src) {
  DEFINEMETHODNAME;
  int             i                = 0;
  Card            card             = 0;
  int             point            = 1;
  int             gameType         = FIRST_GAMETYPE;
  Array<GameType> games;
  Array<int>      points;
  GameType        selectedGameType;
  int             selectedPoints;
  CardHand        hand;
  Tokenizer       tok(src,_T(" "));

  for(; tok.hasNext() && i < 62; i++) {
    UINT v = tok.getInt();
    if(i < 4) {
      if(v != 0 && v != 1) {
        throwException(_T("%s:Illegal string <%s>. element[%d]=%d. Must be 0 or 1"), method, src.cstr(),i,v);
      }
      if(v == 1) {
        games.add((GameType)gameType);
      }
      gameType++;
    } else if(i < 8) {
      if(v != 0 && v != 1) {
        throwException(_T("%s:Illegal string <%s>. element[%d]=%d. Must be 0 or 1"), method, src.cstr(),i,v);
      }
      if(v == 1) {
        points.add(point);
      }
      point++;
    } else if(i < 60) {
      if(v != 0 && v != 1) {
        throwException(_T("%s:Illegal string <%s>. element[%d]=%d. Must be 0 or 1"), method, src.cstr(),i,v);
      }
      if(v == 1) {
        hand.addCard(card);
      }
      card++;
    } else if(i == 60) {
      selectedGameType = (GameType)v;
    } else if(i == 61) {
      selectedPoints = v;
    }
  }

  if(i < 62 || tok.hasNext()) {
    throwException(_T("Wrong number of values on line:<%s>. Must be 62."),src.cstr());
  }

  if(games.size() != points.size()) {
    throwException(_T("%s:Illegal string <%s>. No. of gamesPlayed=%d != No. of pointsPlayed=%d"), method, games.size(), points.size());
  }

  GamesPlayed gamesPlayed;
  for(size_t i = 0; i < games.size(); i++) {
    gamesPlayed.addPlayedGame(games[i],points[i]);
  }
  hand.sort();

  init(hand,gamesPlayed);
  setSelectedGameAndPoint(selectedGameType,selectedPoints);
}

String DecideGameTrainingSet::toString() const {
  String result;
  for(int t = FIRST_GAMETYPE; t <= LAST_GAMETYPE; t++) {
    result += m_gamesPlayed.isGameUsed((GameType)t) ? _T("1 ") : _T("0 ");
  }
  result += _T("   ");
  for(int p = 1; p <= 4; p++) {
    result += m_gamesPlayed.isPointsUsed(p) ? _T("1 ") : _T("0 ");
  }
  result += _T("   ");
  for(Card card = 0; card < 52; card++) {
    result += m_hand.contains(card) ? _T("1 ") : _T("0 ");
  }
  result += _T("   ");

  result += String(m_selectedGameType);
  result += _T(" ");
  result += String(m_selectedPointsPerTrick);

  return result;
}

void DecideGameTrainingSet::init(const CardHand &hand, const GamesPlayed &gamesPlayed) {
  m_hand                   = hand;
  m_gamesPlayed            = gamesPlayed;
  m_selectedGameType       = (GameType)-1;
  m_selectedPointsPerTrick = -1;
  validate();
}

void DecideGameTrainingSet::validate() {
  DEFINEMETHODNAME;
  if(m_gamesPlayed.getCount() > 2) {
    throwException(_T("%s:Number of played games=%d. Must be [0..2]"), method, m_gamesPlayed.getCount());
  }
  if(m_hand.getCount() != 13) {
    throwException(_T("%s:Number of cards on hand=%d. Must be 13"), method, m_hand.getCount());
  }
}

void DecideGameTrainingSet::generateRandomTestData() {
  Card deck[52];

  for(int i = 0; i < 52; i++ ) {
    deck[i] = (Card)i;
  }
  for(int i = 0; i < 52; i++ ) {
    int r = rand() % 52;
    Card tmp = deck[i];
    deck[i] = deck[r];
    deck[r] = tmp;
  }

  CardHand hand;
  int c = 0;
  for(int i = 0; i < 13; i++) {
    hand.addCard(deck[c++]);
  }

  hand.sort();
  GamesPlayed gamesPlayed;
  gamesPlayed.setRandom(rand() % 3);
  init(hand,gamesPlayed);
}

void DecideGameTrainingSet::setSelectedGameAndPoint(GameType gameType, int pointsPerTrick) {
  validateSelection(gameType,pointsPerTrick);
  m_selectedGameType       = gameType;
  m_selectedPointsPerTrick = pointsPerTrick;
}

void DecideGameTrainingSet::validateSelection(GameType gameType, int pointsPerTrick) const {
  DEFINEMETHODNAME;
  if(gameType < FIRST_GAMETYPE || gameType > LAST_GAMETYPE) {
    throwException(_T("%s:Invalid gametype:%d. Must be [0..3]"), method, gameType);
  }
  if(pointsPerTrick < 1 || pointsPerTrick > 4) {
    throwException(_T("%s:Invalid points:%d. Must be [1..4]"), method, pointsPerTrick);
  }

  if(m_gamesPlayed.isGameUsed(gameType)) {
    throwException(_T("Gametype %d is alredy played"),gameType);
  }
  if(m_gamesPlayed.isPointsUsed(pointsPerTrick)) {
    throwException(_T("Point %d already played"),pointsPerTrick);
  }
}

Array<double> &DecideGameTrainingSet::getBpnInput(Array<double> &input) const {
  input.clear();
  input.add(m_gamesPlayed.isGameUsed(GAMETYPE_SOL)   ? 1 : 0);
  input.add(m_gamesPlayed.isGameUsed(GAMETYPE_SANS)  ? 1 : 0);
  input.add(m_gamesPlayed.isGameUsed(GAMETYPE_SPAR)  ? 1 : 0);
  input.add(m_gamesPlayed.isGameUsed(GAMETYPE_FARVE) ? 1 : 0);
  input.add(m_gamesPlayed.isPointsUsed(1)            ? 1 : 0);
  input.add(m_gamesPlayed.isPointsUsed(2)            ? 1 : 0);
  input.add(m_gamesPlayed.isPointsUsed(3)            ? 1 : 0);
  input.add(m_gamesPlayed.isPointsUsed(4)            ? 1 : 0);

  for(Card card = 0; card < 52; card++) {
    input.add(m_hand.contains(card) ? 1 : 0);
  }
  return input;
}

Array<double> &DecideGameTrainingSet::getBpnOutput(Array<double> &output) const {
  output.clear();
  for(int i = 0; i < 8; i++) {
    output.add(0.1);
  }
  output[m_selectedGameType] = 0.9;
  output[3+m_selectedPointsPerTrick] = 0.9;
  return output;
}

double DecideGameTrainingSet::getGameTypeScore(GameType gameType) const {
  DEFINEMETHODNAME;
  if(gameType < FIRST_GAMETYPE || gameType > LAST_GAMETYPE) {
    throwException(_T("%s:Illegal gameType=%d. Must be [0..3]"), method, gameType);
  }
  return m_bpnOutput[gameType];
}

double DecideGameTrainingSet::getPointScore(int pointsPerTrick) const {
  DEFINEMETHODNAME;
  if(pointsPerTrick < 1 || pointsPerTrick > 4) {
    throwException(_T("%s:Invalid points:%d. Must be [1..4]"), method, pointsPerTrick);
  }
  return m_bpnOutput[pointsPerTrick+3];
}

class GameTypeScore {
public:
  const GameType m_gameType;
  const double   m_score;

  GameTypeScore(const GameType gameType, double score) : m_gameType(gameType), m_score(score) {
  }
};

class PointScore {
public:
  const int    m_pointsPerTrick;
  const double m_score;

  PointScore(int pointsPerTrick, double score) : m_pointsPerTrick(pointsPerTrick), m_score(score) {
  }
};

static int gameTypeScoreCmp(const GameTypeScore &s1, const GameTypeScore &s2) {
  return sign(s2.m_score - s1.m_score);
}

static int pointScoreCmp(const PointScore &s1, const PointScore &s2) {
  return sign(s2.m_score - s1.m_score);
}

void DecideGameTrainingSet::setSelectedGameAndPoint(const Array<double> &bpnOutput) {
  m_bpnOutput = bpnOutput;

  Array<GameTypeScore> gameTypeScores;
  Array<PointScore>    pointScores;

  for(int t = FIRST_GAMETYPE; t <= LAST_GAMETYPE; t++) {
    gameTypeScores.add(GameTypeScore((GameType)t,getGameTypeScore((GameType)t)));
  }

  for(int p = 1; p <= 4; p++) {
    pointScores.add(PointScore(p,getPointScore(p)));
  }

  gameTypeScores.sort(gameTypeScoreCmp);
  pointScores.sort(pointScoreCmp);

  GameType selectedGameType;
  for(size_t i = 0; i < gameTypeScores.size(); i++) {
    const GameTypeScore &gs = gameTypeScores[i];
    if(!m_gamesPlayed.isGameUsed(gs.m_gameType)) {
      selectedGameType = gs.m_gameType;
      break;
    }
  }

  int selectedPointsPerTrick;
  for(size_t i = 0; i < pointScores.size(); i++) {
    const PointScore &ps = pointScores[i];
    if(!m_gamesPlayed.isPointsUsed(ps.m_pointsPerTrick)) {
      selectedPointsPerTrick = ps.m_pointsPerTrick;
      break;
    }
  }
  setSelectedGameAndPoint(selectedGameType, selectedPointsPerTrick);
}

void DecideGameTrainingSet::save() {
  validate();
  validateSelection(m_selectedGameType,m_selectedPointsPerTrick);
  FILE *f = FOPEN(getFileName(),_T("a"));
  _ftprintf(f,_T("%s\n"),toString().cstr());
  fclose(f);
}

Array<DecideGameTrainingSet> DecideGameTrainingSet::loadTrainingData() { // static 
  Array<DecideGameTrainingSet> result;
  FILE *f = NULL;
  try {
    f = FOPEN(getFileName(),_T("r"));
    String line;
    while(readLine(f,line)) {
      result.add(DecideGameTrainingSet(line));
    }
    fclose(f);
    f = NULL;
    if(result.size() == 0) {
      throwException(_T("No trainingdata."));
    }
  } catch(...) {
    if(f != NULL) {
      fclose(f);
      f = NULL;
    }
    throw;
  }
  return result;
}

// --------------------------------------------------DecideGameBpn------------------------------------------

Array<int> DecideGameBpn::getLayerCount() {
  Array<int> units;
  units.add(60);
  units.add(20);
  units.add(8);
  return units;
}

DecideGameBpn::DecideGameBpn() : Bpn(getLayerCount()) {
}

String DecideGameBpn::getFileName() {
  return FileNameSplitter(thisFile).setFileName(_T("DecideGameBpn")).setExtension(_T("dat")).getFullPath();
}

void DecideGameBpn::load() {
  Bpn::load(getFileName());
}

void DecideGameBpn::save() {
  Bpn::save(getFileName());
}

void DecideGameBpn::learn(const DecideGameTrainingSet &data) {
  Array<double> input;
  Array<double> output;
  Bpn::learn(data.getBpnInput(input),data.getBpnOutput(output));
}

void DecideGameBpn::recognize(DecideGameTrainingSet &data) {
  Array<double> input;
  Array<double> output;
  Bpn::recognize(data.getBpnInput(input),output);
  data.setSelectedGameAndPoint(output);
}

double DecideGameBpn::getPatternError(const DecideGameTrainingSet &data) const {
  Array<double> output;
  return Bpn::getPatternError(data.getBpnOutput(output));
}
