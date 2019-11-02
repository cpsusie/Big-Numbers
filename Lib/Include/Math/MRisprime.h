#pragma once

#include "MathLib.h"
#include "AlgorithmHandler.h"
#include "BigReal.h"

class MillerRabinCheck {
public:
  const int     m_threadId;
  const BigInt &m_number;
  int           m_count;
  String        m_msg;
  MillerRabinCheck(int threadId, const BigInt &number, int count, const String &msg)
    : m_threadId(threadId)
    , m_number(number)
  {
    m_count = count;
    m_msg   = msg;
  }
};

typedef AlgorithmHandler<MillerRabinCheck> MillerRabinHandler;

bool MRisprime(const BigInt &n, int threadId = -1, MillerRabinHandler *handler = NULL); // Miller-Rabin probabilistic primality test
BigInt findRandomPrime(int digitCount, int threadCount = 1, DigitPool *pool = NULL, MillerRabinHandler *handler = NULL);
