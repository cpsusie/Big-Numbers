typedef union {
  struct {
    unsigned int isatom : 1;
    unsigned int isnumb : 1;
    unsigned int ismarked:1;
    unsigned int isalloc: 1;
  } bits;
  char x;
} cell_attr;

typedef char *symbol;
typedef long number;

typedef struct storage_cell {
  union {
    number ivalue;
    symbol svalue;
    struct {
      struct storage_cell *car, *cdr;
    } cons;
  } value;
  cell_attr attr;
} *pointer;

#define ISATOM(p)       ((p)->attr.bits.isatom   )
#define ISNUMB(p)       ((p)->attr.bits.isnumb   )
#define ISMARKED(p)     ((p)->attr.bits.ismarked )
#define ISALLOC(p)      ((p)->attr.bits.isalloc  )
#define INIT_ATTR(p)    ((p)->attr.x = 0         )

#define is_symbol(p)    ((p)->attr.bits.isatom && !(p)->attr.bits.isnumb)
#define is_number(p)    ((p)->attr.bits.isatom &&  (p)->attr.bits.isnumb)
#define is_cons(p)      (!(p)->attr.bits.isatom)
#define get_svalue(p)   ((p)->value.svalue)
#define get_ivalue(p)   ((p)->value.ivalue)
#define get_car(p)      ((p)->value.cons.car)
#define get_cdr(p)      ((p)->value.cons.cdr)
#define set_ivalue(p,n) ((p)->value.ivalue = (n))
#define set_svalue(p,s) ((p)->value.svalue = string_store(s))
#define set_car(p,c)    ((p)->value.cons.car = (c))
#define set_cdr(p,c)    ((p)->value.cons.cdr = (c))

typedef int boolean;

#define TRUE 1
#define FALSE 0

pointer alloc_number( number n );
pointer alloc_symbol( symbol s );
pointer alloc_cons(   pointer car, pointer cdr );
char   *string_store( char *s );

void    storage_init( void );
void    putexp(       pointer e );
pointer getexp(       void );
pointer getexplist(   void );
void    open_input(   char *fname );
void    open_output(  char *fname );

extern pointer REG_S, REG_E, REG_C, REG_D, REG_W, REG_T, REG_F, nil;

pointer execute( pointer fn, pointer args );
