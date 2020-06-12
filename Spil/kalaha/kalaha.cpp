#include "stdafx.h"
#include <Random.h>
#include <Console.h>

// #define DEBUG



static void vmessage( int c, int r, char *format, va_list argptr ) {
  Console::setCursorPos( c, r );
  vprintf( format, argptr );
}

static void message( int c, int r, char *format, ... ) {
  va_list argptr;
  va_start( argptr,format );
  vmessage( c, r, format, argptr );
  va_end( argptr );
}

static void cmessage( int c, int r, char *format, ... ) {
  va_list argptr;
  va_start( argptr,format );
  vmessage( c, r, format, argptr );
  va_end( argptr );
}

static int getKeyEvent( char *prompt ) {
  cmessage( 1, 22, "%s", prompt );

  for(;;) {
    int c  = Console::getKeyEvent();
    if( EVENTSCAN(c) == SCAN_ESCAPE)
      exit(0);
    if(EVENTDOWN(c)) return EVENTSCAN(c);
  }
}

static int getKey( char *prompt ) {
  cmessage( 1, 22, "%s", prompt );

  int c  = Console::getKey();
  if( c == KEY_ESCAPE )
    exit(0);

  return( c );
}

#define clearprompt() cmessage( 1, 22, " " )

static void fatalError( int line) {
  message(1,1,"Fejl i linie %d",line);
  exit(-1);
}

#define error() fatalError(__LINE__)

#define ME  0
#define YOU 1

typedef char holecont;

typedef struct boardside {
  holecont hole[7];
} boardside;

typedef struct gametype {
  boardside board[2];
  char who;
} gametype;

static gametype game;
typedef char move;

static gametype gamestak[25];

static unsigned char gamestaktop;

#define GAMESTAK_INIT()   gamestaktop = 0
#define GAMESTAK_EMPTY() (gamestaktop == 0)
#define GAMESTAK_PUSH()  (gamestak[gamestaktop++] = game)
#define GAMESTAK_POP()   (game = gamestak[--gamestaktop])



#define ISMOVE(m)   m
#define INITMOVE(m) m = 0
#define NEXTSIDE(s) (1 - (s))
#define OPPOSITE(h) (7 - (h))

#define TRIVIAL_MOVE() ( game.board[game.who].hole[1] == 1 ? 1 : \
                         game.board[game.who].hole[1] == 0 &&    \
                         game.board[game.who].hole[2] == 2 ? 2 : 0 )

int evaluate_me( void ) {
  return( game.board[ME].hole[0] - game.board[YOU].hole[0] );
}

int evaluate_you( void ) { /*  == -evaluate_me() */
  return( game.board[YOU].hole[0] - game.board[ME].hole[0] );
}

static move nextmove( move lastmove ) {
  move      i;
  holecont *hp;
  move      m = 0;

  for(i = lastmove + 1, hp = &game.board[game.who].hole[i]; i < 7; i++, hp++ )
    if( *hp > 0 ) {
      m = i;
      return( m );
    }
  return( m );
}

static move firstmove( void ) {
  move m = 0;
  return( nextmove( m ) );
}

static move prevmove( move lastmove ) {
  move      i;
  holecont *hp;
  move      m = 0;

  for(i = lastmove - 1, hp = &game.board[game.who].hole[i]; i >= 1; i--, hp-- )
    if( *hp > 0 ) {
      m = i;
      return( m );
    }
  return( m );
}

static move lastmove( void ) {
  move m = 7;
  return( prevmove( m ) );
}

static holecont total_count( int side ) {
  holecont s = 0;
  int i;
  holecont *hp = &game.board[side].hole[1];

  for( i = 1; i < 7; i++, hp++ )
    s += *hp;
  return( s );
}

static void do_move( move m ) {
  move i       = m;
  holecont c   = game.board[game.who].hole[m];
  int side     = game.who;
  holecont *op, *lp;
  char changeside = FALSE;

  GAMESTAK_PUSH();

  game.board[game.who].hole[i] = 0;
  i--;

  for( ;c > 1; c-- ) {

/* c > 1 fordi den sidste kugle skal behandles specielt */

    game.board[side].hole[i]++;
    if( i == ( side != game.who ) ) {

/* egentlig: if( ( side == who && i == 0 ) || ( side != who && i == 1 ) ) */

      side = NEXTSIDE(side);
      i = 6;
    }
    else
      i--;
  }
  lp = &game.board[side].hole[i];
  (*lp)++;

  if( i ) {                               /* Havnede sidste kugle i et hul ? */
    if( side == game.who && *lp == 1 ) {  /* var det p† egen side og tomt    */
      op = &game.board[NEXTSIDE(game.who)].hole[OPPOSITE(i)];
      game.board[game.who].hole[0] += (*op) + 1;
      *lp = *op = 0;
    }
    changeside = TRUE;
  }

  if( total_count( game.who ) == 0 ) {      /* spillet er slut */
    game.board[game.who].hole[0] += total_count( NEXTSIDE(game.who) );
    for( i = 1; i < 7; i++ )
      game.board[NEXTSIDE(game.who)].hole[i] = 0;
  }
  else
    if( changeside )
      game.who = NEXTSIDE(game.who);
}

#define undo_move()  GAMESTAK_POP()

static void game_init( int start_player ) {
  int i,who;

  for(who = 0; who < 2; who++) {
    for(i = 1; i < 7; i++)
      game.board[who].hole[i] = 6;
    game.board[who].hole[0] = 0;
  }
  game.who = start_player;
}

#define LMARG           20
#define TMARG           10
#define ROW(who)        (TMARG + 4 * (who))
#define OFFSET(hole)    (LMARG + 5 * (hole))
#define COL(who,hole)   ((who)==YOU ? OFFSET(7-(hole)) : OFFSET(hole))
#define KALROW          (TMARG + 2)
#define KALCOL(who)     COL(who,0)

static void game_disp( void ) {
  int i;

  for(i = 1; i < 7; i++)
    message( COL(ME,i), ROW(ME),     "%2d", game.board[ME].hole[i] );
  message( KALCOL(ME), KALROW, "%02d", game.board[ME].hole[0] );

  for(i = 1; i < 7; i++)
    message( COL(YOU,i), ROW(YOU),     "%2d", game.board[YOU].hole[i] );
  message( KALCOL(YOU), KALROW, "%02d", game.board[YOU].hole[0] );

}

static void board_disp( void ) {
  Console::clear();
  message( COL(ME,-1),ROW(ME)-1, "        ÚÄÄÄÄÂÄÄÄÄÂÄÄÄÄÂÄÄÄÄÂÄÄÄÄÂÄÄÄÄ¿      " );
  message( COL(ME,-1),ROW(ME)+0, "        ³    ³    ³    ³    ³    ³    ³      " );
  message( COL(ME,-1),ROW(ME)+1, "   ÚÄÄÄÄÅÄÄÄÄÁÄÄÄÄÁÄÄÄÄÁÄÄÄÄÁÄÄÄÄÁÄÄÄÄÅÄÄÄÄ¿ " );
  message( COL(ME,-1),ROW(ME)+2, "   ³    ³                             ³    ³ " );
  message( COL(ME,-1),ROW(ME)+3, "   ÀÄÄÄÄÅÄÄÄÄÂÄÄÄÄÂÄÄÄÄÂÄÄÄÄÂÄÄÄÄÂÄÄÄÄÅÄÄÄÄÙ " );
  message( COL(ME,-1),ROW(ME)+4, "        ³    ³    ³    ³    ³    ³    ³      " );
  message( COL(ME,-1),ROW(ME)+5, "        ÀÄÄÄÄÁÄÄÄÄÁÄÄÄÄÁÄÄÄÄÁÄÄÄÄÁÄÄÄÄÙ      " );

  game_disp();
}


static void unmarkhole( int who, int hole ) {
  if( who == YOU )
    message( COL(YOU,hole), ROW(YOU) + 2, "  ");
  else
    message( COL(ME ,hole), ROW(ME ) - 2, "  ");
}

static void markhole( int who, int hole ) {
  if( who == YOU )
    message( COL(YOU,hole), ROW(YOU) + 2, " %c", 24 );
  else
    message( COL(ME ,hole), ROW(ME ) - 2, " %c", 25 );
}


#define MIN_SCORE -99
#define MAX_SCORE  99

/* --------------- A L P H A - B E T A  P R U N I N G -------- */

static int  maxdepth;
static int  bestscore;
static move bestmove;
static int near (*evaluate)( void ) = evaluate_me;

static int player_in_turn;

static int maximize( int c_min, int depth );
static int minimize( int c_max, int depth );

static int maximize( int c_min, int depth ) {
  int  v,e;
  move p,r;
  char done = FALSE;

  if(depth >= maxdepth)
    v = evaluate();
  else {

    if( ( p = TRIVIAL_MOVE() ) != 0 ) {
      do_move(p);
      v = maximize( c_min, depth );
      undo_move();
      return(v);
    }

    p = firstmove();
    if( !ISMOVE(p) ) return( evaluate() );

    for( v = MIN_SCORE; ISMOVE(p) && !done; p = nextmove( p ) ) {

      do_move(p);

#if defined(DEBUG)
message( depth,depth,"%*.*sA[%2d]%d %2d\n", depth, depth, " ", c_min, p, v );
#endif

      e = game.who == player_in_turn ?
        maximize( c_min, depth+1 ) : minimize( v, depth+1 );
      if( e > v ) {
        v = e;
        r = p;
      }

      if( v >= c_min ) {  /* min(c_min, max(v, e...)) = c_min */

        done = TRUE;

#if defined(DEBUG)
message( depth,depth,"%*.*sA-Cut[%2d]C%d %2d\n", depth, depth, "-", c_min, p, v );
#endif

      }

      undo_move();
    };
  }

  if( depth == 0 && v > bestscore ) {
    bestscore = v;
    bestmove  = r;
  }
  return(v);
}

static int minimize( int c_max, int depth ) {
  int v,e;
  move p;
  char done = FALSE;

  if(depth >= maxdepth)
    v = evaluate();
  else {

    if( ( p = TRIVIAL_MOVE() ) != 0 ) {
      do_move(p);
      v = minimize( c_max, depth );
      undo_move();
      return(v);
    }

    p = firstmove();
    if( !ISMOVE(p) ) return( evaluate() );

    for( v = MAX_SCORE; ISMOVE(p) && !done; p = nextmove( p ) ) {

      do_move(p);

#if defined(DEBUG)
message( depth,depth,"%*.*sB[%2d]%d %2d\n", depth, depth, " ", c_max, p, v );
#endif

      e = game.who == player_in_turn ?
        maximize( v, depth+1 ) : minimize( c_max, depth+1 );

      if( e < v )
        v = e;

      if( v <= c_max ) {  /* max(c_max, min(v, e...)) = c_max */

        done = TRUE;

#if defined(DEBUG)
message( depth,depth,"%*.*sB-Cut[%2d]C%d %2d\n", depth, depth, "-", c_max, p, v );
#endif

      }

      undo_move();
    };
  }
  return(v);
}

static move findmove( int lookahead ) {

  bestscore      = MIN_SCORE;
  INITMOVE(bestmove);

  maxdepth       = lookahead;
  player_in_turn = game.who;
  GAMESTAK_INIT();
  evaluate       = player_in_turn == ME ? evaluate_me : evaluate_you;


  if( ( bestmove = TRIVIAL_MOVE() ) != 0 )
    bestscore = 0;
  else
    maximize( MAX_SCORE, 0 );
  return(bestmove);
}

static move usermove( int lookahead ) {
  move m,m1;
  int c;

  game_disp();

  m1 = m = firstmove();
  for( ;; ) {

    unmarkhole( YOU, m1 );
    markhole(   YOU, m );
    m1 = m;

    switch(c = getKeyEvent( "Din tur" ) ) {
      case SCAN_LEFT:
        if( !(m = nextmove(m)) ) m = firstmove();
        continue;
      case SCAN_RIGHT :
        if( !(m = prevmove(m)) ) m = lastmove();
        continue;
      case SCAN_ENTER:
        break;
      default:
        continue;
    }
    break;
  }

  unmarkhole( YOU, m );
  clearprompt();

  return(m);
}

static char *winnerstring[] = {
  "Hvem er s† bedst, mennesket eller maskinen ?",
  "v b›v ... Du tabte",
  "Pr›v igen...",
  "Mon ikke du skulle g† hjem og ›ve dig lidt...",
  "Du skal bare spille lige som mig...",
  "Du har chancen i n‘ste spil...",
  "Tab og vind...",
  "Een skal jo tabe",
  "Een skal jo vinde ... det var ikke dig"
};

static char *looserstring[] = {
  "Der var du vist heldig...",
  "Ok - du vandt, men du var heldig...",
  "H›r, her er vist noget galt...",
  "Du sku' jo n›dig blive ked af det. Jeg lod dig vinde ... ",
  "Een skal jo tabe",
  "Een skal jo vinde"
};

static int random(int max) {
  return rand() % max;
}

#define randomstring(strarray) strarray[random(ARRAYSIZE(strarray))]

static void game_run( int selfplay, int starter, int waiting,
               int lookahead_me, int lookahead_you ) {
  move m;
  int c;

  game_init( starter );

  board_disp();

  while(total_count( game.who )) {
    switch( game.who ) {
      case YOU:
        if( !selfplay )
          m =  usermove( lookahead_me );
        else {
          m = findmove( lookahead_you );
          if(waiting ) {
            markhole( YOU, m );
            sleep(1);
            unmarkhole( YOU, m );
          }
        }
        break;
      case ME :
        m = findmove( lookahead_me );
        if(waiting ) {
          markhole( ME, m );
          sleep(1);
          unmarkhole( ME, m );
        }
        break;
    }
    if( !m ) error();

    do_move(m);

    game_disp();

  }

  game_disp();
  c = game.board[ME].hole[0] - game.board[YOU].hole[0];
  if( c < 0 )
    cmessage( 1, 21, randomstring(looserstring) );
  else
  if( c > 0 )
    cmessage( 1, 21, randomstring(winnerstring) );
  else
    cmessage( 1, 21, "Uafgjort" );
}

static void usage( void ) {
  fprintf( stderr,
  "usage kalaha [-s] [-l#lookahead(me)] [-p#lookahead(you)]\n");
  exit(-1);
}

int main(int argc, char **argv) {
  char *cp;
  int lookahead_me  = 6;
  int lookahead_you = 6;
  int starter       = YOU;
  int selfplay      = FALSE;
  int waiting       = TRUE;

  for(argv++;*argv && *(cp = *argv) == '-';argv++)
    for(cp++;*cp;cp++) {
      switch(*cp) {
        case 'l':
          if( sscanf( cp+1, "%d", &lookahead_me ) != 1 )
            usage();
          break;
        case 'p':
          if( sscanf( cp+1, "%d", &lookahead_you ) != 1 )
            usage();
          selfplay = TRUE;
          break;

        case 's':
          starter = ME;
          continue;

        case 'w':
          waiting = FALSE;
          continue;

        default:
          usage();
      }
      break;
    }

  randomize();

  for(;;) {
    game_run( selfplay, starter, waiting, lookahead_me, lookahead_you );
    if(getKey( "Vil du spille igen" ) == 'n' )
      break;
    clearprompt();
    starter = !starter;
  }

  return 0;
}
