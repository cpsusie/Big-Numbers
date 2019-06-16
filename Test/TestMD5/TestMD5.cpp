/* testmd5.C - test driver for MD2, MD4 and MD5
 *
 * 
 * Copyright (C) 1990-2, RSA Data Security, Inc. Created 1990. All
 * rights reserved.
 * 
 * RSA Data Security, Inc. makes no representations concerning either
 * the merchantability of this software or the suitability of this
 * software for any particular purpose. It is provided "as is"
 * without express or implied warranty of any kind.
 * 
 * These notices must be retained in any copies of any part of this
 * documentation and/or software.
 */

#include "stdafx.h"
#include <MyUtil.h>
#include <time.h>
#include <MD5.h>

// Digests a String and prints the result.
static void MDString(const char *s) {
  MD5HashCode code;
  String codeString = MD5::getHashCode(code, ByteArray((BYTE*)s, strlen(s))).toString();
  USES_ACONVERSION;
  const char *codeStrA = T2A(codeString.cstr());
  printf("MD5(\"%s\"):\"%s\"\n", s, codeStrA);
}

// Digests a reference suite of strings and prints the results.
static void MDTestSuite () {
  printf ("MD5 test suite:\n");

  MDString("");
  MDString("a");
  MDString("abc");
  MDString("message digest");
  MDString("abcdefghijklmnopqrstuvwxyz");
  MDString("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  MDString("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
}

// Digests a file and prints the result.
static void MDFile(const String &fileName = EMPTYSTRING) {
  try {
    MD5HashCode code;
    MD5::getHashCode(code, fileName.length() ? ByteInputFile(fileName) : ByteInputFile(stdin));
    
    _tprintf(_T("MD5(%s) = %s\n"), fileName.cstr(), code.toString().cstr());
  } catch (Exception e) {
    _tprintf(_T("%s\n"), e.what());
  }
}

/*
 * A.5 Test suite
 * 
 *    The MD5 test suite (driver option "-x") should print the following
 *    results:
 * 
 * MD5 test suite:
 * MD5 ("") = d41d8cd98f00b204e9800998ecf8427e
 * MD5 ("a") = 0cc175b9c0f1b6a831c399e269772661
 * MD5 ("abc") = 900150983cd24fb0d6963f7d28e17f72
 * MD5 ("message digest") = f96b697d7cb7938d525a2f31aaf161d0
 * MD5 ("abcdefghijklmnopqrstuvwxyz") = c3fcd3d76192e4007dfb496cca67e13b
 * MD5 ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") =
 * d174ab98d277d9f5a5611c2c9f419d9f
 * MD5 ("123456789012345678901234567890123456789012345678901234567890123456
 * 78901234567890") = 57edf4a22be3c955ac49da2e2107b67a
 * 
 * Security Considerations
 * 
 *    The level of security discussed in this memo is considered to be
 *    sufficient for implementing very high security hybrid digital-
 *    signature schemes based on MD5 and a public-key cryptosystem.
 * 
 * Author's Address
 * 
 *    Ronald L. Rivest
 *    Massachusetts Institute of Technology
 *    Laboratory for Computer Science
 *    NE43-324
 *    545 Technology Square
 *    Cambridge, MA  02139-1986
 * 
 *    Phone: (617) 253-5880
 *    EMail: rivest@theory.lcs.mit.edu
 * 
 */

static void Usage() {
  fprintf(stderr,"Usage:mddriver [options] [filename1 filename2...]\n"
                 "Options: (may be any combination):\n"
                 "  -sstring - digests String\n"
                 "  -t       - runs time trial\n"
                 "  -x       - runs test script\n"
                 "filename - digests file\n"
                 "(none)   - digests standard input\n"
         );
  exit(-1);
}

int main (int argc, char **argv) {
  if (argc == 1) {
    MDFile();
  } else {
    char *cp;
    for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
      for(cp++;*cp;cp++) {
        switch(*cp) {
        case 's':
          MDString(cp+1);
          break;
        case 'x':
          MDTestSuite();
          break;
        default:
          Usage();
      }
      break;
      }
    }
    for(; *argv; argv++) {
      MDFile(*argv);
    }
  }
  return (0);
}
