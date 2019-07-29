#pragma once

#include "PixRect.h"
#include <Thread.h>
#include <Semaphore.h>
#include <SynchronizedQueue.h>

class MMCaptureDevice {
private:
  String m_name;
  String m_version;
public:
  MMCaptureDevice(const String &name, const String &version);

  const String &getName() const {
    return m_name;
  }

  const String &getVersion() const {
    return m_version;
  }

  static Array<MMCaptureDevice> getDevices();
};

class MMCapture;

class CaptureReceiver {
public:
  virtual HWND getWindow()      = 0;
  virtual LRESULT captureVideoStreamCallback(MMCapture &capture, PixRect *image)                   { return S_OK; }
  virtual LRESULT captureWaveStreamCallback( MMCapture &capture, WAVEHDR *audioHeader)             { return S_OK; }
  virtual LRESULT captureFrameCallback(      MMCapture &capture, PixRect *image)                   { return S_OK; }
  virtual LRESULT captureStatusCallback(     MMCapture &capture, int id, const TCHAR *description) { return S_OK; }
  virtual LRESULT captureControlCallback(    MMCapture &capture, int state)                        { return S_OK; }
  virtual LRESULT captureErrorCallback(      MMCapture &capture, int id, const TCHAR *message)     { return S_OK; }
  virtual void vlog(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
  void log(         _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
};

class AudioPlayerThread : public Thread {
private:
  MMCapture &m_capture;
  UINT run();
public:
  AudioPlayerThread(MMCapture &capture);
 ~AudioPlayerThread();
};

typedef enum {
  STOPAUDIO
 ,PLAYAUDIO
} AudioCommand;

typedef enum {
  MESSAGE_STARTCAPTURE
 ,MESSAGE_STOPCAPTURE
 ,MESSAGE_TERMINATE
 ,MESSAGE_CAPTURESTOPPED
} CaptureMessage;

#define CAPTURE_VIDEO 0x1
#define CAPTURE_AUDIO 0x2

class AudioQueueElement {
public:
  AudioCommand m_command;
  WAVEHDR      m_waveHeader;
  AudioQueueElement(const WAVEHDR *waveHeader);
  AudioQueueElement();
};

class MMCapture : private Thread {
private:
  const BYTE                           m_captureWhat; // any combination of CAPTURE_VIDEO and CAPTURE_AUDIO
  CaptureReceiver                     &m_receiver;
  HWND                                 m_captureWindow;
  PixRectDevice                        m_device;
  BITMAPINFO                           m_videoFormat;
  WAVEFORMATEX                         m_audioFormat;
  PixRect                             *m_imagePr;   // Compatible with Screen
  PixRect                             *m_videoPr;   // Compatible with the webcams LPVIDEOHDR.
  CRect                                m_videoRect; // CRect(0,0,videoPr->getWidth(),videoPr->height())
  HWAVEOUT                             m_hWaveOut;
  SynchronizedQueue<AudioQueueElement> m_audioQueue;
  SynchronizedQueue<CaptureMessage>    m_messageQueue;
  Semaphore                            m_gate, m_soundDone;
  bool                                 m_webCamConnected;
  bool                                 m_capturing;
  bool                                 m_stopPending;
  bool                                 m_captureBlocked;
  AudioPlayerThread                   *m_audioThread;
  int                                  m_videoFrameCount;
  int                                  m_audioSampleCount;
  const bool                           m_playAudio;

  void captureInit(UINT framesPerSecond, UINT audioBufferSize);
  void captureCleanup();

  void pixRectInit();
  void pixRectCleanup();

  friend LRESULT CALLBACK captureVideoStreamCallback(HWND captureWindow, VIDEOHDR *videoHeader);
  friend LRESULT CALLBACK captureWaveStreamCallback( HWND captureWindow, WAVEHDR  *audioHeader);
  friend LRESULT CALLBACK captureFrameCallback(      HWND captureWindow, VIDEOHDR *videoHeader);
  friend LRESULT CALLBACK captureStatusCallback(     HWND captureWindow, int id, TCHAR *description);
  friend LRESULT CALLBACK captureControlCallback(    HWND captureWindow, int state);
  friend void    CALLBACK captureWaveOutCallback(    HWAVEOUT hWaveOut, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);

  MMCapture(const MMCapture &capture);        // Not defined. Class not cloneable
  MMCapture &operator=(const MMCapture &src); // Not defined. Class not cloneable

  bool saveVideoFrame(VIDEOHDR *videoHeader);
  void saveAudioFrame(const WAVEHDR  *audioHeader);
  friend class AudioPlayerThread;
  UINT run();
public:
  MMCapture(BYTE captureWhat, CaptureReceiver &receiver, UINT framesPerSecond = 15, bool playAudio=true, UINT audioBufferSize=0);
  virtual ~MMCapture();
  void startCapture();
  void stopCapture();
  inline bool captureVideo() const {
    return (m_captureWhat & CAPTURE_VIDEO) != 0;
  }
  inline bool captureAudio() const {
    return (m_captureWhat & CAPTURE_AUDIO) != 0;
  }
  CaptureReceiver    &getReceiver()                 { return m_receiver;          }
  HWND                getCaptureWindow()      const { return m_captureWindow;     }
  CAPSTATUS           getStatus()             const;
  const BITMAPINFO   &getVideoFormat()        const { return m_videoFormat;       }
  const WAVEFORMATEX &getAudioFormat()        const { return m_audioFormat;       }
  CSize               getImageSize()          const;
  CAPDRIVERCAPS       getDriverCapabilities() const;
  int                 getPendingAudioFrames() const { return (int)m_audioQueue.size(); }
  int                 getVideoFrameCount()    const { return m_videoFrameCount;   }
  int                 getAudioSampleCount()   const { return m_audioSampleCount;  }
  bool                isCapturing()           const { return m_capturing;         }
  void                paintLastFrame(CDC &dc, const CRect &rect);
  void                paintLastFrame(HDC hdc, const CRect &rect);
  void                blockCapturing(bool blocked);
};

void checkMMResult(TCHAR *fileName, int line, MMRESULT mmResult);
