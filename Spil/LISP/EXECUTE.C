#include <stdio.h>
#include "secd.h"
typedef enum {
  LD = 1,
  LDC,
  LDF,
  AP,
  RTN,
  DUM,
  RAP,
  SEL,
  JOIN,
  CAR,
  CDR,
  ATOM,
  CONS,
  EQ,
  ADD,
  SUB,
  MUL,
  DIV,
  REM,
  LEQ,
  STOP,
  CAAR,
  CADR,
  CDAR,
  CDDR,
  CAAAR,
  CAADR,
  CADAR,
  CADDR,
  CDAAR,
  CDADR,
  CDDAR,
  CDDDR
} instructions;

static void dumpreg( char *n, pointer p ) {
  printf("%s:", n );
  putexp(p);
  printf("\n");
}

static void dumpregs( void ) {
 dumpreg( "REG_S:", REG_S );
 dumpreg( "REG_E:", REG_E );
 dumpreg( "REG_C:", REG_C );
 dumpreg( "REG_D:", REG_D );
}

#define get_caar(s)   get_car(get_car(s))
#define get_cadr(s)   get_car(get_cdr(s))
#define get_cdar(s)   get_cdr(get_car(s))
#define get_cddr(s)   get_cdr(get_cdr(s))
#define get_caaar(s)  get_caar(get_car(s))
#define get_caadr(s)  get_caar(get_cdr(s))
#define get_cadar(s)  get_cadr(get_car(s))
#define get_caddr(s)  get_cadr(get_cdr(s))
#define get_cdaar(s)  get_cdar(get_car(s))
#define get_cdadr(s)  get_cdar(get_cdr(s))
#define get_cddar(s)  get_cddr(get_car(s))
#define get_cdddr(s)  get_cddr(get_cdr(s))

#define unary_op(  op ) alloc_cons( op(get_car(REG_S)),get_cdr(REG_S));
#define binary_op( op ) alloc_number(get_ivalue(get_car(get_cdr(REG_S))) \
                                  op get_ivalue(get_car(REG_S)))

pointer execute( pointer fn, pointer args ) {
  int i, n;

  REG_T = alloc_symbol( "T" );
  REG_F = alloc_symbol( "F" );
  REG_S = alloc_cons( args, nil );
  REG_E = nil;
  REG_C = fn;
  REG_D = nil;

  for(;;) {

    switch( get_ivalue( get_car( REG_C ) ) ) {
      case LD:   REG_W = REG_E;
                 n = get_ivalue(get_car(get_car(get_cdr(REG_C))));
                 for(i = 0; i < n; i++ ) REG_W = get_cdr(REG_W);
                 REG_W = get_car(REG_W);
                 n = get_ivalue(get_cdr(get_car(get_cdr(REG_C))));
                 for(i = 0; i < n; i++ ) REG_W = get_cdr(REG_W);
                 REG_W = get_car(REG_W);
                 REG_S = alloc_cons(REG_W, REG_S);
                 REG_C = get_cdr(get_cdr(REG_C));
                 break;

      case LDC:  REG_S = alloc_cons(get_car(get_cdr(REG_C)),REG_S);
                 REG_C = get_cdr(get_cdr(REG_C));
                 break;

      case LDF:  REG_W = alloc_cons( get_car( get_cdr(REG_C)), REG_E);
                 REG_S = alloc_cons( REG_W, REG_S );
                 REG_C = get_cdr(get_cdr(REG_C));
                 break;

      case AP:   REG_W = alloc_cons( get_cdr(REG_C), REG_D);
                 REG_W = alloc_cons( REG_E, REG_W );
                 REG_D = alloc_cons( get_cdr(get_cdr(REG_S)), REG_W );
                 REG_E = alloc_cons( get_car(get_cdr(REG_S)),
                                     get_cdr(get_car(REG_S)));
                 REG_C = get_car(get_car(REG_S));
                 REG_S = nil;
                 break;

      case RTN:  REG_S = alloc_cons( get_car(REG_S),get_car(REG_D));
                 REG_E = get_car( get_cdr(REG_D) );
                 REG_C = get_car( get_cdr(get_cdr(REG_D)));
                 REG_D = get_cdr( get_cdr(get_cdr(REG_D)));
                 break;

      case DUM:  REG_E = alloc_cons( nil,REG_E);
                 REG_C = get_cdr(REG_C);
                 break;

      case RAP:  REG_W = alloc_cons( get_cdr(REG_C), REG_D );
                 REG_W = alloc_cons( get_cdr(REG_E), REG_W );
                 REG_D = alloc_cons( get_cdr(get_cdr(REG_S)), REG_W );

                 REG_E = get_cdr(get_car(REG_S));
                 set_car(REG_E, get_car(get_cdr(REG_S)));
                 REG_C = get_car(get_car(REG_S));
                 REG_S = nil;
                 break;

      case SEL:  REG_D = alloc_cons(get_cdr(get_cdr(get_cdr(REG_C))),REG_D);
                 if( get_ivalue(get_car(REG_S)) == get_ivalue(REG_T) )
                   REG_C = get_car(get_cdr(REG_C));
                 else
                   REG_C = get_car(get_cdr(get_cdr(REG_C)));
                 REG_S = get_cdr(REG_S);
                 break;

      case JOIN: REG_C = get_car(REG_D);
                 REG_D = get_cdr(REG_D);
                 break;

      case CAR:  REG_S = unary_op( get_car );
                 REG_C = get_cdr(REG_C);
                 break;

      case CDR:  REG_S = unary_op( get_cdr );
                 REG_C = get_cdr(REG_C);
                 break;

      case ATOM: if( is_cons(get_car(REG_S)) )
                   REG_S = alloc_cons( REG_F, get_cdr(REG_S) );
                 else
                   REG_S = alloc_cons( REG_T, get_cdr(REG_S) );
                 REG_C = get_cdr(REG_C);
                 break;

      case CONS: REG_W = alloc_cons( get_car(REG_S),get_car(get_cdr(REG_S)));
                 REG_S = alloc_cons( REG_W, get_cdr(get_cdr(REG_S)));
                 REG_C = get_cdr(REG_C);
                 break;

      case EQ:   if(is_symbol(get_car(REG_S))           &&
                    is_symbol(get_car(get_cdr(REG_S)))  &&
                    get_ivalue(get_car(REG_S)) ==
                    get_ivalue(get_car(get_cdr(REG_S))) ||
                    is_number(get_car(REG_S))           &&
                    is_number(get_car(get_cdr(REG_S)))  &&
                    get_ivalue(get_car(REG_S)) ==
                    get_ivalue(get_car(get_cdr(REG_S)))
                    )
                      REG_S = alloc_cons(REG_T, get_cdr(get_cdr(REG_S)));
                  else
                    REG_S = alloc_cons(REG_F, get_cdr(get_cdr(REG_S)));
                 REG_C = get_cdr(REG_C);
                 break;

      case ADD:  REG_W = binary_op( + );
                 REG_S = alloc_cons( REG_W, get_cdr(get_cdr(REG_S)));
                 REG_C = get_cdr(REG_C);
                 break;

      case SUB:  REG_W = binary_op( - );
                 REG_S = alloc_cons( REG_W, get_cdr(get_cdr(REG_S)));
                 REG_C = get_cdr(REG_C);
                 break;

      case MUL:  REG_W = binary_op( * );
                 REG_S = alloc_cons( REG_W, get_cdr(get_cdr(REG_S)));
                 REG_C = get_cdr(REG_C);
                 break;

      case DIV:  REG_W = binary_op( / );
                 REG_S = alloc_cons( REG_W, get_cdr(get_cdr(REG_S)));
                 REG_C = get_cdr(REG_C);
                 break;

      case REM:  REG_W = binary_op( % );
                 REG_S = alloc_cons( REG_W, get_cdr(get_cdr(REG_S)));
                 REG_C = get_cdr(REG_C);
                 break;


      case LEQ:  if( get_ivalue(get_car(get_cdr(REG_S))) <=
                     get_ivalue(get_car(REG_S)))
                   REG_S = alloc_cons(REG_T, get_cdr(get_cdr(REG_S)));
                 else
                   REG_S = alloc_cons(REG_F, get_cdr(get_cdr(REG_S)));
                 REG_C = get_cdr(REG_C);
                 break;

      case STOP: goto endcycle;


      case CAAR  : REG_S = unary_op( get_caar );
                   REG_C = get_cdr(REG_C);
                   break;

      case CADR  : REG_S = unary_op( get_cadr );
                   REG_C = get_cdr(REG_C);
                   break;

      case CDAR  : REG_S = unary_op( get_cdar );
                   REG_C = get_cdr(REG_C);
                   break;

      case CDDR  : REG_S = unary_op( get_cddr );
                   REG_C = get_cdr(REG_C);
                   break;

      case CAAAR : REG_S = unary_op( get_caaar );
                   REG_C = get_cdr(REG_C);
                   break;

      case CAADR : REG_S = unary_op( get_caadr );
                   REG_C = get_cdr(REG_C);
                   break;

      case CADAR : REG_S = unary_op( get_cadar );
                   REG_C = get_cdr(REG_C);
                   break;

      case CADDR : REG_S = unary_op( get_caddr );
                   REG_C = get_cdr(REG_C);
                   break;

      case CDAAR : REG_S = unary_op( get_cdaar );
                   REG_C = get_cdr(REG_C);
                   break;

      case CDADR : REG_S = unary_op( get_cdadr );
                   REG_C = get_cdr(REG_C);
                   break;

      case CDDAR : REG_S = unary_op( get_cddar );
                   REG_C = get_cdr(REG_C);
                   break;

      case CDDDR : REG_S = unary_op( get_cdddr );
                   REG_C = get_cdr(REG_C);
                   break;

      default    : fprintf( stderr, "Illegal instruction %ld\n",
                          get_ivalue( get_car( REG_C ) ) );
                 exit(-1);
    }
  }

endcycle:
  return( get_car( REG_S ) );
}