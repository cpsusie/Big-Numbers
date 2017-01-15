#pragma once

namespace ThreeD {

#ifdef USE_MATERIAL

class Color {
protected:
  float _r, _g, _b;
public:
  Color(float red = 0.0f, float green = 0.0f, float blue = 0) : _r(red), _g(green), _b(blue) {
  }

  /*
  Color(float red, float green, float blue)
      : _r((int)(red / 65535.0f)), _g((int)(green / 65535.0f)), _b((int)(blue / 65535.0f)) {};
  */

  float red()   const { return _r; }
  float green() const { return _g; }
  float blue()  const { return _b; }
  
  friend inline bool operator==(const Color &a, const Color &b);
  friend inline bool operator!=(const Color &a, const Color &b);

  Color &operator+=(const Color &v) { return operator=(*this + v); }
  Color &operator-=(const Color &v) { return operator=(*this - v); }
  Color &operator*=(const Color &v) { return operator=(*this * v); }
  Color &operator/=(const Color &v) { return operator=(*this / v); }

  Color &operator+=(float f) { return operator=(*this + f); }
  Color &operator-=(float f) { return operator=(*this - f); }
  Color &operator*=(float f) { return operator=(*this * f); }
  Color &operator/=(float f) { return operator=(*this / f); }

  Color &operator+=(double f) { return operator=(*this + f); }
  Color &operator-=(double f) { return operator=(*this - f); }
  Color &operator*=(double f) { return operator=(*this * f); }
  Color &operator/=(double f) { return operator=(*this / f); }

  Color operator+() const { return *this; }
  Color operator-() const { return Color(-_r, -_g, -_b); }

  friend inline Color operator+(const Color &a, const Color &b);
  friend inline Color operator-(const Color &a, const Color &b);
  friend inline Color operator*(const Color &a, const Color &b);
  friend inline Color operator/(const Color &a, const Color &b);

  friend inline Color operator+(const Color &v, float f);
  friend inline Color operator-(const Color &v, float f);
  friend inline Color operator*(const Color &v, float f);
  friend inline Color operator/(const Color &v, float f);

  friend inline Color operator+(const Color &v, double f);
  friend inline Color operator-(const Color &v, double f);
  friend inline Color operator*(const Color &v, double f);
  friend inline Color operator/(const Color &v, double f);
};

inline bool operator==(const Color &a, const Color &b) {
  return (a._r == b._r) && (a._g == b._g) && (a._b == b._b);
}

inline bool operator!=(const Color &a, const Color &b) {
  return (a._r != b._r) || (a._g != b._g) || (a._b != b._b);
}

inline Color operator+(const Color &a, const Color &b) {
  return Color( (a._r + b._r), (a._g + b._g), (a._b + b._b) );
}

inline Color operator-(const Color &a, const Color &b) {
  return Color( (a._r - b._r), (a._g - b._g), (a._b - b._b) );
}

inline Color operator*(const Color &a, const Color &b) {
  return Color( (a._r * b._r), (a._g * b._g), (a._b * b._b) );
}

inline Color operator/(const Color &a, const Color &b) {
  return Color( (a._r / b._r), (a._g / b._g), (a._b / b._b) );
}

inline Color operator+(const Color &v, float f) {
  return Color( (v._r + f), (v._g + f), (v._b + f) );
}

inline Color operator-(const Color &v, float f) {
  return v + (-f);
}

inline Color operator*(const Color &v, float f) {
  return Color( (v._r * f), (v._g * f), (v._b * f) );
}

inline Color operator/(const Color &v, float f) {
  return v * (1.0 / f);
}

inline Color operator+(const Color &v, double f) {
  return Color( (float)(v._r + f), (float)(v._g + f), (float)(v._b + f) );
}

inline Color operator-(const Color &v, double f) {
  return v + (-f);
}

inline Color operator*(const Color &v, double f) {
  return Color( (float)(v._r * f), (float)(v._g * f), (float)(v._b * f) );
}

inline Color operator/(const Color &v, double f) {
  return v * (1.0 / f);
}

typedef struct {
  Color color;
  Color ambient;
  float diffuse;
  float brilliance;
  float specular;
} Material;

Material avg3(const Material &m1, const Material &m2, const Material &m3);

#endif // USE_MATERIAL

} // namespace ThreeD

