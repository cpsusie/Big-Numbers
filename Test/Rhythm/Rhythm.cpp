#include "stdafx.h"
#include <Thread.h>
#include <InputValue.h>
#include <console.h>

class Beat {
public:
  char   m_n;
  double m_t;
  Beat() {}
  Beat(char n, double t) : m_n(n), m_t(t) {
  }
};

int beatcmp(const Beat &b1, const Beat &b2) {
  double c = b1.m_t - b2.m_t;
  return c < 0 ? -1 : c > 0 ? 1 : 0;
}

static double speed = 2000;

class controlthread : public Thread {
  UINT run();
};

UINT controlthread::run() {
  for(;;) {
    int event = Console::getKeyEvent();
    switch(EVENTSCAN(event)) {
      case SCAN_LEFT : speed *= 1.1; break;
      case SCAN_RIGHT: speed /= 1.1; break;
      case SCAN_ESCAPE: exit(0);
    }
  }
  return 0;
}


int main(int argc, char **argv) {
  int t1 = inputValue<int>(_T("Indtast antal slag for takt 1:"));
  int t2 = inputValue<int>(_T("Indtast antal slag for takt 2:"));
  CompactArray<Beat> beatSequence;
  int len = t1 + t2 - 1;
  int i,c=1;

  beatSequence.add(Beat(1,0));
  for(i = 1; i < t1; i++) {
    beatSequence.add(Beat(2, (double)i / t1));
  }
  for(i = 1; i < t2; i++) {
    beatSequence.add(Beat(3, (double)i / t2));
  }
  beatSequence.sort(beatcmp);
  double t = beatSequence[1].m_t; // slå dobbeltslag sammen til 1
  for(i = 2; i < len; i++) {
    if(fabs(beatSequence[i].m_t - t) < 1e-5) {
      beatSequence[i-1].m_n = 1;
      beatSequence.remove(i);
      len--;
    }
    t = beatSequence[i].m_t;
  }
  TCHAR *displaystr[] = { EMPTYSTRING,_T("v+h"),_T("v"),_T("h") };
  for(i = 0; i < len; i++) {
    _tprintf(_T("%.5lf %s\n"),beatSequence[i].m_t, displaystr[beatSequence[i].m_n]);
  }
  _tprintf(_T("tryk \x1a eller \x1b for hastighed eller ctrl+C for at stoppe...\n"));
#define BEEPLEN 15

  speed = 400 * len;
  controlthread thr;
  thr.resume();

  for(i=0;;) {
    Beep(beatSequence[i].m_n * 220,BEEPLEN);
    double delay = i == (len-1) ? (1.0 - beatSequence[i].m_t) : beatSequence[i+1].m_t - beatSequence[i].m_t;
//    printf("%d:%lf\n",i,delay);
    int sl = (int)(delay*speed-BEEPLEN);
    if(sl < 0) sl = 1;
    Sleep(sl);
    i++;
    if(i == len) i=0;
  }
  return 0;
}
