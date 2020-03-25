#include "stdafx.h"
#include <Random.h>
#include <Math/BigReal/BigReal.h>
#include <Math/BigReal/BigRealResourcePool.h>
#include <Math/BigReal/MrIsprime.h>

using namespace std;

class GCDResult {
public:
  BigInt x,gcd;
  GCDResult(const BigInt &x, const BigInt &gcd);
};

GCDResult::GCDResult(const BigInt &x, const BigInt &gcd){
  this->x = x;
  this->gcd = gcd;
};

GCDResult extendedGCD(BigInt a, BigInt b) {
  BigInt x = 0, lastx = 1;
  BigInt y = 1, lasty = 0;
  while(!b.isZero()) {
    BigInt quot = a / b;
    BigInt temp = a % b;
    a = b;
    b = temp;

    temp = lastx - quot*x;
    lastx = x;
    x = temp;

    temp = lasty - quot*y;
    lasty = y;
    y = temp;
  }
  return GCDResult(lastx,a);
}

BigInt multiplicativeInverse(const BigInt &a, const BigInt &m) {
  GCDResult temp = extendedGCD(a,m);
  if(temp.gcd == 1)
    return (temp.x + m) % m;
  else
    return 0;
}
/*
int main(int argc, char **argv) {
  for(;;) {
    BigInt a,m;
    printf("Enter a,m:");
    char line[100];
    gets(line);
    if(sscanf(line,"%u %u",&a,&m) != 2) {
      printf("fejl");
      continue;
    }
    BigInt x = multiplicativeInverse(a,m);
    printf("multiplicativeInverse(%u,%u)=%u\n",a,m,x);
    printf("a*x mod m = %u\n",(a*x) % m);
  }

  return 0;
}
*/

class PublicKey {
public:
  BigInt g,n;
  PublicKey(const BigInt &g, const BigInt &n);
  BigInt enchrypt(const BigInt &msg);
};

class PrivateKey {
public:
  BigInt k,n;
  PrivateKey(const BigInt &k, const BigInt &n);
  BigInt dechrypt(const BigInt &msg);
};

PublicKey::PublicKey(const BigInt &g, const BigInt &n) {
  this->g = g;
  this->n = n;
}

BigInt PublicKey::enchrypt(const BigInt &msg) {
  return powmod(msg,g,n);
}

tostream &operator<<(tostream &s, const PublicKey &key) {
  s << _T("g:") << key.g << _T("\n") << _T("n:") << key.n << _T("\n");
  return s;
}

PrivateKey::PrivateKey(const BigInt &k, const BigInt &n) {
  this->k = k;
  this->n = n;
}

tostream &operator<<(tostream &s, const PrivateKey &key) {
  s << _T("k:") << key.k << _T("\n") << _T("n:") << key.n << _T("\n");
  return s;
}

BigInt PrivateKey::dechrypt(const BigInt &msg) {
  return powmod(msg,k,n);
}

int main(int argc, char **argv) {
  try{
    randomize();

    tcout << "Finding first prime..."; cout.flush();
    BigInt p = findRandomPrime(100);
    tcout << "Done\n"; cout.flush();

    tcout << "Finding second prime..."; tcout.flush();
    BigInt q = findRandomPrime(100);
    tcout << "Done\n"; tcout.flush();

    tcout << "Finding k..."; tcout.flush();
    BigInt k = findRandomPrime(200);
    tcout << "Done\n"; tcout.flush();

    BigRealResourcePool::resetAllPoolCalculations();

    BigInt r = (p-1)*(q-1);

    BigInt g = multiplicativeInverse(k,r);
    BigInt n = p*q;

    PublicKey  publicKey(g,n);
    PrivateKey privateKey(k,n);

    BigInt msg = BigInt("1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");

    tcout << "Enchrypting..."; tcout.flush();
    BigInt c = publicKey.enchrypt(msg);
    tcout << "Done\n"; tcout.flush();

    tcout << "Dechrypting..."; tcout.flush();
    BigInt msg1 = privateKey.dechrypt(c);
    tcout << "Done\n"; tcout.flush();

    tcout << "Public key :\n" << publicKey << endl;
    tcout << "Private key:\n" << privateKey << endl;
    tcout << "Message    :" << msg  << endl;
    tcout << "Cipher     :\n" << c    << endl << endl;
    tcout << "Dechrypted :" << msg1 << endl;

    tcout.flush();

    return 0;
  } catch (Exception e) {
    _ftprintf(stderr, _T("Exception:%s\n"), e.what());
    return -1;
  }
}

