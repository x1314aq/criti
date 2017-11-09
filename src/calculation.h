//
// Created by xaq on 10/26/17.
//

#include "common.h"

#ifndef TTYW_CALCULATION_H
#define TTYW_CALCULATION_H

#ifdef __cplusplus
extern "C" {
#endif

void run_calculation(CALC_MODE_T mode);

void calc_criticality();

void calc_fixed_source();

void calc_burnup();

void calc_point_burn();

#ifdef __cplusplus
}
#endif

#endif //TTYW_CALCULATION_H