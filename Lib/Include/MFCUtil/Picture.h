#pragma once

#include <afxwin.h>         
#include <ocidl.h>        
#include <MyString.h>

typedef enum {
  PFT_BMP
 ,PFT_DIB
 ,PFT_GIF
 ,PFT_ICO
 ,PFT_CUR
 ,PFT_JPG
 ,PFT_WMF
 ,PFT_PNG
 ,PFT_TIFF
 ,PFT_UNKNOWN
} PictureFormatType;

class CPicture {
private:
	void freePictureData();
	void loadPictureData(const BYTE *pBuffer, int nSize);
	IPicture *m_IPicture; // same as LPPICTURE (typedef IPicture __RPC_FAR *LPPICTURE)
	CSize     m_size;     // size in pixels ignore what current device context uses)
	LONG      m_weight;   // size of the image object in bytes (file or resource)
  bool      m_hasAlpha;
  void copyPictureData(const CPicture &src);
  void initPictureData();
  void setSize();
  static HICON loadIcon(  const String &fileName);
  static HICON loadCursor(const String &fileName);
  void loadPicture(       const ByteArray &bytes);
  void createPictureFromIcon(HICON icon);
  void createPictureFromBitmap(HBITMAP bitmap);
  void loadAsFormat(const ByteArray &bytes, PictureFormatType ft);
  void loadAsIcon(  const String &fileName);
  void loadAsCursor(const String &fileName);
  void load(const ByteArray &bytes, int firstIndex=-1);
public:
	CPicture();
  CPicture(const CPicture &src);
  CPicture(HBITMAP bm);
 ~CPicture();
  CPicture &operator=(const CPicture &src);
  operator HBITMAP() const;
  static PictureFormatType getFormatTypeFromFileName(const String &fileName);
  CPicture &load(ByteInputStream &in);
  CPicture &load(const String &name);
	CPicture &loadFromResource(int resId, const String &typeName);
  void unload();
  bool isLoaded() const {
    return m_IPicture != NULL;
  }
  bool hasAlpha() const {
    return m_hasAlpha;
  }
	void saveAsBitmap(const String &name);
  void show(HDC hdc) const;
	void show(HDC hdc, const CRect &dstRect) const;
  void show(HDC hdc, const CRect &dstRect, const CRect &srcRect) const;
	static void showBitmapResource(HDC hdc, int resId, const CPoint &p);
	void updateSizeOnDC(CDC *pDC);
  int getWidth()         const { return m_size.cx;  }
  int getHeight()        const { return m_size.cy;  }
  int getWeight()        const { return m_weight;   }
  const CSize &getSize() const { return m_size;     }
  CRect getRectangle()   const { return CRect(0, 0, m_size.cx, m_size.cy); }
};
