#pragma once

class CardBitmap {
private:
  static CBitmap  cardBitmap;
  static CDC      cardDC;

  CardBitmap() {
  }
  static void init();
public:
  static void paintCard(CDC &dc, int x, int y, int n);
  static void paintCard(CDC &dc, const CPoint &point, int n);

  static int getCardWidth();
  static int getCardHeight();
};
