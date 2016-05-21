// HelloWorld.cpp : main project file.

#include "stdafx.h"

using namespace System;

int main(array<System::String ^> ^args)
{
  Console::WriteLine(L"sizeof(long double): " + sizeof(long double));
  for (int i = 0; i < 10; i++) {
		Console::WriteLine(L"Hello World " + i);
	}
    return 0;
}
