#include "stdafx.h"
#include <InputValue.h>

class kamp {
public:
  int hold1,hold2;
  kamp(int h1, int h2) { hold1 = h1; hold2 = h2; }
  kamp() { hold1 = hold2 = 0; }
};

#define MAXN 20
class turneringsplan {
public:
  kamp kampe[MAXN][MAXN];
  int antalkampe[MAXN];
  int antalspilledage;
  turneringsplan();
  void findturneringsplan(int antalhold);
  bool holdafsat(int dag, int hold);
  void print(FILE *f = stdout);
};

turneringsplan::turneringsplan() {
  memset(kampe,0,sizeof(kampe));
  memset(antalkampe,0,sizeof(antalkampe));
}

bool turneringsplan::holdafsat(int dag, int hold) {
  for(int k = 0; k < antalkampe[dag]; k++) {
    kamp &kk = kampe[dag][k];
    if(kk.hold1 == hold || kk.hold2 == hold) return true;
  }
  return false;
}

void turneringsplan::findturneringsplan(int antalhold) {
  antalspilledage = antalhold - ((antalhold%2)?0:1);
  int startdag = 0;
  for(int i = 1; i < antalhold; i++) {
    for(int j = i+1; j <= antalhold; j++) {
      for(int dag = startdag;;dag = (dag+1) % antalspilledage) {
        if(!holdafsat(dag,i) && !holdafsat(dag,j)) {
          kampe[dag][antalkampe[dag]++] = kamp(i,j);
          break;
        }
      }
    }
    startdag = (startdag+2) % antalspilledage;
  }
}

void turneringsplan::print(FILE *f) {
  for(int k = 0;; k++) {
    if(k == antalkampe[0]) {
      break;
    }
    for(int dag = 0; dag < antalspilledage; dag++) {
      kamp &kk = kampe[dag][k];
      fprintf(f,"%2d - %2d ",kk.hold1,kk.hold2);
    }
    fprintf(f,"\n");
  }
}

int main(int argc, char **argv) {
  for(;;) {
    int n = inputValue<int>(_T("angiv antal hold:"));
    turneringsplan s;
    s.findturneringsplan(n);
    s.print();
  }
  return 0;
}
