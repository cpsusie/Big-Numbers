#pragma once

class BitSetParameters {
private:
  const ElementType m_type;
  const UINT        m_capacity;
public:
  BitSetParameters(ElementType type, UINT capacity)
    : m_type(       type       )
    , m_capacity(   capacity   )
  {
  }
  inline ElementType getType() const {
    return m_type;
  }
  inline UINT getCapacity() const {
    return m_capacity;
  }
  inline const TCHAR *getElementName(bool plur) const {
    return ElementName::getElementName(getType(), plur);
  }
};


