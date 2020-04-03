#pragma once

/* stormacs.h */

#define max_blocks 10000
#define Nobj 10000
#ifdef DEBUG
#undef DEBUG
#endif

#define DEBUG -7
#define CHECK_OVERSHOOT 1

extern FILE *DFILE;

#define DEBS(qq)  {if (DEBUG>qq) {
#define EDEBS }}
#define DEBOUT DFILE
#define DEB(ll,mes)  DEBS(ll) fprintf(DEBOUT,#mes "\n");fflush(DEBOUT); EDEBS
#define DEBEXP(ll,exp) DEBS(ll) fprintf(DEBOUT,#exp "=%G\n", (double) exp); fflush(DEBOUT); EDEBS
#define DEBTR(ll) DEBS(ll) fprintf(DEBOUT, __FILE__ " line %d \n" ,__LINE__);fflush(DEBOUT); EDEBS

#define warning(lev, x)                                                         \
{ static int messcount;                                                         \
  if (++messcount<=10) {DEB(lev,x) DEBTR(lev)}                                  \
  if (messcount==10) DEB(lev, consider yourself warned)                         \
}                                                                               \

#define SBCHECK(s) /*                                                           \
{ double Sb_check=0;                                                            \
  int i;                                                                        \
  for(i = 1; i < cdim; i++) {                                                   \
    if(s->neigh[i].basis) {                                                     \
      Sb_check+=s->neigh[i].basis->sqb;                                         \
    }                                                                           \
  }                                                                             \
  if((float)(Sb_check - s->Sb) !=0.0) {                                         \
    DEBTR DEB(bad Sb);                                                          \
    DEBEXP(s->Sb) DEBEXP(Sb_check);                                             \
    print_simplex(s);                                                           \
    exit(1);                                                                    \
  }                                                                             \
}*/                                                                             \

#define STORAGE_GLOBALS(X)              \
                                        \
extern size_t X##_size;                 \
extern X *X##_list;                     \
extern X *new_block_##X(int);           \
extern void flush_##X##_blocks(void);   \
void free_##X##_storage(void);          \


#define INCP(X,p,k) ((X*) ( (char*)p + (k) * X##_size)) /* portability? */


#define STORAGE(X)                                              \
                                                                \
size_t  X##_size;                                               \
X       * X##_list = 0;                                         \
                                                                \
X * new_block_##X (int make_blocks)                             \
{       int i;                                                  \
        static  X * X##_block_table[max_blocks];                \
                X *xlm, *xbt;                                   \
        static int num_##X##_blocks;                            \
/* long before; */                                              \
        if (make_blocks) {                                      \
/* DEBEXP(-10, num_##X##_blocks) */                             \
                Assert(num_##X##_blocks<max_blocks);            \
/* before = _memfree(0);*/                                      \
 DEB(0, before) DEBEXP(0, Nobj * X##_size)                      \
                                                                \
                xbt = X##_block_table[num_##X##_blocks++] =     \
                        ( X *)malloc(Nobj * X##_size);          \
                        memset(xbt,0,Nobj * X##_size);          \
/* DEB(0, after) DEBEXP(0, 8*_howbig((long*)xbt))*/             \
                if (!xbt) {                                     \
                        DEBEXP(-10,num_##X##_blocks)            \
/* memstats(2); */                                              \
                }                                               \
                Assert(xbt);                                    \
                                                                \
                xlm = INCP( X ,xbt,Nobj);                       \
                for (i=0;i<Nobj; i++) {                         \
                        xlm = INCP( X ,xlm,(-1));               \
                        xlm->next = X##_list;                   \
                        X##_list = xlm;                         \
                }                                               \
                                                                \
                return X##_list;                                \
        };                                                      \
                                                                \
        for (i=0; i<num_##X##_blocks; i++)                      \
                free( X##_block_table[i]);                      \
        num_##X##_blocks = 0;                                   \
        X##_list = 0;                                           \
        return 0;                                               \
}                                                               \
                                                                \
void free_##X##_storage(void) {new_block_##X (0);}              \


#ifdef TRACE_MEMORY
#define TRACENEWL(p,type,var,method,line) debugLog("FETCH:%p:%s(%d):type:%s,var:%s\n",p,method,line,type,var);
#else
#define TRACENEWL(p,type,var,method,line) 
#endif


#define NEWL(X,p)                                               \
{                                                               \
        p = X##_list ? X##_list : new_block_##X(1);             \
        Assert(p);                                              \
        X##_list = p->next;                                     \
        TRACENEWL(p,#X,#p,__FUNCTION__,__LINE__)                \
}                                                               \


#ifdef TRACE_MEMORY
#define TRACENEWLRC(p,type,var,method,line) debugLog("create:%p:refCount=%d:%s(%d),type:%s,var:%s\n" \
                                                    ,p,p->ref_count, method,line,type,var);
#else
#define TRACENEWLRC(p,type,var,method,line) 
#endif


#define NEWLRC(X,p)                                             \
{                                                               \
        p = X##_list ? X##_list : new_block_##X(1);             \
        Assert(p);                                              \
        X##_list = p->next;                                     \
        p->ref_count = 1;                                       \
        TRACENEWLRC(p,#X,#p,__FUNCTION__,__LINE__)              \
}                                                               \


#define FREEL(X,p)                                              \
{                                                               \
        memset((p),0,X##_size);                                 \
        (p)->next = X##_list;                                   \
        X##_list = p;                                           \
}                                                               \


#define dec_ref(X,v)    {if ((v) && --(v)->ref_count == 0) FREEL(X,(v));}
#define inc_ref(X,v)    {if (v) v->ref_count++;}
#define NULLIFY(X,v)    {dec_ref(X,v); v = NULL;}



#define mod_refs(op,s)                                  \
{                                                       \
        int i;                                          \
        neighbor *mrsn;                                 \
                                                        \
        for (i=-1,mrsn=s->neigh-1;i<cdim;i++,mrsn++)    \
                op##_ref(basis_s, mrsn->basis);         \
}

#define free_simp(s)                                    \
{       mod_refs(dec,s);                                \
        FREEL(basis_s,s->normal);                       \
        FREEL(simplex, s);                              \
}                                                       \


#define copy_simp(new,s)                                \
{       NEWL(simplex,new);                              \
        memcpy(new,s,simplex_size);                     \
        mod_refs(inc,s);                                \
}                                                       \



#if 0
STORAGE_GLOBALS(type)
STORAGE(type)
NEWL(type,xxx)
FREEL(type,xxx)
dec_ref(type,xxxx)
inc_ref(type,xxxx)
NULLIFY(type,xxxx)
#endif
