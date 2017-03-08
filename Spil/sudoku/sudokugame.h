#pragma once

#include <Array.h>
#include <TinyBitSet.h>

class FieldMatrix {
  BYTE m_field[9][9];
  void init();
public:
  FieldMatrix();
  inline int get(int r, int c) const {
    return m_field[r][c];
  }
  inline void put(int r, int c, int v) {
    m_field[r][c] = v;
  }
};

class FieldPos {
private:
  short m_r,m_c,m_sq; // which row, column and square does this field belongs to
public:
  FieldPos(short r, short c);
  FieldPos() {}; // must be there for this class to be used as elementtype in CompactArray
  inline int getRow() const {
    return m_r;
  }
  inline int getColumn() const {
    return m_c;
  }
  inline int getSquare() const {
    return m_sq;
  }
};

typedef enum {
  VERY_EASY,
  EASY,
  MEDIUM,
  DIFFICULT,
  VERY_DIFFICULT
} GameLevel;

class Game {
private:
  BitSet16               m_rows[9],m_cols[9],m_sqr[9];
  CompactArray<FieldPos> m_freeFields;
  FieldMatrix            m_matrix;
  FieldMatrix            m_solution;
  int                    m_solutionCount;
  int                    m_tryCounter;
  int                    m_maxSolutionCount; // terminate scan (true)  when m_solutionCount exceeds this
  int                    m_maxTryCount;      // terminate scan (false) when m_tryCounter exceeds this
  void clearSets();
  void initSets(const FieldMatrix &m);
  void tryField(int field);
  void tryRandomValue(int field);
  int  minTries(const FieldMatrix &gf);
  CompactArray<FieldPos> findBestToRemove(const FieldMatrix &gf);
  void refineGame(int maxaAgTries);
public:
  const FieldMatrix &getMatrix()    const { return m_matrix;        }
  const FieldMatrix &getSolution()  const { return m_solution;      }
  void  generate(GameLevel level);
  bool  solve(const FieldMatrix &m, int maxSolutionCount = 1);
  int   getTryCount()               const { return m_tryCounter;    }
  int   getSolutionCount()          const { return m_solutionCount; }
  int   getFreeFieldCount();
  friend class FieldOrderCmp;
};

