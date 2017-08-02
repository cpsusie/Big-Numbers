#include "pch.h"
#include <MFCUtil/MMCapture.h>

#pragma comment(lib, "winmm.lib")

#define VFWLIBROOT "c:/Program Files (x86)/Windows Kits/8.1/Lib/winv6.3/um/"

#ifdef IS32BIT
#define VFWLIB VFWLIBROOT "x86/"
#else
#define VFWLIB VFWLIBROOT "x64/"
#endif

#pragma comment(lib, VFWLIB "vfw32.lib")

#define V(f) checkMMResult(__TFILE__,__LINE__,f)

// ******************************************** Raw Callback functions ********************************************

static LRESULT CALLBACK captureVideoStreamCallback(HWND captureWindow, VIDEOHDR *videoHeader) {
  MMCapture *capture = (MMCapture*)capGetUserData(captureWindow);
  if(capture->saveVideoFrame(videoHeader)) {
    return capture->getReceiver().captureVideoStreamCallback(*capture,capture->m_imagePr);
  } else {
    return S_OK;
  }
}

static LRESULT CALLBACK captureWaveStreamCallback(HWND captureWindow, WAVEHDR *audioHeader) {
  MMCapture *capture = (MMCapture*)capGetUserData(captureWindow);
  if(capture->m_captureBlocked) {
    return S_OK;
  }
  LRESULT result = capture->getReceiver().captureWaveStreamCallback(*capture,audioHeader);
  if(capture->m_playAudio) {
    capture->saveAudioFrame(audioHeader);
  }
  return result;
}

static LRESULT CALLBACK captureFrameCallback(HWND captureWindow, VIDEOHDR *videoHeader) {
  MMCapture *capture = (MMCapture*)capGetUserData(captureWindow);
  if(capture->saveVideoFrame(videoHeader)) {
    return capture->getReceiver().captureFrameCallback(*capture,capture->m_imagePr);
  } else {
    return S_OK;
  }
}

static LRESULT CALLBACK captureStatusCallback(HWND captureWindow, int id, TCHAR *description) {
  MMCapture *capture = (MMCapture*)capGetUserData(captureWindow);
  switch(id) {
  case IDS_CAP_BEGIN:
    capture->m_audioQueue.clear();
    capture->m_videoFrameCount  = 0;
    capture->m_audioSampleCount = 0;
    capture->m_capturing        = true;
    break;

  case IDS_CAP_END:
    capture->m_messageQueue.put(MESSAGE_CAPTURESTOPPED);
    break;
  }
  capture->getReceiver().captureStatusCallback(*capture,id,description);
  return TRUE;
}

static LRESULT CALLBACK captureControlCallback(HWND captureWindow, int state) {
  MMCapture *capture = (MMCapture*)capGetUserData(captureWindow);
  capture->getReceiver().captureControlCallback(*capture,state);
  switch(state) {
  case CONTROLCALLBACK_PREROLL:
    break;

  case CONTROLCALLBACK_CAPTURING:
    // Continue or terminate capturing
    if(capture->m_stopPending) {
      if(capture->m_playAudio) {
        V(waveOutReset(capture->m_hWaveOut));
      }
      return FALSE;
    }
    break;
  }

  return TRUE;
}

static LRESULT CALLBACK captureErrorCallback(HWND captureWindow, int id, TCHAR *message) {
  MMCapture *capture = (MMCapture*)capGetUserData(captureWindow);
  return capture->getReceiver().captureErrorCallback(*capture,id,message);
}

// ******************************************** class MMCapture ********************************************

MMCapture::MMCapture(BYTE captureWhat, CaptureReceiver &receiver, UINT framesPerSecond, bool playAudio, UINT audioBufferSize)
: m_captureWhat(captureWhat)
, m_receiver(receiver)
, m_playAudio(playAudio)
{
  for(int i = 0; i < 3; i++) {
    m_captureWindow    = NULL;
    m_imagePr          = NULL;
    m_videoPr          = NULL;
    m_audioThread      = NULL;
    m_webCamConnected  = false;
    m_videoFrameCount  = 0;
    m_audioSampleCount = 0;
    m_capturing        = false;
    m_stopPending      = false;
    m_captureBlocked   = false;
    int state = 0;
    try {
      captureInit(framesPerSecond, audioBufferSize);
      state++;
      if(captureVideo()) {
        pixRectInit();
      }
      break;
    } catch(...) {
      if(state == 1) {
        captureCleanup();
      }
      if(i == 2) {
        throw;
      } else {
        continue;
      }
    }
  }
  start();
}

MMCapture::~MMCapture() {
  m_messageQueue.put(MESSAGE_TERMINATE);
  while(stillActive()) {
    Sleep(100);
  }
  pixRectCleanup();
  captureCleanup();
}

void MMCapture::captureInit(UINT framesPerSecond, UINT audioBufferSize) {
  DWORD style = WS_CHILD;

  m_captureWindow = capCreateCaptureWindow(_T("my capture window"), style,0,0,640,480,m_receiver.getWindow(),1);
  if(m_captureWindow == NULL) {
    throwException(_T("%s:Cannot create CaptureWindow:%s"),__TFUNCTION__,getLastErrorText().cstr());
  }

  try {
    CHECKRESULT(capSetUserData(  m_captureWindow, this));
    if(captureVideo()) {
      CHECKRESULT(capDriverConnect(m_captureWindow, 0   ));
      m_webCamConnected = true;
    }

    CAPTUREPARMS param;

    CHECKRESULT(capCaptureGetSetup(m_captureWindow,&param,sizeof(param)));
    param.dwRequestMicroSecPerFrame = 1000000 / framesPerSecond;
    param.fYield            = TRUE;
    param.AVStreamMaster    = AVSTREAMMASTER_AUDIO; // AVSTREAMMASTER_NONE;
    param.dwAudioBufferSize = audioBufferSize;

    CHECKRESULT(capCaptureSetSetup(m_captureWindow,&param,sizeof(param)));
    if(captureAudio()) {
      int audioFormatSize = capGetAudioFormat(m_captureWindow,&m_audioFormat, sizeof(m_audioFormat));
      CHECKRESULT(capSetCallbackOnWaveStream( m_captureWindow, captureWaveStreamCallback));
    }
    if(captureVideo()) {
      int videoFormatSize = capGetVideoFormat(m_captureWindow,&m_videoFormat, sizeof(m_videoFormat));
      CHECKRESULT(capSetCallbackOnVideoStream(m_captureWindow, captureVideoStreamCallback));
      CHECKRESULT(capSetCallbackOnFrame(      m_captureWindow, captureFrameCallback));
    }
    CHECKRESULT(capSetCallbackOnStatus(     m_captureWindow, captureStatusCallback));
    CHECKRESULT(capSetCallbackOnCapControl( m_captureWindow, captureControlCallback));
    CHECKRESULT(capSetCallbackOnError(      m_captureWindow, captureErrorCallback));

    if(captureAudio() && m_playAudio) {
      m_audioThread = new AudioPlayerThread(*this); TRACE_NEW(m_audioThread);
      m_audioThread->start();
    }
  } catch(...) {
    captureCleanup();
    throw;
  }
}

void MMCapture::captureCleanup() {
  if(m_audioThread != NULL) {
    m_audioQueue.put(AudioQueueElement());
    while(m_audioThread->stillActive()) {
      Sleep(100);
    }
    SAFEDELETE(m_audioThread);
  }
  if(m_webCamConnected) {
    CHECKRESULT(capDriverDisconnect(m_captureWindow));
    m_webCamConnected = false;
  }
  if(m_captureWindow != NULL) {
    CHECKRESULT(DestroyWindow(m_captureWindow));
    m_captureWindow = NULL;
  }
}

void MMCapture::pixRectInit() {
  try {
    m_device.attach(m_receiver.getWindow());

    const BITMAPINFOHEADER &bmiHeader   = m_videoFormat.bmiHeader;
    m_videoRect = CRect(0,0,bmiHeader.biWidth, bmiHeader.biHeight);
    // Create screen pixRect
    m_imagePr = new PixRect(m_device, PIXRECT_PLAINSURFACE, m_videoRect.Size(), D3DPOOL_DEFAULT); TRACE_NEW(m_imagePr);

    // Create memory surface with pixelformat equals to videoFormat
    const D3DFORMAT videoPixelFormat = (D3DFORMAT)bmiHeader.biCompression; // D3DFMT_YUY2;
    m_videoPr = new PixRect(m_device, PIXRECT_PLAINSURFACE, m_videoRect.Size(), D3DPOOL_DEFAULT, videoPixelFormat); TRACE_NEW(m_videoPr);
    bool convsupported = m_device.supportFormatConversion(videoPixelFormat, m_imagePr->getPixelFormat());
  } catch(...) {
    pixRectCleanup();
    throw;
  }
}

void MMCapture::pixRectCleanup() {
  SAFEDELETE(m_videoPr);
  SAFEDELETE(m_imagePr);
  m_device.detach();
}

UINT MMCapture::run() {
  for(;;) {
    CaptureMessage message = m_messageQueue.get();
    switch(message) {
    case MESSAGE_STARTCAPTURE:
      m_receiver.log(_T("handle MESSAGE_STARTCAPTURE\n"));
      if(!isCapturing()) {
        m_stopPending = false;
        capCaptureSequenceNoFile(m_captureWindow);
      }
      break;

    case MESSAGE_STOPCAPTURE :
      m_receiver.log(_T("handle MESSAGE_STOPCAPTURE\n"));
      m_stopPending = true;
      break;

    case MESSAGE_CAPTURESTOPPED:
      m_receiver.log(_T("handle MESSAGE_CAPTURESTOPPED\n"));
      m_capturing = false;
      break;

    case MESSAGE_TERMINATE   :
      m_receiver.log(_T("handle MESSAGE_TERMINATE\n"));
      if(isCapturing()) {
        m_receiver.log(_T("isCapturing = true. stopPending...\n"));
        m_stopPending = true;
        for(;;) {
          if(m_messageQueue.get() == MESSAGE_CAPTURESTOPPED) {
            m_receiver.log(_T("got MESSAGE_CAPTURESTOPPED. Now stop loop\n"));
            m_capturing = false;
            break;
          }
        }
      }
      return 0;

    default:
      AfxMessageBox(format(_T("MMCapture:Unknown message in messageQueue:%d"), message).cstr(), MB_ICONSTOP);
      exit(-1);
    }
  }
}

void MMCapture::startCapture() {
  m_messageQueue.put(MESSAGE_STARTCAPTURE);
}

void MMCapture::stopCapture() {
  m_messageQueue.put(MESSAGE_STOPCAPTURE);
}

CAPSTATUS MMCapture::getStatus() const {
  CAPSTATUS result;
  CHECKRESULT(capGetStatus(m_captureWindow, &result, sizeof(CAPSTATUS)));
  return result;
}

CSize MMCapture::getImageSize() const {
  return CSize(m_videoFormat.bmiHeader.biWidth,m_videoFormat.bmiHeader.biHeight);
}

CAPDRIVERCAPS MMCapture::getDriverCapabilities() const {
  CAPDRIVERCAPS result;
  CHECKRESULT(capDriverGetCaps(m_captureWindow, &result, sizeof(CAPDRIVERCAPS)));
  return result;
}

void MMCapture::blockCapturing(bool blocked) {
  m_captureBlocked = blocked;
}

bool MMCapture::saveVideoFrame(VIDEOHDR *videoHeader) {
  m_gate.wait();

  bool result = false;

  if(!m_captureBlocked) {
    m_videoPr->copy(*videoHeader);
    m_imagePr->formatConversion(*m_videoPr);
    m_videoFrameCount++;
    result = true;
  }

  m_gate.signal();
  return result;
}

void MMCapture::paintLastFrame(CDC &dc, const CRect &rect) {
  paintLastFrame(dc.m_hDC,rect);
}

void MMCapture::paintLastFrame(HDC hdc, const CRect &rect) {
  m_gate.wait();

  if(!m_captureBlocked) {
    PixRect::bitBlt(hdc,rect.left,rect.top,rect.Width(),rect.Height(),SRCCOPY,m_imagePr,0,0);
  }

  m_gate.signal();
}

void MMCapture::saveAudioFrame(const WAVEHDR *audioHeader) {
  if(isCapturing()) {
    m_audioQueue.put(AudioQueueElement(audioHeader));
    m_audioSampleCount += audioHeader->dwBytesRecorded / m_audioFormat.wBitsPerSample * 8;
  }
}

// ******************************************** class CaptureReceiver ********************************************
void CaptureReceiver::vlog(const TCHAR *format, va_list argptr) {
  _vtprintf(format,argptr);
}

void CaptureReceiver::log(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vlog(format, argptr);
  va_end(argptr);
}

// ******************************************** class AudioPlayerThread ********************************************

void checkMMResult(TCHAR *fileName, int line, MMRESULT mmResult) {
  if(mmResult == MMSYSERR_NOERROR) {
    return;
  }
  TCHAR buf[1024];
  waveOutGetErrorText(mmResult,buf,ARRAYSIZE(buf));
  throwException(_T("Error in %s line %d. \n%s"), fileName, line, buf);
}

static void CALLBACK captureWaveOutCallback(HWAVEOUT hWaveOut, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
  MMCapture *capture = (MMCapture*)dwInstance;
  if(uMsg == WOM_DONE) { // tell AudioPlayerThread that the buffer send by waveOutWrite is done, so the next can be sent
    capture->m_soundDone.signal();
  }
}

AudioQueueElement::AudioQueueElement() {
  m_command = STOPAUDIO;
  memset(&m_waveHeader, 0, sizeof(m_waveHeader));
}

AudioQueueElement::AudioQueueElement(const WAVEHDR *waveHeader) {
  m_command    = PLAYAUDIO;
  m_waveHeader = *waveHeader;
}

AudioPlayerThread::AudioPlayerThread(MMCapture &capture) : m_capture(capture) {
  WAVEFORMATEX audioFormat = m_capture.getAudioFormat();
  V(waveOutOpen(&m_capture.m_hWaveOut,WAVE_MAPPER,&audioFormat,(DWORD_PTR)captureWaveOutCallback,(DWORD_PTR)&m_capture,CALLBACK_FUNCTION | WAVE_FORMAT_DIRECT));
}

AudioPlayerThread::~AudioPlayerThread() {
  waveOutClose(m_capture.m_hWaveOut);
}

UINT AudioPlayerThread::run() {
  for(;;) {
    AudioQueueElement e = m_capture.m_audioQueue.get();
    switch(e.m_command) {
    case PLAYAUDIO:
      if(!m_capture.isCapturing()) {
        continue;
      }
      break;
    case STOPAUDIO:
      return 0;
    }

    WAVEHDR &waveHeader = e.m_waveHeader;
    HGLOBAL hGlobal     = GlobalAlloc(GMEM_FIXED,waveHeader.dwBufferLength);
    void   *dataBuffer  = GlobalLock(hGlobal);
    try {
      memcpy(dataBuffer, waveHeader.lpData, waveHeader.dwBufferLength);
      waveHeader.lpData  = (char*)dataBuffer;
      waveOutWrite(m_capture.m_hWaveOut, &waveHeader, sizeof(WAVEHDR));
      m_capture.m_soundDone.wait();
    } catch(...) {
      // ignore
    }
    GlobalUnlock(hGlobal);
    GlobalFree(hGlobal);
  }
}

MMCaptureDevice::MMCaptureDevice(const String &name, const String &version) {
  m_name    = name;
  m_version = version;
}

Array<MMCaptureDevice> MMCaptureDevice::getDevices() { // static
  Array<MMCaptureDevice> devices;
  for(int i = 0; i < 10; i++) {
    TCHAR deviceName[100];
    TCHAR deviceVersion[100];
    if(capGetDriverDescription(i, deviceName, ARRAYSIZE(deviceName), deviceVersion, ARRAYSIZE(deviceVersion))) {
      devices.add(MMCaptureDevice(deviceName,deviceVersion));
    } else {
      break;
    }
  }
  return devices;
}

/*
static BOOL CALLBACK myAcmDriverEnumCallback(HACMDRIVERID hadid, DWORD dwInstance, DWORD fdwSupport) {
  Array<ACMDRIVERDETAILS> &result = (Array<ACMDRIVERDETAILS>&)dwInstance;

  ACMDRIVERDETAILS details;
  details.cbStruct = sizeof(ACMDRIVERDETAILS);
  CHECKRESULT(acmFilterDetails(had,&details,
  HACMDRIVER had,
  LPACMFILTERDETAILS pafd,
  DWORD fdwDetails
);

  return TRUE;
}


Array<ACMDRIVERDETAILS> getACMDrivers() {
  Array<ACMDRIVERDETAILS> result;
  CHECKRESULT(acmDriverEnum(myAcmDriverEnumCallback,(DWORD)&result,ACM_DRIVERENUMF_DISABLED));
  return result;
}
*/
