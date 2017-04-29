#include "pch.h"
#include <MFCUtil/Avifile.h>

#pragma comment (lib, "vfw32.lib")

#define SETFRAMEAPPENDER(f) { m_currentFrameAppender = &f; m_currentFrameBitsAppender = &f; }

CAviFile::CAviFile(const String &fileName 
                  ,bool          writeMode
                  ,DWORD         codec          
                  ,DWORD         frameRate      
                  )   
{
  m_fileName             = fileName;
  m_writeMode            = writeMode;
  m_codec                = codec;
  m_frameRate            = frameRate;

  m_dc                   = NULL;
  m_bitmap               = NULL;
  m_heap                 = NULL;
  m_lpBits               = NULL;
  m_frameIndex           = 0;
  m_aviFile              = NULL;
  m_aviStream            = NULL;
  m_compressedAviStream  = NULL;
  m_frameSize            = CSize(0,0);
  m_bitsPerPixel         = 0;

  AVIFileInit();   
  SETFRAMEAPPENDER(CAviFile::appendFrameFirstTime);

  if(!m_writeMode) {
    initMovieRead();
  }
}

CAviFile::~CAviFile() {
  releaseMemory();   
  AVIFileExit();
}

void CAviFile::releaseMemory() {   
  SETFRAMEAPPENDER(CAviFile::appendDummy);
  
  if(m_compressedAviStream) {   
    AVIStreamRelease(m_compressedAviStream);   
    m_compressedAviStream = NULL;
  }
  if(m_aviStream) {
    AVIStreamRelease(m_aviStream);
    m_aviStream           = NULL;
  }
  
  if(m_aviFile) {
    AVIFileRelease(m_aviFile);   
    m_aviFile             = NULL;
  }
  releaseHeapAndBuffer();
  releaseBitmap();
  releaseDC();
}

#define AVICALL(hr) checkAVIResult(__LINE__,hr)

void CAviFile::allocateHeapAndBuffer(size_t size) {
  m_heap = HeapCreate(HEAP_NO_SERIALIZE, size, 0);
  if(m_heap==NULL) {
    throwLastErrorOnSysCallException(_T("HeapCreate"));
  }

  m_lpBits = HeapAlloc(m_heap, HEAP_ZERO_MEMORY | HEAP_NO_SERIALIZE, size);
  if(m_lpBits==NULL) {
    throwException(_T("Unable to allocate memory on heap"));
  }
}

void CAviFile::releaseHeapAndBuffer() {
  if(m_lpBits) {
    HeapFree(m_heap, HEAP_NO_SERIALIZE, m_lpBits);
    m_lpBits = NULL;
  }   
  if(m_heap) {   
    HeapDestroy(m_heap);
    m_heap = NULL;
  }
}

void CAviFile::allocateDC() {
  m_dc = CreateCompatibleDC(NULL);
  if(m_dc == NULL) {
    throwLastErrorOnSysCallException(_T("CreateCompatibleDC"));
  }
}

void CAviFile::releaseDC() {
  if(m_dc) {
    DeleteDC(m_dc);   
    m_dc = NULL;   
  }
}

void CAviFile::allocateBitmap() {
  HDC screenDC = getScreenDC();
  m_bitmap = CreateCompatibleBitmap(screenDC, m_frameSize.cx, m_frameSize.cy);
  if(m_bitmap == NULL) {
    const HRESULT error = GetLastError();
    DeleteDC(screenDC);
    throwException(_T("CreateCompatibleBitmap failed. %s"), getErrorText(error).cstr());
  }
  DeleteDC(screenDC);
}

void CAviFile::releaseBitmap() {
  if(m_bitmap) {
    DeleteObject(m_bitmap);
    m_bitmap = NULL;
  }
}

void CAviFile::initMovieCreation(int nFrameWidth, int nFrameHeight, int bitsPerPixel) {
  int nMaxWidth = GetSystemMetrics(SM_CXSCREEN), nMaxHeight = GetSystemMetrics(SM_CYSCREEN);
  allocateDC();
  
  if(nFrameWidth  > nMaxWidth ) nMaxWidth  = nFrameWidth;
  if(nFrameHeight > nMaxHeight) nMaxHeight = nFrameHeight;

  m_frameSize    = CSize(nFrameWidth, nFrameHeight);
  m_bitsPerPixel = bitsPerPixel;

  allocateHeapAndBuffer(nMaxWidth*nMaxHeight*8);

  AVICALL(AVIFileOpen(&m_aviFile, m_fileName.cstr(), OF_CREATE | OF_WRITE, NULL));

  ZeroMemory(&m_aviStreamInfo, sizeof(AVISTREAMINFO));
  m_aviStreamInfo.fccType               = streamtypeVIDEO;
  m_aviStreamInfo.fccHandler            = m_codec;
  m_aviStreamInfo.dwScale               = 1;
  m_aviStreamInfo.dwRate                = m_frameRate;    // Frames Per Second;
  m_aviStreamInfo.dwQuality             = -1;               // Default Quality
  m_aviStreamInfo.dwSuggestedBufferSize = m_frameSize.cx * m_frameSize.cy*4;
  SetRect(&m_aviStreamInfo.rcFrame, 0, 0, m_frameSize.cx, m_frameSize.cy);
  _tcscpy(m_aviStreamInfo.szName, _T("Video Stream"));

  AVICALL(AVIFileCreateStream(m_aviFile, &m_aviStream, &m_aviStreamInfo));

  ZeroMemory(&m_aviCompressOptions, sizeof(AVICOMPRESSOPTIONS));
  m_aviCompressOptions.fccType          = streamtypeVIDEO;
  m_aviCompressOptions.fccHandler       = m_aviStreamInfo.fccHandler;
  m_aviCompressOptions.dwFlags          = AVICOMPRESSF_KEYFRAMES | AVICOMPRESSF_VALID;//|AVICOMPRESSF_DATARATE;
  m_aviCompressOptions.dwKeyFrameEvery  = 1;
  //m_aviCompressOptions.dwBytesPerSecond=1000/8;
  //m_aviCompressOptions.dwQuality=100;

  AVICALL(AVIMakeCompressedStream(&m_compressedAviStream, m_aviStream, &m_aviCompressOptions, NULL));
    // One reason this error might occur is if you are using a Codec that is not 
    // available on your system. Check the mmioFOURCC() code you are using and make
    // sure you have that codec installed properly on your machine.

  BITMAPINFO bmpInfo;
  ZeroMemory(&bmpInfo, sizeof(BITMAPINFO));
  bmpInfo.bmiHeader.biPlanes            = 1;
  bmpInfo.bmiHeader.biWidth             = m_frameSize.cx;
  bmpInfo.bmiHeader.biHeight            = m_frameSize.cy;
  bmpInfo.bmiHeader.biCompression       = BI_RGB;
  bmpInfo.bmiHeader.biBitCount          = m_bitsPerPixel;
  bmpInfo.bmiHeader.biSize              = sizeof(BITMAPINFOHEADER);
  bmpInfo.bmiHeader.biSizeImage         = m_frameSize.cx*m_frameSize.cy*m_bitsPerPixel/8;

  AVICALL(AVIStreamSetFormat(m_compressedAviStream, 0, (LPVOID)&bmpInfo.bmiHeader, bmpInfo.bmiHeader.biSize));
    // One reason this error might occur is if your bitmap does not meet the Codec requirements.  
    // For example,
    //   your bitmap is 32bpp while the Codec supports only 16 or 24 bpp; Or  
    //   your bitmap is 274 * 258 size, while the Codec supports only sizes that are powers of 2; etc...  
    // Possible solution to avoid this is: make your bitmap suit the codec requirements,  
    // or Choose a codec that is suitable for your bitmap.  
}

void CAviFile::initMovieRead() {
  AVICALL(AVIStreamOpenFromFile(&m_aviStream
                               , m_fileName.cstr()
                               , m_codec
                               , 0
                               , OF_READ
                               , NULL
                               ));

  AVICALL(AVIStreamInfo(m_aviStream, &m_aviStreamInfo, sizeof(m_aviStreamInfo)));
  const CRect r = m_aviStreamInfo.rcFrame;
  m_frameSize = CSize(r.Width(), r.Height());

  LONG formatInfoSize;
  AVICALL(AVIStreamReadFormat(m_aviStream, 0, NULL, &formatInfoSize));
  if(formatInfoSize != sizeof(m_bitmapInfoHeader)) {
    throwException(_T("Wrong format."));
  }
  AVICALL(AVIStreamReadFormat(m_aviStream, 0, &m_bitmapInfoHeader, &formatInfoSize));
  allocateHeapAndBuffer(m_aviStreamInfo.dwSuggestedBufferSize);
  allocateDC();
  allocateBitmap();
}

HBITMAP CAviFile::readFrame() {
  checkIsReadMode();
  if((DWORD)m_frameIndex >= m_aviStreamInfo.dwLength) {
    return NULL;
  }
  LONG samplesRead, bytesRead;
  AVICALL(AVIStreamRead(m_aviStream, m_frameIndex++,1, m_lpBits, m_aviStreamInfo.dwSuggestedBufferSize, &bytesRead, &samplesRead));
  BITMAPINFO bmInfo;
  ZeroMemory(&bmInfo, sizeof(BITMAPINFO));
  bmInfo.bmiHeader = m_bitmapInfoHeader;
  if(SetDIBits(m_dc, m_bitmap, 0, m_bitmapInfoHeader.biHeight, m_lpBits,&bmInfo, DIB_RGB_COLORS) == 0) {
    throwLastErrorOnSysCallException(_T("SetDIBits"));
  }
  return m_bitmap;
}

void CAviFile::appendFrameFirstTime(HBITMAP bm) {
  checkIsWriteMode();
  BITMAP info;
  if(GetObject(bm, sizeof(BITMAP), &info) == 0) {
    throwLastErrorOnSysCallException(_T("GetObject"));
  }
 
  initMovieCreation(info.bmWidth,info.bmHeight,info.bmBitsPixel);

  SETFRAMEAPPENDER(CAviFile::appendFrameUsual);

  appendFrameUsual(bm);
}

void CAviFile::appendFrameUsual(HBITMAP bm) {
  BITMAPINFO bmpInfo;
  bmpInfo.bmiHeader.biBitCount = 0;
  bmpInfo.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
  
  if(GetDIBits(m_dc, bm, 0,0,NULL, &bmpInfo, DIB_RGB_COLORS) == 0) {
    throwLastErrorOnSysCallException(_T("GetDIBits"));
  }
  bmpInfo.bmiHeader.biCompression = BI_RGB;
  if(GetDIBits(m_dc, bm, 0, bmpInfo.bmiHeader.biHeight, m_lpBits, &bmpInfo, DIB_RGB_COLORS) == 0) {
    throwLastErrorOnSysCallException(_T("GetDIBits"));
  }

  LONG samplesWritten, bytesWritten;
  AVICALL(AVIStreamWrite(m_compressedAviStream, m_frameIndex++, 1, m_lpBits, bmpInfo.bmiHeader.biSizeImage,0,&samplesWritten,&bytesWritten));
}

void CAviFile::appendDummy(HBITMAP)  {
  AVICALL(AVIERR_ERROR);
}

void CAviFile::appendFrameFirstTime(int width, int height, LPVOID pBits, int bitsPerPixel) {
  checkIsWriteMode();
  initMovieCreation(width, height, bitsPerPixel);
  
  SETFRAMEAPPENDER(CAviFile::appendFrameUsual);

  appendFrameUsual(width, height, pBits, bitsPerPixel);  
}

void CAviFile::appendFrameUsual(int width, int height, LPVOID pBits, int bitsPerPixel) {
  const DWORD dwSize = width*height*bitsPerPixel/8;
  AVICALL(AVIStreamWrite(m_compressedAviStream, m_frameIndex++, 1, pBits, dwSize, 0, NULL, NULL));
}

void CAviFile::appendDummy(int width, int height, LPVOID pBits,int bitsPerPixel) {
  AVICALL(AVIERR_ERROR);
}

static String formatAviMessage(HRESULT hr) {
  switch(hr) {
  case S_OK                 : return _T("Success");
  case AVIERR_BADFORMAT     : return _T("AVIERR_BADFORMAT: Corrupt file or unrecognized format");
  case AVIERR_MEMORY        : return _T("AVIERR_MEMORY: Insufficient memory");
  case AVIERR_FILEREAD      : return _T("AVIERR_FILEREAD: Disk error while reading file");
  case AVIERR_FILEOPEN      : return _T("AVIERR_FILEOPEN: Disk error while opening file");
  case REGDB_E_CLASSNOTREG  : return _T("REGDB_E_CLASSNOTREG: File type not recognised");
  case AVIERR_READONLY      : return _T("AVIERR_READONLY: File is read-only");
  case AVIERR_NOCOMPRESSOR  : return _T("AVIERR_NOCOMPRESSOR: A suitable compressor could not be found");
  case AVIERR_UNSUPPORTED   : return _T("AVIERR_UNSUPPORTED: Compression is not supported for this type of data");
  case AVIERR_INTERNAL      : return _T("AVIERR_INTERNAL: Internal error");
  case AVIERR_BADFLAGS      : return _T("AVIERR_BADFLAGS");
  case AVIERR_BADPARAM      : return _T("AVIERR_BADPARAM");
  case AVIERR_BADSIZE       : return _T("AVIERR_BADSIZE");
  case AVIERR_BADHANDLE     : return _T("AVIERR_BADHANDLE");
  case AVIERR_FILEWRITE     : return _T("AVIERR_FILEWRITE: Disk error while writing file");
  case AVIERR_COMPRESSOR    : return _T("AVIERR_COMPRESSOR");
  case AVIERR_NODATA        : return _T("AVIERR_NODATA");
  case AVIERR_BUFFERTOOSMALL: return _T("AVIERR_BUFFERTOOSMALL");
  case AVIERR_CANTCOMPRESS  : return _T("AVIERR_CANTCOMPRESS");
  case AVIERR_USERABORT     : return _T("AVIERR_USERABORT");
  case AVIERR_ERROR         : return _T("AVIERR_ERROR");
  default                   : return format(_T("Unknown avi result code:%d"), hr);
  }
}

void CAviFile::checkIsWriteMode() {
  if(!m_writeMode) {
    SETFRAMEAPPENDER(CAviFile::appendDummy);
    throwException(_T("AVI File not opened for writing"));
  }
}

void CAviFile::checkIsReadMode() {
  if(m_writeMode) {
    throwException(_T("AVI File not opened for reading"));
  }
}

void CAviFile::checkAVIResult(int line, HRESULT hr) {
  if(hr != S_OK) {
    releaseMemory();
    throwException(_T("Error in %s line %d:%s"), _T(__FILE__), line, formatAviMessage(hr).cstr());
  }
}
