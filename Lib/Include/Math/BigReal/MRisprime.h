#pragma once

#include <Math/MathLib.h>
#include <Math/AlgorithmHandler.h>
#include "BigReal.h"

class MillerRabinCheck {
public:
  const int     m_threadId;
  const BigInt &m_number;
  const bool    m_isPrime;
  const String  m_msg;
  MillerRabinCheck(int threadId, const BigInt &number, const String &msg, bool isPrime)
    : m_threadId(threadId)
    , m_number(  number  )
    , m_isPrime( isPrime )
    , m_msg  (   msg     )
  {
  }
};

typedef AlgorithmHandler<MillerRabinCheck> MillerRabinHandler;

// Miller-Rabin probabilistic primality test
bool MRisprime(const BigInt &n, int threadId = -1, MillerRabinHandler *handler = nullptr);
// Find count random primes with the specified number of digits
Array<BigInt> findRandomPrimes(int count, int digitCount, int threadCount = 1, DigitPool *pool = nullptr, MillerRabinHandler *handler = nullptr);
BigInt        findRandomPrime(int digitCount, int threadCount = 1, DigitPool *pool = nullptr, MillerRabinHandler *handler = nullptr);
