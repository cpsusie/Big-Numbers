#include <stdio.h>
#include <stdlib.h>
#include <argvexp.h>
#include "secd.h"

static void compile( char **fname ) {
  pointer result;

  open_input( "compile.lob" );
  REG_C = getexp();
  for(;*fname;fname++) {
    open_input( *fname );
    REG_S = getexplist();
    printf("\nNow compiling %s\n", *fname );
    result = execute( REG_C, REG_S );
    putexp( result );
  }
}

static void usage( void ) {
  fprintf( stderr, "Usage: lisp\n" );
  exit(-1 );
}

main( int argc, char **argv) {
  char *cp;
  char *outfname = NULL;

  argvexpand( &argc, &argv );

  for(argv++; *argv && ((*(cp = *argv) == '-')); argv++) {
    for(cp++;*cp;cp++) {
      switch(*cp) {
      break;
    }
  }

  storage_init();

  return( 0 );
}
