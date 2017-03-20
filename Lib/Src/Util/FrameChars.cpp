#include "pch.h"
#include <Console.h>

FrameChars::FrameChars(_TUCHAR ul, _TUCHAR ur, _TUCHAR ll, _TUCHAR lr, _TUCHAR horz, _TUCHAR vert) {
  m_ul   = ul;   // upper left corner
  m_ur   = ur;   // upper right corner
  m_ll   = ll;   // lower left corner
  m_lr   = lr;   // lower right corner
  m_horz = horz; // horizontal line
  m_vert = vert; // vertical line
}

#ifdef UNICODE
const FrameChars FrameChars::s_frames[] = {
  FrameChars(' ', ' ', ' ', ' ', ' ', ' ' )   // NOFRAME
 ,FrameChars(L'\u250C', L'\u2510', L'\u2514', L'\u2518', L'\u2500', L'\u2502' ) // SINGLE_FRAME
 ,FrameChars(L'\u2554', L'\u2557', L'\u255A', L'\u255D', L'\u2550', L'\u2551' ) // DOUBLE_FRAME
// ,FrameChars(213, 184, 212, 190, 205, 179 )   // DOUBLE_HORZ
// ,FrameChars(214, 183, 211, 189, 196, 186 )   // DOUBLE_VERT
};

const _TUCHAR FrameChars::s_leftVertSingleLineDoubleBorder  = _T('\u2520');
const _TUCHAR FrameChars::s_rightVertSingleLineDoubleBorder = _T('\u2528');

const _TUCHAR FrameChars::s_leftVertDoubleLineDoubleBorder  = _T('\u2523');
const _TUCHAR FrameChars::s_rightVertDoubleLineDoubleBorder = _T('\u252B');

#else
const FrameChars FrameChars::s_frames[] = {
  FrameChars(' ', ' ', ' ', ' ', ' ', ' ' )   // NOFRAME
 ,FrameChars(218, 191, 192, 217, 196, 179 )   // SINGLE_FRAME
 ,FrameChars(201, 187, 200, 188, 205, 186 )   // DOUBLE_FRAME
// ,FrameChars(213, 184, 212, 190, 205, 179 )   // DOUBLE_HORZ
// ,FrameChars(214, 183, 211, 189, 196, 186 )   // DOUBLE_VERT
};
#endif

String FrameChars::toString() const {
  return format(_T("'%c','%c','%c','%c','%c','%c'"),m_ul,m_ur,m_ll,m_lr,m_vert,m_horz);
}
