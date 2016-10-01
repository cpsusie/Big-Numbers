#pragma once

#define ORIGIN CPoint(0,0)

typedef enum {
  RESOURCE_BITMAP
 ,RESOURCE_JPEG
} ImageType;

class Image : public PixRect {
private:
  void loadBMP(int resId);
  void loadJPG(int resId);
  bool m_hasTransparentPixels;
  void paintImage(  PixRect &pr, const CPoint &dst, const CSize &size, const CPoint &src, double scale = 1.0, double rotation = 0) const;
  void paintImage(  HDC      dc, const CPoint &dst, const CSize &size, const CPoint &src, double scale = 1.0, double rotation = 0) const;
  void paintRotated(HDC      dc, const CPoint &dst, const CSize &size, const CPoint &src, double scale, double rotation) const;
public:
  Image(const CSize &size);
  Image(int resId, ImageType type = RESOURCE_BITMAP, bool transparentWhite = false);
  void makeWhiteTransparent();
  void makeOpaque();
  void paintImage(PixRect &pr, const CPoint &dst, double scale = 1.0, double rotation = 0) const;
  void paintImage(HDC      dc, const CPoint &dst, double scale = 1.0, double rotation = 0) const;
};

typedef CompactArray<Image*> ImageArray;
