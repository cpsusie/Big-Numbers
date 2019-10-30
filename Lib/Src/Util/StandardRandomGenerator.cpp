#include "pch.h"
#include <Random.h>

static JavaRandom stdRandomGenerator;
RandomGenerator *RandomGenerator::s_stdGenerator = &stdRandomGenerator;
