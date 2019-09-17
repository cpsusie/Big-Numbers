#pragma once

#include "targetver.h"

#include <MFCUTil/WinTools.h>

#include <MyUtil.h>
#include <MathUtil.h>

String dataToBinString(const void *data, size_t n); // n in bits
String dataToHexString(const void *data, size_t n); // n in bytes

