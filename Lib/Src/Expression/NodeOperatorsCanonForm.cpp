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
  static inline ExpressionNode *sumExpr(        SNodeArray &a) {
    return a.getTree().sumExpr(a);
  }
  static inline ExpressionNode *productExpr(    FactorArray &fa) {
    return fa.getTree().productExpr(fa);
  }
  static inline ExpressionNode *unaryMinus(     ExpressionNode *n) {
    return n->getTree().unaryMinus(n);
  }
  static inline ExpressionNode *polyExpr(       SNodeArray &coefArray, ExpressionNode *arg) {
    return arg->getTree().polyExpr(coefArray, arg);
  }
  static inline ExpressionNode *indexedSum(     ExpressionNode *assign, ExpressionNode *end, ExpressionNode *expr) {
    return assign->getTree().indexedSum(assign,end,expr);
  }
  static inline ExpressionNode *indexedProduct( ExpressionNode *assign, ExpressionNode *end, ExpressionNode *expr) {
    return assign->getTree().indexedProduct(assign,end,expr);
  }
  static inline ExpressionNode *factorExpr(     ExpressionNode *base, ExpressionNode *exponent) {
    return base->getTree().factorExpr(base,exponent);
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
    { const SNodeArray &a  = n->getChildArray();
      const size_t      sz = a.size();
      SNodeArray        newChildArray(a.getTree(),sz);
      for(size_t i = 0; i < sz; i++) { // change sign for all elements in list
        newChildArray.add(-a[i]);
      }
      return sumExpr(newChildArray);
    }

  case PRODUCT   :
    { const FactorArray &factors = n->getFactorArray();
      const size_t       sz      = factors.size();
      const int          index   = factors.findFactorWithChangeableSign();
      FactorArray        newFactors(factors.getTree(), sz);
      for(size_t i = 0; i < sz; i++) {
        SNode factor = factors[i];
        if(i != index) {
          newFactors.add(factor);
        } else {
          newFactors.add(factorExp(-factor.base(),factor.exponent()));
        }
      }
      if(index < 0) {
        newFactors.add(factorExp(getMinusOne(n),1));
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
      ExpressionNode *arg = n->getArgument().node();
      return polyExpr(newCoefArray, arg);
    }

  case INDEXEDSUM:
    return indexedSum(n->child(0).node(), n->child(1).node(), (-n->child(2)).node());
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
      const size_t       sz        = factors.size();
      FactorArray newFactors(factors.getTree(), sz);
      for(size_t i = 0; i < sz; i++) {
        SNode factor = factors[i];
        newFactors.add(factorExp(factor.base(), -factor.exponent()));
      }
      return productExpr(newFactors);
    }
  case INDEXEDPRODUCT:
    return indexedProduct(n->child(0).node(), n->child(1).node(), reciprocal(n->child(2).node()));
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
  SNodeArray a(n1->getTree(),2);
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
  SNodeArray a(n1->getTree(),2);
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
  a.add(factorExp(n1,1));
  a.add(factorExp(n2,1));
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
  a.add(factorExp(n1,1));
  if(n2->getSymbol() != PRODUCT) {
    a.add((n2->getSymbol() == POW) ? factorExp(n2->left(), minus(n2->right())) : factorExp(n2, -1));
  } else {
    const FactorArray &a2 = n2->getFactorArray();
    const size_t       sz = a2.size();

    for(size_t i = 0; i < sz; i++) {
      SNode f = a2[i];
      a.add(factorExp(f.base(), -f.exponent()));
    }
  }
  return productExpr(a);
}

ExpressionNode *NodeOperatorsCanonForm::quot(ParserTree *tree, INT64 num, INT64 den) const {
  ExpressionNode *n1 = tree->numberExpr(num);
  ExpressionNode *n2 = tree->numberExpr(den);
  FactorArray a(*tree);
  a.add(factorExp(n1, 1));
  a.add(factorExp(n2,-1));
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

ExpressionNode *NodeOperatorsCanonForm::power(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n2->isZero()) {
    return getOne(n1);
  } else if(n2->isOne()) {
    return n1;
  } else if(n2->isMinusOne()) {
    return reciprocal(n1);
  } else if(n1->getSymbol() == POW) {
    ParserTree &tree = n1->getTree();
    return factorExpr(n1->left(), tree.multiplyExponents(n1->right(), n2));
  } else {
    return factorExpr(n1, n2);
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
  SNodeArray  &toCFormSum(    SNodeArray  &result, bool   positive) const;
  FactorArray &toCFormProduct(FactorArray &result, SNode &exponent) const;
  FactorArray &toCFormPower(  FactorArray &result, SNode &exponent) const;
  FactorArray &toCFormRoot(   FactorArray &result, SNode &exponent) const;

  inline CNode(const SNode &n) : SNode(n) {
    CHECKISCONSISTENT(n);
  }
public:
  inline CNode(ExpressionNode *n) : SNode(n) {
    CHECKISCONSISTENT(*n);
  }
  CNode convert() const {
    ENTERMETHOD();
    RETURNNODE( toCForm() );
  }
};

ExpressionNode *ParserTree::toCanonicalForm(ExpressionNode *n) {
  if((getTreeForm() == TREEFORM_CANONICAL) || (n == NULL)) {
    return n;
  }
  m_ops = NodeOperators::s_canonForm;
  STARTREDUCTION(this);
  return CNode(n).convert().node();
}


#define N(n)  CNode(n)
#define NV(v) SNode(getTree(),v)

// Replace PLUS,(binary) MINUS,PROD,QUOT,ROOT,SQRT,SQR,EXP,EXP10,EXP2,SEC,CSC,COT and negative constanst,
// with combinations of SUM, (unary MINUS), PRODUCT, POW, SIN,COS,TAN, and positive constants
CNode CNode::toCForm() const {
  ENTERMETHOD();
  switch(getSymbol()) {
  case NUMBER         :
  case TYPEBOOL       :
  case NAME           : RETURNNODE(*this);

  case MINUS          : if(isUnaryMinus()) {
                          RETURNNODE(-N(left()).toCForm());
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

  case SEC            : RETURNNODE( sec(N(left()).toCForm()) );
  case CSC            : RETURNNODE( csc(N(left()).toCForm()) );
  case COT            : RETURNNODE( cot(N(left()).toCForm()) );
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
    newChildArray.add(N(a[i]).toCForm());
  }
  RETURNNODE( treeExp(getSymbol(), newChildArray) );
}

CNode CNode::toCFormPoly() const {
  ENTERMETHOD();

  const SNodeArray &coefArray = getCoefArray();
  const size_t      sz        = coefArray.size();
  SNodeArray        newCoefArray(coefArray.getTree(), sz);
  for(size_t i = 0; i < sz; i++) {
    newCoefArray.add(N(coefArray[i]).toCForm());
  }
  SNode newArg = N(getArgument()).toCForm();
  RETURNNODE( polyExp(newCoefArray, newArg) );
}

CNode CNode::toCFormSum() const {
  ENTERMETHOD();
  SNodeArray a(getTree());
  toCFormSum(a, true );
  RETURNNODE( sumExp(a) );
}

SNodeArray &CNode::toCFormSum(SNodeArray &result, bool positive) const {
  ENTERMETHOD2(*this,SNode(getTree(),positive));
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
    { const SNodeArray &a  = getChildArray();
      const size_t      sz = a.size();
      for(size_t i = 0; i < sz; i++) {
        const SNode &e = a[i];
        N(e.left()).toCFormSum(result, e.isPositive() == positive);
      }
    }
    break;
  case PLUS:
    N(right()).toCFormSum(N(left()).toCFormSum(result, positive), positive);
    break;
  case MINUS:
    if(isUnaryMinus()) {
      N(left()).toCFormSum(result, !positive);
    } else {
      N(right()).toCFormSum(N(left()).toCFormSum(result, positive), !positive);
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
  String thisStr = toString();
  String resStr  = result.toString();
  String expoStr = exponent.toString();
  switch(getSymbol()) {
  case PRODUCT:
    { const FactorArray &a = getFactorArray();
      if(exponent.isOne()) {
        result.addAll(a);
      } else {
        const size_t sz = a.size();
        for(size_t i = 0; i < sz; i++) {
          N(a[i]).toCFormProduct(result, exponent);
        }
      }
    }
    break;
  case PROD:
    N(right()).toCFormProduct(N(left()).toCFormProduct(result, exponent), exponent);
    break;
  case QUOT:
    N(right()).toCFormProduct(N(left()).toCFormProduct(result, exponent), -exponent);
    break;
  case POW :
    { FactorArray tmp(getTree());
      result.addAll(toCFormPower(tmp, exponent));
    }
    break;
  case ROOT:
    { CNode s = root(N(left()).toCForm(), N(right()).toCForm());
      if((s.getNodeType() == NT_FACTOR) && exponent.isOne()) {
        result.add(s);
      } else {
        s.toCFormProduct(result,exponent);
      }
    }
    break;
  case SQR:
    { CNode s = sqr(N(left()).toCForm());
      if((s.getNodeType() == NT_FACTOR) && exponent.isOne()) {
        result.add(s);
      } else {
        s.toCFormProduct(result,exponent);
      }
    }
    break;
  case SQRT:
    { CNode s = sqrt(N(left()).toCForm());
      if((s.getNodeType() == NT_FACTOR) && exponent.isOne()) {
        result.add(s);
      } else {
        s.toCFormProduct(result,exponent);
      }
    }
    break;
  case EXP:
    { CNode s = exp(N(left()).toCForm());
      s.toCFormProduct(result, exponent);
    }
    break;
  case EXP10:
    { CNode s = exp10(N(left()).toCForm());
      s.toCFormProduct(result, exponent);
    }
    break;
  case EXP2:
    { CNode s = exp2(N(left()).toCForm());
      s.toCFormProduct(result, exponent);
    }
    break;
  case NUMBER:
    if(isOne()) break; // don't bother about this
    // NB continue case
  case NAME  :
    result.add(factorExp(*this, exponent));
    break;
  default:
    result.add(factorExp(toCForm(), exponent));
    break;
  }
  RETURNSHOWSTR(result);
}

// n.symbol == POW,
FactorArray &CNode::toCFormPower(FactorArray &result, SNode &exponent) const {
  ENTERMETHOD2(*this, exponent);
  SNode base = N(left()).toCForm();
  SNode expo = N(right()).toCForm();

  switch(base.getSymbol()) {
  case POW :
    result.add(factorExp(base.left(), multiplyExponents(multiplyExponents(base.right(),expo), exponent)));
    break;
  case ROOT:
    throwInvalidSymbolForTreeMode(__TFUNCTION__);
    break;
  default                   :
    if(exponent.isOne()) {
      result.add(factorExp(base, expo));
    } else if(expo.isOne()) {
      result.add(factorExp(base, exponent));
    } else {
      result.add(factorExp(base, multiplyExponents(expo, exponent)));
    }
    break;
  }
  RETURNSHOWSTR(result);
}

// n.symbol = ROOT
FactorArray &CNode::toCFormRoot(FactorArray &result, SNode &exponent) const {
  ENTERMETHOD2(*this, exponent);
  SNode rad  = N(left()).toCForm();
  SNode root = N(right()).toCForm();

  switch(rad.getSymbol()) {
  case POW :
    result.add(factorExp(rad.left(), multiplyExponents(divideExponents(rad.right(), root), exponent)));
    break;
  case ROOT:
    throwInvalidSymbolForTreeMode(__TFUNCTION__);
    break;
  default                   :
    result.add(factorExp(rad, divideExponents(exponent,root)));
    break;
  }
  RETURNSHOWSTR( result );
}

// -------------------------------------------------------------------------------------------------------

class CanonicalFormChecker : public ExpressionNodeHandler {
private:
  String                           m_error;
  bool                             m_ok;
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

void ParserTree::checkIsCanonicalForm() {
  CanonicalFormChecker checker;
  traverseTree(checker);
  if(!checker.isOk()) {
    throwException(checker.getErrorMessage());
  }
}

}; // namespace Expr
