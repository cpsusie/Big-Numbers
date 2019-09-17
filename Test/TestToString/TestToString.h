#pragma once

#define TTS_D80                0x01
#define TTS_BR                 0x02
#define TTS_DUMPALLFORMATFLAGS 0x04

#define TTS_ALLTYPES (TTS_D80 | TTS_BR)

void testToString(UINT flags);
