#pragma once

#include <vfw.h>

class CAviFile {   
private:
  String              m_fileName;
  bool                m_writeMode;
  DWORD               m_codec;                    // Video Codec FourCC      
  DWORD               m_frameRate;                // Frames Per Second Rate (FPS)   
  HDC                 m_dc;   
  HBITMAP             m_bitmap;
  HANDLE              m_heap;   
  LPVOID              m_lpBits;
  LONG                m_frameIndex;
  PAVIFILE            m_aviFile;   
  PAVISTREAM          m_aviStream;   
  PAVISTREAM          m_compressedAviStream;   
  AVISTREAMINFO       m_aviStreamInfo;   
  AVICOMPRESSOPTIONS  m_aviCompressOptions;   
  BITMAPINFOHEADER    m_bitmapInfoHeader;
  CSize               m_frameSize;                // in pixels
  int                 m_bitsPerPixel;
  
  void (CAviFile::*m_currentFrameAppender)(HBITMAP bm);
  void (CAviFile::*m_currentFrameBitsAppender)(int, int, LPVOID, int);

  void appendFrameFirstTime(HBITMAP);
  void appendFrameUsual(    HBITMAP);
  void appendDummy(         HBITMAP);
  void appendFrameFirstTime(int, int, LPVOID, int);
  void appendFrameUsual(    int, int, LPVOID, int);
  void appendDummy(         int, int, LPVOID, int);
  /// Takes care of creating the memory, streams, compression options etc. required for the movie   
  void initMovieCreation(int frameWidth, int frameHeight, int bitsPerPixel);   
  void initMovieRead();
  void allocateHeapAndBuffer(size_t size);
  void releaseHeapAndBuffer();
  void allocateDC();
  void releaseDC();
  void allocateBitmap();
  void releaseBitmap();
  /// Takes care of releasing the memory and movie related handles
  void releaseMemory();

  void checkAVIResult(int line, HRESULT hr);
  void checkIsWriteMode();
  void checkIsReadMode();
public:
  /// <Summary>
  /// Constructor accepts the filename, video codec and frame rate settings
  /// as parameters.
  /// lpszFileName: Name of the output movie file to create 
  /// dwCodec: FourCC of the Video Codec to be used for compression
  /// dwFrameRate: The Frames Per Second (FPS) setting to be used for the movie
  /// </Summary>
  /// <Remarks>
  /// The default Codec used here is MPG4. To use a different codec, pass its Fourcc
  /// value as the input parameter for dwCodec.
  /// For example,   pass mmioFOURCC('D','I','V','X') to use DIVX codec, or
  /// pass mmioFOURCC('I','V','5','0') to use IV50 codec etc.
  ///
  /// Also, you can pass just 0 to avoid the codecs altogether. In that case, Frames 
  /// would be saved as they are without any compression; However, the output movie file 
  /// size would be very huge in that case.
  ///
  /// Finally, make sure you have the codec installed on the machine before
  /// passing its Fourcc here
  /// </Remarks>
  CAviFile(const String &fileName
          ,bool          writeMode = true
          ,DWORD         codec     = 0
          ,DWORD         frameRate = 1);


  /// <Summary>
  /// Destructor closes the movie file and flushes all the frames
  /// </Summary>
  ~CAviFile();
  /// </Summary>   
  /// Inserts the given HBitmap into the movie as a new Frame at the end.
  /// </Summary> 
  inline void appendNewFrame(HBITMAP bm) {
    (this->*m_currentFrameAppender)(bm);
  }

  /// </Summary>
  /// Inserts the given bitmap bits into the movie as a new Frame at the end.
  /// The width, height and nBitsPerPixel are the width, height and bits per pixel   
  /// of the bitmap pointed to by the input pBits.
  /// </Summary>
  inline void appendNewFrame(int width, int height, LPVOID pBits,int bitsPerPixel) {
    (this->*m_currentFrameBitsAppender)(width,height, pBits, bitsPerPixel);
  }
  HBITMAP readFrame(); // will return NULL at eof
  const AVISTREAMINFO &getStreamInfo() {
    return m_aviStreamInfo;
  }
};
