#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <MFCUtil/LayoutManager.h>
#include <MFCUtil/Picture.h>

#ifdef _DEBUG

void checkD3DResult(TCHAR *fileName, int line, HRESULT hr);

#define CHECKD3DRESULT(hr) checkD3DResult(__TFILE__,__LINE__,hr)

#else

void checkD3DResult(HRESULT hr);

#define CHECKD3DRESULT(hr) checkD3DResult(hr)

#endif

typedef enum {
  IMAGE_BMP
 ,IMAGE_JPG
} ImageType;

class ComboElement {
public:
  int         m_value;
  String      m_text;
  ComboElement(int value, const char *s1, const char *comment);
};

// Cdx9_rectDemoDlg dialog
class Cdx9_rectDemoDlg : public CDialogEx {
private:
  IDirect3D9         *m_direct3d;
  IDirect3DDevice9   *m_device;
  IDirect3DSurface9  *m_boardSurface;
  IDirect3DSurface9  *m_renderTarget;
  IDirect3DTexture9  *m_someTexture;
  SimpleLayoutManager m_layoutManager;
  CFont               m_comboFont;
  D3DTEXTUREOP        m_textureOp;
  unsigned int        m_arg1, m_arg2;
  D3DBLEND            m_srcBlend, m_dstBlend;
  int                 m_rotation;

  float m_app_scale_x, m_app_scale_y;

  CSize getSize(                 IDirect3DSurface9 *surface);
  CSize getSize(                 IDirect3DTexture9 *texture);
  void makeWhiteTransparent(     IDirect3DSurface9 *surface);
  void makeWhiteTransparent(     IDirect3DTexture9 *texture);
  void makeSemiTransparentOpague(IDirect3DTexture9 *texture);
  void makeOpaque(               IDirect3DSurface9 *surface);
  static void makeWhiteTransparent(D3DLOCKED_RECT &lockedRect, const CSize size);
  static void makeSemiTransparentOpague(D3DLOCKED_RECT &lockedRect, const CSize size);
  static void makeOpaque(          D3DLOCKED_RECT &lockedRect, const CSize size);
  void copySurfaceToTexture(     IDirect3DTexture9 *dst, IDirect3DSurface9 *src);
  void paintPictureOnSurface(    IDirect3DSurface9 *surface, CPicture &picture);
  void alphaBlend(               IDirect3DTexture9 *texture, const CRect &dstRect);
  IDirect3DSurface9 *createSurface(     const CSize &size, D3DFORMAT format, D3DPOOL pool = D3DPOOL_DEFAULT);
  IDirect3DTexture9 *createTexture(     const CSize &size, D3DFORMAT format, D3DPOOL pool = D3DPOOL_DEFAULT);
  IDirect3DSurface9 *createRenderTarget(const CSize &size, D3DFORMAT format = D3DFMT_FORCE_DWORD);
  IDirect3DTexture9 *loadTextureFromResource(int resId, const String &typeName);
  IDirect3DTexture9 *loadTextureFromFile(const String &fileName);
  IDirect3DSurface9 *loadSurfaceFromFile(const String &fileName);
  D3DPRESENT_PARAMETERS getPresentParameters() const;
  void loadTexture();
  void loadBoard();
  void drawSolidRect(CPoint &topLeft, CSize &size, D3DCOLOR color);
  void rectangle(const CPoint &topLeft, const CSize &size, D3DCOLOR color);
  void line(     const CPoint &p1,      const CPoint &p2 , D3DCOLOR color);
  void render2d( const CSize &size);
  bool render();
  inline CWnd *getGraphicsFrame() const {
    return GetDlgItem(IDC_STATIC_GRAPHICS);
  }
  inline CSize getGraphicsSize() {
    return getClientRect(getGraphicsFrame()).Size();
  }
  void updateGraphics();
  void initComboboxes();
  void initCombo(    int id, int startValue, const ComboElement *a, int n);
  int  getComboValue(int id, const ComboElement *a) const;

public:
  Cdx9_rectDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_DX9_RECTDEMO_DIALOG };
#endif

protected:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
  HICON m_hIcon;

  // Generated message map functions
  virtual BOOL OnInitDialog();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnSelchangeComboArg1();
  afx_msg void OnSelchangeComboArg2();
  afx_msg void OnSelchangeComboColorop();
  afx_msg void OnSelchangeComboSrcblend();
  afx_msg void OnSelchangeComboDstblend();
  afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  DECLARE_MESSAGE_MAP()
};
