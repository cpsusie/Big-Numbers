#include "pch.h"

static const BYTE eCharClass[256] = {
  0x20  /*   0  ctrl */
 ,0x20  /*   1  ctrl */
 ,0x20  /*   2  ctrl */
 ,0x20  /*   3  ctrl */
 ,0x20  /*   4  ctrl */
 ,0x20  /*   5  ctrl */
 ,0x20  /*   6  ctrl */
 ,0x20  /*   7  ctrl */
 ,0x20  /*   8  ctrl */
 ,0x68  /* tab  space ctrl blank */
 ,0x28  /*  lf  space ctrl */
 ,0x28  /*  11  space ctrl */
 ,0x28  /*  12  space ctrl */
 ,0x28  /*  cr  space ctrl */
 ,0x20  /*  14  ctrl */
 ,0x20  /*  15  ctrl */
 ,0x20  /*  16  ctrl */
 ,0x20  /*  17  ctrl */
 ,0x20  /*  18  ctrl */
 ,0x20  /*  19  ctrl */
 ,0x20  /*  20  ctrl */
 ,0x20  /*  21  ctrl */
 ,0x20  /*  22  ctrl */
 ,0x20  /*  23  ctrl */
 ,0x20  /*  24  ctrl */
 ,0x20  /*  25  ctrl */
 ,0x20  /*  26  ctrl */
 ,0x20  /*  27  ctrl */
 ,0x20  /*  28  ctrl */
 ,0x20  /*  29  ctrl */
 ,0x20  /*  30  ctrl */
 ,0x20  /*  31  ctrl */
 ,0x48  /*      space blank */
 ,0x10  /* !    punct */
 ,0x10  /* "    punct */
 ,0x10  /* #    punct */
 ,0x10  /* $    punct */
 ,0x10  /* %    punct */
 ,0x10  /* &    punct */
 ,0x10  /* '    punct */
 ,0x10  /* (    punct */
 ,0x10  /* )    punct */
 ,0x10  /* *    punct */
 ,0x10  /* +    punct */
 ,0x10  /* ,    punct */
 ,0x10  /* -    punct */
 ,0x10  /* .    punct */
 ,0x10  /* /    punct */
 ,0x84  /* 0    digit hex */
 ,0x84  /* 1    digit hex */
 ,0x84  /* 2    digit hex */
 ,0x84  /* 3    digit hex */
 ,0x84  /* 4    digit hex */
 ,0x84  /* 5    digit hex */
 ,0x84  /* 6    digit hex */
 ,0x84  /* 7    digit hex */
 ,0x84  /* 8    digit hex */
 ,0x84  /* 9    digit hex */
 ,0x10  /* :    punct */
 ,0x10  /* ;    punct */
 ,0x10  /* <    punct */
 ,0x10  /* =    punct */
 ,0x10  /* >    punct */
 ,0x10  /* ?    punct */
 ,0x10  /* @    punct */
 ,0x81  /* A    upper hex */
 ,0x81  /* B    upper hex */
 ,0x81  /* C    upper hex */
 ,0x81  /* D    upper hex */
 ,0x81  /* E    upper hex */
 ,0x81  /* F    upper hex */
 ,0x01  /* G    upper */
 ,0x01  /* H    upper */
 ,0x01  /* I    upper */
 ,0x01  /* J    upper */
 ,0x01  /* K    upper */
 ,0x01  /* L    upper */
 ,0x01  /* M    upper */
 ,0x01  /* N    upper */
 ,0x01  /* O    upper */
 ,0x01  /* P    upper */
 ,0x01  /* Q    upper */
 ,0x01  /* R    upper */
 ,0x01  /* S    upper */
 ,0x01  /* T    upper */
 ,0x01  /* U    upper */
 ,0x01  /* V    upper */
 ,0x01  /* W    upper */
 ,0x01  /* X    upper */
 ,0x01  /* Y    upper */
 ,0x01  /* Z    upper */
 ,0x10  /* [    punct */
 ,0x10  /* \    punct */
 ,0x10  /* ]    punct */
 ,0x10  /* ^    punct */
 ,0x10  /* _    punct */
 ,0x10  /* `    punct */
 ,0x82  /* a    lower hex */
 ,0x82  /* b    lower hex */
 ,0x82  /* c    lower hex */
 ,0x82  /* d    lower hex */
 ,0x82  /* e    lower hex */
 ,0x82  /* f    lower hex */
 ,0x02  /* g    lower */
 ,0x02  /* h    lower */
 ,0x02  /* i    lower */
 ,0x02  /* j    lower */
 ,0x02  /* k    lower */
 ,0x02  /* l    lower */
 ,0x02  /* m    lower */
 ,0x02  /* n    lower */
 ,0x02  /* o    lower */
 ,0x02  /* p    lower */
 ,0x02  /* q    lower */
 ,0x02  /* r    lower */
 ,0x02  /* s    lower */
 ,0x02  /* t    lower */
 ,0x02  /* u    lower */
 ,0x02  /* v    lower */
 ,0x02  /* w    lower */
 ,0x02  /* x    lower */
 ,0x02  /* y    lower */
 ,0x02  /* z    lower */
 ,0x10  /* {    punct */
 ,0x10  /* |    punct */
 ,0x10  /* }    punct */
 ,0x10  /* ~    punct */
 ,0x20  /* 127  ctrl */
 ,0x00  /* Ä    */
 ,0x00  /* Å     */
 ,0x00  /* Ç    */
 ,0x00  /* É    */
 ,0x00  /* Ñ    */
 ,0x00  /* Ö    */
 ,0x00  /* Ü    */
 ,0x00  /* á    */
 ,0x00  /* à    */
 ,0x00  /* â    */
 ,0x00  /* ä    */
 ,0x00  /* ã    */
 ,0x00  /* å    */
 ,0x00  /* ç     */
 ,0x00  /* é    */
 ,0x00  /* è     */
 ,0x00  /* ê     */
 ,0x10  /* ë    punct */
 ,0x10  /* í    punct */
 ,0x00  /* ì    */
 ,0x00  /* î    */
 ,0x00  /* ï    */
 ,0x00  /* ñ    */
 ,0x00  /* ó    */
 ,0x00  /* ò    */
 ,0x00  /* ô    */
 ,0x00  /* ö    */
 ,0x00  /* õ    */
 ,0x00  /* ú    */
 ,0x00  /* ù     */
 ,0x00  /* û    */
 ,0x00  /* ü    */
 ,0x00  /* †    */
 ,0x10  /* °    punct */
 ,0x10  /* ¢    punct */
 ,0x10  /* £    punct */
 ,0x10  /* §    punct */
 ,0x10  /* •    punct */
 ,0x10  /* ¶    punct */
 ,0x10  /* ß    punct */
 ,0x10  /* ®    punct */
 ,0x10  /* ©    punct */
 ,0x10  /* ™    punct */
 ,0x10  /* ´    punct */
 ,0x10  /* ¨    punct */
 ,0x10  /* ≠    punct */
 ,0x10  /* Æ    punct */
 ,0x10  /* Ø    punct */
 ,0x10  /* ∞    punct */
 ,0x10  /* ±    punct */
 ,0x10  /* ≤    punct */
 ,0x10  /* ≥    punct */
 ,0x10  /* ¥    punct */
 ,0x10  /* µ    punct */
 ,0x10  /* ∂    punct */
 ,0x10  /* ∑    punct */
 ,0x10  /* ∏    punct */
 ,0x10  /* π    punct */
 ,0x10  /* ∫    punct */
 ,0x10  /* ª    punct */
 ,0x10  /* º    punct */
 ,0x10  /* Ω    punct */
 ,0x10  /* æ    punct */
 ,0x10  /* ø    punct */
 ,0x01  /* ¿    upper */
 ,0x01  /* ¡    upper */
 ,0x01  /* ¬    upper */
 ,0x01  /* √    upper */
 ,0x01  /* ƒ    upper */
 ,0x01  /* ≈    upper */
 ,0x01  /* ∆    upper */
 ,0x01  /* «    upper */
 ,0x01  /* »    upper */
 ,0x01  /* …    upper */
 ,0x01  /*      upper */
 ,0x01  /* À    upper */
 ,0x01  /* Ã    upper */
 ,0x01  /* Õ    upper */
 ,0x01  /* Œ    upper */
 ,0x01  /* œ    upper */
 ,0x01  /* –    upper */
 ,0x01  /* —    upper */
 ,0x01  /* “    upper */
 ,0x01  /* ”    upper */
 ,0x01  /* ‘    upper */
 ,0x01  /* ’    upper */
 ,0x01  /* ÷    upper */
 ,0x10  /* ◊    punct */
 ,0x01  /* ÿ    upper */
 ,0x01  /* Ÿ    upper */
 ,0x01  /* ⁄    upper */
 ,0x01  /* €    upper */
 ,0x01  /* ‹    upper */
 ,0x01  /* ›    upper */
 ,0x10  /* ﬁ    punct */
 ,0x10  /* ﬂ    punct */
 ,0x02  /* ‡    lower */
 ,0x02  /* ·    lower */
 ,0x02  /* ‚    lower */
 ,0x02  /* „    lower */
 ,0x02  /* ‰    lower */
 ,0x02  /* Â    lower */
 ,0x02  /* Ê    lower */
 ,0x02  /* Á    lower */
 ,0x02  /* Ë    lower */
 ,0x02  /* È    lower */
 ,0x02  /* Í    lower */
 ,0x02  /* Î    lower */
 ,0x02  /* Ï    lower */
 ,0x02  /* Ì    lower */
 ,0x02  /* Ó    lower */
 ,0x02  /* Ô    lower */
 ,0x02  /*     lower */
 ,0x02  /* Ò    lower */
 ,0x02  /* Ú    lower */
 ,0x02  /* Û    lower */
 ,0x02  /* Ù    lower */
 ,0x02  /* ı    lower */
 ,0x02  /* ˆ    lower */
 ,0x10  /* ˜    punct */
 ,0x02  /* ¯    lower */
 ,0x02  /* ˘    lower */
 ,0x02  /* ˙    lower */
 ,0x02  /* ˚    lower */
 ,0x02  /* ¸    lower */
 ,0x02  /* ˝    lower */
 ,0x10  /* ˛    punct */
 ,0x02  /* ˇ    lower */
};

#define UPPER          0x01    /* upper case letter */
#define LOWER          0x02    /* lower case letter */
#define DIGIT          0x04    /* digit[0-9] */
#define SPACE          0x08    /* tab, carriage return, newline, */
                               /* vertical tab or form feed */
#define PUNCT          0x10    /* punctuation TCHAR */
#define CONTROL        0x20    /* control TCHAR */
#define BLANK          0x40    /* space char */
#define HEX            0x80    /* hexadecimal digit */

int eIsalpha( int c) {
  return (c == EOF) ? 0 : (eCharClass[(BYTE)c] & (UPPER | LOWER));
}

int eIsupper( int c) {
  return (c == EOF) ? 0 : (eCharClass[(BYTE)c] & UPPER);
}

int eIslower( int c) {
  return (c == EOF) ? 0 : (eCharClass[(BYTE)c] & LOWER);
}

int eIsdigit( int c) {
  return (c == EOF) ? 0 : (eCharClass[(BYTE)c] & DIGIT);
}

int eIsxdigit(int c) {
  return (c == EOF) ? 0 : (eCharClass[(BYTE)c] & HEX);
}

int eIsspace( int c) {
  return (c == EOF) ? 0 : (eCharClass[(BYTE)c] & SPACE);
}

int eIspunct( int c) {
  return (c == EOF) ? 0 : (eCharClass[(BYTE)c] & PUNCT);
}

int eIsalnum( int c) {
  return (c == EOF) ? 0 : (eCharClass[(BYTE)c] & (UPPER | LOWER | DIGIT));
}

int eIsprint( int c) {
  return (c == EOF) ? 0 : (eCharClass[(BYTE)c] & (UPPER | LOWER | DIGIT | PUNCT | SPACE));
}

int eIsgraph( int c) {
  return (c == EOF) ? 0 : (eCharClass[(BYTE)c] & (UPPER | LOWER | DIGIT | PUNCT));
}

int eIscntrl( int c) {
  return (c == EOF) ? 0 : (eCharClass[(BYTE)c] & CONTROL);
}

