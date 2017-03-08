#include "stdafx.h"

static inline int squareIndex(int r, int c) {
  return (r/3) * 3 + c/3;
}

FieldPos::FieldPos(short r, short c) { 
  m_r  = r; 
  m_c  = c;
  m_sq = squareIndex(r, c);
}

void FieldMatrix::init() {
  memset(m_field, 0, sizeof(m_field));
}

FieldMatrix::FieldMatrix() {
  init();
}

void Game::clearSets() {
  for(int i = 0; i < 9; i++) {
    m_rows[i].clear();
    m_cols[i].clear();
    m_sqr[i].clear();
  }
  m_freeFields.clear();
}

void Game::initSets(const FieldMatrix &m) {
  clearSets();
  for(int r = 0; r < 9; r++) {
    for(int c = 0; c < 9; c++) {
      int v = m.get(r,c);
      if(v > 0) {
        if(m_rows[r].contains(v) || m_cols[c].contains(v) || m_sqr[squareIndex(r,c)].contains(v)) {
          throw false;
        }
        m_rows[r].add(v);
        m_cols[c].add(v);
        m_sqr[squareIndex(r,c)].add(v);
      } else {
        m_freeFields.add(FieldPos(r,c));
      }
    }
  }
}

int Game::getFreeFieldCount() {
  initSets(m_matrix);
  return (int)m_freeFields.size();
}

void Game::tryField(int field) {
  if(field == m_freeFields.size()) {
    if(m_solutionCount++ == 0) {
      m_solution = m_matrix; // only save the first solution found
    }
    if(m_solutionCount >= m_maxSolutionCount) {
      throw true; // terminate the scan
    }
  } else {
    const FieldPos &p = m_freeFields[field];
    const int r  = p.getRow();
    const int c  = p.getColumn();
    const int sq = p.getSquare();
    for(int v = 1; v <= 9; v++) {
      if(!m_rows[r].contains(v) && !m_cols[c].contains(v) && !m_sqr[sq].contains(v)) {
        m_matrix.put(r,c,v);
        m_rows[r].add(v);
        m_cols[c].add(v);
        m_sqr[sq].add(v);

        if((m_tryCounter++ > m_maxTryCount) && (m_maxTryCount > 0)) {
          throw false;
        }

        tryField(field+1);

        m_matrix.put(r,c,0);
        m_rows[r].remove(v);
        m_cols[c].remove(v);
        m_sqr[sq].remove(v);
      }
    }
  }
}

bool Game::solve(const FieldMatrix &m, int maxSolutionCount) {
  try {
    m_matrix           = m;
    m_solutionCount    = 0;
    m_maxSolutionCount = maxSolutionCount;
    m_tryCounter       = 0;
    m_maxTryCount      = -1;
    initSets(m_matrix);
    tryField(0);
  } catch(bool b) {
    return b;
  }
  return m_solutionCount > 0;
}

typedef enum {
  CMP_FIRST      = 0,
  CMP_ROW        = CMP_FIRST,
  CMP_COL   ,
  CMP_SQR   ,
  CMP_ROWSQR,
  CMP_COLSQR,
  CMP_SQRROW,
  CMP_SQRCOL,
  CMP_LAST       = CMP_SQRCOL
} CompareSequence;

class FieldOrderComparator : public Comparator<FieldPos> {
  CompactIntArray m_permut;
public:
  void randomOrder();
  CompareSequence m_cs;
  bool            m_reverse;
  FieldOrderComparator(CompareSequence cs, bool reverse);
  int compare(const FieldPos &f1, const FieldPos &f2);
  AbstractComparator *clone() const {
    return new FieldOrderComparator(m_cs,m_reverse);
  }
};

FieldOrderComparator::FieldOrderComparator(CompareSequence cs, bool reverse) {
  m_cs      = cs;
  m_reverse = reverse;
  for(int i = 0; i < 9; i++) {
    m_permut.add(i);
  }
}

void FieldOrderComparator::randomOrder() {
  m_permut.shuffle();
}

int FieldOrderComparator::compare(const FieldPos &f1, const FieldPos &f2) {
  int c;
  switch(m_cs) {
  case CMP_ROW   :
    c = m_permut[f1.getRow()] - m_permut[f2.getRow()];
    break;
  case CMP_COL   :
    c = m_permut[f1.getColumn()] - m_permut[f2.getColumn()];
    break;
  case CMP_SQR   :
    c = m_permut[f1.getSquare()] - m_permut[f2.getSquare()];
    break;
  case CMP_ROWSQR:
    c = f1.getRow() - f2.getRow();
    if(c == 0) {
      c = m_permut[f1.getSquare()] - m_permut[f2.getSquare()];
    }
    break;
  case CMP_COLSQR:
    c = m_permut[f1.getColumn()] - m_permut[f2.getColumn()];
    if(c == 0) {
      c = m_permut[f1.getSquare()] - m_permut[f2.getSquare()];
    }
    break;
  case CMP_SQRROW:
    c = m_permut[f1.getSquare()] - m_permut[f2.getSquare()];
    if(c == 0) {
      c = m_permut[f1.getRow()] - m_permut[f2.getRow()];
    }
    break;
  case CMP_SQRCOL:
    c = m_permut[f1.getSquare()] - m_permut[f2.getSquare()];
    if(c == 0) {
      c = m_permut[f1.getColumn()] - m_permut[f2.getColumn()];
    }
    break;
  }

  return m_reverse ? -c : c;
}

static int intcmp1(const int &i1, const int &i2) { return i1 - i2; }

static int gameCount = 0;
static FILE *fff;

int Game::minTries(const FieldMatrix &m) {
  CompactIntArray samples;
  for(CompareSequence cs = CMP_FIRST; cs < CMP_LAST; cs = (CompareSequence)(cs + 1)) {
    for(int rev = 0; rev < 2; rev++) {
      m_matrix           = m;
      m_solutionCount    = 0;
      m_maxSolutionCount = 1;
      m_tryCounter       = 0;
      initSets(m_matrix);
      m_freeFields.shuffle();
      m_maxTryCount      = 2000;
      FieldOrderComparator fc(cs,rev ? true : false);
      fc.randomOrder();
      m_freeFields.sort(fc);
      try {
        tryField(0);
      } catch(bool) {
        samples.add(m_tryCounter);
      }
    }
  }
//  for(unsigned int i = 0; i < samples.size(); i++)
//    fprintf(fff,_T("%d %d\n"),m_freeFields.size(),samples[i]);
  samples.sort(intcmp1);

  return (samples[0] + samples[1] + samples[2]) / 3;
}

class FieldPosScore : public FieldPos {
public:
  int m_score;
  FieldPosScore(short r, short c, int score) : FieldPos(r,c) { m_score = score; }
};

static int scpreCmp(const FieldPosScore &fps1, const FieldPosScore &fps2) {
  return fps2.m_score - fps1.m_score;
}

static int intCmp(const void *i1, const void *i2) { return *(int*)i1 - *(int*)i2; }

CompactArray<FieldPos> Game::findBestToRemove(const FieldMatrix &m) {
  m_matrix = m;
  initSets(m);

  int rc[9],cc[9],qc[9];
  for(int i = 0; i < 9; i++) {
    rc[i] = m_rows[i].size();
    cc[i] = m_cols[i].size();
    qc[i] = m_sqr[ i].size();
  }
  Array<FieldPosScore> fp;
  for(int r = 0; r < 9; r++) {
    for(int c = 0; c < 9; c++) {
      if(m_matrix.get(r,c) != 0) {
        int se[3];
        se[0] = rc[r];
        se[1] = cc[c];
        se[2] = qc[squareIndex(r,c)];
        quickSort(se,3,sizeof(se[0]),intCmp);
        fp.add(FieldPosScore(r,c,se[1] + se[2]));
      }
    }
  }
  fp.shuffle();
  fp.sort(scpreCmp);
  CompactArray<FieldPos> result;
  for(size_t i = 0; i < fp.size(); i++) {
    result.add(fp[i]);
  }
  return result;
}

void Game::refineGame(int maxAvgTries) {
  FieldMatrix m = m_matrix;

//    fprintf(fff,_T("[%d,%d]->%d\n"),fp[i].m_r,fp[i].m_c,fp[i].m_score);
#define MAXREMCOUNT 200
  for(int remCount = 0; remCount < MAXREMCOUNT;) {
    Game g1;
    CompactArray<FieldPos> fp = g1.findBestToRemove(m);
    for(size_t f = 0; f < fp.size(); f++) {
      if(remCount++ >= MAXREMCOUNT) {
        m_matrix = m;
        return;
      }
      const int r    = fp[f].getRow();
      const int c    = fp[f].getColumn();
      const int save = m.get(r,c);

      m.put(r,c,0);
      Game g2;
      g2.solve(m,2);
      if(g2.getSolutionCount() > 1) {
        m.put(r,c,save);
      } else {
        const int d = g2.minTries(m);
//        fprintf(fff,_T("%d %d\n"),g2.m_freeFields.size(),d);
        if(d >= maxAvgTries) {
          m_matrix = m;
//          fclose(fff);
          return;
        } else {
          break;
        }
      }
    }
  }
}

void Game::tryRandomValue(int field) {
  if(field == 9*9) {
    throw true;
  } else {
    const int r  = field / 9;
    const int c  = field % 9;
    const int sq = squareIndex(r, c);
    int i;
    CompactIntArray t;
    for(i = 1; i <= 9; i++) {
      t.add(i);
    }
    t.shuffle();
    for(i = 0; i < 9; i++) {
      int v = t[i];
      if(!m_rows[r].contains(v) && !m_cols[c].contains(v) && !m_sqr[sq].contains(v)) {
        m_matrix.put(r,c,v);
        m_rows[r].add(v);
        m_cols[c].add(v);
        m_sqr[sq].add(v);

        tryRandomValue(field+1);

        m_matrix.put(r,c,0);
        m_rows[r].remove(v);
        m_cols[c].remove(v);
        m_sqr[sq].remove(v);
      }
    }
  }
}

void Game::generate(GameLevel level) {
  int m;
  switch(level) {
  case VERY_EASY     : m = 90  ; break;
  case EASY          : m = 300 ; break;
  case MEDIUM        : m = 800 ; break;
  case DIFFICULT     : m = 1200; break;
  case VERY_DIFFICULT: m = 1500; break;
  }
  try {
    clearSets();
    tryRandomValue(0);
  } catch(bool) {

//    TCHAR fname[100];
//    sprintf(fname,_T("c:\\temp\\sudoku%04d.txt"),gameCount++);
//    fff = fopen(fname,_T("w"));

    refineGame(m);

//    fclose(fff);

  }
}
