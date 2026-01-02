#ifndef __ADDER_H__
#define __ADDER_H__

#include "ap_int.h"

typedef ap_int<32> data_t;

void adder(data_t A, data_t B, data_t C, data_t &Y);

#endif