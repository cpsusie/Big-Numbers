#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include "secd.h"

#define MAX_LINE_LEN 256

static char line[MAX_LINE_LEN];
static char *cp;
static FILE *input  = NULL;
static FILE *output = stdout;
static char ch;
boolean input_eof;

static void mark_error( char *format, ... ) {
  char *nl;
  va_list argptr;

  if( (nl = strchr( line, '\n' ) ) != NULL ) *nl = '\0';
  printf( "%s\n", line );
  printf( "%*s^---", cp - line, " " );

  va_start( argptr, format );
  vprintf( format, argptr );
  va_end( argptr );
}

static void nextline( void ) {
  if( fgets( line, sizeof(line), input ) )
    input_eof = FALSE;
  else {
    strcpy( line, " " );
    input_eof = TRUE;
  }
  cp = line;
/*  printf("nextline:\"%s\"\n",line ); */
}

static void nextchar( void ) {
  if( *cp == '\0' )
    nextline();
  ch = *(cp++);
/*  printf("nextchar:'%c'\n",ch); */
}


typedef enum {
  TOK_NUMBER,
  TOK_SYMBOL,
  TOK_LPAR,
  TOK_RPAR,
  TOK_DOT
} token_type;

static token_type token;

#define MAX_SYMBOL_LEN 32

static char   the_symbol[MAX_SYMBOL_LEN];
static number the_number;

static void scansymbol( void ) {
  int i = 0;
  do {
    if( i >= MAX_SYMBOL_LEN ) {
      mark_error( "Identifier to long" );
      break;
    }
    the_symbol[i++] = ch;
    nextchar();
  } while( isalpha( ch ) );
  the_symbol[i] = '\0';
}

static void scannumber( void ) {
  int sign;

  if( ch != '-' )
    sign = 1;
  else {
    sign = -1;
    nextchar();
  }
  for( the_number = 0; isdigit( ch ); nextchar() )
    the_number = the_number * 10 + (ch - '0');
  the_number *= sign;
}

static void trace_token( void ) {
    switch( token ) {
      case TOK_SYMBOL: printf( "SYMBOL:\"%s\"\n", the_symbol ); break;
      case TOK_NUMBER: printf( "NWMBER:%d\n", the_number ); break;
      case TOK_LPAR:   printf( "LPAR\n" ); break;
      case TOK_RPAR:   printf( "RPAR\n" ); break;
      case TOK_DOT:    printf( "DOT\n" ); break;
    }
}

static void nexttoken( void ) {
  while( !input_eof && isspace(ch) ) nextchar();
  if( input_eof )
    token = TOK_RPAR;  else
  if( isdigit( ch ) || ch == '-' ) {
    token = TOK_NUMBER;
    scannumber();
  }  else
  if( isalpha( ch ) ) {
    token = TOK_SYMBOL;
    scansymbol();
  } else
  switch( ch ) {
    case '.': token = TOK_DOT;  nextchar(); break;
    case '(': token = TOK_LPAR; nextchar(); break;
    case ')': token = TOK_RPAR; nextchar(); break;
    default:  mark_error( "Illegal character %x", ch); nextchar(); break;
  }
/*  trace_token(); */
}

pointer getexp( void ) {
  pointer e;

/*  printf("getexp start:"); trace_token(); */

  switch( token ) {
    case TOK_LPAR:
      nexttoken(); e = getexplist();
      nexttoken();
      break;
    case TOK_NUMBER:
      e = alloc_number( the_number );
      nexttoken();
      break;
    case TOK_SYMBOL:
      e = alloc_symbol( the_symbol );
      nexttoken();
      break;
    default:
      mark_error( "s-expression expected\n" );
      nexttoken();
  }

/*  printf("getexp end:"); trace_token(); */

  return( e );
}

pointer getexplist( void ) {
  pointer e = alloc_cons( nil, nil ); /* car, cdr unknown yet */

/*  printf("getexplist start:"); trace_token(); */

  set_car( e, getexp() );

  switch( token ) {
    case TOK_DOT:
      nexttoken(); set_cdr( e, getexp() );
      break;
    case TOK_RPAR:
      set_cdr( e, nil );
      break;
    default:
      set_cdr( e, getexplist() );
      break;
  }

/*  printf("getexplist end:"); trace_token(); */

  return( e );
}


void open_input( char *fname ) {
  if( input && input != stdin )
    fclose( input );

  if( fname == NULL )
    input = stdin;
  else {
    input = fopen( fname, "r" );
    if( !input ) {
      perror( fname );
      exit(-1);
    }
  }
  nextline();
  nextchar();
  nexttoken();
}


static void puttoken( char *format, ... ) {
  va_list argptr;
  va_start( argptr, format );
  vfprintf( output, format, argptr );
  va_end( argptr );
}

void putexp( pointer e ) {
  if( is_symbol(e) ) puttoken("%s ",  get_svalue(e) ); else
  if( is_number(e) ) puttoken("%ld ", get_ivalue(e) );
    else {
      puttoken( "(" );
      for( ; is_cons(e); e = get_cdr(e) )
        putexp(get_car(e));
      if( e == nil ) /* issymbol(e) && !strcmp( get_svalue(e), "NIL" ) ) */
        { }
      else {
        puttoken( "." );
        putexp(e);
      }
      puttoken( ")\n" );
    }
}

void open_output( char *fname ) {
  if( output != stdout )
    fclose( output );

  if( fname == NULL )
    output = stdout;
  else {
    output = fopen( fname, "w" );
    if( !output ) {
      perror( fname );
      exit(-1);
    }
  }
}

