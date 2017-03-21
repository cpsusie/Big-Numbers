#include <stdio.h>
#include <stdlib.h>
#include <argvexp.h>
#include "secd.h"

static void test_getexp( char *fname ) {

  open_input( fname );
  putexp( getexp() );
}

static void test_storage( char *fname ) {
  int i;

  printf( "testing storage\n" );

  for(;;) {
    REG_S = nil;
    for( i = 0 ; i < 10; i++ ) {
      REG_W = alloc_number(i);
      REG_D = alloc_cons( REG_W, REG_S );
      REG_S = REG_D;
    }
    putexp( REG_S );
/*
    for(; REG_S != nil; REG_S = get_cdr(REG_S) ) {
      printf("%d ", get_ivalue(get_car(REG_S)) );
    }
*/
    printf("Tryk tast:"); getchar();
  }
}

static void test_execute( char **fname ) {
  pointer result;
  for(;*fname;fname++) {
    printf("Testfile:%s\n", *fname );
    open_input( *fname );
    REG_C  = getexp();
    REG_S  = getexplist();
    printf( "Testing :");
    putexp( REG_C );
    printf(" ");
    putexp( REG_S );
    printf("\nNow testing:\n");
    result = execute( REG_C, REG_S );
    printf("result:");
    putexp(result);
    printf("\n-----------------\n");
    getchar();
  }
}

static void test_comprun( char *fname ) {
  pointer result;

  open_input( "compile.lob" );
  REG_C = getexp();                    /* loading compiler */

  open_input( fname );
  REG_S = getexplist();                /* loading program-source */

  REG_C = execute( REG_C, REG_S );     /* compiling */
  for(;;) {
    printf("Enter args:");
    open_input( NULL );
    REG_S   = getexplist();            /* reading arguments */
    result  = execute( REG_C, REG_S ); /* executing program */
    putexp( result );
    printf("\n" );
  }
}

static void test_compile( char **fname ) {
  pointer result;

  open_input( "compile.lob" );
  REG_C = getexp();
  for(;*fname;fname++) {
    open_input( *fname );
    REG_S = getexplist();
    printf("\nNow compiling:\n");
    result = execute( REG_C, REG_S );
    printf("Result: ");
    putexp( result );
    printf("\n" );
    getchar();
  }
}

static void usage( void ) {
  fprintf( stderr, "Usage: testsecd [ -e | -s | -x | -c ] [fname]\n" );
  exit(-1 );
}

typedef enum {
  COM_TEST_GETEXP,
  COM_TEST_STORAGE,
  COM_TEST_EXECUTE,
  COM_TEST_COMPILE,
  COM_TEST_RUN
} command_type;

main( int argc, char **argv) {
  char *cp;
  command_type command = COM_TEST_GETEXP;
  char *outfname = NULL;

  argvexpand( &argc, &argv );

  for(argv++; *argv && ((*(cp = *argv) == '-')); argv++) {
    for(cp++;*cp;cp++) {
      switch(*cp) {
        case 'e': command = COM_TEST_GETEXP;  continue;
        case 's': command = COM_TEST_STORAGE; continue;
        case 'x': command = COM_TEST_EXECUTE; continue;
        case 'c': command = COM_TEST_COMPILE; continue;
        case 'r': command = COM_TEST_RUN;     continue;
        case 'o': argv++;
                  if(!*argv) usage();
                  outfname = *argv;
                  break;
        default : usage();
      }
      break;
    }
  }

  storage_init();

  open_output( outfname );

  switch( command ) {
    case COM_TEST_GETEXP:  test_getexp(  *argv ); break;
    case COM_TEST_STORAGE: test_storage( *argv ); break;
    case COM_TEST_EXECUTE: test_execute(  argv ); break;
    case COM_TEST_COMPILE: test_compile(  argv ); break;
    case COM_TEST_RUN:     test_comprun( *argv ); break;
  }
  return( 0 );
}
