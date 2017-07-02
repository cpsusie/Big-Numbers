#include "stdafx.h"

class diff {
private:
  int *m_save;
  int m_l1,m_l2;
  int &save(int i,int j) { return m_save[(m_l2+1)*i+j]; }
  int editdist(int i,int j);
public:
  String &m_s1,&m_s2;
  diff(String &s1, String &s2);
  int editdist();
  void dump();
};

void diff::dump() {
  Console::setCursorPos(0,0);
  for(int i = 0; i <= m_l1; i++)
    for(int j = 0; j <= m_l2; j++) {
      Console::setCursorPos(j*3,i);
      printf("%2d",save(i,j));
    }
}

diff::diff(String &s1, String &s2) : m_s1(s1) , m_s2(s2) {
  m_l1 = (int)s1.length();
  m_l2 = (int)s2.length();
  m_save = new int[(m_l1+1)*(m_l2+1)];
  for(int i = 0; i <= m_l1; i++)
    for(int j = 0; j <= m_l2; j++)
      save(i,j) = -1;
  for(int i = 0; i <= m_l1; i++)
    save(i,0) = i;
  for(int i = 0; i <= m_l2; i++)
    save(0,i) = i;
}

int diff::editdist(int i,int j) {
  if(save(i,j) != -1) return save(i,j);
  int res;
  if(m_s1[i-1] == m_s2[j-1])
    res = editdist(i-1,j-1);
  else {
    int m1 = editdist(i,j-1)+1;
    int m2 = editdist(i-1,j)+1;
    res = (m1 < m2) ? m1 : m2;
  }
  save(i,j) = res;
  return res;
}

int diff::editdist() {
/*
  for(int i = 0; i <= m_l1; i++)
    for(int j = 0; j <= m_l2; j++)
      editdist(i,j);
*/
  return editdist(m_l1,m_l2);
}

int main(int argc, char* argv[])
{
  String s1("cbabac");
  String s2("abcabba");
  s1 = "aaaaaa";
  s2 = "bbabbb";

  diff d(s1,s2);

  int res = d.editdist();
  d.dump();
  printf("\n");
  return 0;
}
