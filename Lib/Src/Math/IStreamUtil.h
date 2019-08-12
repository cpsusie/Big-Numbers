#pragma once

#define nextchar(        in) { buf += ch; in.get(); ch = in.peek();       }
#define skipSpace(       in) { while(iswspace(ch)) nextchar(in);          }
#define skipLeadingspace(in) { if(iswspace(ch = in.peek())) skipSpace(in) }

#define SETUPISTREAM(in)         \
const int oldFlags = in.flags(); \
in.unsetf(ios::skipws);          \
String   buf;                    \
CharType ch;                     \
if(oldFlags & ios::skipws) {     \
  skipLeadingspace(in);          \
} else {                         \
  ch = in.peek();                \
}

#define RESTOREISTREAM(in)       in.flags(oldFlags)

#define ungetbuf(in)             \
{ while(!buf.isEmpty()) {        \
    in.unget();                  \
    ch = (CharType)buf.last();   \
    buf.removeLast();            \
  }                              \
}
