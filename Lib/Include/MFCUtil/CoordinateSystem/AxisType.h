#pragma once

#include <Math/Transformation.h>

typedef enum {
  AXIS_LINEAR
 ,AXIS_LOGARITHMIC
 ,AXIS_NORMAL_DISTRIBUTION
 ,AXIS_DATE
} AxisType;

IntervalScale getScale(AxisType type);

typedef BYTE AxisFlags;

#define AXIS_SHOW_VALUES      0x01
#define AXIS_SHOW_VALUEMARKS  0x02
#define AXIS_SHOW_GRIDLINES   0x04

class AxisAttribute {
private:
  AxisType  m_type;
  COLORREF  m_color;
  AxisFlags m_flags;
public:
  AxisAttribute();
  inline void setType(AxisType type) {
    m_type = type;
  }
  inline AxisType getType() const {
    return m_type;
  }
  inline void setColor(COLORREF color) {
    m_color = color;
  }
  inline COLORREF getColor() const {
    return m_color;
  }
  inline void setFlags(AxisFlags add, AxisFlags remove) {
    m_flags |= add; m_flags &= ~remove;
  }
  inline AxisFlags getFlags() const {
    return m_flags;
  }
};

typedef enum {
  XAXIS_INDEX
 ,YAXIS_INDEX
} AxisIndex;
