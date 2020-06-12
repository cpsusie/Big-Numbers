#pragma once

#include "targetver.h"
#include <MFCUtil/WinTools.h>
#include <ScanDir.h>
#include <Game.h>
#include <EndGameTablebase.h>
#include <Options.h>

#define CHESSLIBROOT "../ChessLib/"

#define CHESSLIB_VERSION CHESSLIBROOT _PLATFORM_ _CONFIGURATION_

#if defined(GIVE_LINK_MESSAGE)
#pragma message("link with " CHESSLIB_VERSION)
#endif
#pragma comment(lib, CHESSLIB_VERSION "ChessLib.lib")
