#include "stdafx.h"
#include <conio.h>

#define NROWS        7
#define CARDSPERROW  4
#define MAXSUM      31
#define HASLOST()   (sum > MAXSUM)

static int table[NROWS];                 /* number of cards left (0..CARDSPERROW) in each row      */
static int sum;                          /* the current sum                                        */

#define NGAMESTATE 15625                 /* 7^6 number of possibel states in the game              */

static int  moveTable[NGAMESTATE];       /* Possible moves at each state 3 bits for each candidate */
static BYTE candidateCount[NGAMESTATE];  /* number of candidate at each state                      */
static UINT gameState;                   /* the address in moveTable of the state of the game      */
static UINT stateShift[NROWS];           /* the shift of gameState when a card of row is removed   */

void messat(int x, int y, const TCHAR *str) {
  Console::printf(x, y, _T("                          "));
  Console::printf(x, y, _T("%s"),str);
}

void message(TCHAR *format,...) {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format, argptr);
  va_end(argptr);
  messat(1,16,msg.cstr());
}

void initTables() {
  int i;
  for(i = 0; i < NGAMESTATE; i++) {
    moveTable[i] = 0;
  }
  for(i = 0; i < NGAMESTATE; i++) {
    candidateCount[i] = 0;
  }
  for(stateShift[1] = 1, i = 2; i < NROWS; i++) {
    stateShift[i] = stateShift[i-1]*5;
  }
}

void initGame() {
  sum = 0;
  for(int i = 0; i < NROWS; i++) {
    table[i] = CARDSPERROW;
  }
  gameState = 0;
}

void doMove(int row) {
  table[row]--;
  sum += row;
  gameState += stateShift[row];
}

void undoMove(int row) {
  table[row]++;
  sum -= row;
  gameState -= stateShift[row];
}

int nextMove(int lastMove) {
  for(lastMove++; lastMove < NROWS; lastMove++) {
    if(table[lastMove]) {
      return lastMove;
    }
  }
  return 0;
}

#define firstMove()            nextMove(0)
#define MOVELOST 7
#define getMoveTable(state,i) ((moveTable[state] >> 3*(i)) & 0x7)
#define getMoveCandidate()      getMoveTable(gameState,0)

int setMoveTable(int move) {
  moveTable[gameState] |= (move & 0x7) << (3*(candidateCount[gameState]++));
  return(move);
}

char *sprintbase(char *str, UINT n, int base, int len) {
  char *cp;
  for(cp = str; n; n /= base ) {
    *(cp++) = '0' + (n % base);
  }
  while(cp - str < len) {
    *(cp++) = '0';
  }
  *cp= 0;
/*  return( strrev( str ) ); */
  return str;
}

void saveTable(FILE *f) {
  UINT j, k;
  char tmp[20];
  int  sum;

  for(int i = 0; i < NGAMESTATE; i++) {
    if((k=candidateCount[i])==0) {
      continue;
    }
    sprintbase(tmp, NGAMESTATE-i-1,5,6);
    for(j = 0, sum = 0; j < 6; j++) {
      sum += (j+1) * (4 - (tmp[j] - '0'));
    }
    fprintf(f, "%2d [%s] -> ", sum, tmp);
    for(UINT j = 0; j < k; j++) {
      fprintf(f, "%d ", getMoveTable(i,j) );
    }
    if(sum > 31) {
      fprintf(f, "%s", tmp);
    }
    fprintf(f, "\n");
  }
}

int findMove() {
  int move, cand=MOVELOST;
  if((move = getMoveCandidate()) != 0) {
    return move;
  }
  for(move = firstMove(); move; move = nextMove(move)) {
/*
    message("prøver %6d : %d : %d",gameState,move,sum);
*/
    doMove(move);
    if((!HASLOST()) && (findMove() == MOVELOST)) { /* got a winnermove */
      undoMove(move);
      setMoveTable(move);
      cand = move;
    } else {
      undoMove(move); /* try next */
    }
  }
  if(cand == MOVELOST) {
    setMoveTable(cand);
  }
  return cand;
}

void displayTable() {
  Console::setCursorPos(0,1);
  for(int i = 1; i < NROWS; i++) {
    for(int j = 0; j < table[i]; j++) {
      printf("%d   ",i);
    }
    printf("           \n");
  }
  printf("\n\nSum : %2d", sum);
}

int userMove() {
  int r;
  for(;;) {
    displayTable();
    Console::setCursorPos(10,14);
    String line = inputString(_T("Dit træk:"));
    if(line == _T("hint")) {
      if((candidateCount[gameState] == 0) || (getMoveCandidate() == MOVELOST)) {
        message(_T("Spørg mågerne !"));
      } else {
	      message(_T("Muligheder:"));
	      for(int i = 0; i < candidateCount[gameState]; i++) {
	        printf("%d ",getMoveTable(gameState,i));
        }
      }
      continue;
    }
    if((_stscanf(line.cstr(), _T("%d"), &r) != 1) || (r<1) || (r>=NROWS) || (table[r] == 0)) {
      message(_T("Indtast et tal mellem 1 og %d"),NROWS-1);
    } else {
      break;
    }
  }
  message(EMPTYSTRING);
  return r;
}

int randomMove() {
  for(;;) {
    int r = randInt(1, NROWS);
    if(table[r]) {
      return r;
    }
  }
}

void game(int userStart) {
  int r;

  initGame();
  for(;;) {
    if(userStart) {
      r = userMove();
      doMove(r);
      if(HASLOST()) {
        messat(1,14,_T("Du tabte!"));
        return;
      }
    }
    userStart = 1;
    if((r = findMove()) == MOVELOST) {
      r = randomMove();
      message(_T("Du kan vinde!"));
    } else {
      message(_T("Du har tabt!"));
    }

    doMove(r);
    if(HASLOST()) {
      messat(1,14, _T("Du vandt !"));
      return;
    }
  }
}

int main(int argc, char **argv) {
  initTables();
  initGame();
  findMove(); /* to initialize the tables */

  randomize();
  Console::clear();
  for(int userStart=0;;userStart = 1-userStart) {
    game(userStart); /* userStart */
    message(_T("Et spil til (j/n)?"));
    const char ch = _getch();
    if(ch =='n' || ch == 'N') break;
  }
  FILE *f = fopen( _T("kortspil.dat"), _T("w") );
  saveTable(f);
  fclose(f);
  return 0;
}
