#include "stdafx.h"

/*------------------- KeyFile FUNCTIONS ---------------- */

KeyFile::KeyFile(const Database &db, const String &fileName, DbFileMode mode, bool systemFile) : DbFile(db, fileName, mode, systemFile) {
  init();
};

KeyFile::KeyFile(const String &fileName, DbFileMode mode, LogFile *lf) : DbFile(fileName, mode, lf) {
  init();
}

void KeyFile::init() {
  KeyFileHeader header;
  readHead(header, false);

  if(strcmp(header.m_version, dbVersion) != 0) {
    const String programVersion = dbVersion;
    const String fileVersion    = header.m_version;
    throwSqlError(SQL_WRONGDBVERSION,_T("Wrong database version for file %s. File verison=<%s>, program version=<%s>")
                                    ,getName().cstr(),fileVersion.cstr(), programVersion.cstr());
  }
  SETCONST(m_keydef, header.m_keydef, KeyFileDefinition);
  ((KeyPageInfo&)m_pageInfo).init(m_keydef.getSize());
}

void KeyFile::create(const String &name, const KeyFileDefinition &keydef) {
  if(keydef.getKeyFieldCount() < 1) {
    throwSqlError(SQL_INVALID_KEYDEF,_T("Invalid keydefinition on %s. Keyfieldcount=%d. Must be >= 1"), name.cstr(), keydef.getKeyFieldCount());
  }

/* Check the keysize against pagesize */
  KeyPageInfo pageInfo(keydef.getSize());

  KeyFileHeader header;
  header.init(keydef);

  DbFile::create(name);
  DbFile file(name,DBFMODE_READWRITE, NULL);

  try {
    file.write(0,&header, sizeof(header));
  } catch(sqlca) {
    DbFile::destroy(name);
    throw;
  }
}

void KeyFile::truncate() { // remove all data!!! be careful
  KeyFileHeader header;
  readHead(header, false);
  const KeyFileDefinition keydef = header.m_keydef;
  header.init(keydef);
  DbFile::truncate();
  writeHead(header);
}

void KeyFileHeader::init(const KeyFileDefinition &keydef) {
  memset(this,0,sizeof(KeyFileHeader));

  strcpy(m_version,dbVersion);
  m_root         = DB_NULLADDR;
  m_freeList     = DB_NULLADDR;
  m_freeListSize = 0;
  m_last         = DB_NULLADDR;
  m_keydef       = keydef;
  m_keyCount     = 0;
}

void KeyFile::dumpPage(const KeyPage &page, FILE *f) const {
  _ftprintf(f,_T("pagedump:\n"));
  for(UINT i = 1; i <= page.getItemCount(); i++) {
    KeyPageItem ki;
    page.getItem(i, ki);
    m_keydef.fprintf(f,ki.m_key);
    _ftprintf(f,_T(" child:%s\n"),toString(ki.m_child).cstr());
  }
}

void KeyFile::readPage(KeyPageAddr addr, KeyPage &page) {
  read(getPageOffset(addr), &page, m_pageInfo.m_pageSize);
}

void KeyFile::writePage(KeyPageAddr addr, const KeyPage &page) {
  write(getPageOffset(addr), &page, m_pageInfo.m_pageSize);
}

void KeyFile::logPage(KeyPageAddr addr, const KeyPage &page) {
  if(!isBackLogged()) return;
  appendLog(getPageOffset(addr), &page, m_pageInfo.m_pageSize);
}

void KeyFile::readHead(KeyFileHeader &header, bool log) {
  read(0, &header, sizeof(KeyFileHeader));

  if(log && isBackLogged()) {
    appendLog(0, &header, sizeof(KeyFileHeader));
  }
}

void KeyFile::writeHead(const KeyFileHeader &header) {
  write(0, &header, sizeof(KeyFileHeader));
}

KeyPageAddr KeyFile::getFreeList() {
  KeyFileHeader header;
  readHead(header, false);
  return header.m_freeList;
}

void KeyFile::setFreeList(KeyPageAddr addr, int dsize) {
  KeyFileHeader header;
  readHead(header, true);
  header.m_freeList     =  addr;
  header.m_freeListSize += dsize;
  if(header.m_freeListSize < 0) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyFile::setFreeList:m_header.m_freeListSize<0. (=%d)"),header.m_freeListSize);
  } else if(header.m_freeListSize == 0 && header.m_freeList != DB_NULLADDR) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyFile::setFreeList:m_header.m_freeListSize=0 and m_freeList != NULLADDR."));
  } else if(header.m_freeListSize > 0 && header.m_freeList == DB_NULLADDR) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyFile::setFreeList:m_header.m_freeListSize>0 and m_freeList == NULLADDR."));
  }

  writeHead(header);
}

KeyPageAddr KeyFile::getLast() {
  KeyFileHeader header;
  readHead(header, false);
  return header.m_last;
}

void KeyFile::setLast(KeyPageAddr addr) {
  KeyFileHeader header;
  readHead(header, true);
  header.m_last = addr;
  writeHead(header);
}

void KeyFile::setRoot(KeyPageAddr addr, int delta) {
  KeyFileHeader header;
  readHead(header, false);
  header.m_root      = addr;
  header.m_keyCount += delta;
  writeHead(header);
}

UINT64 KeyFile::getKeyCount() {
  KeyFileHeader header;
  readHead(header, false);
  return header.m_keyCount;
}

KeyPageAddr KeyFile::fetchNewPage(bool leafPage) {
  KeyPageAddr addr;

  KeyPage page(m_pageInfo);
  KeyPageAddr freeList = getFreeList();
  if(freeList == DB_NULLADDR) {
    try {
      addr = getLast() + 1;
      page.init();
      page.setLeafPage(leafPage);
      writePage(addr, page);
      setLast(addr);
    } catch(sqlca) {
      return DB_NULLADDR;
    }
  } else {
    try {
      addr = freeList;
      readPage(freeList, page);
      logPage( freeList, page);
      freeList = page.getNextFree();
      setFreeList(freeList,-1);
      page.init();
      page.setLeafPage(leafPage);
      writePage(addr, page);
    } catch(sqlca) {
      return DB_NULLADDR;
    }
  }
  return addr;
}

void KeyFile::releasePage(KeyPageAddr addr) {
  KeyPage page(m_pageInfo);
  page.setNextFree(getFreeList());
  writePage(addr, page);
  setFreeList(addr,1);
}

const KeyFileDefinition &KeyFile::getDefinition() const {
  return m_keydef;
}

UINT KeyFile::getPageSize() const {
  return m_pageInfo.m_pageSize;
}

UINT KeyFile::getFreePageCount() {
  KeyFileHeader header;
  readHead(header,false);
  return header.m_freeListSize;
}

UINT KeyFile::getPageCount() {
  return (UINT)((getSize() - sizeof(KeyFileHeader)) / m_pageInfo.m_pageSize - getFreePageCount());
}

class KeyPageCounter : public KeyFilePageScanner {
private:
  CompactIntArray &m_counters;
public:
  KeyPageCounter(CompactIntArray &counters) : m_counters(counters) {
  }
  bool handlePage(KeyPageAddr addr, int level, const KeyPage &page);
};

bool KeyPageCounter::handlePage(KeyPageAddr addr, int level, const KeyPage &page) {
  if(level < (int)m_counters.size()-1) {
    m_counters[level+1] += page.getItemCount() + 1;
  }
  return true;
}

KeyFileInfo KeyFile::getInfo() {
  KeyFileHeader header;
  readHead(header,false);
  KeyPage page(header.m_keydef.getSize());

  KeyFileInfo result;
  result.m_header          = header;
  result.m_size            = getSize();
  result.m_pageInfo        = m_pageInfo;
  if(header.m_root == DB_NULLADDR) {
    result.m_rootPageAddress   = 0;
    result.m_rootPageItemCount = 0;
    result.m_treeHeight        = 0;
  } else {
    result.m_rootPageAddress = getPageOffset(header.m_root);
    KeyPage page(m_keydef.getSize());
    readPage(header.m_root,page);
    result.m_rootPageItemCount = page.getItemCount();
    result.m_treeHeight  = 1;
    for(KeyPageAddr addr = page.getChild(0); addr; addr = page.getChild(0)) {
      readPage(addr,page);
      result.m_treeHeight++;
    }
  }
  result.m_rootPageIndex = header.m_root;
  result.m_dataPageCount = (UINT)((result.m_size - sizeof(header)) / m_pageInfo.m_pageSize - header.m_freeListSize);

  UINT nonLeafPageCount = 0;
  UINT leafPageCount    = 0;
  CompactIntArray  &pageCountPerLevel = result.m_pageCountPerLevel;

  if(result.m_treeHeight == 0) { // file is empty
    pageCountPerLevel.add(0);
  } else {
    for(UINT l = 0; l < result.m_treeHeight; l++) {
      pageCountPerLevel.add(0);
    }
    pageCountPerLevel.first() = 1;
    KeyPageCounter pageCounter(pageCountPerLevel);
    pageScan(result.m_header.m_root, 0, pageCounter, max(0, result.m_treeHeight-3));
    if(result.m_treeHeight >= 3) {
      for(size_t i = 0; i < pageCountPerLevel.size()-1; i++) {
        nonLeafPageCount += pageCountPerLevel[i];
      }
      leafPageCount = pageCountPerLevel.last() = result.m_dataPageCount - nonLeafPageCount;
    }
  }

  const ULONG totalPageCount = result.m_dataPageCount + header.m_freeListSize;
  if(totalPageCount == 0) {
    result.m_utilizationRate = 100;
  } else {
    const INT64 maxKeys = (INT64)nonLeafPageCount * m_pageInfo.m_maxItemCount + (INT64)leafPageCount * m_pageInfo.m_maxKeyCount;
    result.m_utilizationRate = (double)((INT64)header.m_keyCount) / maxKeys * 100;
  }
  return result;
}

#define Kb 1024.0
#define Mb (Kb*Kb)
#define Gb (Kb*Mb)
#define Tb (Kb*Gb)

static String formatFileSize(INT64 size) {
  if(size < Kb) {
    return format(_T("%I64u bytes"),size);
  } else if(size < Mb) {
    return format(_T("%.2lf Kb"),(double)size/Kb);
  } else if(size < Gb) {
    return format(_T("%.2lf Mb"),(double)size/Mb);
  } else if(size < Tb) {
    return format(_T("%.2lf Gb"),(double)size/Gb);
  } else {
    return format(_T("%.2lf Tb"),(double)size/Tb);
  }
}

String KeyFileInfo::toString() const {
  String result;
  result = format(_T("KeyFileInfo:\n"
                     "  Version              : %s\n"
                     "  Head size            : %d bytes\n"
                     "  Page size            : %s bytes\n"
                     "  Key size             : %d bytes\n"
                     "  PageItem size        : %d bytes\n"
                     "  Max. keys/leafpage   : %d\n"
                     "  Max. keys/nonleafpage: %d\n"
                     "  File size            : %s (%s bytes)\n"
                     "  Key count            : %s\n"
                     "  Tree height          : %d\n"
                     "  Data pages           : %s\n"
                     "  Free pages           : %s\n"
                     "  Utilization rate     : %.2lf%%\n"
                     "  Rootpage.keyCount    : %d\n"
                     "  Rootpage index       : %s\n")
                  ,m_header.m_version
                  ,sizeof(m_header)
                  ,format1000(m_pageInfo.m_pageSize).cstr()
                  ,m_header.m_keydef.getSize()
                  ,m_pageInfo.m_itemSize
                  ,m_pageInfo.m_maxKeyCount
                  ,m_pageInfo.m_maxItemCount
                  ,formatFileSize(m_size).cstr(), format1000(m_size).cstr()
                  ,format1000(m_header.m_keyCount).cstr()
                  ,m_treeHeight
                  ,format1000(m_dataPageCount).cstr()
                  ,format1000(m_header.m_freeListSize).cstr()
                  ,m_utilizationRate
                  ,m_rootPageItemCount
                  ,format1000(m_rootPageIndex).cstr()
                  );

  if(m_rootPageAddress != DB_NULLADDR) {
    result += format(_T("  Rootpage offset      : %s\n"), format1000(m_rootPageAddress).cstr());
  }
  return result;
}
