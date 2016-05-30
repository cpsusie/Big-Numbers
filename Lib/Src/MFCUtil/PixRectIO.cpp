#include "stdafx.h"
#include <PixRect.h>
#include <Picture.h>
#include <WinTools.h>

typedef enum {
  PICTYPE_BMP
 ,PICTYPE_JPG
 ,PICTYPE_PNG
 ,PICTYPE_TIFF
} PictureFileFormat;

static void writePixRect(PixRect *pr, FILE *f, PictureFileFormat format) {
  HBITMAP bm = *pr;
  try {
    switch(format) {
    case PICTYPE_BMP : ::writeAsBMP( bm, f); break;
    case PICTYPE_JPG : ::writeAsJPG( bm, f); break;
    case PICTYPE_TIFF: ::writeAsTIFF(bm, f); break;
    case PICTYPE_PNG : ::writeAsPNG( bm, f); break;
    default          :
      throwException(_T("Unknown pictureFormat:%d"), format);
    }
    DeleteObject(bm);
  } catch(...) {
    DeleteObject(bm);
    throw;
  }
}

static void writePixRect(PixRect *pr, const String &fileName, PictureFileFormat format) {
  FILE *f = MKFOPEN(fileName, "wb");
  try {
    writePixRect(pr, f, format);
    fclose(f);
  } catch(...) {
    fclose(f);
    throw;
  }
}

void PixRect::writeAsBMP(const String &fileName) {  writePixRect(this, fileName, PICTYPE_BMP);  }
void PixRect::writeAsBMP(FILE *f) {                 writePixRect(this, f       , PICTYPE_BMP);  }
void PixRect::writeAsJPG(const String &fileName) {  writePixRect(this, fileName, PICTYPE_JPG);  }
void PixRect::writeAsJPG(FILE *f) {                 writePixRect(this, f       , PICTYPE_JPG);  }
void PixRect::writeAsPNG( const String &fileName) { writePixRect(this, fileName, PICTYPE_PNG);  }
void PixRect::writeAsPNG( FILE *f) {                writePixRect(this, f       , PICTYPE_PNG);  }
void PixRect::writeAsTIFF(const String &fileName) { writePixRect(this, fileName, PICTYPE_TIFF); }
void PixRect::writeAsTIFF(FILE *f) {                writePixRect(this, f       , PICTYPE_TIFF); }

// ---------------------------------- load ------------------------------------

PixRect *PixRect::load(FILE *f) { // static
  PixRect *result = NULL;
  HDC      hdc    = NULL;
  try {
    CPicture picture; // handles BMP,JPEG,TIFF,PNG,GIF,ICO,,,...
    picture.load(f);
    result = new PixRect(picture.getWidth(), picture.getHeight());
    HDC hdc = result->getDC();
    picture.show(hdc);
    result->releaseDC(hdc);
    hdc = NULL;
    return result;
  } catch(...) {
    if(hdc) {
      result->releaseDC(hdc);
      hdc = NULL;
    }
    if(result) {
      delete result;
    }
    throw;
  }
}

PixRect *PixRect::load(const String &fileName) { // static
  FILE *f = FOPEN(fileName, "rb");
  try {
    PixRect *result = load(f);
    fclose(f);
    return result;
  } catch(...) {
    fclose(f);
    throw;
  }
}

