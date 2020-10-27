#include "stdafx.h"
#include <MyUtil.h>
#include <Math/BigReal/BigRealResourcePool.h>
#include "Calculator.h"
#include "resource.h"

static void throwInvalidInput() {
  throwBigRealException(_T("Invalid input for function"));
}

#define assertion(expr) if(!(expr)) throwInvalidInput()

#define _0 pool->_0()
#define _1 pool->_1()
#define _2 pool->_2()

BigReal Rdms(const BigReal &x, int ndigits, DigitPool *digitPool = nullptr) {
  _SELECTDIGITPOOL(x);
  return dms(x, e(_1, -ndigits, pool));
}

BigReal Rinversdms(const BigReal &x, int ndigits, DigitPool *digitPool = nullptr) {
  _SELECTDIGITPOOL(x);
  return inversdms(x, e(_1, -ndigits, pool));
}

BigReal Rfac(const BigReal &x, int ndigits, DigitPool *digitPool = nullptr) {
  _SELECTDIGITPOOL(x);
  BigReal result = _1;
  if(isInteger(x)) {
    assertion(x >= 0);
    for(BigReal n = BigReal(x,pool); n.isPositive(); --n) {
      result = rProd(result, n, ndigits);
    }
    return result;
  } else {
    return rFactorial(x, ndigits, pool);
  }
}

BigReal Calculator::toRadians(const BigReal &x) const {
  DigitPool *pool = getDigitPool();
  switch(m_trigonometricBase) {
  case TRIGO_RADIANS:
    return x;
  case TRIGO_DEGREES:
  case TRIGO_GRADS  :
    { BRExpoType fpe;
      const BigReal base((m_trigonometricBase == TRIGO_DEGREES) ? 180 : 200,pool);
      const BigReal tmp = x % (base*_2);
      const BRExpoType xe = BigReal::getExpo10(tmp);
      if(xe < 0) {
        fpe = xe;
      } else { // expo(tmp) >= 0
        fpe = -xe - abs(tmp.getLow()) * BIGREAL_LOG10BASE;
      }

      const BigReal fp(e(_1, fpe - m_ndigits - 8,pool),pool);
      const BigReal Pi = pi(fp,pool);
      return Pi * quot(tmp, base, fp);
    }
  }
  return x; // actually an error
}

BigReal Calculator::fromRadians(const BigReal &x) const {
  DigitPool *pool = getDigitPool();
  const BRExpoType xe = BigReal::getExpo10(x);
  const BRExpoType fe = __min(0,xe);
  const BigReal f = e(_1, fe - m_ndigits - 3,pool);
  switch(m_trigonometricBase) {
  case TRIGO_RADIANS: return x;
  case TRIGO_DEGREES: return BigReal(180,pool) * rQuot(x, pi(f), m_ndigits+3);
  case TRIGO_GRADS  : return BigReal(200,pool) * rQuot(x, pi(f), m_ndigits+3);
  }
  return x;
}

static BigReal rSqr(const BigReal &x, int ndigits, DigitPool *digitPool = nullptr) {
  _SELECTDIGITPOOL(x);
  return rProd(x, x, ndigits, pool);
}

static BigReal rCubicRoot(const BigReal &x, int ndigits, DigitPool *digitPool = nullptr) {
  _SELECTDIGITPOOL(x);
  BigReal ex3 = rQuot(_1, BigReal(3,pool), 2*ndigits,pool);
  if(x > 0) {
    return rPow(x, ex3, ndigits, pool);
  } else {
    return -rPow(-x, ex3, ndigits, pool);
  }
}

static BigReal rPow3(const BigReal &x, int ndigits, DigitPool *digitPool = nullptr) {
  _SELECTDIGITPOOL(x);
  return rProd(rSqr(x, ndigits,pool), x, ndigits,pool);
}

static BigReal rSinh(const BigReal &x, int ndigits, DigitPool *digitPool = nullptr) {
  _SELECTDIGITPOOL(x);
  return rDif(rExp(x, ndigits, pool), rExp(-x, ndigits, pool), ndigits, pool).divide2();
}

static BigReal rCosh(const BigReal &x, int ndigits, DigitPool *digitPool = nullptr) {
  _SELECTDIGITPOOL(x);
  return rSum(rExp(x, ndigits,pool), rExp(-x, ndigits, pool), ndigits, pool).divide2();
}

static BigReal rTanh(const BigReal &x, int ndigits, DigitPool *digitPool = nullptr) {
  _SELECTDIGITPOOL(x);
  const BigReal e1 = rExp(x, ndigits, pool);
  const BigReal e2 = rExp(-x, ndigits, pool);
  return rQuot((e1-e2),(e1+e2), ndigits, pool);
}

static BigReal rAcosh(const BigReal &x, int ndigits, DigitPool *digitPool = nullptr) {
  _SELECTDIGITPOOL(x);
  return rLn(x + rSqrt(x*x - _1, ndigits, pool), ndigits, pool);
}

static BigReal rAsinh(const BigReal &x, int ndigits, DigitPool *digitPool = nullptr) {
  _SELECTDIGITPOOL(x);
  return rLn(x + rSqrt(x*x + _1, ndigits, pool), ndigits, pool);
}

static BigReal rAtanh(const BigReal &x, int ndigits, DigitPool *digitPool = nullptr) {
  _SELECTDIGITPOOL(x);
  return rLn(rSqrt(rQuot((_1 + x), (_1 - x), ndigits, pool), ndigits, pool), ndigits, pool);
}

void Calculator::initDisplay() {
  m_inBigReal      = false;
  m_hasComma       = false;
  m_inExponent     = false;
  m_lastWasBinOp   = false;
  m_gotError       = false;
  _tcscpy(m_mantissa, _T("0"));
  _tcscpy(m_exponent, _T("+0"));
  ajourDisplay();
}

void Calculator::initMemory() {
  m_memory = 0;
}

void Calculator::init() {
  initDisplay();
  m_numberStack.clear();
  m_opStack.clear();
  BigRealResourcePool::resetAllPoolCalculations();
//  m_numberstacktop = 0;
//  m_opstacktop     = 0;
  m_inverse        = false;
  m_hyperbolic     = false;
  m_lastWasBinOp   = false;
  m_paranthesLevel = 0;
}

Calculator::Calculator() : m_minusOne(-1), m_hasInput(0) {
  m_busy           = false;
  m_killed         = false;
  m_digitGrouping  = false;
  m_gotError       = false;
  m_radix          = 10;
  init();
  initMemory();
  m_trigonometricBase = TRIGO_DEGREES;
  m_opsize            = OPSIZE_QWORD;
  m_ndigits           = 40;
}

Calculator::~Calculator() {
  m_killed = true;
  if(isBusy()) {
    terminateCalculation();
  } else {
    enter(0);
  }
  waitUntilJobDone();
}

static void appendChar(TCHAR *s, char ch) {
  TCHAR tmp[2];
  tmp[0] = ch;
  tmp[1] = '\0';
  _tcscat(s,tmp);
}

unsigned int Calculator::maxBigRealLen() const {
  TCHAR f = m_mantissa[0];
  switch(m_radix) {
  case 10:
    return sizeof(m_mantissa) - 1;
  case 2:
    switch(m_opsize) {
    case OPSIZE_BYTE : return 8;
    case OPSIZE_WORD : return 16;
    case OPSIZE_DWORD: return 32;
    case OPSIZE_QWORD: return 64;
    case OPSIZE_OWORD: return 128;
    }
  case 8 :
    switch(m_opsize) {
    case OPSIZE_BYTE : return 2  + ((f <= '3')?1:0);
    case OPSIZE_WORD : return 5  + ((f == '1')?1:0);
    case OPSIZE_DWORD: return 10 + ((f <= '3')?1:0);
    case OPSIZE_QWORD: return 21 + ((f == '1')?1:0);
    case OPSIZE_OWORD: return 42 + ((f <= '3')?1:0);
    }
  case 16:
    switch(m_opsize) {
    case OPSIZE_BYTE : return 2;
    case OPSIZE_WORD : return 4;
    case OPSIZE_DWORD: return 8;
    case OPSIZE_QWORD: return 16;
    case OPSIZE_OWORD: return 32;
    }
  }
  return 10;
}

void Calculator::ajourDisplay() {
  const String tmp = m_digitGrouping ? groupDigits(m_mantissa) : m_mantissa;
  putDisplayText(m_inExponent ? (tmp + format(_T("e%s"), m_exponent)) : tmp);
  m_displayDirty = true;
}

void Calculator::handleNumButton(char ch) {
  int value;
  if(isdigit(ch)) {
    value = ch - '0';
  } else {
    value = ch - 'A' + 10;
  }
  if(value >= m_radix) {
    return;
  }

  if(m_inBigReal) {
    if(m_inExponent) {
      if(m_exponent[1] == '0') {
        m_exponent[1] = ch;
      } else {
        appendChar(m_exponent,ch);
      }
    } else {
      if(_tcslen(m_mantissa) < maxBigRealLen()) {
        appendChar(m_mantissa, ch);
      }
    }
  } else {
    _stprintf(m_mantissa, _T("%c"), ch);
    if(value != 0) {
      m_inBigReal = true;
    }
  }
  m_lastWasBinOp = false;
  ajourDisplay();
}

void Calculator::handleComma() {
  if(m_hasComma) {
    return;
  }
  if(m_inExponent) {
    return;
  }
  if(m_radix != 10) {
    return;
  }
  if(m_inBigReal) {
    appendChar(m_mantissa, ',');
  } else {
    _stprintf(m_mantissa, _T("0,"));
  }

  m_hasComma     = true;
  m_inBigReal    = true;
  m_lastWasBinOp = false;
  ajourDisplay();
}

static void changeSign(TCHAR *s, bool showPositive) {
  if(s[0] == '-') {
    if(showPositive) {
      s[0] = '+';
    } else {
      _tcscpy(s, s+1);
    }
  } else {
    if(s[0] == '+') {
      s[0] = '-';
    } else {
      TCHAR tmp[1000];
      _tcscpy(tmp, s);
      _stprintf(s, _T("-%s"), tmp);
    }
  }
}

void Calculator::handleSign() {
  if(m_inBigReal) {
    if(m_inExponent) { // change sign of exponent
      changeSign(m_exponent, true);
    } else {            // change sign of mantissa
      changeSign(m_mantissa, false);
    }
    ajourDisplay();
  } else {
    setDisplay(-getDisplay());
  }
  m_lastWasBinOp = false;
}

void Calculator::handleEE() {
  if(m_inExponent)  {
    return;
  }
  if(m_radix != 10) {
    return;
  }
  _tcscpy(m_exponent, _T("+0"));
  m_inBigReal    = true;
  m_inExponent   = true;
  m_lastWasBinOp = false;
  ajourDisplay();
}

void Calculator::handleBackspace() {
  size_t l;
  if(!m_inBigReal) {
    return;
  }
  if(m_inExponent) {
    l = _tcslen(m_exponent);
    if(l == 2) {
      if(m_exponent[1] == '0') {
        m_inExponent = false;
        if(_tcscmp(m_mantissa, _T("0")) == 0) {
          m_inBigReal = false;
        }
      } else {
        m_exponent[1] = '0';
      }
    } else {
      m_exponent[l-1] = '\0';
    }
  } else { // in mantissa
    l = _tcslen(m_mantissa);
    if(l == 1) {
      if(m_mantissa[0] != '0') {
        m_mantissa[0] = '0';
      }
    } else {
      if(l == 2 && m_mantissa[0] == '-') {
        _stprintf(m_mantissa, _T("0"));
      } else {
        m_mantissa[l-1] = '\0';
      }
    }
    if(_tcscmp(m_mantissa, _T("0")) == 0) {
      m_inBigReal = false;
    }
  }
  m_hasComma = _tcschr(m_mantissa, ',') != nullptr;
  m_lastWasBinOp = false;
  ajourDisplay();
}

static _uint128 decToBin(const BigReal &x) {
  const int s = sign(x);
  switch(sign(x)) {
  case 0:
    return 0;
  case -1:
    { BigReal tmp(x);
      tmp.changeSign();
      BigInt bi = floor(tmp);
      if(bi > _I128_MAX) {
        bi %= _I128_MAX;
      }
      return -(_int128)bi;
    }
  case 1:
    { BigInt bi = floor(x);
      if(bi > _UI128_MAX) {
        bi %= _UI128_MAX;
      }
      return (_uint128)bi;
    }
  }
  throwInvalidArgumentException(__TFUNCTION__,_T("x=%s"), toString(x).cstr());
  return 0;
}

BigReal Calculator::binToDec(_int128 x) const {
  switch(getOperandSize()) {
  case OPSIZE_BYTE : return BigReal((char )(int)x);
  case OPSIZE_WORD : return BigReal((short)(int)x);
  case OPSIZE_DWORD: return BigReal(       (int)x);
  case OPSIZE_QWORD: return BigReal(   (__int64)x);
  case OPSIZE_OWORD: return BigReal(            x);
  }
  return x;
}

_uint128 Calculator::cutWord(_uint128 b) const {
  switch(getOperandSize()) {
  case OPSIZE_BYTE : return b & 0xff;
  case OPSIZE_WORD : return b & 0xffff;
  case OPSIZE_DWORD: return b & 0xffffffff;
  case OPSIZE_QWORD: return b & 0xffffffffffffffffui64;
  case OPSIZE_OWORD: return b;
  }
  return b;
}

BigReal Calculator::scanRadix(const String &str) const {
  BigReal x;
  switch(m_radix) {
  case 2:
  case 8:
  case 16:
    x = binToDec(cutWord(_wcstoui128(str.cstr(),nullptr,m_radix)));
    break;
  case 10:
    { String tmp = str;
      tmp.replace(',', '.');
      x = BigReal(tmp.cstr());
    }
    break;
  }
  return x;
}

String Calculator::groupDigits(const String &str) const {
  char filler = m_radix == 10 ? '.' : ' ';
  int groupSize;
  switch(m_radix) {
  case 2 : groupSize = 4; break;
  case 8 : groupSize = 3; break;
  case 16: groupSize = 4; break;
  case 10:
    { groupSize = 3;
      intptr_t comma    = str.find(',');
      intptr_t exponent = str.find('e');
      String needGrouping = (comma > 0) ? substr(str, 0, comma) : (exponent > 0) ? substr(str, 0, exponent) : str;
      size_t length = needGrouping.length();
      size_t i = 0;
      String result;
      if(needGrouping[i] == '-') {
        result += _T("-");
        i++;
      }
      intptr_t t = (length-i) % groupSize;
      if(t == 0) {
        t = groupSize;
      }
      for(;i < length; i++) {
        result += str[i];
        if(--t == 0 && i < length-1) {
          result += filler;
          t = groupSize;
        }
      }
      if(comma > 0) {
        result += substr(str, comma, str.length());
      } else if(exponent > 0) {
        result += substr(str, exponent, str.length());
      }
      return result;
    }
  }

  String result;
  size_t length = str.length();
  intptr_t t = length % groupSize;
  if(t == 0) {
    t = groupSize;
  }
  for(size_t i = 0; i < length; i++) {
    result += str[i];
    if(--t == 0 && i < length-1) {
      result += filler;
      t = groupSize;
    }
  }
  return result;
}

String Calculator::printRadix(const BigReal &x) const {
  if(!isfinite(x)) {
    switch (_fpclass(x)) {
    case _FPCLASS_PINF: return _T("Infinity" );
    case _FPCLASS_NINF: return _T("-Infinity");
    default           : return _T("Undefined");
    }
  }
  String tmp;
  switch(m_radix) {
  case 2 :
  case 8 :
  case 16:
    { wchar_t buf[256];
      tmp = _tcsupr(_ui128tow(cutWord(decToBin(x)),buf, m_radix));
    }
    break;
  case 10:
    { std::wstringstream stream;
      stream << (FullFormatBigReal&)round(x, m_ndigits - BigReal::getExpo10(x));
      tmp = stream.str().c_str();
      tmp.replace('.', ',');
    }
    break;
  }
  return m_digitGrouping ? groupDigits(tmp) : tmp;
}

const BigReal &Calculator::getDisplay() const {
  if(m_displayDirty) {
    const String tmp = m_inExponent ? format(_T("%se%s"), m_mantissa, m_exponent) : m_mantissa;
    BigReal x = scanRadix(tmp);
    m_display = x;
  }
  m_displayDirty = false;
  return m_display;
}

void Calculator::setDisplay(const BigReal &x) {
  putDisplayText(printRadix(x));
  if((m_radix == 10) || !isfinite(x)) {
    m_display = x;
  } else {
    m_display = binToDec(cutWord(decToBin(x)));
  }
  m_inBigReal    = false;
  m_inExponent   = false;
  m_hasComma     = false;
  m_lastWasBinOp = false;
  m_displayDirty = false;
}

void Calculator::pasteText(const String &s) {
  String tmp = s;
  switch(getRadix()) {
  case 10:
    setDisplay(BigReal(tmp.replace(',', '.')));
    break;
  default:
    setDisplay(scanRadix(s));
    break;
  }
}

void Calculator::putDisplayText(const String &s) {
  m_lock.wait();
  m_displayText = s;
  m_lock.notify();
}

String Calculator::getDisplayText() const {
  m_lock.wait();
  const String s = m_displayText;
  m_lock.notify();
  return s;
}

void Calculator::handleRadix(int radix) {
  if(radix == m_radix) {
    return;
  }
  const BigReal x = getDisplay();
  if(m_radix == 10) {
    const _uint128 b = decToBin(x);
    m_radix = radix;
    setDisplay(binToDec(b));
  } else {
    m_radix = radix;
    setDisplay(x);
  }
}

void Calculator::handleOpSize(OperandSize size) {
  m_opsize = size;
  setDisplay(binToDec(cutWord(decToBin(getDisplay()))));
}

void Calculator::pushDisplay() {
  m_numberStack.push(getDisplay());
  m_inBigReal  = false;
  m_inExponent = false;
  m_hasComma   = false;
}

static int precedens(int button) { // precedens of binary operators
  switch(button) {
  case IDC_BUTTONLPAR     :
    return -1;
  case IDC_BUTTONOR       :
    return 0;
  case IDC_BUTTONAND      :
  case IDC_BUTTONXOR      :
    return 1;
  case IDC_BUTTONADD      :
  case IDC_BUTTONSUB      :
    return 2;
  case IDC_BUTTONDIV      :
  case IDC_BUTTONMULT     :
  case IDC_BUTTONMOD      :
    return 3;
  case IDC_BUTTONLSH      :
    return 4;
  case IDC_BUTTONPOW      :
    return 5;
  }
  throwBigRealException(_T("Unknown precedens:%d"), button);
  return 0;
}

void Calculator::doBinaryOp() {
  const BigReal rightOperand = m_numberStack.pop();
  const BigReal leftOperand  = m_numberStack.pop();
  BigReal result;
  const int op = m_opStack.pop();
  switch(op) {
  case IDC_BUTTONAND      :
    result = (_int128)leftOperand  & (_int128)rightOperand;
    break;
  case IDC_BUTTONXOR      :
    result = (_int128)leftOperand  ^ (_int128)rightOperand;
    break;
  case IDC_BUTTONOR       :
    result = (_int128)leftOperand  | (_int128)rightOperand;
    break;
  case IDC_BUTTONLSH      :
    if(m_inverse) {
      result = (_int128)leftOperand >> (int)rightOperand;
      m_inverse = false;
    } else {
      result = (_int128)leftOperand << (int)rightOperand;
    }
    break;
  case IDC_BUTTONADD      :
    result = rSum(leftOperand, rightOperand, m_ndigits);
    break;
  case IDC_BUTTONSUB      :
    result = rDif(leftOperand, rightOperand, m_ndigits);
    break;
  case IDC_BUTTONDIV      :
    result = rQuot(leftOperand, rightOperand, m_ndigits);
    break;
  case IDC_BUTTONMULT     :
    result = rProd(leftOperand, rightOperand, m_ndigits);
    break;
  case IDC_BUTTONMOD      :
    result = leftOperand % rightOperand;
    break;
  case IDC_BUTTONPOW      :
    if(m_inverse) {
      result = rRoot(leftOperand, rightOperand, m_ndigits);
      m_inverse = false;
    } else {
      result = rPow(leftOperand, rightOperand, m_ndigits);
    }
    break;
  default:
    throwBigRealException(_T("Unknown binaryoperator:%d"), op);
  }
  m_numberStack.push(result);
}

void Calculator::handleBinaryOperator(int button) {
  if(m_lastWasBinOp) { // replace top operator
    m_opStack.pop();
    m_opStack.push(button);
    return;
  }
  pushDisplay();
  switch(button) {
  case IDC_BUTTONADD      :
  case IDC_BUTTONSUB      :
  case IDC_BUTTONDIV      :
  case IDC_BUTTONMULT     :
  case IDC_BUTTONMOD      :
  case IDC_BUTTONPOW      :
  case IDC_BUTTONLSH      :
  case IDC_BUTTONAND      :
  case IDC_BUTTONOR       :
  case IDC_BUTTONXOR      :
    while(!m_opStack.isEmpty() && precedens(m_opStack.top()) >= precedens(button) ) {
      doBinaryOp();
    }
    setDisplay(m_numberStack.top());
    m_opStack.push(button);
    break;
  }
  m_lastWasBinOp = true;
}

void Calculator::handleLPar() {
  m_opStack.push(IDC_BUTTONLPAR);
  m_paranthesLevel++;
  m_lastWasBinOp = false;
}

void Calculator::doParanthes() {
  while(m_opStack.top() != IDC_BUTTONLPAR) {
    doBinaryOp();
  }
  m_opStack.pop();
  m_paranthesLevel--;
}

void Calculator::handleRPar() {
  if(m_paranthesLevel == 0) {
    return;
  }
  if(m_lastWasBinOp) {
    m_opStack.pop();
  } else {
    pushDisplay();
  }
  doParanthes();
  setDisplay(m_numberStack.pop());
}

void Calculator::handleEqual() {
  if(m_lastWasBinOp) {
    m_opStack.pop();
  } else {
    pushDisplay();
  }
  while(m_paranthesLevel > 0) { // terminate all open parantheses
    doParanthes();
  }

  while(!m_opStack.isEmpty()) {
    doBinaryOp();
  }
  setDisplay(m_numberStack.pop());
}

void Calculator::handleTrigo(int button) {
  DigitPool *pool = getDigitPool();
  if(m_radix != 10) {
    return;
  }
  if(m_hyperbolic) {
    if(m_inverse) {
      switch(button) {
      case IDC_BUTTONSIN:
        setDisplay(rAsinh(getDisplay(), m_ndigits));
        break;
      case IDC_BUTTONCOS:
        assertion(getDisplay() >= _1);
        setDisplay(rAcosh(getDisplay(), m_ndigits));
        break;
      case IDC_BUTTONTAN:
        assertion(getDisplay() > m_minusOne && getDisplay() < _1);
        setDisplay(rAtanh(getDisplay(), m_ndigits));
        break;
      }
    } else { // !m_inverse
      switch(button) {
      case IDC_BUTTONSIN:
        setDisplay(rSinh(getDisplay(), m_ndigits));
        break;
      case IDC_BUTTONCOS:
        setDisplay(rCosh(getDisplay(), m_ndigits));
        break;
      case IDC_BUTTONTAN:
        setDisplay(rTanh(getDisplay(), m_ndigits));
        break;
      }
    }
  } else { // !m_hyperbolic
    if(m_inverse) {
      switch(button) {
      case IDC_BUTTONSIN:
        assertion(getDisplay() >= m_minusOne && getDisplay() <= _1);
        setDisplay(fromRadians(rAsin(getDisplay(), m_ndigits)));
        break;
      case IDC_BUTTONCOS:
        assertion(getDisplay() >= m_minusOne && getDisplay() <= _1);
        setDisplay(fromRadians(rAcos(getDisplay(), m_ndigits)));
        break;
      case IDC_BUTTONTAN:
        setDisplay(fromRadians(rAtan(getDisplay(), m_ndigits)));
        break;
      }
    } else {
      BigReal rightAngle = 90;
      switch(m_trigonometricBase) {
      case TRIGO_RADIANS:
        switch(button) {
        case IDC_BUTTONSIN:
          setDisplay(rSin(getDisplay(), m_ndigits));
          break;
        case IDC_BUTTONCOS:
          setDisplay(rCos(getDisplay(), m_ndigits));
          break;
        case IDC_BUTTONTAN:
          try {
            setDisplay(rTan(getDisplay(), m_ndigits));
          } catch(BigRealException) {
            throwInvalidInput();
          }
          break;
        }
        break;

      case TRIGO_GRADS  :
        rightAngle = 100;
        // NB continue case
      case TRIGO_DEGREES:
        { BigReal x = getDisplay();
          bool negative = false;;
          if(x.isNegative()) {
            x = -x;
            negative = true;
          }
          x %= (4 * rightAngle);
          switch(button) {
          case IDC_BUTTONSIN:
            if(x < rightAngle) {
              x =  rSin(toRadians(x), m_ndigits);
            } else if(x < 2*rightAngle) {
              x =  rCos(toRadians(x-rightAngle), m_ndigits);
            } else if(x < 3*rightAngle) {
              x = -rSin(toRadians(x % rightAngle), m_ndigits);
            } else {
              x = -rCos(toRadians((x-rightAngle)%rightAngle), m_ndigits);
            }
            setDisplay(negative ? -x : x);
            break;
          case IDC_BUTTONCOS:
            if(x < rightAngle) {
              x =  rCos(toRadians(x), m_ndigits);
            } else if(x < 2*rightAngle) {
              x = -rSin(toRadians(x-rightAngle), m_ndigits);
            } else if(x < 3*rightAngle) {
              x = -rCos(toRadians(x % rightAngle), m_ndigits);
            } else {
              x =  rSin(toRadians((x-rightAngle)%rightAngle), m_ndigits);
            }
            setDisplay(x);
            break;
          case IDC_BUTTONTAN:
            try {
              if(x < rightAngle) {
                x =  rTan(toRadians(x), m_ndigits);
              } else if(x < 2*rightAngle) {
                x = -rCot(toRadians(x-rightAngle), m_ndigits);
              } else if(x < 3*rightAngle) {
                x =  rTan(toRadians(x % rightAngle), m_ndigits);
              } else {
                x = -rCot(toRadians((x-rightAngle)%rightAngle), m_ndigits);
              }
              setDisplay(negative ? -x : x);
            } catch(BigRealException) {
              throwInvalidInput();
            }
            break;
          }
        }
      }
    }
  }
  m_inverse    = false;
  m_hyperbolic = false;
}

void Calculator::handleDigitGrouping() {
  m_digitGrouping = !m_digitGrouping;
  setDisplay(getDisplay());
}

void Calculator::setPrecision(int ndigits) {
  if(gotError()) {
    handleButton(IDC_BUTTONCLEAR);
  }
  m_ndigits = ndigits;
  setDisplay(getDisplay());
}

void Calculator::handleButton(int button) {
  DigitPool *pool = getDigitPool();
  if(m_gotError && (button != IDC_BUTTONCLEAR) && (button != IDC_BUTTONCE)) {
    return;
  }
  switch(button) {
  case IDC_BUTTONCE          :
    initDisplay();
    break;
  case IDC_BUTTONBACKSPACE   :
    handleBackspace();
    break;
  case IDC_BUTTONCLEAR       :
    init();
    break;
  case IDC_BUTTON0           :
    handleNumButton( '0');
    break;
  case IDC_BUTTON1           :
    handleNumButton( '1');
    break;
  case IDC_BUTTON2           :
    handleNumButton( '2');
    break;
  case IDC_BUTTON3           :
    handleNumButton( '3');
    break;
  case IDC_BUTTON4           :
    handleNumButton( '4');
    break;
  case IDC_BUTTON5           :
    handleNumButton( '5');
    break;
  case IDC_BUTTON6           :
    handleNumButton( '6');
    break;
  case IDC_BUTTON7           :
    handleNumButton( '7');
    break;
  case IDC_BUTTON8           :
    handleNumButton( '8');
    break;
  case IDC_BUTTON9           :
    handleNumButton( '9');
    break;
  case IDC_BUTTONA           :
    handleNumButton( 'A');
    break;
  case IDC_BUTTONB           :
    handleNumButton( 'B');
    break;
  case IDC_BUTTONC           :
    handleNumButton( 'C');
    break;
  case IDC_BUTTOND           :
    handleNumButton( 'D');
    break;
  case IDC_BUTTONE           :
    handleNumButton( 'E');
    break;
  case IDC_BUTTONF           :
    handleNumButton( 'F');
    break;
  case IDC_BUTTONSIGN        :
    handleSign();
    break;
  case IDC_BUTTONCOMMA       :
    handleComma();
    break;
  case IDC_BUTTONEE          :
    handleEE();
    break;
  case IDC_BUTTONADD         :
  case IDC_BUTTONSUB         :
  case IDC_BUTTONMULT        :
  case IDC_BUTTONDIV         :
  case IDC_BUTTONMOD         :
  case IDC_BUTTONPOW         :
  case IDC_BUTTONAND         :
  case IDC_BUTTONOR          :
  case IDC_BUTTONXOR         :
  case IDC_BUTTONLSH         :
    handleBinaryOperator(button);
    break;
  case IDC_BUTTONLPAR        :
    handleLPar();
    break;
  case IDC_BUTTONRPAR        :
    handleRPar();
    break;
  case IDC_BUTTONEQUAL       :
    handleEqual();
    break;
  case IDC_BUTTONMC          :
    m_memory = 0;
    break;
  case IDC_BUTTONMR          :
    setDisplay(m_memory);
    break;
  case IDC_BUTTONMS          :
    m_memory = getDisplay();
    setDisplay(getDisplay());
    break;
  case IDC_BUTTONMADD        :
    m_memory = rSum(m_memory, getDisplay(), m_ndigits);
    setDisplay(getDisplay());
    break;
  case IDC_BUTTONNOT         :
    if(m_radix == 10 && getDisplay().isNegative()) {
      setDisplay(~(_int128)getDisplay());
    } else {
      setDisplay(~(_uint128)getDisplay());
    }
    break;
  case IDC_BUTTONPI          :
    if(m_radix == 10) {
      setDisplay(rPi(m_ndigits));
    }
    break;
  case IDC_BUTTONINT         :
    if(m_inverse) {
      setDisplay(fraction(getDisplay()));
    } else {
      setDisplay(trunc(getDisplay()));
    }
    m_inverse = false;
    break;
  case IDC_BUTTONLN          :
    if(m_inverse) {
      setDisplay(rExp(getDisplay(), m_ndigits));
    } else {
      assertion(getDisplay() > 0);
      setDisplay(rLn(getDisplay(), m_ndigits));
    }
    m_inverse = false;
    break;
  case IDC_BUTTONLOG         :
    if(m_inverse) {
      setDisplay(rPow(10, getDisplay(), m_ndigits));
    } else {
      assertion(getDisplay() > 0);
      setDisplay(rLog(10, getDisplay(), m_ndigits));
    }
    m_inverse = false;
    break;
  case IDC_BUTTONFAC         :
    setDisplay(Rfac(getDisplay(), m_ndigits));
    break;
  case IDC_BUTTONPOW3        :
    if(m_inverse) {
      setDisplay(rCubicRoot(getDisplay(), m_ndigits));
    } else {
      setDisplay(rPow3(getDisplay(), m_ndigits));
    }
    m_inverse = false;
    break;
  case IDC_BUTTONRECIPROC    :
    setDisplay(rQuot(_1, getDisplay(), m_ndigits));
    break;
  case IDC_BUTTONSQUARE      :
    if(m_inverse) {
      assertion(getDisplay() >= 0);
      setDisplay(rSqrt(getDisplay(), m_ndigits));
    } else {
      setDisplay(rSqr(getDisplay(), m_ndigits));
    }
    m_inverse = false;
    break;
  case IDC_BUTTONSIN         :
  case IDC_BUTTONCOS         :
  case IDC_BUTTONTAN         :
    handleTrigo(button);
    break;
  case IDC_BUTTONDMS         :
    if(m_inverse) {
      setDisplay(Rinversdms(getDisplay(), m_ndigits));
    } else {
      setDisplay(Rdms(getDisplay(), m_ndigits));
    }
    m_inverse = false;
    break;
  case IDC_CHECKINV          :
    m_inverse    = !m_inverse;
    break;
  case IDC_CHECKHYP          :
    m_hyperbolic = !m_hyperbolic;
    break;
  case IDC_RADIODEGREES      :
    m_trigonometricBase = TRIGO_DEGREES;
    break;
  case IDC_RADIORADIANS      :
    m_trigonometricBase = TRIGO_RADIANS;
    break;
  case IDC_RADIOGRADS        :
    m_trigonometricBase = TRIGO_GRADS;
    break;
  case IDC_RADIOBIN          :
    handleRadix( 2);
    break;
  case IDC_RADIOOCT          :
    handleRadix( 8);
    break;
  case IDC_RADIODEC          :
    handleRadix(10);
    break;
  case IDC_RADIOHEX          :
    handleRadix(16);
    break;
  case IDC_RADIOBYTE         :
    handleOpSize(OPSIZE_BYTE );
    break;
  case IDC_RADIOWORD         :
    handleOpSize(OPSIZE_WORD );
    break;
  case IDC_RADIODWORD        :
    handleOpSize(OPSIZE_DWORD);
    break;
  case IDC_RADIOQWORD        :
    handleOpSize(OPSIZE_QWORD);
    break;
  case IDC_RADIOOWORD        :
    handleOpSize(OPSIZE_OWORD);
    break;
  case ID_VIEW_DIGITGROUPING :
    handleDigitGrouping();
    break;
  default:
    putDisplayText(format(_T("Invalid key:%d"), button));
    break;
  }
}

UINT Calculator::safeRun() {
  while(!m_killed) {
    m_hasInput.wait();
    if(m_killed) break;
    m_busy = true;
    try {
      handleButton(m_buttonPressed);
    } catch(BigRealException e) {
      putDisplayText(e.what());
      m_gotError = true;
    }
    m_busy = false;
  }
  return 0;
}

void Calculator::enter(int button) {
  if(isBusy()) return;
  m_buttonPressed = button;
  m_hasInput.notify();
}

void Calculator::terminateCalculation() {
  BigRealResourcePool::terminateAllPoolCalculations();
}
