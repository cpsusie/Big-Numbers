#pragma once

#include "targetver.h"

// Headers for CppUnitTest
#include "CppUnitTest.h"

#include <MyUtil.h>

typedef void (*vprintFunction)(const TCHAR *format, va_list argptr);
void generateTestSequence(vprintFunction vpf);
void callAssemblerCode();