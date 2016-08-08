#pragma once

#include "Number.h"

class ParserTree;
class ExpressionNode;
class AddentArray;

class SumElement {
  friend class ParserTree;

private:
  DECLARECLASSNAME;
  ExpressionNode *m_n;
  bool            m_positive; // true for +, false for -
  bool            m_marked;   // used for garbage-collection
public:
  SumElement(ExpressionNode *n, bool positive);
  SumElement *clone(ParserTree *tree) const;

  inline void mark() {
    m_marked = true;
  }
  inline void unMark() {
    m_marked = false;
  }
  inline bool isMarked() const {
    return m_marked;
  }
  const ExpressionNode *createExpressionNode() const;

  inline ExpressionNode *getNode() {
    return m_n;
  }
  inline const ExpressionNode *getNode() const {
    return m_n;
  }

  inline bool isPositive() const {
    return m_positive;
  }
  
  bool isConstant() const;
  
  int compare(const SumElement *e) const;

  String toString() const;
};
