#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

// -------------------------------------Operators for CanonicalForm-------------------------------------------------------------------------
// Not using PLUS,(binary) MINUS,PROD,QUOT,ROOT,SQR,SQRT,EXP,COT,CSC,SEC

class NodeOperatorsCanonForm : public NodeOperators {
private:
  static inline ExpressionNode *addentExpr(     ExpressionNode *n, bool positive) {
    return n->getTree().addentExpr(n,positive);
  }
  static inline ExpressionNode *sumExpr(        AddentArray &a) {
    return a.getTree().sumExpr(a);
  }
  static inline ExpressionNode *productExpr(    FactorArray &fa) {
    return fa.getTree().productExpr(fa);
  }
  static inline ExpressionNode *unaryMinus(     ExpressionNode *n) {
    return n->getTree().unaryMinus(n);
  }
  static inline ExpressionNode *polyExpr(       SNodeArray &coefArray, SNode arg) {
    return arg.getTree().polyExpr(coefArray, arg);
  }
  static inline ExpressionNode *indexedSum(     SNode assign, SNode end, SNode expr) {
    return assign.getTree().indexedSum(assign,end,expr);
  }
  static inline ExpressionNode *indexedProduct( SNode assign, SNode end, SNode expr) {
    return assign.getTree().indexedProduct(assign,end,expr);
  }
  static inline ExpressionNode *powerExpr(      ExpressionNode *base, ExpressionNode *exponent) {
    return base->getTree().powerExpr(base,exponent);
  }
  static inline ExpressionNode *constExpr(      ExpressionNode *n, const TCHAR *name) {
    return n->getTree().constExpr(name);
  }
public:
  ExpressionNode *minus(     ExpressionNode *n) const;
  ExpressionNode *reciprocal(ExpressionNode *n) const;
  ExpressionNode *sqr(       ExpressionNode *n) const;
  ExpressionNode *sqrt(      ExpressionNode *n) const;
  ExpressionNode *exp(       ExpressionNode *n) const;
  ExpressionNode *exp10(     ExpressionNode *n) const;
  ExpressionNode *exp2(      ExpressionNode *n) const;
  ExpressionNode *cot(       ExpressionNode *n) const;
  ExpressionNode *csc(       ExpressionNode *n) const;
  ExpressionNode *sec(       ExpressionNode *n) const;

  ExpressionNode *sum(       ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *diff(      ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *prod(      ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *quot(      ExpressionNode *n1, ExpressionNode *n2) const;
  // Return a division node without reduction to rational. to be used with exponents ie sqrt(x^2) != x
  ExpressionNode *quot(      ParserTree *tree, INT64 num, INT64 den) const;
  ExpressionNode *mod(       ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *power(     ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *root(      ExpressionNode *n1, ExpressionNode *n2) const;
  ParserTreeForm  getTreeForm() const {
    return TREEFORM_CANONICAL;
  }
  void checkTreeFormConsistent(const ParserTree *tree) const {
    tree->checkIsCanonicalForm();
  }
};

class NodeOperatorsCanonNumForm : public NodeOperatorsCanonForm {
public:
  ParserTreeForm  getTreeForm() const {
    return TREEFORM_NUMERIC;
  }
};

ExpressionNode *NodeOperatorsCanonForm::minus(ExpressionNode *n) const {
  switch(n->getSymbol()) {
  case NUMBER    :
    return numberExpr(n,-n->getNumber());
  case MINUS     :
    assert(n->isUnaryMinus());
    return n->left();
  case ADDENT    :
    return addentExpr(n->left(),!n->isPositive());
  case SUM       :
    { const AddentArray &a  = n->getAddentArray();
      const size_t       sz = a.size();
      AddentArray        newAddentArray(a.getTree(),sz);
      for(size_t i = 0; i < sz; i++) { // change sign for all elements in list
        newAddentArray.add(-a[i]);
      }
      return sumExpr(newAddentArray);
    }

  case PRODUCT   :
    { const FactorArray &factors = n->getFactorArray();
      const size_t       sz      = factors.size();
      const int          index   = factors.findFactorWithChangeableSign();
      FactorArray        newFactors(factors.getTree(), sz);
      if(index < 0) {
        newFactors *= factors;
        newFactors *= -1;
      } else {
        for(size_t i = 0; i < sz; i++) {
          SNode factor = factors[i];
          if(i != index) {
            newFactors *= factor;
          } else {
            newFactors *= powerExp(-factor.base(),factor.exponent());
          }
        }
      }
      return productExpr(newFactors);
    }
  case POLY      :
    { const SNodeArray &coefArray = n->getCoefArray();
      const size_t      sz        = coefArray.size();
      SNodeArray        newCoefArray(coefArray.getTree(),sz);
      for(size_t i = 0; i < sz; i++) {
        newCoefArray.add(-coefArray[i]);
      }
      return polyExpr(newCoefArray, n->getArgument());
    }
  case INDEXEDSUM:
    return indexedSum(n->child(0), n->child(1), -n->child(2));
  }
  return unaryMinus(n);
}

ExpressionNode *NodeOperatorsCanonForm::reciprocal(ExpressionNode *n) const {
  switch(n->getSymbol()) {
  case NUMBER        :
    if(n->isRational()) {
      return numberExpr(n,::reciprocal(n->getRational()));
    }
    break;

  case PRODUCT       :
    { const FactorArray &factors = n->getFactorArray();
      FactorArray newFactors(n->getTree(), factors.size());
      newFactors /= factors;
      return productExpr(newFactors);
    }
  case INDEXEDPRODUCT:
    return indexedProduct(n->child(0), n->child(1), reciprocal(n->child(2).node()));
  case POW           : // reciprocal(l^r) = l^-r
    return power(n->left(), minus(n->right()));
  }
  return quot(getOne(n), n);
}

ExpressionNode *NodeOperatorsCanonForm::sum(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isRational() && n2->isRational()) {
    return numberExpr(n1, n1->getRational() + n2->getRational());
  } else if(n1->isZero()) {
    return n2;
  } else if(n2->isZero()) {
    return n1;
  }
  AddentArray a(n1->getTree(),2);
  a.add(addentExpr(n1, true));
  a.add(addentExpr(n2, true));
  return sumExpr(a);
}

ExpressionNode *NodeOperatorsCanonForm::diff(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isRational() && n2->isRational()) {
    return numberExpr(n1, n1->getRational() - n2->getRational());
  } else if(n2->isZero()) {
    return n1;
  } else if(n1->isZero()) {
    return minus(n2);
  }
  AddentArray a(n1->getTree(),2);
  a.add(addentExpr(n1, true ));
  a.add(addentExpr(n2, false));
  return sumExpr(a);
}

ExpressionNode *NodeOperatorsCanonForm::prod(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isZero() || n2->isZero()) {
    return getZero(n1);
  }
  if(n1->isRational() && n2->isRational()) {
    return numberExpr(n1, n1->getRational() * n2->getRational());
  }
  FactorArray a(n1->getTree(),2);
  a *= n1;
  a *= n2;
  return productExpr(a);
}

ExpressionNode *NodeOperatorsCanonForm::quot(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isZero()) {
    return getZero(n1);
  }
  if(n1->isRational() && n2->isRational()) {
    return numberExpr(n1, n1->getRational() / n2->getRational());
  } else if(n2->isOne()) {
    return n1;
  }
  FactorArray a(n1->getTree());
  a *= n1;
  a /= n2;
  return productExpr(a);
}

ExpressionNode *NodeOperatorsCanonForm::quot(ParserTree *tree, INT64 num, INT64 den) const {
  ExpressionNode *n1 = tree->numberExpr(num);
  ExpressionNode *n2 = tree->numberExpr(den);
  FactorArray a(*tree);
  a *= n1;
  a /= n2;
  ExpressionNode *q = productExpr(a);
  q->setReduced();
  return q;
}

ExpressionNode *NodeOperatorsCanonForm::mod(ExpressionNode *n1, ExpressionNode *n2) const {
  return binaryExpr(MOD, n1, n2);
}

ExpressionNode *NodeOperatorsCanonForm::sqr(ExpressionNode *n) const {
  return power(n, getTwo(n));
}

ExpressionNode *NodeOperatorsCanonForm::sqrt(ExpressionNode *n) const {
  return power(n, getHalf(n));
}

// doesn't always return a node of type NT_POW
ExpressionNode *NodeOperatorsCanonForm::power(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n2->isZero()) {
    return getOne(n1);
  } else if(n2->isOne()) {
    return n1;
  } else if(n2->isMinusOne()) {
    return reciprocal(n1);
  } else if(n1->getSymbol() == POW) {
    ParserTree &tree = n1->getTree();
    return powerExpr(n1->left(), tree.multiplyExponents(n1->right(), n2));
  } else {
    return powerExpr(n1, n2);
  }
}

ExpressionNode *NodeOperatorsCanonForm::root(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n2->isOne()) {
    return n1;
  } else if(n2->isMinusOne()) {
    return reciprocal(n1);
  }
  return power(n1, reciprocal(n2));
}

ExpressionNode *NodeOperatorsCanonForm::exp(ExpressionNode *n) const {
  return power(constExpr(n,_T("e")), n);
}

ExpressionNode *NodeOperatorsCanonForm::exp10(ExpressionNode *n) const {
  return power(getTen(n), n);
}

ExpressionNode *NodeOperatorsCanonForm::exp2(ExpressionNode *n) const {
  return power(getTwo(n), n);
}

ExpressionNode *NodeOperatorsCanonForm::cot(ExpressionNode *n) const {
  return reciprocal(funcExpr(TAN, n));
}

ExpressionNode *NodeOperatorsCanonForm::csc(ExpressionNode *n) const {
  return reciprocal(funcExpr(SIN, n));
}

ExpressionNode *NodeOperatorsCanonForm::sec(ExpressionNode *n) const {
  return reciprocal(funcExpr(COS, n));
}

static const NodeOperatorsCanonForm    canonFormOps;
static const NodeOperatorsCanonNumForm canonNumFormOps;

const NodeOperators *NodeOperators::s_canonForm    = &canonFormOps;
const NodeOperators *NodeOperators::s_canonNumForm = &canonNumFormOps;


// ------------------------------- tree conversion classes/functions ------------------
// ----------------------------------------- toCanonicalForm ------------------------------------------------------------------

class CNode : public SNode {
private:
  CNode toCForm()         const;
  CNode toCFormTreeNode() const;
  CNode toCFormPoly()     const;
  CNode toCFormSum()      const;
  CNode toCFormProduct()  const;
  AddentArray &toCFormSum(    AddentArray &result, bool   positive) const;
  FactorArray &toCFormProduct(FactorArray &result, SNode &exponent) const;
  FactorArray &toCFormPower(  FactorArray &result, SNode &exponent) const;
  FactorArray &toCFormRoot(   FactorArray &result, SNode &exponent) const;

  // assume !n.isEmpty()
  inline CNode(const SNode &n) : SNode(n) {
    CHECKISCONSISTENT(n);
  }
public:
  // assume n != NULL
  inline CNode(ExpressionNode *n) : SNode(n) {
    CHECKISCONSISTENT(*n);
  }
  SNode convert() const {
    ENTERMETHOD();
    RETURNNODE( toCForm() );
  }
};

SNode ParserTree::toCanonicalForm(SNode n) {
  if(getTreeForm() == TREEFORM_CANONICAL) {
    return n;
  }
  m_ops = NodeOperators::s_canonForm;
  STARTREDUCTION();
  if(n.isEmpty()) {
    return n;
  } else {
    return CNode(n.node()).convert();
  }
}

#define C(n)  CNode(n)

// Replace PLUS,(binary) MINUS,PROD,QUOT,ROOT,SQRT,SQR,EXP,EXP10,EXP2,SEC,CSC,COT and negative constanst,
// with combinations of SUM, (unary MINUS), PRODUCT, POW, SIN,COS,TAN, and positive constants
CNode CNode::toCForm() const {
  ENTERMETHOD();
  switch(getSymbol()) {
  case NUMBER         :
  case TYPEBOOL       :
  case NAME           : RETURNTHIS;

  case MINUS          : if(isUnaryMinus()) {
                          RETURNNODE(-C(left()).toCForm());
                        }
                        // NB continue case

  case PLUS           :
  case SUM            : RETURNNODE( toCFormSum() );

  case PRODUCT        :
  case PROD           :
  case QUOT           :
  case POW            :
  case SQR            :
  case ROOT           :
  case SQRT           :
  case EXP            :
  case EXP10          :
  case EXP2           : RETURNNODE( toCFormProduct() );

  case SEC            : RETURNNODE( sec(C(left()).toCForm()) );
  case CSC            : RETURNNODE( csc(C(left()).toCForm()) );
  case COT            : RETURNNODE( cot(C(left()).toCForm()) );
  case POLY           : RETURNNODE( toCFormPoly()     );
  default             : RETURNNODE( toCFormTreeNode() );
  }
}

CNode CNode::toCFormTreeNode() const {
  ENTERMETHOD();
  const SNodeArray &a  = getChildArray();
  const size_t      sz = a.size();
  SNodeArray newChildArray(a.getTree(),sz);
  for(size_t i = 0; i < sz; i++) {
    newChildArray.add(C(a[i]).toCForm());
  }
  RETURNNODE( treeExp(getSymbol(), newChildArray) );
}

CNode CNode::toCFormPoly() const {
  ENTERMETHOD();
  const SNodeArray &coefArray = getCoefArray();
  const size_t      sz        = coefArray.size();
  SNodeArray        newCoefArray(coefArray.getTree(), sz);
  for(size_t i = 0; i < sz; i++) {
    newCoefArray.add(C(coefArray[i]).toCForm());
  }
  SNode newArg = C(getArgument()).toCForm();
  RETURNNODE( polyExp(newCoefArray, newArg) );
}

CNode CNode::toCFormSum() const {
  ENTERMETHOD();
  AddentArray a(getTree());
  toCFormSum(a, true );
  RETURNNODE( sumExp(a) );
}

AddentArray &CNode::toCFormSum(AddentArray &result, bool positive) const {
  ENTERMETHOD2(*this,SNV(positive));
  switch(getSymbol()) {
  case NUMBER:
    if(isZero()) break; // don't bother about this
    if((getNumber() < 0) && !positive) { // -(-5) == +5
      result.add(addentExp(-*this,true));
      break;
    }
    // NB continue case
  case NAME  :
    result.add(addentExp(node(), positive));
    break;
  case SUM:
    { const AddentArray &a  = getAddentArray();
      const size_t       sz = a.size();
      for(size_t i = 0; i < sz; i++) {
        const SNode &e = a[i];
        C(e.left()).toCFormSum(result, e.isPositive() == positive);
      }
    }
    break;
  case PLUS:
    C(right()).toCFormSum(C(left()).toCFormSum(result, positive), positive);
    break;
  case MINUS:
    if(isUnaryMinus()) {
      C(left()).toCFormSum(result, !positive);
    } else {
      C(right()).toCFormSum(C(left()).toCFormSum(result, positive), !positive);
    }
    break;
  default:
    result.add(addentExp(toCForm(), positive));
    break;
  }
  RETURNSHOWSTR( result );
}

CNode CNode::toCFormProduct() const {
  ENTERMETHOD();
  FactorArray a(getTree());
  toCFormProduct(a, _1());
  RETURNNODE(productExp(a));
}

// Accumulate nodes in result
FactorArray &CNode::toCFormProduct(FactorArray &result, SNode &exponent) const {
  ENTERMETHOD2(*this, exponent);
  switch(getSymbol()) {
  case PRODUCT:
    { const FactorArray &a = getFactorArray();
      if(exponent.isOne()) {
        result *= a;
      } else {
        const size_t sz = a.size();
        for(size_t i = 0; i < sz; i++) {
          C(a[i]).toCFormProduct(result, exponent);
        }
      }
    }
    break;
  case PROD:
    C(right()).toCFormProduct(C(left()).toCFormProduct(result, exponent), exponent);
    break;
  case QUOT:
    C(right()).toCFormProduct(C(left()).toCFormProduct(result, exponent), -exponent);
    break;
  case POW :
    { FactorArray tmp(getTree());
      result *= toCFormPower(tmp, exponent);
    }
    break;
  case ROOT:
    { CNode s = root(C(left()).toCForm(), C(right()).toCForm());
      if((s.getNodeType() == NT_POWER) && exponent.isOne()) {
        result *= s;
      } else {
        s.toCFormProduct(result,exponent);
      }
    }
    break;
  case SQR:
    { CNode s = sqr(C(left()).toCForm());
      if((s.getNodeType() == NT_POWER) && exponent.isOne()) {
        result *= s;
      } else {
        s.toCFormProduct(result,exponent);
      }
    }
    break;
  case SQRT:
    { CNode s = sqrt(C(left()).toCForm());
      if((s.getNodeType() == NT_POWER) && exponent.isOne()) {
        result *= s;
      } else {
        s.toCFormProduct(result,exponent);
      }
    }
    break;
  case EXP:
    { CNode s = exp(C(left()).toCForm());
      s.toCFormProduct(result, exponent);
    }
    break;
  case EXP10:
    { CNode s = exp10(C(left()).toCForm());
      s.toCFormProduct(result, exponent);
    }
    break;
  case EXP2:
    { CNode s = exp2(C(left()).toCForm());
      s.toCFormProduct(result, exponent);
    }
    break;
  case NUMBER:
    if(isOne()) break; // don't bother about this
    // NB continue case
  case NAME  :
    result *= powerExp(*this, exponent);
    break;
  default:
    result *= powerExp(toCForm(), exponent);
    break;
  }
  RETURNSHOWSTR(result);
}

// n.symbol == POW,
FactorArray &CNode::toCFormPower(FactorArray &result, SNode &exponent) const {
  ENTERMETHOD2(*this, exponent);
  SNode base = C(left()).toCForm();
  SNode expo = C(right()).toCForm();

  switch(base.getSymbol()) {
  case POW :
    result *= powerExp(base.left(), multiplyExponents(multiplyExponents(base.right(),expo), exponent));
    break;
  case ROOT:
    throwInvalidSymbolForTreeMode(__TFUNCTION__);
    break;
  default                   :
    if(exponent.isOne()) {
      result *= powerExp(base, expo);
    } else if(expo.isOne()) {
      result *=powerExp(base, exponent);
    } else {
      result *= powerExp(base, multiplyExponents(expo, exponent));
    }
    break;
  }
  RETURNSHOWSTR(result);
}

// n.symbol = ROOT
FactorArray &CNode::toCFormRoot(FactorArray &result, SNode &exponent) const {
  ENTERMETHOD2(*this, exponent);
  SNode rad  = C(left()).toCForm();
  SNode root = C(right()).toCForm();

  switch(rad.getSymbol()) {
  case POW :
    result *= powerExp(rad.left(), multiplyExponents(divideExponents(rad.right(), root), exponent));
    break;
  case ROOT:
    throwInvalidSymbolForTreeMode(__TFUNCTION__);
    break;
  default                   :
    result *= powerExp(rad, divideExponents(exponent,root));
    break;
  }
  RETURNSHOWSTR( result );
}

// -------------------------------------------------------------------------------------------------------

class CanonicalFormChecker : public ExpressionNodeHandler {
private:
  String m_error;
  bool   m_ok;
public:
  CanonicalFormChecker() : m_ok(true) {
  }

  bool handleNode(ExpressionNode *n, int level);
  bool isOk() const {
    return m_ok;
  }
  const String &getErrorMessage() const {
    return m_error;
  }
};

bool CanonicalFormChecker::handleNode(ExpressionNode *n, int level) {
  static const ExpressionSymbolSet illegalSymbolSet(
    PLUS  ,PROD  ,QUOT  ,ROOT  ,SQRT
   ,SQR   ,EXP   ,EXP10 ,EXP2  ,SEC
   ,CSC   ,COT
   ,EOI
  );

  if(illegalSymbolSet.contains(n->getSymbol()) || n->isBinaryMinus()) {
    m_error = format(_T("Illegal symbol in canonical form:<%s>. node=<%s>")
                    ,n->getSymbolName().cstr()
                    ,n->toString().cstr());
    m_ok = false;
    return false;
  }
  return true;
}

void ParserTree::checkIsCanonicalForm() const {
  CanonicalFormChecker checker;
  ((ParserTree*)this)->traverseTree(checker);
  if(!checker.isOk()) {
    throwException(checker.getErrorMessage());
  }
}

}; // namespace Expr
