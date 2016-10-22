//  GREKO Chess Engine
//  (c) 2002-2011 Vladimir Medvedev <vrm@bk.ru>
//  http://greko.110mb.com

//  win32.cpp: Windows-specific code
//  modified: 1-Aug-2011

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include "utils.h"

extern PROTOCOL_T g_protocol;

static int is_pipe = 0;
static HANDLE input_handle = 0;

void SleepMilliseconds(int ms)
{
  Sleep(ms);
}
////////////////////////////////////////////////////////////////////////////////

void Highlight(int on)
{
  //
  // Turn highlight of the console text on or off
  //

  if (on)
    SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE),
                             FOREGROUND_RED |
                             FOREGROUND_GREEN |
                             FOREGROUND_BLUE | FOREGROUND_INTENSITY );
  else
    SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE),
                             FOREGROUND_RED |
                             FOREGROUND_GREEN |
                             FOREGROUND_BLUE );
}
////////////////////////////////////////////////////////////////////////////////

void InitInput()
{
  DWORD dw;
  input_handle = GetStdHandle(STD_INPUT_HANDLE);
  is_pipe = !GetConsoleMode(input_handle, &dw);

  if (is_pipe)
    g_protocol = UCI;

  setbuf(stdout, NULL);
}
////////////////////////////////////////////////////////////////////////////////

int InputAvailable()
{
  DWORD nchars;

  /* When using Standard C input functions, also check if there
  is anything in the buffer. After a call to such functions,
  the input waiting in the pipe will be copied to the buffer,
  and the call to PeekNamedPipe can indicate no input available.
  Setting stdin to unbuffered was not enough, IIRC */

  if (stdin->_cnt > 0)
    return 1;

  if (is_pipe)
  {
    /* When running under a GUI, you will end here. */

    if (!PeekNamedPipe(input_handle, NULL, 0, NULL, &nchars, NULL))

      /* Something went wrong. Probably the parent program exited.
        Could call exit() here. Returning 1 will make the next call
        to the input function return EOF, where this should be
        catched then. */

      return 1;

    return (nchars != 0);
  }
  else
    return _kbhit() != 0; /* In "text-mode" without GUI */
}
////////////////////////////////////////////////////////////////////////////////

