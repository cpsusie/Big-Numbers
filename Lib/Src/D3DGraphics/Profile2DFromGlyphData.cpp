#include "pch.h"
#include <MFCUtil/PixRect.h>
#include <D3DGraphics/Profile2D.h>

ProfilePolygon2D::ProfilePolygon2D(const GlyphPolygon &gp) {
  m_start  = gp.getStart();
  m_closed = true;
  const Array<PolygonCurve2D> &a = gp.getCurveArray();
  const size_t                 n = a.size();
  m_curveArray.setCapacity(n);
  for(size_t i = 0; i < n; i++) {
    m_curveArray.add(a[i]);
  }
}

Profile2D::Profile2D(const GlyphCurveData &glyphData, const String &name) : _PersistentData(name) {
  const Array<GlyphPolygon> &a = glyphData.getPolygonArray();
  const size_t n = a.size();
  m_polygonArray.setCapacity(n);
  for(size_t i = 0; i < n; i++) {
    addPolygon(ProfilePolygon2D(a[i]));
  }
}
