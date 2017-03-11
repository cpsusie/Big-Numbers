#include "stdafx.h"
#include <MyUtil.h>
#include <Console.h>
#include <Random.h>

static int random(int max) {
  return randInt(max);
}

char *youWinText[] = {
  "Der var du heldig ....",
  "Du har vist smugtrænet ....",
  "Hør lige engang ....",
  "Hvad er meningen ....",
  "Det skulle ikke kunne lade sig gøre .... ",
  "Der er vist noget galt her .... "
};

char *IWinText[] = {
  "æv bæv, du tabte ....",
  "Prøv igen ....",
  "Bedre held næste gang ....",
  "Det kan jo smutte, det skidt ....",
  "Man kan ikke være lige heldig hver gang ....",
  "Mon ikke du skulle gå hjem og øve dig ....",
  "Selv den bedste kan fejle .... ",
  "Det er menneskeligt at fejle ....",
};

#define MESS_LINE 22
#define ASK_LINE  23

void message(int x, int y, char *format, ...) {
  va_list argptr;
  Console::setCursorPos(x, y);
  printf("%*.*s",50,50,"");
  va_start(argptr, format);
  Console::setCursorPos(x, y);
  vprintf(format, argptr);
  va_end(argptr);
}

char *gets(char *str) {
  fgets(str, 1000, stdin);
  return str;
}

#define NROW 4
#define PIN(r) 2*r+1
#define PP(r) PIN(r)+1

typedef BYTE MOVE;

int row[NROW];
int npinds;

MOVE mtab[PP(0)][PP(1)][PP(2)][PP(3)];

#define tabval mtab[row[0]][row[1]][row[2]][row[3]]
#define tabsave(c) (tabval=(c))

#define doMove(r,n) { row[r]-=(n); npinds-=(n); }
#define undoMove(r,n) doMove(r,-(n))

#define movecode(r,n) (MOVE)(((r)<<5)|(n))
#define crow(c) (int)((c)>>5)
#define cpin(c) (int)((c)&0x1f)

#define tablost 0x20

void drawLine(int r, int p, char ch) {
#define LH 3
  for(int i = 0; i < LH; i++ ) {
    Console::setCursorPos( (p+1)*4 + 30 - 2 * PIN(r), (r+1)*(LH+1)+i );
    printf("%c", ch);
  }
}

void showPinds() {
  for(int r = 0; r < NROW; r++) {
    for(int p = 0; p < row[r]; p++) {
      drawLine( r, p, (char)219 );
    }
    for(int p = row[r]; p < PIN(r); p++) {
      drawLine( r, p, ' ' );
    }
  }
}

void initGame() {
  npinds = 0;
  for(int r = 0; r < NROW; r++) {
    row[r] = PIN(r);
    npinds += row[r];
  }
}

MOVE findMove() {
  MOVE v;
  if(v = tabval) {
    return v;
  }
  if(npinds == 1) {
    return(tabsave(tablost));
  }
  for(int r = 0; r < NROW; r++) {
    for(int n = 1; n <= row[r]; n++) {
      doMove(r,n);
      if(findMove() == tablost) {
        undoMove(r,n);
        return(tabsave(movecode(r,n)));
      };
      undoMove(r,n);
    }
  }
  return(tabsave(tablost));
}

MOVE randomMove() {
  int rn[NROW];
  int c = 0;
  for(int r = 0; r < NROW; r++) {
    if(row[r]) rn[c++]=r;
  }
  int r = rn[randInt(c)];
  int n = randInt(1, row[r]);
  return(movecode(r,n));
}

void userMove() {
  for(;;) {
    message( 1, ASK_LINE, "Angiv række og antal ( række = 1..4 ):");
    char line[50];
    gets(line);
    int r,n;
    if(sscanf(line,"%d %d",&r,&n)!=2) continue;
    r--;
    if(( r <  0 ) || ( r >= NROW   ) ) {
      message( 1, MESS_LINE, "Række = 1..%d", NROW );
      continue;
    }
    if(( n <= 0 ) || ( n >  row[r] ) ) {
      message( 1, MESS_LINE, "Antal pinde i række %d = %d", r+1, row[r] );
      continue;
    }
    doMove(r,n);
    message(1, MESS_LINE, "");
    return;
  }
}

bool humanTurn,selfplay;

static void runGame() {
  MOVE c;
  initGame();
  showPinds();
  while(npinds>=1) {
    switch(humanTurn) {
    case false :
      if((c = findMove()) == tablost) {
        c = randomMove();
      }
      doMove(crow(c),cpin(c));
      message( 1, MESS_LINE, "Tænker ...." );
      sleep(1);
      showPinds();
      message( 1, MESS_LINE, "");
      if(!selfplay)
        humanTurn = true;
      break;
    case true:
      userMove();
      showPinds();
      humanTurn = false;
      break;
    }
  }
  showPinds();
  message( 1, MESS_LINE, "%s"
                       ,humanTurn
                      ? youWinText[random( ARRAYSIZE( youWinText))]
                      : IWinText[  random( ARRAYSIZE( IWinText  ))]
         );
}

int main(int argc, char **argv) {
  char line[10];

  Console::clear();
  randomize();

  for(;;) {
    message(1, ASK_LINE, "Vil du starte [jns]:");
    gets(line);
    switch(line[0]) {
      case 'n': humanTurn = false; selfplay = false; break;
      case 's': humanTurn = false; selfplay = true;  break;
      default : humanTurn = true;  selfplay = false; break;
    }

/*
    humanTurn = false;
*/
    runGame();
    message( 1, ASK_LINE, "Mere [jn]:");
    gets(line);
    if(line[0] == 'n') break;
  }
}
