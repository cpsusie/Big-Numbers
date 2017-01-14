#pragma once

#include "GameTypes.h"

class CardButton : public CBitmapButton {
private:
  Card m_card;
protected:
  CDialog *m_parent;
public:
  CardButton(CDialog *parent, Card card);
  void Draw();
  void SetPosition(int x, int y, bool redraw = true);
  void setCard(Card card);
  void Create(int x, int y);
  void DrawItem(LPDRAWITEMSTRUCT lpdrawitemstruct);
  Card getCard() const {
    return m_card;
  }
  void SetAbove(CWnd *under);
};

class MyCardButton : public CardButton {
private:
  bool m_marked;
  int  m_pos;
public:
  MyCardButton(CDialog *parent, Card card);
  void SetPosition(int pos, bool redraw = true);
  void mark();
  void unmark();
  bool isMarked() {
    return m_marked;
  }
};
