#pragma once

#include <Math/BigReal.h>
#include <Thread.h>
#include <Stack.h>
#include <Semaphore.h>
//typedef double BigReal;

#define MAXPRECISION 500000

typedef enum {
  TRIGO_RADIANS, // 2*pi radians
  TRIGO_DEGREES, // 360 degrees
  TRIGO_GRADS    // 400 degrees
} Trigonometric;

typedef enum {
  OPSIZE_BYTE
 ,OPSIZE_WORD
 ,OPSIZE_DWORD
 ,OPSIZE_QWORD
 ,OPSIZE_OWORD
} OperandSize;

class Calculator {
private:
  const BigReal   m_minusOne;
  String          m_displayText;
  TCHAR           m_mantissa[MAXPRECISION+100];
  TCHAR           m_exponent[30];
  bool            m_inBigReal;
  bool            m_hasComma;
  bool            m_inExponent;
  bool            m_lastWasBinOp;
  mutable bool    m_displayDirty;
  mutable BigReal m_display;
  BigReal         m_memory;
  int             m_paranthesLevel;
  Stack<BigReal>  m_numberStack;
  Stack<int>      m_opStack;
  int             m_ndigits;
  int             m_radix;
  bool            m_inverse;
  bool            m_hyperbolic;
  bool            m_digitGrouping;
  bool            m_gotError;
  Trigonometric   m_trigonometricBase;
  OperandSize     m_opsize;

  void             initDisplay();
  void             initMemory();
  void             init();
  void             doBinaryOp();
  void             doParanthes();
  unsigned int     maxBigRealLen() const;
  void             handleNumButton(char ch);
  void             handleComma();
  void             handleSign();
  void             handleEE();
  void             handleBackspace();
  void             handleRadix(int radix);
  void             handleOpSize(OperandSize size);
  void             handleBinaryOperator(        int button);
  void             handleTrigo(                 int button);
  void             handleDigitGrouping();
  void             handleLPar();
  void             handleRPar();
  void             handleEqual();
  BigReal          scanRadix(  const String &str) const;
  String           groupDigits(const String &str) const;
  _uint128         cutWord( _uint128 b) const;
  BigReal          binToDec(_int128  b) const;
  String           printRadix(const BigReal &x) const;
  void             ajourDisplay();
  void             setDisplay(const BigReal &x);
  const BigReal    &getDisplay() const;
  DigitPool        *getDigitPool() const {
    return m_display.getDigitPool();
  }

  void             pushDisplay();
  BigReal          toRadians(  const BigReal &x) const;
  BigReal          fromRadians(const BigReal &x) const;
  void             enterButton(int button);
public:
  Calculator();
  void          enter(int button);
  const String &getDisplayText()        const { return m_displayText;       }
  void          setDisplayText(const String &s);
  int           getRadix()              const { return m_radix;             }
  bool          getInverse()            const { return m_inverse;           }
  bool          getHyperbolic()         const { return m_hyperbolic;        }
  bool          getMemoryIndicator()    const { return !m_memory.isZero();  }
  int           getParanthesLevel()     const { return m_paranthesLevel;    }
  Trigonometric getTrigonometricBase()  const { return m_trigonometricBase; }
  OperandSize   getOperandSize()        const { return m_opsize;            }
  bool          gotError()              const { return m_gotError;          }
  int           getPrecision()          const { return m_ndigits;           }
  void          setPrecision(int ndigits);
  bool          getDigitGrouping()      const { return m_digitGrouping;     }
};

BigReal dms(const BigReal &x, const BigReal &f);
BigReal inversdms(const BigReal &x, const BigReal &f);

class CalculatorThread : public Thread,  public Calculator {
private:
  Semaphore m_sem;
  int       m_buttonPressed;
  bool      m_busy;
  bool      m_killed;
  unsigned int run();
public:
  CalculatorThread();
 ~CalculatorThread();
  void enter(int button);
  inline bool isBusy() const {
    return m_busy;
  }
  void terminateCalculation();
};
