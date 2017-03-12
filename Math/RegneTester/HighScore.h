#pragma once

#include <Packer.h>
#include <ByteStream.h>

class BestTime {
public:
  String    m_name;
  double    m_time;
  Timestamp m_timestamp;
  BestTime();
  String toString() const;
  bool isBetterTime(double time) const;
  bool isDefined() const {
    return m_time > 0;
  }
  friend Packer &operator<<(Packer &p, const BestTime &t);
  friend Packer &operator>>(Packer &p,       BestTime &t);
};

typedef BestTime ExeciseHighScore[10];

class HighScore {
private:
  static const char *s_version; // must be char

  ExeciseHighScore m_bestTimes[4];
  String getFileName() const;
  void save(ByteOutputStream &s) const;
  void load(ByteInputStream  &s);
public:
  HighScore();
  ~HighScore();
  bool isBetterTime(int execiseType, int tabel, double time) const;
  const BestTime &getBestTime(int execiseType, int index) const {
    return m_bestTimes[execiseTypeToIndex(execiseType)][index];
  }

  static void update(int execiseType, int tabel, const String &name, double bestTime); // tabel = [1..10]
  void save() const;
  void load();
  String toString() const;
  String toString(int execiseType) const;
  static int execiseTypeToIndex(int execiseType);

  static const int allExeciseTypes[4];
};

String       secondsToString(   double msec       );
TCHAR        getOperatorChar(   int    execiseType);
const TCHAR *getExeciseName(    int    execiseType);
bool         isValidTableNumber(int    tableNumber);
bool         isValidExeciseType(int    execiseType);
void         copyFile(const String &to, const String &from);

class Options {
private:
  void save(ByteOutputStream &s) const;
  void load(ByteInputStream  &s);
public:
  String m_password;
  String m_highScoreFileName;
  Options();
  ~Options();
  bool save() const;
  void load();
  String getMD5Password() const;
};
