#pragma once

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include "targetver.h"

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#define _CRT_SECURE_NO_WARNINGS 1

#include <afxwin.h>         // MFC core and standard components

#include <ChessGame.h>

#define CHESSLIB_ROOT "C:/mytools2015/Spil/ChessLib/"

#ifndef _M_X64
#define CHESSLIB_PLATFORM "Win32/"
#else
#define CHESSLIB_PLATFORM "X64/"
#endif

#ifdef _DEBUG
#define CHESSLIB_CONFIG "TablebaseBuilder Debug/"
#else
#define CHESSLIB_CONFIG "TablebaseBuilder Release/"
#endif

#define CHESSLIB_VERSION CHESSLIB_ROOT CHESSLIB_PLATFORM CHESSLIB_CONFIG 

#ifdef GIVE_LINK_MESSAGE
#pragma message(__FILE__ ":link with " CHESSLIB_VERSION)
#endif
#pragma comment(lib, CHESSLIB_VERSION "ChessLib.lib")

//{{AFX_INSERT_LOCATION}}
