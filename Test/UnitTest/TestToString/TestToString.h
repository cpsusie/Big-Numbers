#pragma once

#define TTS_D80 0x01
#define TTS_BR  0x02
#define TTS_ALL (TTS_D80 | TTS_BR)

void testToString(UINT flags = TTS_ALL);
