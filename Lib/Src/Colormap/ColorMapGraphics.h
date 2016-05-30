#pragma once

#include <afxwin.h>
#include <MFCUtil/ColorSpace.h>

typedef enum {
  TEXT_ALIGNLEFT
 ,TEXT_ALIGNRIGHT
 ,TEXT_ALIGNCENTER
} TextAlignment;

long textAlignmentToLong(TextAlignment textAlignment);
TextAlignment longToTextAlignment(long value);

class LabelledRect : public CRect {
public:
  CString m_label;
  LabelledRect(int left,int top,int right,int bottom,const char *label);
  LabelledRect() {}
};

class ColorMapGraphics {
private:
  CRect         m_rect, m_mapRect, m_scaleRect, m_scalePosRect, m_resultRect;
  LabelledRect  m_redFieldRect, m_greenFieldRect, m_blueFieldRect;
  LabelledRect  m_hueFieldRect, m_saturationFieldRect, m_luminationFieldRect;
  LSHColor      m_currentLSHColor;
  CWnd         *m_wnd;
  CDC           m_dc;
  CBitmap       m_colorBitmap,m_posBitmap,m_arrowBitmap;
  COLORREF      m_backgroundColor;
  CString       m_caption;
  bool          m_propertiesDirty    : 1;
  bool          m_colorScaleDirty    : 1;
  bool          m_hasBorder          : 1;
  bool          m_sunken             : 1;
  bool          m_clientEdge         : 1;
  bool          m_staticEdge         : 1;
  bool          m_modalFrame         : 1;
  TextAlignment m_textAlignment;
  RGBColor    **m_map;
  RGBColor     *m_colorScale;

  bool bitmapsInitialized();
  void initBitmaps(CDC &dc);
  void releaseBitmaps();
  void makeColorScale(const RGBColor &color);
  const RGBColor &getMapColor(const CPoint &p);
  bool valid(const CRect &r) const { return r.Width() > 0 && r.Height() > 0; }
  bool borderVisible() const;
  void drawColorMap(   CDC &dc);
  void drawColorScale( CDC &dc);
  void drawResultColor(CDC &dc);
  void drawMapPosition(CDC &dc);
  void drawScalePosition(CDC &dc);
  void drawBorder(CDC &dc);
  void drawLabels(CDC &dc, BOOL enabled);
  void drawLabel(CDC &dc, LabelledRect &r, BOOL enabled);
  void drawSurroundingEdge(CDC &dc, const CRect &r);
  int  getMapXCoord(float hue) const;
  int  getMapYCoord(float saturation) const;
  float getHue(int x) const;
  float getSaturation(int y) const;
  float getLumination(int y) const;
  void cleanup();
public:
  ColorMapGraphics();
  ~ColorMapGraphics();
  void init(const CRect &rect);
  void      setControl(CWnd *wnd);
  bool      isPropertiesDirty()  const { return m_propertiesDirty; }
  void      setCurrentLSHColor(  const LSHColor &lshColor);
  LSHColor  getCurrentLSHColor() const { return m_currentLSHColor; }
  void      setCurrentColor(COLORREF color);
  COLORREF  getCurrentColor()    const;
  COLORREF  getBackgroundColor() const { return m_backgroundColor; }

  void setCurrentMapPoint(const CPoint &p);    // m_mapRect contains p
  void setCurrentScalePoint(const CPoint &p);
  void setCurrentLumination(float lumination); // 0 <= lumination <= 1
  void setCurrentSaturation(float saturation); // 0 <= saturation <= 1
  void setCurrentHue(       float hue);        // 0 <= hue        <= 1
  void setHasBorder(      bool hasBorder);
  bool getHasBorder()     const { return m_hasBorder;  }
  void setIsSunken(       bool sunken);
  bool getIsSunken()      const { return m_sunken;     }
  void setHasClientEdge(  bool clientEdge);
  bool getHasClientEdge() const { return m_clientEdge; }
  void setHasStaticEdge(  bool staticEdge);
  bool getHasStaticEdge() const { return m_staticEdge; }
  void setHasModalFrame(  bool modalFrame);
  bool getHasModalFrame() const { return m_modalFrame; }
  void setTextAlignment(  TextAlignment textAlignment);
  TextAlignment getTextAlignment() const { return m_textAlignment; }
  void setCaption(const CString &caption);
  CString getCaption() const { return m_caption; }
  CPoint getCurrentMapPoint() const;
  CPoint getCurrentScalePoint() const;
  float getCurrentLumination()     const { return getCurrentLSHColor().m_lumination; }
  float getCurrentSaturation()     const { return getCurrentLSHColor().m_saturation; }
  float getCurrentHue()            const { return getCurrentLSHColor().m_hue;        }
  const CRect &getRect()           const { return m_rect;           }
  const CRect &getMapRect()        const { return m_mapRect;        }
  const CRect &getScaleRect()      const { return m_scaleRect;      }
  const CRect &getScalePosRect()   const { return m_scalePosRect;   }
  const CRect &getResultRect()     const { return m_resultRect;     }

  const CRect &getRedRect()        const { return m_redFieldRect;   }
  const CRect &getGreenRect()      const { return m_greenFieldRect; }
  const CRect &getBlueRect()       const { return m_blueFieldRect;  }
  const CRect &getHueRect()        const { return m_hueFieldRect;   }
  const CRect &getSaturationRect() const { return m_saturationFieldRect; }
  const CRect &getLuminationRect() const { return m_luminationFieldRect; }

  void draw(CDC &dc, const CRect &rect, BOOL enabled);
  void draw(CDC &dc, BOOL enabled);
};
