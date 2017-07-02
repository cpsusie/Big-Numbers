#include "pch.h"
#include <Vfw.h>
#include <MP3Player.h>

MP3Exception::MP3Exception(MCIERROR error, const TCHAR *text) : Exception(text) {
  m_error = error;
}

void MP3Player::checkResult(MCIERROR ret) {
  if(ret != 0) {
    TCHAR tmp[256];
    mciGetErrorString(ret, tmp, ARRAYSIZE(tmp));
    throw MP3Exception(ret,tmp);
  }
}

String MP3Player::sendString(const TCHAR *format,...) {
  TCHAR result[1024];
  MEMSET(result,0,ARRAYSIZE(result));
  va_list argptr;
  va_start(argptr,format);
  const String tmp = vformat(format, argptr);
  va_end(argptr);
  checkResult(mciSendString(tmp.cstr(), result, ARRAYSIZE(result),NULL));
  return result;
}

MP3PlayerStatus MP3Player::getStatus() {
  try {
    String s = sendString(_T("status %s mode"), m_alias);

    if(s == _T("playing")) return playing;
    if(s == _T("stopped")) return stopped;
    if(s == _T("paused") ) return paused;
    return stopped;
  } catch(MP3Exception) {
    return stopped;
  }
}

MP3Player::MP3Player(const TCHAR *alias) {
  m_speedFactor = 1;
  _tcscpy(m_alias,alias);
}

MP3Player::~MP3Player() {
  if(getStatus() != stopped) {
    stop();
  }
  try {
    close();
  } catch(MP3Exception) {
  }
}

void MP3Player::open(const TCHAR *fileName) {
  try {
    close();
  } catch(MP3Exception) {
  }
  TCHAR shortFileName[256];
  long lenShort = GetShortPathName(fileName, shortFileName, ARRAYSIZE(shortFileName));

  if(FileNameSplitter(fileName).getExtension() == _T(".wav")) {
    sendString(_T("open \"waveaudio!%s\" alias %s buffer 6"), shortFileName, m_alias);
  } else {
    sendString(_T("open %s type mpegvideo alias %s"), shortFileName, m_alias);
  }
//  sendString("set %s time format milliseconds", m_alias);
//  sendString("set %s tempo 100.0",m_alias);
}

void MP3Player::close() {
  sendString(_T("close %s"), m_alias);
}

void MP3Player::setSpeed(double speedFactor) {
  m_speedFactor = speedFactor;
  int f = (int)(m_speedFactor * 1000);
  sendString(_T("set %s speed %d"), m_alias, f);
}

void MP3Player::play(const TCHAR *fileName) {
  open(fileName);
  start();
}

void MP3Player::start() {
  switch(getStatus()) {
  case stopped:
  case paused :
//o    mmSpeed(m_speedFactor);
    sendString(_T("play %s"), m_alias);
  }
}

void MP3Player::pause() {
  if(getStatus() == playing) {
    sendString(_T("pause %s"), m_alias);
  }
}

void MP3Player::stop() {
  sendString(_T("stop %s"), m_alias);
}

void MP3Player::seekTo(double sec) {
  switch(getStatus()) {
  case playing:
    sendString(_T("set %s time format milliseconds"), m_alias);
    sendString(_T("play %s from %.0lf"),m_alias,sec * 1000.0);
    break;
  case paused:
    sendString(_T("set %s time format milliseconds"), m_alias);
    sendString(_T("seek %s to %.0lf"),m_alias,sec * 1000.0);
    break;
  }
}

static double ATOF(const TCHAR *s) {
  double v = 0;
  _stscanf(s, _T("%lg"), &v);
  return v;
}

double MP3Player::getPositionInSeconds() {
  String s;
  switch(getStatus()) {
  case playing:
  case paused:
    sendString(_T("set %s time format milliseconds"), m_alias);
    s = sendString(_T("status %s position"), m_alias);
    return ATOF(s.cstr()) / 1000.0;
  default:
    return 0.0;
  }
}

double MP3Player::getLengthInSeconds() {
  String s;
  switch(getStatus()) {
  case playing:
  case paused:
    sendString(_T("set %s time format milliseconds"), m_alias);
    s = sendString(_T("status %s length"), m_alias);
    return ATOF(s.cstr()) / 1000.0;
  default:
    return 0.0;
  }
}

void MP3Player::setMute(int channel, bool on) {
  TCHAR *onstr = on ? _T("on") : _T("off");
  if((channel & CHANNEL_LEFT) && (channel & CHANNEL_RIGHT)) {
    sendString(_T("set %s audio all %s"), m_alias, onstr);
  } else {
    if(channel & CHANNEL_LEFT) {
      sendString(_T("set %s audio left %s"), m_alias, onstr);
    }
    if(channel & CHANNEL_RIGHT) {
      sendString(_T("set %s audio right %s"), m_alias, onstr);
    }
  }
}
/*
void MP3Player::setVolume(int level) {
  HWND w = MCIWndCreate(NULL,AfxGetApp()->m_hInstance,MCIWNDF_SHOWALL,tmp);
}
*/
