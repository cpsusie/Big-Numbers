#include "stdafx.h"
#include <BitSet.h>
#include <MFCUtil/ColorSpace.h>
#include "SavedImageArray.h"
#include "MakeGifDoc.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMakeGifDoc, CDocument)

BEGIN_MESSAGE_MAP(CMakeGifDoc, CDocument)
END_MESSAGE_MAP()

const TCHAR *CMakeGifDoc::s_defaultName = _T("Untitled");

ImageSettings::ImageSettings() {
  m_imageScaleFactor = 1;
  m_colorCount       = 256;
}

CMakeGifDoc::CMakeGifDoc() {
  m_gif      = nullptr;
  setDocName();
  m_rawPrArray.addPropertyChangeListener(      this);
  m_scaledPrArray.addPropertyChangeListener(   this);
  m_quantizedPrArray.addPropertyChangeListener(this);
}

CMakeGifDoc::~CMakeGifDoc() {
  m_quantizedPrArray.removePropertyChangeListener(this);
  m_scaledPrArray.removePropertyChangeListener(   this);
  m_rawPrArray.removePropertyChangeListener(      this);
  closeGif();
}

void CMakeGifDoc::setDocName(const String &name) {
  if(name.length() == 0) {
    m_name     = s_defaultName;
    m_fileSize = -1;
  } else {
    m_name     = name;
    m_fileSize = STAT(m_name).st_size;
  }
}

BOOL CMakeGifDoc::OnNewDocument() {
  if(!__super::OnNewDocument()) {
    return FALSE;
  }
  setDocName();
  updateTimestamp();
  return TRUE;
}

void CMakeGifDoc::Serialize(CArchive& ar) {
  if (ar.IsStoring()) {
  } else {
  }
}

#if defined(_DEBUG)
void CMakeGifDoc::AssertValid() const {
  __super::AssertValid();
}

void CMakeGifDoc::Dump(CDumpContext& dc) const {
  __super::Dump(dc);
}
#endif //_DEBUG


static int readGifStreamFunction(GifFileType *gf, GifByteType *buffer, int n) {
  return (int)(((ByteInputStream*)gf->UserData)->getBytes((BYTE*)buffer, n));
}

static int writeGifStreamFunction(GifFileType *gf, const GifByteType *buffer, int n) {
  ((ByteOutputStream*)gf->UserData)->putBytes((const BYTE*)buffer, n);
  return n;
}

static String gifErrorCodeToString(int errorCode) {
  const char *errorMsg = GifErrorString(errorCode);
  if(errorMsg == nullptr) {
    return _T("No error");
  } else {
    return errorMsg;
  }
}

static void throwGifErrorCode(int errorCode, const TCHAR *fileName, int line) {
  throwException(_T("Gif error in %s line %d: error=%s")
                 ,fileName, line
                 ,gifErrorCodeToString(errorCode).cstr()
               );
}

#define THROWGIFERROR(code) throwGifErrorCode(code, __TFILE__, __LINE__)

static GifFileType *loadGif(ByteInputStream &in) {
  int error;
  GifFileType *gf = DGifOpen(&in, readGifStreamFunction, &error);
  if(gf == nullptr) {
    THROWGIFERROR(error);
  }
  try {
    if(DGifSlurp(gf) != GIF_OK) {
      THROWGIFERROR(gf->Error);
    }
    return gf;
  } catch(...) {
    DGifCloseFile(gf, &error);
    throw;
  }
}

static void saveGif(ByteOutputStream &out, GifFileType *gf) {
  int error;

  GifFileType *gifOut = nullptr;
  try {
    gifOut = EGifOpen(&out, writeGifStreamFunction, &error);
    if(gifOut == nullptr) {
      THROWGIFERROR(error);
    }
    gifOut->SWidth           = gf->SWidth;
    gifOut->SHeight          = gf->SHeight;
    gifOut->SColorResolution = gf->SColorResolution;
    gifOut->SBackGroundColor = gf->SBackGroundColor;
    gifOut->SColorMap        = GifMakeMapObject(gf->SColorMap->ColorCount
                                               ,gf->SColorMap->Colors);

    for(int i = 0; i < gf->ImageCount; i++) {
      GifMakeSavedImage(gifOut, &gf->SavedImages[i]);
    }
    if(gifOut->ImageCount > 0) {
      SavedImage *image0 = gifOut->SavedImages;
      addNetscapeBlock(image0);
    }

    EGifSetGifVersion(gifOut, true);
    if(EGifSpew(gifOut) != GIF_OK) {
      THROWGIFERROR(gifOut->Error);
    }
  } catch(Exception e) {
    if(gifOut != nullptr) {
      EGifCloseFile(gifOut, &error);
    }
    throw e;
  } catch(...) {
    if(gifOut != nullptr) {
      EGifCloseFile(gifOut, &error);
    }
    throw;
  }
}

void CMakeGifDoc::loadGif(const String &fileName) {
  GifFileType *oldGif = m_gif;

  try {;
    m_gif = ::loadGif(ByteInputFile(fileName));
    setDocName(fileName);
    updateTimestamp();
    if(oldGif) {
      deallocateGif(oldGif);
    }
    setSaveTime(m_lastUpdate);
  } catch(Exception e) {
    m_gif = oldGif;
    throw;
  }
}

void CMakeGifDoc::saveGif(const String &fileName) {
  if(m_gif == nullptr) {
    return;
  }
  ::saveGif(ByteOutputFile(fileName), m_gif);
  setDocName(fileName);
  setSaveTime(Timestamp());
}

void CMakeGifDoc::closeGif() {
  if(hasGifFile()) {
    deallocateGif(m_gif);
    m_gif = nullptr;
    setDocName();
    updateTimestamp();
  }
}

void CMakeGifDoc::clear() {
  closeGif();
  m_rawPrArray.clear();
  m_scaledPrArray.clear();
  m_quantizedPrArray.clear();
}

static void dumpPr(const PixRect *pr) {
  CBitmap bm;
  pr->toBitmap(bm);
  DebugBitmap::showBitmap(bm);
}

void CMakeGifDoc::setImageSettings(const ImageSettings &settings) {
  if(settings.m_imageScaleFactor != m_imageSettings.m_imageScaleFactor) {
    m_scaledPrArray.clear();
//    m_quantizedPrArray.clear();
  }
  if(settings.m_colorCount != m_imageSettings.m_colorCount) {
    m_quantizedPrArray.clear();
  }
  m_imageSettings = settings;
}

bool CMakeGifDoc::addImagesToGif() {
  const PixRectArray &prArray = getQuantizedPrArray();
  if(prArray.size() == 0) {
    return false;
  }
  if(!hasGifFile()) {
    const CSize size = prArray.getMaxSize();
    m_gif = allocateGifFile(size.cx, size.cy);
  }
  const SavedImageArray images(prArray, m_imageSettings.m_colorCount);
  for(size_t i = 0; i < images.size(); i++) {
    GifMakeSavedImage(m_gif, images[i]);
  }

  const SavedImage *image0 = images[0];
  const ColorMapObject *colorMap = image0->ImageDesc.ColorMap;
  m_gif->SColorMap = GifMakeMapObject(colorMap->ColorCount, colorMap->Colors);
  updateTimestamp();
  return true;
}

void CMakeGifDoc::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(source == &m_rawPrArray) {
    notifyPropertyChanged(RAW_IMAGE_LIST      , oldValue, newValue);
  } else if(source == &m_scaledPrArray) {
    notifyPropertyChanged(SCALED_IMAGE_LIST   , oldValue, newValue);
  } else if(source == &m_quantizedPrArray) {
    notifyPropertyChanged(QUANTIZED_IMAGE_LIST, oldValue, newValue);
  }
}

void CMakeGifDoc::addPixRectArray(const PixRectArray &prArray) {
  m_rawPrArray.append(prArray);
}

bool CMakeGifDoc::removeAllImages() {
  if(m_rawPrArray.size() == 0) {
    return false;
  }
  m_rawPrArray.clear();
  return true;
}

const PixRectArray &CMakeGifDoc::getScaledPrArray() const {
  if(m_scaledPrArray.needUpdate(m_rawPrArray)) {
    m_scaledPrArray = m_rawPrArray;
    if(m_imageSettings.m_imageScaleFactor != 1) {
      m_scaledPrArray.scale(m_imageSettings.m_imageScaleFactor);
    }
  }
  return m_scaledPrArray;
}

const PixRectArray &CMakeGifDoc::getQuantizedPrArray() const {
  const PixRectArray &scaledArray = getScaledPrArray();
  if(m_quantizedPrArray.needUpdate(scaledArray)) {
    m_quantizedPrArray = scaledArray;
    m_quantizedPrArray.quantize(m_imageSettings.m_colorCount);
  }
  return m_quantizedPrArray;
}
