#include "stdafx.h"
#include <math.h>
#include <ctype.h>
#include <bpn.h>

static int screenwid,screenhei;
static setfillstyle(int style, int color) {}
static bar(int left, int top, int right, int bottom) {}
static void setcolor(int color) {}
static void line(int x0, int y0, int x1, int y1) {}
static int  getcolor() { return 0; }
#define SOLID_FILL 1
#define YELLOW     1
#define BLUE       2

#define unit_col( n, i ) ( (long)screenwid * ( (i) + 1 )/( (n) + 1 ) )
#define unit_row( n, i ) ( (long)screenhei * ( (i) + 1 )/( (n) + 1 ) )

static int weight_color( float w ) {
  if( w < -2.0  ) return(  1 );
/*
  if( w < -1.0  ) return(  3 );
  if( w >  1.0  ) return( 14 );
*/
  if( w >  2.0  ) return( 15 );
  return( 0 );
}

static void draw_unit(int left, int top, int width, int height, int color ) {
  setfillstyle( SOLID_FILL, color );
  bar( left, top, left + width - 1, top + height - 1 );
}

static void draw_layer_weights( bpnlayer *l, int fr, int tr ) {
  int    n = l->m_noutputs;
  int    m = l->m_ninputs;
  float *w;
  int    c;

  for( int i = 0; i < n; i++ ) {
    w = l->m_weights[i];
    for( int j = 0; j < m; j++ ) {
      if( (c = weight_color( w[j] ) ) == 0 )
        continue;
      setcolor( c );
      line( unit_col( n, i ), tr, unit_col( m, j ), fr );
    }
  }
}

static void bpn_draw_weights( bpn *t ) {
  int n = t->gethiddenlayercount();

  for( int i = 0; i < n; i++ )
    draw_layer_weights(
      t->gethiddenlayer(i), unit_row( n+2, i ), unit_row( n+2, i+1 ) );

  draw_layer_weights(
    t->getoutputlayer(), unit_row( n+2, n ), unit_row( n+2, n+1 ) );

}

static void bpn_draw_layer_units( int r, bpnlayer *l ) {
  int i;
  int n           = l->m_noutputs;
  float  *outputs = l->m_outputs;

  for( i = 0; i < n; i++ )
    draw_unit( unit_col( n, i ) - 5, r - 5, 10, 10,
               outputs[i] > 0.5 ? YELLOW : BLUE );
}

static void bpn_draw_units( bpn *t ) {
  int n = t->gethiddenlayercount();
  int oldc = getcolor();

  bpn_draw_layer_units(   unit_row( n+2,   0 ), t->getinputlayer() );

  for( int i = 0; i < n; i++ )
    bpn_draw_layer_units( unit_row( n+2, i+1 ), t->gethiddenlayer(i));

  bpn_draw_layer_units(   unit_row( n+2, n+1 ), t->getoutputlayer());

  setcolor( oldc );
}

static bpn_grafik bpn_grafics = { bpn_draw_weights, bpn_draw_units };
bpn_grafik *BPN_GRAFIK = &bpn_grafics;

