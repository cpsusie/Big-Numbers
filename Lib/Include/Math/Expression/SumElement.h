#pragma once

#include "Number.h"

class ParserTree;
class ExpressionNode;
class AddentArray;

class SumElement {
#ifdef _DEBUG
private:
  String                m_debugString;
  static bool           s_debugStringEnabled;
  void initDebugString();
public:
  inline const String &getDebugString() const { return m_debugString; }
#define INITSUMELEMENTDEBUGSTRING() { if(SumElement::s_debugStringEnabled) initDebugString(); }
#else
#define INITSUMELEMENTDEBUGSTRING()
#endif

  friend class ParserTree;

private:
  DECLARECLASSNAME;
  const ExpressionNode *m_n;
  bool                        m_positive; // true for +, false for -
  mutable bool                m_marked;   // used for garbage-collection
public:
  SumElement(const ExpressionNode *n, bool positive);
  SumElement *clone(const ParserTree *tree) const;

  inline void mark() const {
    m_marked = true;
  }
  inline void unMark() const {
    m_marked = false;
  }
  inline bool isMarked() const {
    return m_marked;
  }
  const ExpressionNode *createExpressionNode() const;

  inline const ExpressionNode *getNode() const {
    return m_n;
  }

  inline bool isPositive() const {
    return m_positive;
  }
  
  bool isConstant() const;
  
  int compare(const SumElement *e) const;

  String toString() const;

  inline static bool enableDebugString(bool enabled) {
#ifdef _DEBUG
    const bool ret = s_debugStringEnabled;
    s_debugStringEnabled = enabled;
    return ret;
#else
    return false;
#endif
  }
};
