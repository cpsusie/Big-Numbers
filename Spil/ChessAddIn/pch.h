#pragma once

#include "targetver.h"

#define _CRT_SECURE_NO_WARNINGS 1

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include "ChessAddIn.h"
#include <ChessGame.h>
#include <EndGameUtil.h>

#define CHESSLIB_ROOT "C:/mytools2015/Spil/ChessLib/"

#define CHESSLIB_CONFIG "TablebaseBuilder" _CONFIGURATION_

#define CHESSLIB_VERSION CHESSLIB_ROOT _PLATFORM_ CHESSLIB_CONFIG

#if defined(GIVE_LINK_MESSAGE)
#pragma message(__FILE__ ":link with " CHESSLIB_VERSION)
#endif
#pragma comment(lib, CHESSLIB_VERSION "ChessLib.lib")


