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

#ifdef TRACE_MEMORY
#define TRACENEWL( p,type,var,method,line)   debugLog("NEW:%p:%s(%d):type:%s,var:%s\n"                        \
                                                     ,p,method,line,type,var);
#define TRACEFREEL(p,type,var,method,line)   debugLog("DELETE:%p:%s(%d):type:%s,var:%s\n"                     \
                                                     ,p,method,line,type,var);

#define TRACENEWLRC( p,type,var,method,line) debugLog("REFCNT:create:%p:refCount=%d:%s(%d),type:%s,var:%s\n"  \
                                                     ,p,p->ref_count, method,line,type,var);
#define TRACEADDREF( p,type,var,method,line) debugLog("REFCNT:addref:%p:refCount=%d:%s(%d),type:%s,var:%s\n"  \
                                                     ,p,p->ref_count, method,line,type,var);
#define TRACERELEASE(p,type,var,method,line) debugLog("REFCNT:release:%p:refCount=%d:%s(%d),type:%s,var:%s\n" \
                                                     ,p,p->ref_count, method,line,type,var);

#else
#define TRACENEWL(   p,type,var,method,line)
#define TRACEFREEL(  p,type,var,method,line)
#define TRACENEWLRC( p,type,var,method,line)
#define TRACEADDREF( p,type,var,method,line)
#define TRACERELEASE(p,type,var,method,line)

#endif

#define USE_TEMPLATEPOOL

#ifndef USE_TEMPLATEPOOL

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
{                                                               \
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
                for (int i=0;i<Nobj; i++) {                     \
                        xlm = INCP( X ,xlm,(-1));               \
                        xlm->next = X##_list;                   \
                        X##_list = xlm;                         \
                }                                               \
                                                                \
                return X##_list;                                \
        };                                                      \
                                                                \
        for(int i=0; i<num_##X##_blocks; i++) {                 \
          xbt = X##_block_table[i];                             \
          memset(xbt,0,Nobj * X##_size);                        \
        }                                                       \
        X##_list = 0;                                           \
        for(int i = 0; i < num_##X##_blocks; i++) {             \
          xbt = X##_block_table[i];                             \
          xlm = INCP(X, xbt, Nobj);                             \
          for(int j = 0; j < Nobj; j++) {                       \
            xlm = INCP(X, xlm, (-1));                           \
            xlm->next = X##_list;                               \
            X##_list = xlm;                                     \
          }                                                     \
        }                                                       \
        return 0;                                               \
}                                                               \
                                                                \
void free_##X##_storage(void) {new_block_##X (0);}              \


#define NEWL(X,p)                                               \
{                                                               \
        p = X##_list ? X##_list : new_block_##X(1);             \
        Assert(p);                                              \
        X##_list = p->next;                                     \
        TRACENEWL(p,#X,#p,__FUNCTION__,__LINE__)                \
}                                                               \

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

#else // USE_TEMPLATEPOOL

template<typename X> class ElementPool {
private:
  const string    m_typeName;
  size_t          m_Xsize;
  fixedarray<X*>  m_blockTable;
  uint            m_blockCount;
  X              *m_list;

  inline size_t blockSize() const {
    return Nobj * m_Xsize;
  }
  void initBlock(X *block) {
    size_t bsize = blockSize();
    memset(block, 0, bsize);
    X *xlm = (X*)(((char*)block + bsize));
    for(int i = 0; i < Nobj; i++) {
      (char*&)xlm -= m_Xsize;
      xlm->next = m_list;
      m_list = xlm;
    }
  }

  void newBlock() {
    Assert(m_Xsize  != 0);
    Assert(m_blockCount < max_blocks);
    X *xbt = (X*)malloc(blockSize());
    Assert(xbt);
    m_blockTable[m_blockCount++] = xbt;
    initBlock(xbt);
  }

public:
  ElementPool(const string &name)
    : m_typeName(name)
    , m_Xsize(0)
    , m_blockTable(max_blocks)
    , m_blockCount(0)
    , m_list(NULL)
  {
  }
  ~ElementPool() {
    for(uint i = 0; i < m_blockCount; i++) {
      free(m_blockTable[i]);
    }
  }
  void releaseAll() {
    m_list = NULL;
    for(uint i = 0; i < m_blockCount; i++) {
      initBlock(m_blockTable[i]);
    }
  }
  X *fetch(const X *src = NULL) {
    if(m_list == NULL) newBlock();
    X *result = m_list;
    m_list = m_list->next;
    if(src) {
      memcpy(result, src, m_Xsize);
    }
    return result;
  }
  void release(X *p) {
    memset(p, 0, m_Xsize);
    p->next = m_list;
    m_list = p;
  }
  void setXsize(size_t Xsize) {
    if(Xsize != m_Xsize) {
      Assert(m_blockCount == 0);
      m_Xsize = Xsize;
    }
  }
  inline size_t getXsize() const {
    return m_Xsize;
  }
};

#define STORAGE_GLOBALS(X)                                \
typedef ElementPool<X> X##ElementPool;                    \
extern X##ElementPool X##pool;                            \
inline void free_##X##_storage() {                        \
  X##pool.releaseAll();                                   \
}

#define STORAGE(X)                                        \
X##ElementPool X##pool(#X);

#define NEWL(X,p,...) {                                   \
  p = X##pool.fetch(__VA_ARGS__);                         \
  Assert(p);                                              \
}                                                         \

#define FREEL(X,p) {                                      \
  X##pool.release(p);                                     \
}

#define NEWLRC(X,p) {                                     \
  p = X##pool.fetch();                                    \
  Assert(p);                                              \
  p->ref_count = 1;                                       \
}

#define FREELRC(X,p) {                                    \
  X##pool.release(p);                                     \
}

#define dec_ref(X,p) {                                    \
  if((p) && --(p)->ref_count == 0) {                      \
    FREELRC(X, (p));                                      \
  }                                                       \
}

#define inc_ref(X,p) {                                    \
  if(p) {                                                 \
    (p)->ref_count++;                                     \
  }                                                       \
}

#define NULLIFY(X,p) {                                    \
  dec_ref(X,p);                                           \
  p = NULL;                                               \
}

#define mod_refs(op,s) {                                  \
  int i;                                                  \
  neighbor *mrsn;                                         \
  for(i = -1, mrsn=s->neigh-1;i<cdim;i++,mrsn++) {        \
    op##_ref(basis_s, mrsn->basis);                       \
  }                                                       \
}

#define free_simp(s) {                                    \
  mod_refs(dec,s);                                        \
  FREEL(basis_s,s->normal);                               \
  FREEL(simplex, s);                                      \
}                                                         \

#define copy_simp(dst,s) {                                \
  NEWL(simplex,dst,s);                                    \
  mod_refs(inc,s);                                        \
}                                                         \

#define basis_s_size basis_spool.getXsize()
#endif // USE_TEMPLATEPOOL

#if 0
STORAGE_GLOBALS(type)
STORAGE(type)
NEWL(type,xxx)
FREEL(type,xxx)
dec_ref(type,xxxx)
inc_ref(type,xxxx)
NULLIFY(type,xxxx)
#endif
