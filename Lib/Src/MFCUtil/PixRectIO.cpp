#include "pch.h"
#include <MFCUtil/PixRect.h>
#include <MFCUtil/Picture.h>

typedef enum {
  PICTYPE_BMP
 ,PICTYPE_JPG
 ,PICTYPE_PNG
 ,PICTYPE_TIFF
} PictureFileFormat;

static void writePixRect(const PixRect *pr, ByteOutputStream &out, PictureFileFormat format) {
  HBITMAP bm = *pr;
  try {
    switch(format) {
    case PICTYPE_BMP : ::writeAsBMP( bm, out); break;
    case PICTYPE_JPG : ::writeAsJPG( bm, out); break;
    case PICTYPE_TIFF: ::writeAsTIFF(bm, out); break;
    case PICTYPE_PNG : ::writeAsPNG( bm, out); break;
    default          :
      throwException(_T("Unknown pictureFormat:%d"), format);
    }
    DeleteObject(bm);
  } catch(...) {
    DeleteObject(bm);
    throw;
  }
}

void PixRect::writeAsBMP( ByteOutputStream &out) const { writePixRect(this, out, PICTYPE_BMP);  }
void PixRect::writeAsJPG( ByteOutputStream &out) const { writePixRect(this, out, PICTYPE_JPG);  }
void PixRect::writeAsPNG( ByteOutputStream &out) const { writePixRect(this, out, PICTYPE_PNG);  }
void PixRect::writeAsTIFF(ByteOutputStream &out) const { writePixRect(this, out, PICTYPE_TIFF); }

// ---------------------------------- load ------------------------------------

PixRect *PixRect::load(PixRectDevice &device, ByteInputStream &in) { // static
  PixRect *result = nullptr;
  HDC      hdc    = nullptr;
  try {
    CPicture picture; // handles BMP,JPEG,TIFF,PNG,GIF,ICO,,,...
    picture.load(in);
    result = new PixRect(device, PIXRECT_PLAINSURFACE, picture.getWidth(), picture.getHeight()); TRACE_NEW(result);
    HDC hdc = result->getDC();
    picture.show(hdc);
    result->releaseDC(hdc);
    hdc = nullptr;
    return result;
  } catch(...) {
    if(hdc) {
      result->releaseDC(hdc);
      hdc = nullptr;
    }
    SAFEDELETE(result);
    throw;
  }
}
