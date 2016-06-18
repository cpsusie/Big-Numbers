#include "pch.h"
#include <CallCounter.h>

#if(SP_OPT_METHOD == SP_OPT_BY_REG64)

#if(BIGREALBASE != 1000000000000000000)
#error For SP_OPT_METHOD == SP_OPT_BY_REG64 BIGREALBASE must be 1e18
#endif

//static CallCounter addSPCpp("addSubProductCpp");
//static CallCounter addSPTotal("addSubProductTotal");

class _uint128 {
public:
  unsigned __int64 lo;
  unsigned __int64 hi;
};

// return 0 if sum of products has been added. 1 if we need to do it in C-code. if so, the sum is returned in bigsum
extern "C" int BigRealMultiplyColumn(const Digit *yk, const Digit *xk, Digit *dst, _uint128 &bigSum);

// assume x != 0 and y != 0. and loopCount > 0
BigReal &BigReal::shortProductNoZeroCheck(const BigReal &x, const BigReal &y, size_t loopCount) { // return *this
  int              digitsAdded = 0;
  Digit           *cd;
  _uint128         bigSum128;

  clearDigits();
  (cd = m_first = newDigit())->n = 0;
  m_expo = m_low = x.m_expo + y.m_expo;

  for(const Digit *xk = x.m_first, *yk = y.m_first;;) { // loopcondition at the end
    cd = fastAppendDigit(cd);
    digitsAdded++;

    if (BigRealMultiplyColumn(yk, xk, cd, bigSum128)) {
      goto NextDigit;
    }
    throwException(_T("Sum too big. Not implemented yet"));
#ifdef __NEVER__
    BRDigitType carry = 0;
    Digit      *d = cd;
    d->n = bigSum64 % BIGREALBASE;
    bigSum64 /= BIGREALBASE;
    do {
      d = d->prev;
      carry += (unsigned long)(d->n + bigSum64 % BIGREALBASE);
      d->n = carry % BIGREALBASE;
      carry    /= BIGREALBASE;
      bigSum64 /= BIGREALBASE;
    } while(bigSum64 || carry);
// dont call trimZeroes() !!;
#endif
NextDigit:
    if(--loopCount <= 0) break;
    if(yk->next) {
      yk = yk->next;
    } else if(!(xk = xk->next)) {
      break; // we are done
    }
  }

  if(cd->n == 0) { // Fixup both ends of digit chain, to reestablish invariant (See comments in assertIsValidBigReal)
    m_last = cd;
    for(digitsAdded--, cd = m_last->prev; cd->n == 0; cd = cd->prev, digitsAdded--);
    deleteDigits(cd->next, m_last);
  }
  m_low -= digitsAdded - 1;
  (m_last = cd)->next = NULL;

  if(m_first->n == 0) {
    cd = m_first;
    (m_first = m_first->next)->prev = NULL;
    deleteDigits(cd,cd);
  } else {
    m_first->prev = NULL;
    m_expo++;
  }
  return setSignByProductRule(x, y);
}

#ifdef USE_DIV128_1

static int bsr(uint64_t x) {
  uint64_t y;
  uint64_t r;

  r = (x > 0xFFFFFFFF) << 5; x >>= r;
  y = (x > 0xFFFF    ) << 4; x >>= y; r |= y;
  y = (x > 0xFF      ) << 3; x >>= y; r |= y;
  y = (x > 0xF       ) << 2; x >>= y; r |= y;
  y = (x > 0x3       ) << 1; x >>= y; r |= y;

  return static_cast<int>(r | (x >> 1));
}

static uint64_t mul(uint64_t a, uint64_t b, uint64_t &y) {
  auto a_lo = a & 0x00000000FFFFFFFF;
  auto a_hi = a >> 32;

  auto b_lo = b & 0x00000000FFFFFFFF;
  auto b_hi = b >> 32;

  auto c0 = a_lo * b_lo;
  auto c1 = a_hi * b_lo;
  auto c2 = a_hi * b_hi;

  auto u1 = c1 + (a_lo * b_hi);
  if(u1 < c1){
    c2 += 1LL << 32;
  }

  auto u0 = c0 + (u1 << 32);
  if(u0 < c0){
    ++c2;
  }

  y = c2 + (u1 >> 32);

  return u0;
}

static uint64_t div128BitA(uint64_t a_lo, uint64_t a_hi, uint64_t b, uint64_t &r) {
  uint64_t p_lo;
  uint64_t p_hi;
  uint64_t q = 0;

  auto r_hi = a_hi;
  auto r_lo = a_lo;

  int s = 0;
  if(0 == (b >> 63)) {

    // Normalize so quotient estimates are
    // no more than 2 in error.

    // Note: If any bits get shifted out of
    // r_hi at this point, the result would
    // overflow.

    s = 63 - bsr(b);
    const auto t = 64 - s;

    b <<= s;
    r_hi = (r_hi << s)|(r_lo >> t);
    r_lo <<= s;
  }

  const auto b_hi = b >> 32;

  /*
  The first full-by-half division places b
  across r_hi and r_lo, making the reduction
  step a little complicated.

  To make this easier, u_hi and u_lo will hold
  a shifted image of the remainder.

  [u_hi||    ][u_lo||    ]
  [r_hi||    ][r_lo||    ]
  [ b  ||    ]
  [p_hi||    ][p_lo||    ]
  |
  V
  [q_hi||    ]
  */

  auto q_hat = r_hi / b_hi;

  p_lo = mul(b, q_hat, p_hi);

  const auto u_hi = r_hi >> 32;
  const auto u_lo = (r_hi << 32)|(r_lo >> 32);

  // r -= b*q_hat
  //
  // At most 2 iterations of this...
  while((p_hi > u_hi) || ((p_hi == u_hi) && (p_lo > u_lo)) ) {
    if(p_lo < b){
      --p_hi;
    }
    p_lo -= b;
    --q_hat;
  }

  auto w_lo = (p_lo << 32);
  auto w_hi = (p_hi << 32)|(p_lo >> 32);

  if(w_lo > r_lo){
    ++w_hi;
  }

  r_lo -= w_lo;
  r_hi -= w_hi;

  q = q_hat << 32;

  /*
  The lower half of the quotient is easier,
  as b is now aligned with r_lo.

  |r_hi][r_lo||    ]
  [ b  ||    ]
  [p_hi||    ][p_lo||    ]
  |
  V
  [q_hi||q_lo]
  */

  q_hat = ((r_hi << 32)|(r_lo >> 32)) / b_hi;

  p_lo = mul(b, q_hat, p_hi);

  // r -= b*q_hat
  //
  // ...and at most 2 iterations of this.
  while((p_hi > r_hi) || ((p_hi == r_hi) && (p_lo > r_lo)) ) {
    if(p_lo < b){
      --p_hi;
    }
    p_lo -= b;
    --q_hat;
  }

  r_lo -= p_lo;

  q |= q_hat;

  r = r_lo >> s;

  return q;
}
#else

static uint64_t div128BitB(uint64_t a_lo, uint64_t a_hi, uint64_t b, uint64_t &r) {
  uint64_t q   = a_lo << 1; // quotient
  uint64_t rem = a_hi; // remainder

  uint64_t carry = a_lo >> 63;
  uint64_t temp_carry = 0;
  for(int i = 0; i < 64; i++) {
    temp_carry = rem >> 63;
    rem <<= 1;
    rem |= carry;
    carry = temp_carry;

    if(carry == 0) {
      if(rem >= b) {
        carry = 1;
      } else {
        temp_carry = q >> 63;
        q <<= 1;
        q |= carry;
        carry = temp_carry;
        continue;
      }
    }

    rem -= b;
    rem -= (1 - carry);
    carry = 1;
    temp_carry = q >> 63;
    q <<= 1;
    q |= carry;
    carry = temp_carry;
  }
  r = rem;
  return q;
}

#endif

#define SPLIT_LENGTH 240

size_t BigReal::s_splitLength = SPLIT_LENGTH; // Value found by experiments with measureSplitFactor in testnumber.cpp

#define MAX_DIGITVALUE (BIGREALBASE-1)

unsigned int BigReal::getMaxSplitLength() { // static
  return 340;
}

#endif // SP_OPT_METHOD == SP_OPT_BY_REG32
