#pragma once

#include "Complex.h"

void fft(CompactArray<Complex> &x, bool forward=true);
void dft(CompactArray<Complex> &x, bool forward=true);

