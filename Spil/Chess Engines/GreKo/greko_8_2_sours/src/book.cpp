//  GREKO Chess Engine
//  (c) 2002-2011 Vladimir Medvedev <vrm@bk.ru>
//  http://greko.110mb.com

//  book.cpp: opening book
//  modified: 1-Aug-2011

#include "book.h"
#include "notation.h"
#include "utils.h"

Book g_book;

struct MoveAndValue
{
  MoveAndValue(Move mv, int value) : m_mv(mv), m_value(value) {}
  ~MoveAndValue() {}

  bool operator< (const MoveAndValue& m) const
  { 
    return m_value > m.m_value; // reverse order!
  }

  Move m_mv;
  int  m_value;
};
////////////////////////////////////////////////////////////////////////////////

Move Book::GetMove(const Position& pos, char* buf)
{
  strcpy(buf, "");

  // check to avoid lines like 1. e4 e5 2. Nf3 a6 3. Bb5 Nc6?
  map<U64, int>::iterator it0 = m_data.find(pos.Hash());
  if (it0 == m_data.end())
    return 0;

  m_pos = pos;
  vector<MoveAndValue> x;

  MoveList mvlist;
  mvlist.GenAllMoves(m_pos);

  int sumVal = 0;
  for (int i = 0; i < mvlist.Size(); ++i)
  {
    Move mv = mvlist[i].m_mv;
    if (m_pos.MakeMove(mv))
    {
      map<U64, int>::iterator it = m_data.find(m_pos.Hash());
      if (it != m_data.end() && it->second > 0)
      {
        x.push_back(MoveAndValue(mv, it->second));
        sumVal += it->second;
      }
      m_pos.UnmakeMove();
    }
  }

  if (sumVal > 0)
  {
    sort(x.begin(), x.end());
    char mvstr[32];
    for (size_t i = 0; i < x.size(); ++i)
    {
      MoveToStrShort(x[i].m_mv, m_pos, mvstr);
      strcat(buf, mvstr);
      sprintf(mvstr, " %d%c", int(100 * x[i].m_value / sumVal), '%');
      strcat(buf, mvstr);
      if (i != x.size() - 1)
        strcat(buf, ", ");
    }

    int N = int(Rand64() % sumVal);
    for (size_t i = 0; i < x.size(); ++i)
    {
      N -= x[i].m_value;
      if (N <= 0)
        return x[i].m_mv;
    }
  }

  return 0;
}
////////////////////////////////////////////////////////////////////////////////

bool Book::Import(char* s)
{ 
  char* path = strtok(s, " ");
  if (!path)
    return false;

  FILE* src = fopen(path, "rt");
  if (!src)
  {
    printf("can't open %s\n", path);
    return false;
  }

  int maxPly = 20;
  char* tk = strtok(NULL, " ");
  if (tk)
    maxPly = atoi(tk);

  out("maxPly = %d\n", maxPly);

  bool addColor[2] = {true, true};

  tk = strtok(NULL, " ");
  if (tk)
  {
    if (tk[0] == 'w')
    {
      addColor[BLACK] = false;
      out("white's moves only\n");
    }
    else if (tk[0] == 'b')
    {
      addColor[WHITE] = false;
      out("black's moves only\n");
    }
  }
  
  Position startpos;
  startpos.SetInitial();

  int nGames = 0;
  char buf[4096];

  while (fgets(buf, sizeof(buf), src))
  {
    if (strlen(buf) < 2)
      continue;
    if (buf[0] == '[')
      continue;

    tk = strtok(buf, " ");
    while (tk)
    {
      if (strstr(tk, "1.") == tk || !strcmp(tk, "1"))
      {
        m_pos = startpos;
        ++nGames;
        printf("Games: %d, nodes: %d\r", nGames, m_data.size());
      }

      char* p1 = strstr(tk, ".");
      if (p1)
        tk = p1 + 1;

      if (m_pos.Ply() >= maxPly)
      {
        tk = strtok(NULL, " ");
        continue;
      }

      Move mv = StrToMove(tk, m_pos);
      if (mv)
      {
        m_pos.MakeMove(mv);

        if (addColor[m_pos.Side() ^ 1])
          ++m_data[m_pos.Hash()];
        else
          m_data[m_pos.Hash()] += 0;
      }
      tk = strtok(NULL, " ");
    }
  }

  m_data.insert(pair<U64, int>(startpos.Hash(), 1));

  printf("Games: %d, nodes: %d\n", nGames, m_data.size());
  fclose(src);
  return true;
}
////////////////////////////////////////////////////////////////////////////////

void Book::Init()
{
  Clean();

  if (Load("book.bin"))
    ;
  else
  { 
    char buf[256];
    FILE* src = fopen("book.txt", "rt");

    if (src)
    {
      out("reading book.txt...\n");
      while (fgets(buf, sizeof(buf), src))
      {
        m_pos.SetInitial();
        ProcessLine(buf);
      }
      fclose (src);
      out("book.txt: %d nodes\n", m_data.size());
    }
    else
    {
      out("book.txt not found\n");
    }
  } 
}
////////////////////////////////////////////////////////////////////////////////

bool Book::Load(const char* path)
{
  FILE* srcBin = fopen(path, "rb");
  if (srcBin)
  {
    U64 hash;
    int be;

    while (fread(&hash, sizeof(U64), 1, srcBin))
    {
      fread(&be, sizeof(int), 1, srcBin);
      m_data[hash] += be;
    }
    fclose(srcBin);
    out("%s: %d nodes\n", path, m_data.size());
    return true;
  }
  else
  {
    out("can't open %s\n", path);
    return false;
  }
}
////////////////////////////////////////////////////////////////////////////////

void Book::ProcessLine(const char* str)
{
  char* buf = new char[strlen(str) + 1];
  strcpy(buf, str);
  char* tk = strtok(buf, " ");
  while (tk)
  {
    Move mv = StrToMove(tk, m_pos);
    if (mv)
    {
      m_pos.MakeMove(mv);
      ++m_data[m_pos.Hash()];
    }
    tk = strtok(NULL, " ");
  }
  delete[] buf;
}
////////////////////////////////////////////////////////////////////////////////

bool Book::Save(const char* path)
{
  FILE* dest = fopen(path, "wb");
  if (dest)
  {
    if (!m_data.empty())
    {
      out("writing %s...\n", path);
    }
    map<U64, int>::iterator it;
    for (it = m_data.begin(); it != m_data.end(); ++it)
    {
      U64 hash = it->first;
      int be = it->second;
      fwrite(&hash, sizeof(U64), 1, dest);
      fwrite(&be, sizeof(int), 1, dest);
    }
    fclose(dest);
    return true;
  }

  return false;
}
////////////////////////////////////////////////////////////////////////////////

