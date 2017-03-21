#include <system.h>
#include <string.h>
#include <assert.h>
#include "secd.h"

#define BLOCK_SIZE 2000
#define MAX_BLOCK 10
#define MAX_STRINGS 5000

static pointer       storage[MAX_BLOCK];
static char        **strings;
static int           stringcount = 0;
static pointer       freelist = NULL;

pointer REG_S, REG_E, REG_C, REG_D, REG_W, REG_T, REG_F, nil;

char *string_store( char *s ) {
  unsigned int i;
  char *ret;
  for( i = 0; i < stringcount; i++ )
    if( !strcmp( strings[i], s ) )
      return( strings[i] );
  if( i == MAX_STRINGS ) {
    fprintf( stderr, "out of string storage. Increment MAX_STRINGS\n" );
    exit(-1);
  }
  if( ( ret = strings[stringcount++] = strdup( s ) ) == NULL ) {
    fprintf( stderr, "out of memory\n" );
    exit(-1);
  }
  return( ret );
}

#define MARKSTAK_SIZE 5000
static pointer *markstak;
static unsigned int staktop;

static void markstak_open( void ) {
  markstak = MALLOC( pointer, MARKSTAK_SIZE );
  staktop = 0;
}

static void markstak_close( void ) {
  free( markstak );
}

static void markstak_push(pointer p) {
  if( staktop >= MARKSTAK_SIZE ) {
    fprintf(stderr, "markstak overflow. Increment MARKSTAK_SIZE\n");
    exit(-1);
  }
  markstak[staktop++] = p;
}

#define markstak_pop     markstak[--staktop]
#define markstak_empty (!staktop)

static void storage_mark( pointer p ) {

  markstak_push(p);
  while( !markstak_empty ) {
    p = markstak_pop;
    if( ISMARKED(p) == 0 ) {
      ISMARKED(p) = 1;
      if( !ISATOM(p) ) {
        markstak_push( get_car(p) );
        markstak_push( get_cdr(p) );
      }
    }
  }
}

static void mark_regs( void ) {

  markstak_open( );

  storage_mark(REG_S); storage_mark(REG_E);
  storage_mark(REG_C); storage_mark(REG_D);
  storage_mark(REG_W); storage_mark(REG_T);
  storage_mark(REG_F); storage_mark(nil  );

  markstak_close( );
}

static void collect( void ) {
  unsigned int i, b;
  pointer p;
  long freecount = 0;

  for( b = 0; b < MAX_BLOCK; b++ )
    for( i = 0, p = storage[b]; i < BLOCK_SIZE; i++, p++ )
      if( ISMARKED(p) == 0) {
        INIT_ATTR(p);
        set_cdr(p, freelist);
        freelist = p;
        freecount++;
      }
  if( freelist == NULL ) {
    fprintf( stderr, "out of working-storage. Increment MAX_BLOCK\n" );
    exit(-1);
  }
  printf("freecount:%ld\n", freecount );
}

static void collect_garbage( void ) {
  unsigned int i, b;
  pointer p;

  for( b = 0; b < MAX_BLOCK; b++ )
    for( i = 0, p = storage[b]; i < BLOCK_SIZE; i++, p++ )
      ISMARKED(p) = 0;

  mark_regs();
  collect();
}

static pointer alloc_pointer( void ) {
  pointer p;
  if( freelist == NULL ) collect_garbage();
  p = freelist;

  ISALLOC(p) = 1;

  freelist = get_cdr(freelist);
  return(p);
}

pointer alloc_cons( pointer car, pointer cdr ) {
  pointer p = alloc_pointer();

  set_car(p, car);
  set_cdr(p, cdr);
  return( p );
}

pointer alloc_number( number n ) {
  pointer p = alloc_pointer();
  ISATOM(p) = ISNUMB(p) = 1;
  set_ivalue(p,n);
  return( p );
}

pointer alloc_symbol( symbol s ) {
  pointer p = alloc_pointer();
  ISATOM(p) = 1; ISNUMB(p) = 0;
  set_svalue(p,s);
  return( p );
}

void storage_init( void ) {
  int b;

  for( b = 0; b < MAX_BLOCK; b++ ) {
    storage[b] = MALLOC( struct storage_cell, BLOCK_SIZE );
    memset( storage[b], sizeof(storage[0]) * BLOCK_SIZE, 0 );
  }

  strings    = MALLOC( char *,              MAX_STRINGS );

  collect();

/* initialize nil-constant */
  nil = alloc_symbol( "NIL" );
  REG_S = REG_E = REG_C = REG_D = REG_W = REG_T = REG_F = nil ;

}
