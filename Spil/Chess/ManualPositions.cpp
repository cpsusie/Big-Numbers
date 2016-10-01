#include "stdafx.h"
#include <io.h>
#include "EndGameUtil.h"

#ifdef TABLEBASE_BUILDER

String AllManualPositions::getFileName() { // static 
  return getResourceFileName(_T("ManualPositions.txt"));
}

void AllManualPositions::clear() {
  m_endGamePositions.clear();
}

void AllManualPositions::load() {
  clear();

  const String fileName  = getFileName();
  FILE        *f         = NULL;
  int          lineCount = 0;
  try {
    if(access(fileName.cstr(),0) == 0) {
      FILE *f = FOPEN(fileName, _T("r"));
      char line[1000];
      while(FGETS(line, sizeof(line), f)) {
        lineCount++;
        const String name = strTrim(line);
        get(name).load(f, lineCount);
      }
      fclose(f);
      f = NULL;
    }
  } catch(Exception e) {
    verbose(_T("Error in line %s line %d:%s\n"), fileName.cstr(), lineCount, e.what());
    if(f != NULL) {
      fclose(f);
    }
    throw e;
  }
}

EndGameManualPositions &AllManualPositions::get(const String &name) {
  for(int i = 0; i < m_endGamePositions.size(); i++) {
    EndGameManualPositions &ep = m_endGamePositions[i];
    if(ep.getName().equalsIgnoreCase(name)) {
      return ep;
    }
  }
  m_endGamePositions.add(EndGameManualPositions(EndGameTablebase::getInstanceByName(name).getKeyDefinition()));
  return m_endGamePositions[i];
}


void AllManualPositions::save() {
  FILE *f = MKFOPEN(getFileName(),_T("w"));

  for(int i = 0; i < m_endGamePositions.size(); i++) {
    m_endGamePositions[i].save(f);
  }
  fclose(f);
}

/*
static void extractManualPositions(EndGameTablebase &t, EndGameManualPositions &mp) {
  _tprintf(_T("Extracting manual keys for %s\n"), t.getName().cstr());
  t.insertManualPositions();
  for(Iterator<EndGameKey> it = t.getIndex().getKeyIterator(); it.hasNext();) {
    mp.addKey(it.next());
  }
  t.unload();
}

void generateManualPositionFile() {
  const EndGameTablebaseList &tablebaseList = EndGameTablebase::getRegisteredEndGameTablebases();
  AllManualPositions tmp;
  tmp.clear();
  for(int i = 0; i < tablebaseList.size(); i++) {
    EndGameTablebase &tb = *tablebaseList[i];
    EndGameManualPositions &mp = tmp.get(tb.getName());
    extractManualPositions(tb, mp);
  }
  tmp.save();
}

*/

static int endGameKeyComparator3Men(const EndGameKey &k1, const EndGameKey &k2) {
  int c;
  if(c = k1.getPosition0() - k2.getPosition0()) return c;
  if(c = k1.getPosition1() - k2.getPosition1()) return c;
  if(c = k1.getPosition2() - k2.getPosition2()) return c;
  return (int)k1.getPlayerInTurn() - (int)k2.getPlayerInTurn();
}

static int endGameKeyComparator4Men(const EndGameKey &k1, const EndGameKey &k2) {
  int c;
  if(c = k1.getPosition0() - k2.getPosition0()) return c;
  if(c = k1.getPosition1() - k2.getPosition1()) return c;
  if(c = k1.getPosition2() - k2.getPosition2()) return c;
  if(c = k1.getPosition3() - k2.getPosition3()) return c;
  return (int)k1.getPlayerInTurn() - (int)k2.getPlayerInTurn();
}

static int endGameKeyComparator5Men(const EndGameKey &k1, const EndGameKey &k2) {
  int c;
  if(c = k1.getPosition0() - k2.getPosition0()) return c;
  if(c = k1.getPosition1() - k2.getPosition1()) return c;
  if(c = k1.getPosition2() - k2.getPosition2()) return c;
  if(c = k1.getPosition3() - k2.getPosition3()) return c;
  if(c = k1.getPosition4() - k2.getPosition4()) return c;
  return (int)k1.getPlayerInTurn() - (int)k2.getPlayerInTurn();
}

typedef int (*EndGameKeyComparator)(const EndGameKey &k1, const EndGameKey &key2);

static const EndGameKeyComparator comparators[] = {
  NULL
 ,NULL
 ,NULL
 ,endGameKeyComparator3Men
 ,endGameKeyComparator4Men
 ,endGameKeyComparator5Men
};

void EndGameManualPositions::save(FILE *f) {
  if(m_keySet.isEmpty()) {
    return;
  }
  verbose(_T("Saving manual positions for %s  \r"), getName().cstr());
  _ftprintf(f, _T("%s\n"), getName().cstr());
  _ftprintf(f, _T("  begin positions\n"));
  CompactArray<EndGameKey> tmp(m_keySet.size());
  for(EndGameKeyIterator it = getKeyIterator(); it.hasNext();) {
    tmp.add(it.next());
  }
  tmp.sort(comparators[m_keydef.getPieceCount()]);
  for(int i = 0; i < tmp.size(); i++) {
    _ftprintf(f, _T("    %s\n"), m_keydef.createInitKeyString(tmp[i]).cstr());
  }
  _ftprintf(f, _T("  end positions\n"));
}

EndGameManualPositions::EndGameManualPositions(const EndGameKeyDefinition &keydef)
: m_keydef(keydef)
{
}

EndGameKey EndGameManualPositions::createKey(const String &str) const {
  EndGameKey result;

  Tokenizer tok(str, _T(","));
  const String playerStr = tok.next();
  if(playerStr.equalsIgnoreCase(_T("whiteplayer"))) {
    result.setPlayerInTurn(WHITEPLAYER);
  } else if(playerStr.equalsIgnoreCase(_T("blackplayer"))) {
    result.setPlayerInTurn(BLACKPLAYER);
  } else {
    throwException(_T("Invalid player:%s"), playerStr.cstr());
  }

  for(int index = 0; tok.hasNext() && index < m_keydef.getPieceCount();) {
    const String posStr = tok.next();
    result.setPosition(index++, decodePosition(toLowerCase(posStr)));
  }
  if(index != m_keydef.getPieceCount() || tok.hasNext()) {
    throwException(_T("Wrong number of positions specified in %s:<%s>"), m_keydef.toString().cstr(), str.cstr());
  }
  return result;
}

bool EndGameManualPositions::addKey(const EndGameKey &key) {
  return m_keySet.add(key);
}

void EndGameManualPositions::load() {
  const String fileName  = AllManualPositions::getFileName();
  const String name      = getName();
  FILE        *f         = NULL;
  int          lineCount = 0;

  try {
    if(access(fileName.cstr(),0) == 0) {
      FILE *f = FOPEN(fileName, _T("r"));
      char line[100];
      while(FGETS(line, sizeof(line), f)) {
        lineCount++;
        const String tmp = strTrim(line);
        if(tmp == name) {
          load(f, lineCount);
          break;
        }
      }
      fclose(f);
    }
  } catch(Exception e) {
    verbose(_T("Error in %s line %d while loading %s:%s\n"), fileName.cstr(), lineCount, name.cstr(), e.what());
    if(f != NULL) {
      fclose(f);
    }
    throw e;
  }
}

void EndGameManualPositions::load(FILE *f, int &lineCount) {
  verbose(_T("Loading manual positions for %s    \n"), getName().cstr());
  char line[1000];
  FGETS(line, sizeof(line),f);
  lineCount++;
  strTrim(line);
  if(stricmp(line,_T("begin positions")) == 0) {
    while(FGETS(line, sizeof(line),f)) {
      lineCount++;
      strTrim(line);
      if(stricmp(line,_T("end positions")) == 0) {
        break;
      }
      addKey(createKey(line));
    }
  }
}

EndGameKeyIterator EndGameManualPositions::getKeyIterator() {
  return m_keySet.getIterator();
}

CompactArray<EndGameKey> EndGameManualPositions::readUndefinedKeysLog(const String &fileName) const {
  CompactArray<EndGameKey> result;
  FILE *f = NULL;
  try {
    f = FOPEN(fileName, _T("r"));
    char line[1000];
    int lineCount = 0;
    while(FGETS(line, sizeof(line), f)) {
      lineCount++;
      try {
        result.add(createKey(strTrim(line)));
      } catch(Exception e) {
        throwException(_T("Error in %s line %d:%s"), fileName.cstr(), lineCount, e.what());
      }
    }
    fclose(f);
    return result;
  } catch(...) {
    if(f) {
      fclose(f);
    }
    throw;
  }
}

#endif
