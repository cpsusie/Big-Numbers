#include "stdafx.h"
#include <CompactHashMap.h>
#include <Singleton.h>
#include "BitmapRotate.h"

static int MarkerBitmaps[] = {
  IDB_BLACKSQUAREBITMAP
 ,IDB_LEFTRIGHTARROWBITMAP
 ,IDB_ROTATECORNERBITMAP
 ,IDB_CIRCLEARROWBITMAP
};

class BitmapKey {
public:
  int m_id, m_theta; // theta in degrees
  BitmapKey() : m_id(0), m_theta(0) {
  };
  BitmapKey(int id, int theta) : m_id(id), m_theta(theta) {
  };
  bool operator==(const BitmapKey &key) const {
    return (m_id == key.m_id) && (m_theta == key.m_theta);
  }
  ULONG hashCode() const {
    return 360 * m_id + m_theta;
  }
};

class BitmapCache : public CompactHashMap<BitmapKey,CBitmap*,ARRAYSIZE(MarkerBitmaps)*360>, public Singleton {
private:
  LPDIRECT3DDEVICE m_device;
  Semaphore        m_lock;

  static D3DPRESENT_PARAMETERS getPresentParameters(HWND hwnd);
  void createDevice(HWND hwnd);
  void releaseDevice();
  BitmapCache();
  ~BitmapCache() override;
public:
  LPDIRECT3DDEVICE getDevice() {
    return m_device;
  }
  inline BitmapCache &wait() {
    m_lock.wait();
    return *this;
  }
  inline void notify() {
    m_lock.notify();
  }
  DEFINESINGLETON(BitmapCache);
};

BitmapCache::BitmapCache() : Singleton(__TFUNCTION__) {
  m_device = nullptr;

  createDevice(*theApp.GetMainWnd());

  for(int i = 0; i < ARRAYSIZE(MarkerBitmaps); i++) {
    int id = MarkerBitmaps[i];
    CBitmap *b = new CBitmap(); TRACE_NEW(b);
    if(b->LoadBitmap(id) == 0) {
      SAFEDELETE(b);
      throwException(_T("LoadBitmap(%d) failed"), id);
    }
    put(BitmapKey(id, 0), b);
  }
}

BitmapCache::~BitmapCache() {
  releaseDevice();

  for(auto it = getIterator(); it.hasNext();) {
    CBitmap *b = it.next().getValue();
    b->DeleteObject();
    SAFEDELETE(b);
  }
}

D3DPRESENT_PARAMETERS BitmapCache::getPresentParameters(HWND hwnd) { // static
  D3DPRESENT_PARAMETERS param;
  ZeroMemory(&param, sizeof(param));

  const CSize sz = getClientRect(hwnd).Size();

  param.Windowed               = TRUE;
  param.MultiSampleType        = D3DMULTISAMPLE_NONE;
  param.SwapEffect             = D3DSWAPEFFECT_DISCARD;
  param.EnableAutoDepthStencil = FALSE;
  param.BackBufferFormat       = D3DFMT_X8R8G8B8;
  param.hDeviceWindow          = nullptr;
  param.Flags                  = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
  param.BackBufferCount        = 1;
  param.BackBufferWidth        = sz.cx;
  param.BackBufferHeight       = sz.cy;
  param.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
  return param;
}

void BitmapCache::createDevice(HWND hwnd) {
  D3DPRESENT_PARAMETERS present = getPresentParameters(hwnd);
  m_device = DirectXDeviceFactory::getInstance().createDevice(hwnd, &present);
}

void BitmapCache::releaseDevice() {
  SAFERELEASE(m_device);
}

static CBitmap *cloneBitmap(CBitmap *bm) {
  const BITMAP info = getBitmapInfo(*bm);

  HDC srcDC = CreateCompatibleDC(nullptr);
  HDC dstDC = CreateCompatibleDC(nullptr);

  CBitmap *result = new CBitmap; TRACE_NEW(result);
  result->CreateBitmap(info.bmWidth, info.bmHeight, info.bmPlanes, info.bmBitsPixel, nullptr);

  SelectObject(srcDC, *bm);
  SelectObject(dstDC, *result);

  BitBlt(dstDC, 0,0,info.bmWidth,info.bmHeight, srcDC, 0,0, SRCCOPY);

  DeleteDC(dstDC);
  DeleteDC(srcDC);
  return result;
}

HBITMAP bitmapRotate(HBITMAP b0, double degree) {
  BitmapCache &cache = BitmapCache::getInstance().wait();
  try {
    HBITMAP br = bitmapRotate(cache.getDevice(), b0, degree);
    cache.notify();
    return br;
  } catch(...) {
    cache.notify();
    throw;
  }
}

CBitmap *bitmapRotate(CBitmap *b0, double degree) {
  BitmapCache &cache = BitmapCache::getInstance().wait();
  try {
    CBitmap *br = bitmapRotate(cache.getDevice(), b0, degree);
    cache.notify();
    return br;
  } catch(...) {
    cache.notify();
    throw;
  }
}

CBitmap *getRotatedBitmapResource(int id, int degree) {
  BitmapCache &cache = BitmapCache::getInstance().wait();
  try {
    BitmapKey    key(id, degree);
    CBitmap    **b = cache.get(key);
    if(b == nullptr) {
      const BitmapKey k0(id,0);
      CBitmap **b0 = cache.get(k0);
      if(b0 == nullptr) {
        throwException(_T("No bitmap with id=%d"),id);
      }
      CBitmap *br = bitmapRotate(cache.getDevice(), *b0, degree);
      cache.put(key,br);
      b = cache.get(key);
    }
    cache.notify();
    return *b;
  } catch(...) {
    cache.notify();
    throw;
  }
}
