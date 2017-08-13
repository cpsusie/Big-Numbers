#include "stdafx.h"
#include <Math.h>
#include <ByteFile.h>
#include <CompressFilter.h>

#include "RegneTesterDlg.h"
#include "HighScore.h"

BestTime::BestTime() {
  m_name = EMPTYSTRING;
  m_time = -1;
}

bool BestTime::isBetterTime(double time) const {
  return (time > 0) && ((m_time < 0) || (time < m_time));
}

String BestTime::toString() const {
  if(m_time < 0) {
    return format(_T("%-50s %-25s %-25s"), spaceString(50,'-').cstr(), spaceString(25, '-').cstr(), spaceString(25,'-').cstr());
  } else {
    return format(_T("%-50s %-25s %-25s"), m_name.cstr(), secondsToString(m_time).cstr(), m_timestamp.toString().cstr());
  }
}

Packer &operator<<(Packer &p, const BestTime &t) {
  p << t.m_name
    << t.m_time
    << t.m_timestamp;
  return p;
}

Packer &operator>>(Packer &p, BestTime &t) {
  p >> t.m_name
    >> t.m_time
    >> t.m_timestamp;
  return p;
}

HighScore::HighScore() {
  load();
}

HighScore::~HighScore() {
}

int HighScore::execiseTypeToIndex(int execiseType) { // static
  switch(execiseType) {
  case ID_OPGAVETYPE_ADD      : return 0;
  case ID_OPGAVETYPE_SUBTRACT : return 1;
  case ID_OPGAVETYPE_MULTIPLY : return 2;
  case ID_OPGAVETYPE_DIVIDE   : return 3;
  default                     : showWarning_T("Invalid execiseType:%d."), execiseType);
                                return 0;
  }
}

const int HighScore::allExeciseTypes[4] = { // static
  ID_OPGAVETYPE_ADD
 ,ID_OPGAVETYPE_SUBTRACT
 ,ID_OPGAVETYPE_MULTIPLY
 ,ID_OPGAVETYPE_DIVIDE
};

TCHAR getOperatorChar(int execiseType) {
  const TCHAR *operatorChar = _T("+-x:");
  return operatorChar[HighScore::execiseTypeToIndex(execiseType)];
}

static const TCHAR *execiseName[] = {
  _T("Plus")
 ,_T("Minus")
 ,_T("Gange")
 ,_T("Divider")
};

const TCHAR *getExeciseName(int execiseType) {
  return execiseName[HighScore::execiseTypeToIndex(execiseType)];
}

bool isValidExeciseType(int execiseType) {
  switch (execiseType) {
  case ID_OPGAVETYPE_ADD      :
  case ID_OPGAVETYPE_SUBTRACT :
  case ID_OPGAVETYPE_MULTIPLY :
  case ID_OPGAVETYPE_DIVIDE   :
    return true;
  default:
    return false;
  }
}

bool isValidTableNumber(int tableNumber) {
  return 1 <= tableNumber && tableNumber <= 10;
}

void HighScore::update(int execiseType, int tabel, const String &name, double time) { // static
  const int index = tabel - 1;
  HighScore hs;
  BestTime &t = hs.m_bestTimes[execiseTypeToIndex(execiseType)][index];
  if(t.isBetterTime(time)) {
    t.m_name      = name;
    t.m_time      = time;
    t.m_timestamp = Timestamp();
    hs.save();
  }
}

bool HighScore::isBetterTime(int execiseType, int tabel, double time) const {
  return m_bestTimes[execiseTypeToIndex(execiseType)][tabel-1].isBetterTime(time);
}

String HighScore::toString() const {
  String result;
  for(int i = 0; i < ARRAYSIZE(allExeciseTypes); i++) {
    int execiseType = allExeciseTypes[i];
    if(i > 0) {
      result += _T("\r\n\r\n");
    }
    result += toString(execiseType);
  }
  return result;
}

String HighScore::toString(int execiseType) const {
  String result = format(_T("%c  %-16s %-50s %-25s %-25s\r\n"), getOperatorChar(execiseType), _T("Tabel"), _T("Navn"), _T("Tid"), _T("Tidspunkt"));
  const ExeciseHighScore &bestTimes = m_bestTimes[execiseTypeToIndex(execiseType)];
  for(int i = 0; i < 10; i++) {
    result += format(_T("  %-16s %-s\r\n"), format(_T("%2d-tabel"),i+1).cstr(), bestTimes[i].toString().cstr());
  }
  return result;
}

String HighScore::getFileName() const {
  const Options options;
  String fileName = options.m_highScoreFileName;
  if(fileName.length() == 0) {
    FileNameSplitter info(getModuleFileName());
    info.setExtension(_T("dat"));
    fileName = info.getAbsolutePath();
  }
  return fileName;
}

void HighScore::load() {
  try {
    load(DecompressFilter(ByteInputFile(getFileName())));
  } catch(Exception e) {
//    showException(e);
  }
}

void HighScore::save() const {
  try {
    save(CompressFilter(ByteOutputFile(getFileName())));
  } catch(Exception e) {
    showException(e);
  }
}

const char *HighScore::s_version = "1.0.0.1"; // must be char

void HighScore::save(ByteOutputStream &s) const {
  Packer p;
  p << s_version;
  for(int e = 0; e < 4; e++) {
    for(int i = 0; i < 10; i++) {
      p << m_bestTimes[e][i];
    }
  }
  p.write(s);
}

void HighScore::load(ByteInputStream &s) {
  Packer p;
  p.read(s);
  char fileVersion[20];
  p >> fileVersion;
  for(int e = 0; e < 4; e++) {
    for(int i = 0; i < 10; i++) {
      p >> m_bestTimes[e][i];
    }
  }
}

String secondsToString(double msec) {
  if(msec < 60000) {
    return format(_T("%.2lf sek."), msec / 1000.0);
  } else {
    const int seconds = (int)(msec / 1000);
    if(seconds < 3600) {
      return format(_T("%02d:%02d.%02d min."), seconds/60, seconds%60,(int)fmod(msec/10,100));
    } else if(seconds < 24*3600) {
      return format(_T("%02d:%02d:%02d timer"), seconds/3600, (seconds/60) % 60, seconds % 60);
    } else {
      const int days    =  seconds / (3600*24);
      const int hours   = (seconds / 3600) % 24;
      const int minutes = (seconds / (24*60)) % 60;
      const int sec     = seconds % 60;
      return format(_T("%d dage, %02d:%02d:%02d timer"), days, hours, minutes, sec);
    }
  }
}
