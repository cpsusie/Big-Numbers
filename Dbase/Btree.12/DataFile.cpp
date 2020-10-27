#include "stdafx.h"

/*
#define DEBUG_FREELIST
*/

typedef struct {
  UINT m_recSize : 31;
  UINT m_inUse   : 1;
} DataPage;

#define DATAPAGESIZE sizeof(DataPage)

DataFile::DataFile(const Database &db, const String &fileName, DbFileMode mode, bool systemFile)
: DbFile(db,fileName,mode,systemFile)
{
  init();
};

DataFile::DataFile(const String &fileName, DbFileMode mode, LogFile *lf)
: DbFile(fileName,mode,lf)
{
  init();
}

void DataFile::init() {
  DataFileHeader header;
  readHead(header, false);
  header.checkFileVersion(getName());
}

void DataFile::create(const String &name) {
  DataFileHeader header;
  header.init();

  DbFile::create(name);
  DbFile file(name, DBFMODE_READWRITE, nullptr);

  try {
    file.write(0,&header, sizeof(header));
  } catch(sqlca) {
    DbFile::destroy(name);
    throw;
  }
}

void DataFile::truncate() {
  DataFileHeader header;
  readHead(header, false);
  header.init();
  DbFile::truncate();
  writeHead(header);
}

void DataFileHeader::init() {
  memset(this,0,sizeof(DataFileHeader));

  setFileVersion();
  m_freeList = DB_NULLADDR;
  m_freeTree = DB_NULLADDR;
  m_end      = sizeof(DataFileHeader);
}

void DataFile::writeHead(const DataFileHeader &dh) {
//  _tprintf(_T("sizeof(DATAPAGESIZE:%d\n"),DATAPAGESIZE);
  write(0, &dh, sizeof(DataFileHeader));
}

void DataFile::readHead(DataFileHeader &dh, bool log) const {
  read(0, &dh, sizeof(DataFileHeader));
  if(log && isBackLogged()) {
    appendLog(0, &dh, sizeof(DataFileHeader));
  }
}

int DataFile::freeKeyCmp(const FreeKey &key1, const FreeKey &key2) const {
  return key1.m_recSize - key2.m_recSize;
}

int DataFile::freeKeyCmpIns( const FreeKey &key1, const FreeKey &key2) const {
  if(key1.m_recSize != key2.m_recSize) {
    return freeKeyCmp(key1,key2);
  } else {
    return sign((INT64)key1.m_addr - (INT64)key2.m_addr);
  }
}

//#define FREEPAGE_ITEMCOUNT(page) (page).m
//#define FREEPAGE_P0(page)        (page).p0

#if defined(DEBUG_FREELIST)

void DataFile::listFreeList() {
  _tprintf(_T("freelist:("));
  for(DbAddr hh = dh.freelist; hh != DB_NULLADDR; hh = page.p0) {
    _tprintf(_T("%s "),toString(hh).cstr());
    FreePage page;
    read(hh,&page,sizeof(page));
  }
  _tprintf(_T(")\n"));
}
#endif

DbAddr DataFile::dataFetchFreePage() {
  DbAddr addr;

  if(getFreeList() == DB_NULLADDR) {
    addr = end() + 1;
    m_dh.m_end = addr + sizeof(FreePage);
  } else {
    try {
      FreePage page;
      read(getFreeList(),&page,sizeof(FreePage));
      if(isBackLogged()) {
        appendLog(getFreeList(),&page,sizeof(FreePage));
      }
      addr = getFreeList();
      m_dh.m_freeList = page.getChild(0);
    } catch(sqlca) {
      return DB_NULLADDR;
    }
  }
#if defined(DEBUG_FREELIST)
  _tprintf(_T("fetched FreePage addr %s"),toString(addr).cstr());
  listFreeList();
#endif
  return addr;
}

void DataFile::dataReleaseFreePage(DbAddr addr) {
  FreePage page;

  page.init();
  page.setChild(0,getFreeList());
  write(addr, &page, sizeof(page)); // page has been logged
  m_dh.m_freeList = addr;
#if defined(DEBUG_FREELIST)
  _tprintf(_T("released FreePage addr %s "),toString(addr).cstr());
  listFreeList();
#endif
}

void DataFile::freePageInsert(DbAddr        a  ,
                              FreeKey      &key,
                              bool         &h  ,
                              FreePageItem &v  ) {
  if(a == DB_NULLADDR) {
    h = true;
    v.m_child = DB_NULLADDR;
    v.m_key = key;
  } else {
    FreePage apage;
    read(a,&apage,sizeof(apage));

    int l = 1;
    int r = apage.getItemCount() + 1;
    while(l<r) {
      const int m = (l+r)/2;
      if(freeKeyCmpIns(apage.getKey(m),key) <= 0) {
        l = m + 1;
      } else {
        r = m;
      }
    }
    r--;
    if(r > 0 && freeKeyCmpIns(apage.getKey(r),key)==0) { // found DUPKEY
      throwSqlError(SQL_DBCORRUPTED,_T("Datafile <%s> corrupted. (dupkey in freelist)"),getName().cstr()); // something is quit qrong
    }

    FreePageItem u;
    freePageInsert(apage.getChild(r), key, h, u);

    if(h) { // insert u to the right of item[r]

      if(isBackLogged()) {
        appendLog(a, &apage, sizeof(apage));
      }

      if(apage.getItemCount() < FREEPAGEMAXCOUNT) { // check for pageoverflow
        h = false;
        apage.insertItem(r+1, u);
      } else { // overflow. split page a into a,b and pass middle item to v
        DbAddr b = dataFetchFreePage();
        if(b == DB_NULLADDR) {
          return;
        }

        FreePage bpage;
        bpage.init();
        bpage.setItemCount(FREEPAGEHALFSIZE); // will receive this amount of elements

        if(r <= FREEPAGEHALFSIZE) {
          if(r == FREEPAGEHALFSIZE) {
            v = u;
          } else { // r < FREEPAGE_HALFSIZE
            v = apage.getItem(FREEPAGEHALFSIZE);
            apage.copyItems(r+2,FREEPAGEHALFSIZE,apage,r+1);
            apage.setItem(r+1,u);
          }
          bpage.copyItems(1,FREEPAGEHALFSIZE,apage,FREEPAGEHALFSIZE+1); // copy upper half of a to lower half of b
        } else { // r > halfSize. insert in right page
          r -= FREEPAGEHALFSIZE;
          v = apage.getItem(FREEPAGEHALFSIZE+1);
          bpage.copyItems(1,r-1,apage,FREEPAGEHALFSIZE+2);
          bpage.setItem(r,u);

          bpage.copyItems(r+1,FREEPAGEHALFSIZE,apage,FREEPAGEHALFSIZE+r+1);
        }
        apage.setItemCount(FREEPAGEHALFSIZE);
        bpage.setChild(0,v.m_child);
        v.m_child = b;

        write(b, &bpage, sizeof(bpage));
      }
      write(a, &apage, sizeof(apage));
    }
  }
}

void DataFile::freePageUnderflow(DbAddr    a ,
                                 FreePage &c ,
                                 int       s ,
                                 bool     &h ) {
  FreePage apage;
  FreePage bpage;

  int mc = c.getItemCount();
  if(s < mc) {
    s++;
    DbAddr b = c.getChild(s);
    read(b, &bpage, sizeof(bpage));
    if(isBackLogged()) {
      appendLog(b, &bpage, sizeof(bpage));
    }
    read(a, &apage, sizeof(apage));
    if(isBackLogged()) {
      appendLog(a, &apage, sizeof(apage));
    }
    int mb = bpage.getItemCount();
    int k  = (mb-FREEPAGEHALFSIZE+1)/2;
                                       //  ITEM(a,n) = ITEM(c,s);
    if(k>0) { // move k items from b to a
      apage.setItemCount(FREEPAGEHALFSIZE-1+k);
      apage.setItem(FREEPAGEHALFSIZE,c.getItem(s));
      apage.setChild(FREEPAGEHALFSIZE,bpage.getChild(0));
      apage.copyItems(FREEPAGEHALFSIZE+1,FREEPAGEHALFSIZE-1+k,bpage,1);
      c.setItem(s,bpage.getItem(k));
      c.setChild(s,b);
      bpage.setChild(0,bpage.getChild(k));
      mb -= k;
      bpage.copyItems(1,mb,bpage,k+1);
      bpage.setItemCount(mb);
      h = false;

      write(a, &apage, sizeof(apage));
      write(b, &bpage, sizeof(bpage));
    } else { // k == 0. merge pages a and b
      apage.setItemCount(FREEPAGEMAXCOUNT);
      apage.setItem(FREEPAGEHALFSIZE,c.getItem(s));
      apage.setChild( FREEPAGEHALFSIZE,bpage.getChild(0));
      apage.copyItems(FREEPAGEHALFSIZE+1,FREEPAGEMAXCOUNT,bpage,1);
      c.copyItems(s,mc-1,c,s+1);
      c.setItemCount(mc-1);
      h = (mc <= (int)FREEPAGEHALFSIZE);

      write(a, &apage, sizeof(apage));

      dataReleaseFreePage( b );
    }
  } else { // s >= mc. b := page to the left of a
    DbAddr b = c.getChild(s-1);

    read(b, &bpage, sizeof(bpage));
    if(isBackLogged()) {
      appendLog(b, &bpage, sizeof(bpage));
    }
    read(a, &apage, sizeof(apage));
    if(isBackLogged()) {
      appendLog(a, &apage, sizeof(apage));
    }

    int mb = bpage.getItemCount() + 1;
    int k = (mb-FREEPAGEHALFSIZE)/2;
    if(k > 0) { // move k items from page b to a
      apage.copyItems(k + 1, FREEPAGEHALFSIZE - 1 + k, apage, 1);
      apage.setItem(k,c.getItem(s));
      apage.setChild(k,apage.getChild(0));
      mb -= k;
      apage.copyItems(1, k - 1, bpage, mb + 1);
      apage.setChild(0,bpage.getChild(mb));
      c.setItem(s,bpage.getItem(mb));
      c.setChild(s,a);
      bpage.setItemCount(mb-1);
      apage.setItemCount(FREEPAGEHALFSIZE-1+k);
      h = false;

      write(a, &apage, sizeof(apage));
      write(b, &bpage, sizeof(bpage));
    } else { // k == 0. merge pages a and b
      bpage.setItemCount(FREEPAGEMAXCOUNT);
      bpage.setItem(mb,c.getItem(s));
      bpage.setChild(mb,apage.getChild(0));
      bpage.copyItems(mb + 1, mb + FREEPAGEHALFSIZE - 1, apage, 1);
      c.setItemCount(mc - 1);
      h = (mc <= (int)FREEPAGEHALFSIZE);

      dataReleaseFreePage(a);
      write(b, &bpage, sizeof(bpage));
    }
  }
}

void DataFile::freePageDel(DbAddr    p ,
                           FreePage &a ,
                           int       r ,
                           bool     &h ) {
  FreePage ppage;
  read(p, &ppage, sizeof(ppage));
  DbAddr q = ppage.getChild(ppage.getItemCount());
  if(q != DB_NULLADDR) {
    freePageDel(q,a,r,h);
    if(h) {
      if(isBackLogged()) {
        appendLog(p, &ppage, sizeof(ppage));
      }
      freePageUnderflow(q, ppage, ppage.getItemCount(),h);
      write(p, &ppage, sizeof(ppage));
    }
  } else {
    // we have reached the bottom. page.child[page.itemcount] = a.child[r]
    if(isBackLogged()) {
      appendLog(p, &ppage,sizeof(ppage));
    }

    ppage.setChild(ppage.getItemCount(),a.getChild(r));
    a.setItem(r,ppage.getLastItem());
    ppage.itemCountDecr();
    h = (ppage.getItemCount() < FREEPAGEHALFSIZE);
    write(p, &ppage,sizeof(ppage));
  }
}

void DataFile::freePageDelete( DbAddr   a  ,
                               FreeKey &key,
                               bool    &h  ) {
  if(a == DB_NULLADDR) {
    h = false;
    return;
  } else {
    FreePage apage;
    read(a, &apage, sizeof(apage));
    UINT l = 1;
    UINT r = apage.getItemCount() + 1;  // binary search
    while(l < r) {
      const UINT m = (l+r)/2;
      if(freeKeyCmp(apage.getKey(m),key) < 0) {
        l = m + 1;
      } else {
        r = m;
      }
    }
    DbAddr q = apage.getChild(r-1);
    if(r <= apage.getItemCount() && freeKeyCmp(apage.getKey(r), key ) == 0 ) { // found, now delete
      if(isBackLogged()) {
        appendLog(a, &apage, sizeof(apage));
      }

      key = apage.getKey(r); // return the found addr

      if(q == DB_NULLADDR) { // a is a terminal page
        apage.removeItem(r);
        h = (apage.getItemCount() < FREEPAGEHALFSIZE);
      } else {
        freePageDel(q, apage, r, h);
        if(h) {
          freePageUnderflow(q, apage, r-1, h);
        }
      }
    } else {
      freePageDelete(q, key, h);
      if(h) {
        if(isBackLogged()) {
          appendLog(a, &apage, sizeof(apage));
        }
        freePageUnderflow(q, apage, r-1, h);
      }
    }
    write(a, &apage, sizeof(apage));
  }
}

void DataFile::freeTreeInsert(DbAddr addr, ULONG recSize) {
  bool         h;
  FreePageItem u;
  FreePage     rootPage;
  FreeKey      key;

  key.m_recSize = recSize;
  key.m_addr    = addr;

  freePageInsert(getFreeTree(), key, h, u);

  if(h) {
    DbAddr q = getFreeTree();
    rootPage.init();
    rootPage.setItemCount(1);
    rootPage.setChild(0,q);
    rootPage.setItem(1,u);

    m_dh.m_freeTree = dataFetchFreePage();
    if(getFreeTree() == DB_NULLADDR) {
      throwSqlError(SQL_DBCORRUPTED,_T("Datafile <%s> corrupted"),getName().cstr());
    }
    write(getFreeTree(), &rootPage, sizeof(rootPage));
  }
}

DbAddr DataFile::freeTreeSearchDelete(ULONG recSize) {
  bool     h;
  FreePage rootPage;
  FreeKey  key;

  key.m_recSize = recSize;     // key
  key.m_addr    = DB_NULLADDR;

  try {
    freePageDelete(getFreeTree(),key,h);
  } catch(sqlca) {
    return DB_NULLADDR;
  }
  if(key.m_addr == DB_NULLADDR) {
    return DB_NULLADDR;
  }

  if(h) { // base page size reduced
    try {
      read(getFreeTree(),&rootPage,sizeof(rootPage));
      if(isBackLogged()) {
        appendLog(getFreeTree(),&rootPage,sizeof(rootPage));
      }
    } catch(sqlca) {
      return DB_NULLADDR;
    }
    if(rootPage.getItemCount() == 0) { // deallocate rootPage
      DbAddr q = getFreeTree();
      m_dh.m_freeTree = rootPage.getChild(0);
      try {
        dataReleaseFreePage(q);
      } catch(sqlca) {
        return DB_NULLADDR;
      }
    }
  }
  return key.m_addr;
}

DbAddr DataFile::insert(const void *rec, ULONG recSize) {
  DbAddr   newAddr;
  DataPage newPage;
  bool     updateHead = false;

  readHead(m_dh, true);

  newPage.m_recSize = recSize;
  if(newPage.m_recSize != recSize) {
    throwSqlError(SQL_INVALID_RECSIZE,_T("Recordsize %lu too big"),recSize);
  }

  if(m_dh.m_freeTree == DB_NULLADDR) {
    newAddr    = m_dh.m_end + 1;
    m_dh.m_end = newAddr + DATAPAGESIZE + recSize;
    updateHead = true;
  } else { // try in freetree
    DbAddr pageaddr = freeTreeSearchDelete(recSize);
    if(pageaddr == DB_NULLADDR) { // none found in freetree
      newAddr    = m_dh.m_end + 1;
      m_dh.m_end = newAddr + DATAPAGESIZE + recSize;
      updateHead = true;
    } else { // use the one found in freetree
      newAddr    = pageaddr;
      read(pageaddr,&newPage,sizeof(newPage));
      if(isBackLogged()) {
        appendLog(pageaddr,&newPage,sizeof(newPage));
      }
      if(newPage.m_inUse) {
        throwSqlError(SQL_DBCORRUPTED,_T("Datafile <%s> corrupted"),getName().cstr());
      }
    }
  }

  // now write the new page on newAddr
  newPage.m_recSize  = recSize;
  newPage.m_inUse    = true;
  write(newAddr, &newPage, DATAPAGESIZE);
  DbAddr dataAddr = newAddr + DATAPAGESIZE;
  write(dataAddr, rec, recSize);

  if(updateHead) {
    writeHead(m_dh);
  }

  return newAddr;
}

void DataFile::remove(DbAddr addr) {
  DataPage page;

  readHead(m_dh, true);

  if(addr < sizeof(DataFileHeader) || addr > m_dh.m_end) {
    throwSqlError(SQL_INVALID_ADDRESS,_T("Trying to delete %d of <%s>"),addr,getName().cstr());
  }

  read(addr, &page, DATAPAGESIZE);

  if(!page.m_inUse) {
    throwSqlError(SQL_INVALID_ADDRESS,_T("Datapage in <%s> is not inuse"),getName().cstr());
  }
  if(isBackLogged()) {
    appendLog(addr, &page, DATAPAGESIZE);
  }

  freeTreeInsert(addr, page.m_recSize);

  page.m_inUse = false;
  write(addr, &page, DATAPAGESIZE);
  writeHead(m_dh);
}

void DataFile::update(DbAddr addr, const void *rec) {
  DbAddr dataAddr = addr + DATAPAGESIZE;
  DataPage page;

  read(addr, &page, DATAPAGESIZE);
  if(!page.m_inUse) {
    throwSqlError(SQL_INVALID_ADDRESS,_T("Datapage in <%s> is not inuse"),getName().cstr());
  }
  if(page.m_recSize == 0) {
    return; // no need to do anything
  }
  if(!isBackLogged()) {
    write(dataAddr,rec,page.m_recSize);
  } else {
    char *old = new char[page.m_recSize];
    try {
      read(dataAddr,old,page.m_recSize);
      appendLog(addr, old,page.m_recSize);
      write(dataAddr,rec,page.m_recSize);
    } catch(...) {
      delete[] old;
      throw;
    }
    delete[] old;
  }
}

void DataFile::readRecord(DbAddr addr, void *rec, ULONG maxSize) {
  DataPage page;

  read(addr, &page, DATAPAGESIZE);
  DbAddr dataAddr = addr + DATAPAGESIZE;
  if(!page.m_inUse) {
    throwSqlError(SQL_INVALID_ADDRESS,_T("Datapage in <%s> is not inuse"),getName().cstr());
  }
  if(page.m_recSize > maxSize) {
    throwSqlError(SQL_INVALID_RECSIZE,_T("Recordsize too big in <%s>. Recordsize=%d. Max size=%d"),getName().cstr(),page.m_recSize,maxSize);
  }
  read(dataAddr, rec, page.m_recSize);
}

void DataFile::readVarChar(DbAddr addr, varchar &vchar) const {
  DataPage page;

  read(addr, &page, DATAPAGESIZE);
  DbAddr dataAddr = addr + DATAPAGESIZE;
  if(!page.m_inUse) {
    throwSqlError(SQL_INVALID_ADDRESS,_T("Datapage in <%s> is not inuse"),getName().cstr());
  }

  varchar tmp(page.m_recSize);
  if(page.m_recSize == 0) {
    return; // no need to do anything
  }

  read(dataAddr, tmp.data(), page.m_recSize);
  vchar = tmp;
}

void DataFile::freePageDump(DbAddr addr, FreePage &page, int level, FILE *f) const {
  _ftprintf(f,_T("%*.*s level %d addr %s itemcount:%d P0:%s\n")
             ,level,level,_T(" ")
             ,level,toString(addr).cstr()
             ,page.getItemCount()
             ,toString(page.getChild(0)).cstr());

  for(UINT i = 1; i <= page.getItemCount();i++) {
    const FreePageItem &p = page.getItem(i);
    _ftprintf(f,_T("%*.*s  item %u [size:%u,addr:%s,child:%s]\n")
               ,level,level,_T(" ")
               ,i
               ,p.m_key.m_recSize
               ,toString(p.m_key.m_addr).cstr()
               ,toString(p.m_child).cstr());

    DataPage dataPage;

    read(p.m_key.m_addr,&dataPage,DATAPAGESIZE);
    if(dataPage.m_inUse) {
      _ftprintf(f,_T("ERROR!!!! Datapage at %s in <%s> freetree but is also inuse !!\n")
                 ,toString(p.m_key.m_addr).cstr(),getName().cstr());
    }
    if(dataPage.m_recSize != p.m_key.m_recSize) {
      _ftprintf(f,_T("ERROR!!!! Datapage.recsize at %s is %d but key.recsize is %d\n")
                 ,toString(p.m_key.m_addr).cstr(),dataPage.m_recSize,p.m_key.m_recSize);
    }
  }
}

void DataFile::freeAddrDump( DbAddr addr, int level, FILE *f) const {
  FreePage page;
  if(addr == DB_NULLADDR) return;
  read(addr,&page,sizeof(page));
  freePageDump(addr,page,level,f);

  for(UINT i = 0; i <= page.getItemCount(); i++) {
    freeAddrDump(page.getChild(i),level+1,f);
  }
}

void DataFile::freeTreeDump( FILE *f ) const {
  DataFileHeader dh;

  readHead(dh, false);
  _ftprintf(f,_T("freelist:("));
  FreePage page;
  for(DbAddr addr = dh.m_freeList; addr != DB_NULLADDR; addr = page.getChild(0)) {
    read(addr, &page, sizeof(page));
    _ftprintf(f,_T("[%s next:%s] "),toString(addr).cstr(),toString(page.getChild(0)).cstr());
  }
  _ftprintf(f,_T(")\nend:%s freetree:%s\n"),toString(dh.m_end).cstr(),toString(dh.m_freeTree).cstr());
  freeAddrDump(dh.m_freeTree, 1, f);
}
