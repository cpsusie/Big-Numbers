#pragma once

#include <Mmsystem.h>
#include "Exception.h"

class MP3Exception : public Exception {
public:
  MP3Exception(MCIERROR error, const TCHAR *text);
  MCIERROR m_error;
};

enum MP3PlayerStatus {
  stopped
 ,playing
 ,paused
};

#define CHANNEL_LEFT  0x1
#define CHANNEL_RIGHT 0x2
#define CHANNEL_BOTH (CHANNEL_LEFT | CHANNEL_RIGHT)

class MP3Player {
private:
  TCHAR m_alias[32];
  double m_speedFactor;

  String sendString(const TCHAR *format, ...);
  void checkResult(MCIERROR ret);

public:
  MP3Player(const TCHAR *alias = _T("Musicplayer"));
  ~MP3Player();
  void   open(const TCHAR *fname);
  void   close();
  void   play(const TCHAR *fname);
  void   setMute(int channel, bool on);
  void   pause();
  void   start();
  void   stop();
  void   seekTo(double sec);
  void   setSpeed(double factor); // factor = [0;...] 1 = normal speed
  double getSpeed() const {
    return m_speedFactor;
  }
  void   setVolume(int level);
  double getPositionInSeconds();
  double getLengthInSeconds();
  MP3PlayerStatus getStatus();
};

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "vfw32.lib")
