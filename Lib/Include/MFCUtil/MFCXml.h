#pragma once

#include <atltypes.h>
#include <XMLUtil.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const CPoint         &p);
void getValue(XMLDoc &doc, XMLNodePtr n,       CPoint         &p);
void setValue(XMLDoc &doc, XMLNodePtr n, const CSize          &s);
void getValue(XMLDoc &doc, XMLNodePtr n,       CSize          &s);

class D3PCOLOR;

void setValue(XMLDoc &doc, XMLNodePtr n, const D3PCOLOR       &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3PCOLOR       &v);

void setValue(XMLDoc &doc, XMLNodePtr n, const D3DCOLORVALUE  &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       D3DCOLORVALUE  &v);

class PolygonCurve2D;
class GlyphPolygon;
class GlyphCurveData;

void setValue(XMLDoc &doc, XMLNodePtr n, const PolygonCurve2D &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       PolygonCurve2D &v);

void setValue(XMLDoc &doc, XMLNodePtr n, const GlyphPolygon   &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       GlyphPolygon   &v);

void setValue(XMLDoc &doc, XMLNodePtr n, const GlyphCurveData &v);
void getValue(XMLDoc &doc, XMLNodePtr n,       GlyphCurveData &v);

class AnimationParameters;

void setValue(XMLDoc &doc, XMLNodePtr n, const AnimationParameters &d);
void getValue(XMLDoc &doc, XMLNodePtr n,       AnimationParameters &d);
