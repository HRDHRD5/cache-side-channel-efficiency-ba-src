#ifndef CBSC_MEASUREMENT_UTIL_H
#define CBSC_MEASUREMENT_UTIL_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "constants.h"

uint64_t random_uint_64();

void uint_to_bin_str(const uint64_t in, char *out, const uint64_t out_len);

#endif