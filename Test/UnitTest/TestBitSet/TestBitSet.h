#pragma once

#include <BitSet.h>

BitSet &genRandomSet(BitSet &dst, unsigned int capacity, int size);
BitSet genRandomSet(unsigned int capacity, int size=-1);

void testBitSet();
void testTinyBitSet();
void testBitMatrix();
void testMatrixIterator();
